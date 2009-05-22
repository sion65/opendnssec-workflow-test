/* $Id$ */

/*
 * Copyright (c) 2009 .SE (The Internet Infrastructure Foundation).
 * Copyright (c) 2009 NLNet Labs.
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

#include "config.h"

#include <ldns/ldns.h>
#include <uuid/uuid.h>

#define HSM_MAX_SESSIONS 10

/*! Data type to describe an HSM */
typedef struct {
	unsigned int id;           /*!< HSM numerical identifier */
	char *name;          /*!< name of module */
	char *path;          /*!< path to PKCS#11 library */
	void *handle;        /*!< handle from dlopen()*/
	void *sym;  /*!< Function list from dlsym */
} hsm_module_t;

/*! HSM Session */
typedef struct {
	hsm_module_t *module;
	unsigned long session;
} hsm_session_t;

/*! HSM Key Pair */
typedef struct {
	const hsm_module_t *module;  /*!< pointer to module */
	unsigned long private_key;  /*!< private key within module */
	unsigned long public_key;  /*!< public key within module */
	uuid_t *uuid;          /*!< UUID of key (if available) */
} hsm_key_t;

/*! HSM context to keep track of sessions */
typedef struct {
	hsm_session_t *session[HSM_MAX_SESSIONS];  /*!< HSM sessions */
	size_t session_count;     /*!< number of configured HSMs */
} hsm_ctx_t;

/*! Extra information for signing rrsets (algorithm, expiration, etc) */
typedef struct {
	/** The DNS signing algorithm identifier */
	ldns_algorithm algorithm;
	/** Key flags */
	uint16_t flags;
	/** The inception date of signatures made with this key. */
	uint32_t inception;
	/** The expiration date of signatures made with this key. */
	uint32_t expiration;
	/** The keytag of the key (is this necessary?) */
	uint16_t keytag;
	/** The owner name of the key */
	ldns_rdf *owner;
} hsm_sign_params_t;

/*! Open HSM library

\param config path to OpenDNSSEC XML configuration file
\param pin_callback This function will be called for tokens that have
                    no PIN configured. The default hsm_prompt_pin() can
                    be used. If this value is NULL, these tokens will
                    be skipped
\param data optional data that will be directly passed to the callback
            function
\return 0 if successful, !0 if failed

Attaches all configured HSMs, querying for PINs (using the given
callback function) if not known.
Also creates initial sessions (not part of any context; every API
function that takes a context can be passed NULL, in which case the
global context will be used) and log into each HSM.
*/
int hsm_open(const char *config,
             char *(pin_callback)(const char *token_name, void *),
             void *data);


/*! Function that queries for a PIN, can be used as callback
    for hsm_open()

\param token_name The name will be included in the prompt
\param data This value is unused
\return The string the user enters
*/
char *hsm_prompt_pin(const char *token_name, void *data);


/*! Close HSM library

    Log out and detach from all configured HSMs
    This cleans up all data for libhsm, and should be the last function
    called.
*/
int hsm_close();


/*! Create new HSM context

Creates a new session for each attached HSM. The returned hsm_ctx_t *
can be freed with hsm_destroy_context()
*/
hsm_ctx_t *hsm_create_context(void);

/*! Destroy HSM context

\param context HSM context

Also destroys any associated sessions.
*/
void hsm_destroy_context(hsm_ctx_t *context);


/**
 * Returns an allocated hsm_sign_params_t with some defaults
 */
hsm_sign_params_t *hsm_sign_params_new();

/*!
Free the signer parameters structure

If params->owner has been set, ldns_rdf_deep_free() will be called
on it.

\param params The signer parameters to free
*/
void hsm_sign_params_free(hsm_sign_params_t *params);


/*! List all known keys in all attached HSMs

After the function has run, the value at count contains the number
of keys found. 

The resulting key list can be freed with hsm_key_list_free()
Alternatively, each individual key structure in the list could be
freed with hsm_key_free()

\param context HSM context
\param count location to store the number of keys found
*/
hsm_key_t **hsm_list_keys(const hsm_ctx_t *context, size_t *count);


/*! Find a key pair by UUID

The returned key structure can be freed with hsm_key_free()

\param context HSM context
\param uuid UUID of key to find
\return key identifier or NULL if not found
*/
hsm_key_t *hsm_find_key_by_uuid(const hsm_ctx_t *context,
                                const uuid_t *uuid);


