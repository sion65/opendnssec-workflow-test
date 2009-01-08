/* $Id$ */

/*
 * Copyright (c) 2008 .SE (The Internet Infrastructure Foundation).
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/************************************************************
*
* This class handles the internal state.
* Mainly session and object handling.
*
************************************************************/

#include "SoftHSMInternal.h"

// Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <string>

// Includes for the crypto library
#include <botan/pipe.h>
#include <botan/filters.h>
#include <botan/hex.h>
#include <botan/sha2_32.h>
using namespace Botan;

SoftHSMInternal::SoftHSMInternal(bool threading, CK_CREATEMUTEX cMutex,
  CK_DESTROYMUTEX dMutex, CK_LOCKMUTEX lMutex, CK_UNLOCKMUTEX uMutex) {

  openSessions = 0;

  for(int i = 0; i < MAX_SESSION_COUNT; i++) {
    sessions[i] = NULL_PTR;
  }

  objects = new SoftObject();

  pin = NULL_PTR;

  createMutexFunc = cMutex;
  destroyMutexFunc = dMutex;
  lockMutexFunc = lMutex;
  unlockMutexFunc = uMutex;
  usesThreading = threading;

  db = new SoftDatabase();
}

SoftHSMInternal::~SoftHSMInternal() {
  for(int i = 0; i < MAX_SESSION_COUNT; i++) {
    if(sessions[i] != NULL_PTR) {
      delete sessions[i];
      sessions[i] = NULL_PTR;
    }
  }

  openSessions = 0;

  if(objects != NULL_PTR) {
    delete objects;
    objects = NULL_PTR;
  }

  if(pin != NULL_PTR) {
    free(pin);
    pin = NULL_PTR;
  }

  if(db != NULL_PTR) {
    delete db;
    db = NULL_PTR;
  }
}

int SoftHSMInternal::getSessionCount() {
  return openSessions;
}

// Creates a new session if there is enough space available.

CK_RV SoftHSMInternal::openSession(CK_FLAGS flags, CK_VOID_PTR pApplication, CK_NOTIFY Notify, CK_SESSION_HANDLE_PTR phSession) {
  if(openSessions >= MAX_SESSION_COUNT) {
    return CKR_SESSION_COUNT;
  }

  if((flags & CKF_SERIAL_SESSION) == 0) {
    return CKR_SESSION_PARALLEL_NOT_SUPPORTED;
  }

  if(!phSession) {
    return CKR_ARGUMENTS_BAD;
  }

  for(int i = 0; i < MAX_SESSION_COUNT; i++) {
    if(sessions[i] == NULL_PTR) {
      openSessions++;
      sessions[i] = new SoftSession(flags & CKF_RW_SESSION);
      sessions[i]->pApplication = pApplication;
      sessions[i]->Notify = Notify;
      *phSession = (CK_SESSION_HANDLE)(i+1);
      return CKR_OK;
    }
  }

  return CKR_SESSION_COUNT;
}

// Closes the specific session.

CK_RV SoftHSMInternal::closeSession(CK_SESSION_HANDLE hSession) {
  if(hSession > MAX_SESSION_COUNT || hSession < 1) {
    return CKR_SESSION_HANDLE_INVALID;
  }

  if(sessions[hSession-1] == NULL_PTR) {
    return CKR_SESSION_CLOSED;
  }

  delete sessions[hSession-1];
  sessions[hSession-1] = NULL_PTR;

  openSessions--;

  // Last session. Clear objects.
  if(openSessions == 0) {
    if(pin != NULL_PTR) {
      delete pin;
      pin = NULL_PTR;
    }

    clearObjectsAndCaches();
  }

  return CKR_OK;
}

// Closes all the sessions.

CK_RV SoftHSMInternal::closeAllSessions() {
  for (int i = 0; i < MAX_SESSION_COUNT; i++) {
    if(sessions[i] != NULL_PTR) {
      delete sessions[i];
      sessions[i] = NULL_PTR;
    }
  }

  openSessions = 0;

  if(pin != NULL_PTR) {
    delete pin;
    pin = NULL_PTR;
  }

  clearObjectsAndCaches();

  return CKR_OK;
}

// Return information about the session.

