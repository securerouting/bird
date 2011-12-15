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
#include "nest/route.h"

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
 * stored in 'sig_len'.
 *
 * Internally this looks up the certificate and then calls
 * bgpsec_sign_data_with_cert(), defined below.
 *
 * Returns: The length of the signature actually created, or -1 on error.
 */
int bgpsec_sign_data_with_fp(byte *octets, int octets_len, xxxunknown *fp,
                             int algorithm, byte *signature, int in_sig_len);

/*
 * Signs a blob of octets in 'octets' with the certificate 'cert' using
 * the algorithm indicated by 'signature_algorithm'.  The resulting signature
 * is placed in the pre-allocated 'signature' block, who's
 * pre-allocated length bust be stored in 'sig_len'.
 *
 * Returns: The length of the signature actually created, or -1 on error.
 */
int bgpsec_sign_data_with_cert(byte *octets, int octets_len,
                               xxxunknown *cert, int signature_algorithm,
                               byte *signature, int sig_len);



#endif 
