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
                             int algorithm, byte *signature,
                             int in_signature_len) {

    /* XXX: find proper cert */
    return bgpsec_sign_data_with_cert(octets, octets_len, XXXNULL,
                                      algorithm, signature, in_signature_len);
                                      
}

int bgpsec_sign_data_with_cert(byte *octets, int octets_len,
                               xxxunknown *cert, int signature_algorithm,
                               byte *signature, int signature_len) {
    switch (signature_algorithm) {
    case BGPSEC_ALGORITHM_SHA256_ECDSA_P_256:
        /* XXX: create signature and return it's length */
        break;
    default:
        return -1;
    }
    return -1;
}

int bgpsec_verify_signature_with_cert(byte *octets, int octets_len,
                                      xxxunknown *cert, int signature_algorithm,
                                      byte *signature, int signature_len) {
    byte generated_signature[1024];
    int  generated_signature_len;

    /* first we need to sign with the expected certificate and see if
       we can generate an identical signature */
    generated_signature_len = 
        bgpsec_sign_data_with_cert(octets, octets_len, cert, signature_algorithm,
                                   generated_signature,
                                   sizeof(generated_signature));

    if (generated_signature_len <= 0)
        return BGPSEC_SIGNATURE_ERROR;
    
    if (generated_signature_len != signature_len)
        return BGPSEC_SIGNATURE_MISMATCH;

    if (memcmp(signature, generated_signature, generated_signature_len) != 0)
        return BGPSEC_SIGNATURE_MISMATCH;

    return BGPSEC_SIGNATURE_MATCH;
}
    
int bgpsec_verify_signature_with_fp(byte *octets, int octets_len,
                                    xxxunknown *fp, int signature_algorithm,
                                    byte *signature, int signature_len) {
    /* XXX: find proper cert */
    return bgpsec_verify_signature_with_cert(octets, octets_len,
                                             XXXNULL, signature_algorithm,
                                             signature, signature_len);
}