CK_RV SoftHSMInternal::getSessionInfo(CK_SESSION_HANDLE hSession, CK_SESSION_INFO_PTR pInfo) {
  SoftSession *session = getSession(hSession);

  if(session == NULL_PTR) {
    return CKR_SESSION_HANDLE_INVALID;
  }

  if(pInfo == NULL_PTR) {
    return CKR_ARGUMENTS_BAD;
  }

  pInfo->slotID = 1;

  if(pin) {
    if(session->isReadWrite()) {
      pInfo->state = CKS_RW_USER_FUNCTIONS;
    } else {
      pInfo->state = CKS_RO_USER_FUNCTIONS;
    }
  } else {
    if(session->isReadWrite()) {
      pInfo->state = CKS_RW_PUBLIC_SESSION;
    } else {
      pInfo->state = CKS_RO_PUBLIC_SESSION;
    }
  }

  pInfo->flags = CKF_SERIAL_SESSION;
  if(session->isReadWrite()) {
    pInfo->flags |= CKF_RW_SESSION;
  }
  pInfo->ulDeviceError = 0;

  return CKR_OK;
}

// Logs the user into the token.

CK_RV SoftHSMInternal::login(CK_SESSION_HANDLE hSession, CK_USER_TYPE userType, CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen) {
  SoftSession *session = getSession(hSession);

  if(session == NULL_PTR) {
    return CKR_SESSION_HANDLE_INVALID;
  }

  if(pPin == NULL_PTR) {
    return CKR_ARGUMENTS_BAD;
  }

  if(ulPinLen < 4 || ulPinLen > 8000) {
    return CKR_PIN_INCORRECT;
  }

  // Digest the PIN
  // We do not use any salt
  Pipe *digestPIN = new Pipe(new Hash_Filter(new SHA_256), new Hex_Encoder);
  digestPIN->start_msg();
  digestPIN->write(pPin, ulPinLen);
  digestPIN->write(pPin, ulPinLen);
  digestPIN->write(pPin, ulPinLen);
  digestPIN->end_msg();

  // Get the digested PIN
  SecureVector<byte> pinVector = digestPIN->read_all();
  int size = pinVector.size();
  char *tmpPIN = (char *)malloc(size + 1);
  tmpPIN[size] = '\0';
  memcpy(tmpPIN, pinVector.begin(), size);
  delete digestPIN;

  // Is someone already logged in?
  if(pin != NULL_PTR) {
    // Is it the same password?
    if(strcmp(tmpPIN, pin) == 0) {
      return CKR_OK;
    } else {
      free(pin);
      clearObjectsAndCaches();
    }
  }

  pin = tmpPIN;

  getAllObjects();

  return CKR_OK;
}

// Logs out the user from the token.
// Closes all the objects.

CK_RV SoftHSMInternal::logout(CK_SESSION_HANDLE hSession) {
  SoftSession *session = getSession(hSession);

  if(session == NULL_PTR) {
    return CKR_SESSION_HANDLE_INVALID;
  }

  if(pin != NULL_PTR) {
    free(pin);
    pin = NULL_PTR;
  }

  clearObjectsAndCaches();

  return CKR_OK;
}

// Retrieves the session pointer associated with the session handle.

SoftSession* SoftHSMInternal::getSession(CK_SESSION_HANDLE hSession) {
  if(hSession > MAX_SESSION_COUNT || hSession < 1) {
    return NULL_PTR;
  }

  return sessions[hSession-1];
}

// Retrieves the object pointer associated with the object handle.
// Returns NULL_PTR if no matching object is found.

SoftObject* SoftHSMInternal::getObject(CK_OBJECT_HANDLE hObject) {
  return objects->getObject(hObject);
}

// Checks if the user is logged in.

bool SoftHSMInternal::isLoggedIn() {
  if(pin == NULL_PTR) {
    return false;
  } else {
    return true;
  }
}

char* SoftHSMInternal::getPIN() {
  return pin;
}

// Retrieves the attributes specified by the template.
// There can be different error states depending on 
// if the given buffer is too small, the attribute is 
// sensitive, or not supported by the object.
// If there is an error, then the most recent one is
// returned.

CK_RV SoftHSMInternal::getAttributeValue(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount) {
  SoftSession *session = getSession(hSession);

  if(session == NULL_PTR) {
    return CKR_SESSION_HANDLE_INVALID;
  }

  SoftObject *object = objects->getObject(hObject);

  if(object == NULL_PTR) {
    return CKR_OBJECT_HANDLE_INVALID;
  }

  CK_RV result = CKR_OK;
  CK_RV objectResult = CKR_OK;

  for(CK_ULONG i = 0; i < ulCount; i++) {
    objectResult = object->getAttribute(&pTemplate[i]);
    if(objectResult != CKR_OK) {
      result = objectResult;
    }
  }

  return result;
}

// Set the attributes according to the template.

