/*
 *	bgpsec: validation functions
 *
 *	Copyright 2011 SPARTA, Inc.
 *
 *	XXX: license
 *
 */

#ifndef _BIRD_VALIDATE_H_
#define _BIRD_VALIDATE_H_

#include <stdint.h>
#include <openssl/x509.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>

#include "nest/route.h"

/* XXX: these need to be configurable in the bird config file instead */
#define KEY_REPO_PATH "/tmp/bgpsec-keys"

/* XXX: temporary curve to use; needs the real one */
/* the real one should be NID_secp256r1 (the 'r' is the difference) */
/* but doesn't exist in openssl at this point? */
#define BGPSEC_DEFAULT_CURVE NID_secp256k1

/*
 * structure to store keying data in; we create a generic union until
 * we know what type of key we actually want to make the routines generic
 */
typedef union {
   X509     *x509_public;
   EVP_PKEY *x509_private;

   EC_KEY   *ecdsa_key;
} bgpsec_key_data;

/* Generic error codes */
#define BGPSEC_SUCCESS 0
#define BGPSEC_ERROR   1


/* These match the defined algorithm bytes from the protocol definition */

/* Algorithm #:         1
 * Digest algorithm:    SHA-256
 * Signature algorithm: ECDSA P-256
 */
/* XXX: IANA has yet to assign this number; 1 is a logical guess */
/* XXX: Definiton in draft-turner-sidr-bgpsec-algs-00.txt */
#define BGPSEC_ALGORITHM_SHA256_ECDSA_P_256 1

/* XXX: To be removed before final push: defines only objects we don't
   have a structure for yet; anything that references this isn't
   complete yet */
typedef void xxxunknown;
#define XXXNULL NULL

/*
 * Signs a blob of octets in 'octets' with the certificate found using
 * the 'subject_key_ident' using the algorithm indicated by
 * 'signature_algorithm'.  The resulting signature is placed in the
 * pre-allocated 'signature' block, who's pre-allocated length bust be
 * stored in 'signature_len'.
 *
 * Internally this looks up the certificate and then calls
 * bgpsec_sign_data_with_cert(), defined below.
 *
 * Returns: The length of the signature actually created, or -1 on error.
 */
int bgpsec_sign_data_with_ski(byte *octets, int octets_len, xxxunknown *ski,
                             int algorithm, byte *signature,
                             int in_signature_len);

/*
 * Signs a blob of octets in 'octets' with the certificate 'cert' using
 * the algorithm indicated by 'signature_algorithm'.  The resulting signature
 * is placed in the pre-allocated 'signature' block, who's
 * pre-allocated length bust be stored in 'signature_len'.
 *
 * Returns: The length of the signature actually created, or -1 on error.
 */
int bgpsec_sign_data_with_cert(byte *octets, int octets_len,
                               bgpsec_key_data cert,
                               int signature_algorithm,
                               byte *signature, int signature_len);


#define BGPSEC_SIGNATURE_MATCH    0
#define BGPSEC_SIGNATURE_ERROR    1
#define BGPSEC_SIGNATURE_MISMATCH 2
/*
 * Validates a signature on a block and returns an error code if the
 * signature dosen't match.  The data to check the signature for
 * should be in 'octets' with length 'octets_len', and the certificate
 * to check with should be in 'cert' using algorithm
 * 'signature_algorithm'.  The signature from the bgp packet should
 * should be in 'signature' with length 'signature_len'.
 *
 * Returns:
 *   Success: BGPSEC_SIGNATURE_MATCH
 *   Failure: BGPSEC_SIGNATURE_ERROR
 *            BGPSEC_SIGNATURE_MISMATCH
 */
int bgpsec_verify_signature_with_cert(byte *octets, int octets_len,
                                      bgpsec_key_data cert,
                                      int signature_algorithm,
                                      byte *signature, int signature_len);

int bgpsec_verify_signature_with_ski(byte *octets, int octets_len,
                                    xxxunknown *ski, int signature_algorithm,
                                    byte *signature, int signature_len);

/* --- key manipulation routines (loading, unloading, etc) --- */

/*
 * Saves a ECDSA key to a file name with two suffixes, ".private" and
 * ".pub".  The curveId should be the eliptic curve specifier to use.
 * If savePrivateKey is 0, the private key won't be saved.
 *
 * Returns:
 *   Success: BGPSEC_SUCCESS
 *   Failure: BGPSEC_FAILURE
 */
int bgpsec_save_key(const char *filePrefix, bgpsec_key_data *key_data,
                    int curveId, int savePrivateKey);

/*
 * Loads a ECDSA key from a file name with two suffixes, ".private" and
 * ".pub".  The curveId should be the eliptic curve specifier to use.
 * If loadPrivateKey is 0, the private key won't be loaded and the key
 * will only be useful for verifying signatures and not for signing.
 *
 * Returns:
 *   Success: BGPSEC_SUCCESS
 *   Failure: BGPSEC_FAILURE
 */
int bgpsec_load_key(const char *filePrefix, bgpsec_key_data *key_data,
                    int curveId, int loadPrivateKey);

#endif 
