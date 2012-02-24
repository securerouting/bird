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

#include "nest/route.h"

typedef union {
   X509     *x509_public;
   EVP_PKEY *x509_private;

   EC_KEY   *ecdsa_key;
} bgpsec_key_data;

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
int bgpsec_sign_data_with_fp(byte *octets, int octets_len, xxxunknown *fp,
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

int bgpsec_verify_signature_with_fp(byte *octets, int octets_len,
                                    xxxunknown *fp, int signature_algorithm,
                                    byte *signature, int signature_len);

#endif 