CK_RV SoftHSMInternal::setAttributeValue(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount) {
  SoftSession *session = getSession(hSession);

  if(session == NULL_PTR) {
    return CKR_SESSION_HANDLE_INVALID;
  }

  SoftObject *object = objects->getObject(hObject);

  if(object == NULL_PTR) {
    return CKR_OBJECT_HANDLE_INVALID;
  }

  if(!session->isReadWrite()) {
    return CKR_SESSION_READ_ONLY;
  }

  CK_RV result = CKR_OK;
  CK_RV objectResult = CKR_OK;

  // Loop through all the attributes in the template
  for(CK_ULONG i = 0; i < ulCount; i++) {
    objectResult = object->setAttribute(&pTemplate[i], session->db);
    if(objectResult != CKR_OK) {
      result = objectResult;
    }
  }

  return result;
}

// Initialize the search for objects.
// The template specifies the search pattern.

CK_RV SoftHSMInternal::findObjectsInit(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount) {
  SoftSession *session = getSession(hSession);

  if(session == NULL_PTR) {
    return CKR_SESSION_HANDLE_INVALID;
  }

  if(session->findInitialized) {
    return CKR_OPERATION_ACTIVE;
  }

  if(session->findAnchor != NULL_PTR) {
    delete session->findAnchor;
  }

  // Creates the search result chain.
  session->findAnchor = new SoftFind();
  session->findCurrent = session->findAnchor;

  SoftObject *currentObject = objects;

  // Check with all objects.
  while(currentObject->nextObject != NULL_PTR) {
    CK_BBOOL findObject = CK_TRUE;

    // See if the object match all attributes.
    for(CK_ULONG j = 0; j < ulCount; j++) {
      if(currentObject->matchAttribute(&pTemplate[j]) == CK_FALSE) {
        findObject = CK_FALSE;
      }
    }

    // Add the handle to the search results if the object matched the attributes.
    if(findObject == CK_TRUE) {
      session->findAnchor->addFind(currentObject->index);
    }

    // Iterate
    currentObject = currentObject->nextObject;
  }

  session->findInitialized = true;

  return CKR_OK;
}

// Destroys the object.

CK_RV SoftHSMInternal::destroyObject(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject) {
  SoftSession *session = getSession(hSession);

  if(session == NULL_PTR) {
    return CKR_SESSION_HANDLE_INVALID;
  }

  // Remove the key from the sessions' key cache
  for(int i = 0; i < MAX_SESSION_COUNT; i++) {
    if(sessions[i] != NULL_PTR) {
      sessions[i]->keyStore->removeKey(hObject);
    }
  }

  // Delete the object from the database
  db->deleteObject(this->getPIN(), hObject);

  // Delete the object from the internal state
  return objects->deleteObj(hObject);
}

// Wrapper for the mutex function.

CK_RV SoftHSMInternal::createMutex(CK_VOID_PTR_PTR newMutex) {
  if(!usesThreading) {
    return CKR_OK;
  }

  // Calls the real mutex function via its function pointer.
  return createMutexFunc(newMutex);
}

// Wrapper for the mutex function.

CK_RV SoftHSMInternal::destroyMutex(CK_VOID_PTR mutex) {
  if(!usesThreading) {
    return CKR_OK;
  }

  // Calls the real mutex function via its function pointer.
  return destroyMutexFunc(mutex);
}

// Wrapper for the mutex function.

CK_RV SoftHSMInternal::lockMutex(CK_VOID_PTR mutex) {
  if(!usesThreading) {
    return CKR_OK;
  }

  // Calls the real mutex function via its function pointer.
  return lockMutexFunc(mutex);
}

// Wrapper for the mutex function.

CK_RV SoftHSMInternal::unlockMutex(CK_VOID_PTR mutex) {
  if(!usesThreading) {
    return CKR_OK;
  }

  // Calls the real mutex function via its function pointer.
  return unlockMutexFunc(mutex);
}

// Add a new object from the database
// Returns an index to the object

void SoftHSMInternal::getObjectFromDB(int keyRef) {
  SoftObject *newObject = db->populateObj(keyRef);

  if(newObject != NULL_PTR) {
    newObject->nextObject = objects;
    objects = newObject;
  }
}

// Get all objects associated with the given PIN
// and store them in the object buffer

void SoftHSMInternal::getAllObjects() {
  int objectCount = 0;
  int *objectRefs = db->getObjectRefs(pin, objectCount);

  for(int i = 0; i < objectCount; i++) {
    this->getObjectFromDB(objectRefs[i]);
  }
}

// Clear all the objects and the session caches

void SoftHSMInternal::clearObjectsAndCaches() {
  // Clear all the objects
  if(objects != NULL_PTR) {
    delete objects;
  }
  objects = new SoftObject();

  // Clear the key caches
  for(int i = 0; i < MAX_SESSION_COUNT; i++) {
    if(sessions[i] != NULL_PTR) {
      if(sessions[i]->keyStore != NULL_PTR) {
        delete sessions[i]->keyStore;
      }
      sessions[i]->keyStore = new SoftKeyStore();
    }
  }  
}