/*! Generate new key pair in HSM

Keys generated by libhsm will have a UUID set as CKA_ID and CKA_LABEL.
Other stuff, like exponent, may be needed here as well.

The returned key structure can be freed with hsm_key_free()

\param context HSM context
\param repository repository in where to create the key
\param keysize Size of RSA key
\return return key identifier or NULL if key generation failed
*/
hsm_key_t *hsm_generate_rsa_key(const hsm_ctx_t *context,
                                const char *repository,
                                unsigned long keysize);


/*! Remove a key pair from HSM

When a key is removed, the module pointer is set to NULL, and
the public and private key handles are set to 0. The structure still
needs to be freed.

\param context HSM context
\param key Key pair to be removed
\return 0 if successful, !0 if failed
*/
int hsm_remove_key(const hsm_ctx_t *context, hsm_key_t *key);

/*! Free the memory for a key structure.

If the uuid* value in the key is not NULL, it is freed as well

\param key The key structure to free
*/
void hsm_key_free(hsm_key_t *key);


/*! Free the memory of an array of key structures, as returned by
hsm_list_keys()

\param key_list The array of keys to free
\param count The number of keys in the array
*/
void hsm_key_list_free(hsm_key_t **key_list, size_t count);


/*! Get UUID using key identifier

The returned uuid is only a pointer to within the key structure, and
does not need to be freed separately
\param context HSM context
\param key Key pair to get UUID from
\return UUID of key pair
*/
uuid_t *hsm_get_uuid(const hsm_ctx_t *context, const hsm_key_t *key);


/*! Sign RRset using key

The returned ldns_rr structure can be freed with ldns_rr_free()

\param context HSM context
\param rrset RRset to sign
\param key Key pair used to sign
\return ldns_rr* Signed RRset
*/
ldns_rr* hsm_sign_rrset(const hsm_ctx_t *ctx,
                        const ldns_rr_list* rrset,
                        const hsm_key_t *key,
                        const hsm_sign_params_t *sign_params);


/*! Generate a base32 encoded hashed NSEC3 name

\param ctx HSM context
\param name Domain name to hash
\param algorithm NSEC3 algorithm (must be 1 atm)
\param iteration number of hash iterations
\param salt_length the length of the salt
\param salt the salt
*/
ldns_rdf *
hsm_nsec3_hash_name(const hsm_ctx_t *ctx,
                    ldns_rdf *name,
                    uint8_t algorithm,
                    uint16_t iterations,
                    uint8_t salt_length,
                    uint8_t *salt);


/*! Get DNSKEY RR

The returned ldns_rr structure can be freed with ldns_rr_free()

\param context HSM context
\param key Key to get DNSKEY RR from
\param sign_params the signing parameters (flags, algorithm, etc)
\return ldns_rr*
*/
ldns_rr* hsm_get_dnskey(const hsm_ctx_t *ctx,
                        const hsm_key_t *key,
                        const hsm_sign_params_t *sign_params);


/*! Fill a buffer with random data from any attached HSM

\param context HSM context
\param buffer Buffer to fill with random data
\param length Size of random buffer
\return 0 if successful, !0 if failed

*/
int hsm_random_buffer(const hsm_ctx_t *ctx,
                      unsigned char *buffer,
                      unsigned long length);


/*! Return unsigned 32-bit random number from any attached HSM
\param context HSM context
\return 32-bit random number, or 0 if no HSM with a random generator is
               attached 
*/
uint32_t hsm_random32(const hsm_ctx_t *ctx);


/*! Return unsigned 64-bit random number from any attached HSM
\param context HSM context
\return 64-bit random number, or 0 if no HSM with a random generator is
               attached 
*/
uint64_t hsm_random64(const hsm_ctx_t *ctx);



/*
 * Additional functions for debugging, and non-general use-cases.
 */

/*! Attached a named HSM using a PKCS#11 shared library and
   optional credentials (may be NULL, but then undefined)
   This function changes the global state, and is not threadsafe

\param token_name the name of the token to attach
\param path the path of the shared PKCS#11 library
\param pin the PIN to log into the token
\return 0 on success, -1 on error
*/
int hsm_attach(char *token_name,
               char *path,
               char *pin);

/*! Detach a named HSM
   This function changes the global state, and is not threadsafe
\param token_name the token to detach
\return 0 on success, -1 on error
*/
int hsm_detach(const char *token_name);

/*! Check whether a named token has been initialized in this context
\param ctx HSM context
\param token_name The name of the token
\return 1 if the token is attached, 0 if not found
*/
int hsm_token_attached(const hsm_ctx_t *ctx, const char *token_name);

/* a few debug functions for applications */
void hsm_print_session(hsm_session_t *session);
void hsm_print_ctx(hsm_ctx_t *gctx);
void hsm_print_key(hsm_key_t *key);

#endif /* HSM_H */
