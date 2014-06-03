/*
 *	bgpsec: validation functions
 *
 *
 *      Parsons, Inc.
 *      (c) 2013-2013
 *
 *	Code can be used under either license:
 *      - Freely distributed and used under the terms of the GNU GPLv2.
 *      - Freely distributed and used under a BSD license, See README.bgpsec.
 */

#ifndef _BIRD_VALIDATE_H_
#define _BIRD_VALIDATE_H_

#include <stdint.h>
#include <openssl/evp.h>

#include "nest/route.h"
#include "../bgp.h"

/* XXX: these need to be configurable in the bird config file instead */
#define DEFAULT_KEY_REPO_PATH "/usr/share/bird/bgpsec-keys"

/*
 * Structure to store keying data in. This used to be a union, but
 * since we should be using EVP_PKEY everywhere it's now just a wrapper.
 */
typedef struct {
   EVP_PKEY *pkey;
} bgpsec_key_data;

/* Generic error codes */
#define BGPSEC_SUCCESS 0
#define BGPSEC_FAILURE -1


/* These match the defined algorithm bytes from the protocol definition */

/* Algorithm #:         1
 * Digest algorithm:    SHA-256
 * Signature algorithm: ECDSA P-256
 */
/* XXX: IANA has yet to assign this number; 1 is a logical guess */
/* XXX: Definiton in draft-turner-sidr-bgpsec-algs-00.txt */
#define BGPSEC_ALGORITHM_SHA256_ECDSA_P_256 1

/* P-256, frequently called secp256r1 in the specs and NIST
   documentation, is called "prime256v1" in OpenSSL, which has a
   NID specifier of NID_X9_62_prime256v1. */
//#define BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256 NID_secp256k1
#define BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256 NID_X9_62_prime256v1

#define BGPSEC_DEFAULT_CURVE BGPSEC_ALGORITHM_SHA256_ECDSA_P_256

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
int bgpsec_sign_data_with_ascii_ski(struct bgp_config *conf,
                                    byte *octets, int octets_len,
                                    char *ski, size_t ski_len,
                                    int algorithm, byte *signature,
                                    int in_signature_len);

int bgpsec_sign_data_with_bin_ski(struct bgp_config *conf,
                                  byte *octets, int octets_len,
                                  char *ski, size_t ski_len,
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
int bgpsec_sign_data_with_cert(struct bgp_config *conf,
                               byte *octets, int octets_len,
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
int bgpsec_verify_signature_with_cert(struct bgp_config *conf,
                                      byte *octets, int octets_len,
                                      bgpsec_key_data cert,
                                      int signature_algorithm,
                                      byte *signature, int signature_len);

/* verifies a signature when passed an ascii SKI */
int bgpsec_verify_signature_with_ascii_ski(struct bgp_config *conf,
                                           byte *octets, int octets_len,
                                           char *ski, size_t ski_len,
                                           int signature_algorithm,
                                           byte *signature, int signature_len);

/* verifies a signature when passed a binary SKI
   (internally, this is a wrapper around the above function and merely
   prints the binary to an hex-encoded ascii first) */
int bgpsec_verify_signature_with_bin_ski(struct bgp_config *conf,
                                         byte *octets, int octets_len,
                                         char *ski, size_t ski_len,
                                         int signature_algorithm,
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
int bgpsec_save_key(struct bgp_config *conf,
                    const char *filePrefix, bgpsec_key_data *key_data,
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
int bgpsec_load_key(struct bgp_config *conf,
                    const char *filePrefix, bgpsec_key_data *key_data,
                    int curveId, int loadPrivateKey);

char *generate_ski_filename(char *filenamebuf, size_t filenamebufLen,
                            const char *rootPath,
                            const char *ski, size_t skiLen);

#endif 
