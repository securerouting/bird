/*
 *	bgpsec: validation functions
 *
 *	Copyright 2011 SPARTA, Inc.
 *
 *	XXX: license
 *
 */

#include "validate.h"

int bgpsec_sign_data_with_fp(byte *octets, int octets_len, xxxunknown *fp,
                             int algorithm, byte *signature, int in_sig_len) {

    /* XXX: find proper cert */
    return bgpsec_sign_data_with_cert(octets, octets_len, XXXNULL,
                                      algorithm, signature, in_sig_len);
                                      
}

int bgpsec_sign_data_with_cert(byte *octets, int octets_len,
                               xxxunknown *cert, int signature_algorithm,
                               byte *signature, int sig_len) {
    switch (signature_algorithm) {
    case BGPSEC_ALGORITHM_SHA256_ECDSA_P_256:
        /* XXX: create signature and return it's length */
        break;
    default:
        return -1;
    }
    return -1;
}

