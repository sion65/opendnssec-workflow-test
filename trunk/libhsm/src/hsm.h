/* $Id$ */

/*
 * Copyright (c) 2009 .SE (The Internet Infrastructure Foundation).
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

#ifndef HSM_H
#define HSM_H 1

#include <pkcs11.h>

/*
 * N.B: DRAFT PROPOSAL ONLY
 */


/* Data type to describe an HSM */
typedef struct {
	const char *name;    /* name from hsm_attach() */
	const char *path;    /* path from hsm_attach() */
	const void *handle;  /* handle from dlopen()*/
	CK_FUNCTION_LIST_PTR sym;   /* Function list from dlsym */
	CK_SESSION_HANDLE session;  /* session to that hsm */
} hsm_module_t;

/* Data type to describe a key pair at any HSM */
typedef struct {
	const hsm_module_t *module;       /* pointer to module */
	const CK_OBJECT_HANDLE key;       /* key within session */
	const uuid_t *uuid;               /* UUID of key (if available) */
} hsm_key_t;



/* Initialize HSM library using XML configuration file. Attached all
   configured HSMs, querying for PINs if not known. */
int hsm_autoconf(const char *config);

/* List all known keys in all attached HSMs */
hsm_key_t **hsm_list_keys(void);

/* Find a key pair by UUID, return key identifier or NULL if not found.
   Other find_key_by_xxx functions may be added later if needed. */
const hsm_key_t *hsm_find_key_by_uuid(const uuid_t *uuid);

/* Generate new key pair, return key identifier or NULL if
   key generation failed.
   Keys generated by libhsm will have a UUID set as CKA_ID and CKA_LABEL.
   Other stuff, like exponent, may be needed here as well */
const hsm_key_t *hsm_generate_rsa_key(const char *repository, unsigned long keysize);

/* Remove key */
int hsm_remove_key(const hsm_key_t *key);

/* Get UUID using key identifier */
const uuid_t *hsm_get_uuid(const hsm_key_t *key);

/* Sign RRset */
ldns_rr* hsm_sign_rrset(const ldns_rr_list* rrset, const hsm_key_t *key);

/* Get DNSKEY RR */
ldns_rr* hsm_get_dnskey(const hsm_key_t *key);

/* Fill a buffer with random data from any attached HSM */
int hsm_random_buffer(unsigned long length, unsigned char *buffer);

/* Return unsigned 32-bit random number from any attached HSM */
u_int32_t hsm_random32(void);

/* Return unsigned 64-bit random number from any attached HSM */
u_int64_t hsm_random64(void);



/*
 * Additional internal functions
 */

/* Initialize HSM library. This is done once per application. */
int hsm_init(void);

/* Attached a named HSM using a PKCS#11 shared library and
   optional credentials (may be NULL, but then undefined) */
int hsm_attach(const char *repository, const char *path, const char *pin);

/* Detach a named HSM */
int hsm_detach(const char *name);


#endif /* HSM_H */
