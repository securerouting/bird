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

    bgpsec_key_data keys;

    /* XXX: find proper cert */
    return bgpsec_sign_data_with_cert(octets, octets_len, keys,
                                      algorithm, signature, in_signature_len);
                                      
}

int bgpsec_sign_data_with_cert(byte *octets, int octets_len,
                               bgpsec_key_data cert,
                               int signature_algorithm,
                               byte *signature, int signature_len) {

    switch (signature_algorithm) {
    case BGPSEC_ALGORITHM_SHA256_ECDSA_P_256:
    {
        
        /* RFC 3279, section 2.2.3 defines the update signature format:
            Ecdsa-Sig-Value  ::=  SEQUENCE  {
                 r     INTEGER,
                 s     INTEGER  }
        */

        /* XXX: fix memory leaks */

        /* XXX: create signature and return it's length */
        ECDSA_SIG *result;
        ASN1_INTEGER *ar = ASN1_INTEGER_new();
        ASN1_INTEGER *as = ASN1_INTEGER_new();
        unsigned char buffer[1024], *cp = &buffer[2];
        int length = 0;

        buffer[0] = 0x30;

        result = ECDSA_do_sign(octets, octets_len, cert.ecdsa_key);

        BN_to_ASN1_INTEGER(result->r, ar);
        BN_to_ASN1_INTEGER(result->s, as);
        length += i2d_ASN1_INTEGER(ar, &cp);
        length += i2d_ASN1_INTEGER(ar, &cp);

        /* XXX: deal with proper sequences later */
        if (length > 128) {
            return -1;
        }
        
        buffer[1] = length;

        length += 2;
        
        if (length > signature_len) {
            return -1;
        }

        memcpy(signature, buffer, length);
        return length;
    }
    default:
        return -1;
    }
    return -1;
}

int bgpsec_verify_signature_with_cert(byte *octets, int octets_len,
                                      bgpsec_key_data cert,
                                      int signature_algorithm,
                                      byte *signature, int signature_len) {
    byte generated_signature[1024];
    int  generated_signature_len;

    /* first we need to sign with the expected certificate and see if
       we can generate an identical signature */

    /* XXX: don't verify by signing silly; verify by verifying... */
    generated_signature_len = 
        bgpsec_sign_data_with_cert(octets, octets_len, cert,
                                   signature_algorithm, generated_signature,
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
    bgpsec_key_data keys;

    /* XXX: find proper cert */
    return bgpsec_verify_signature_with_cert(octets, octets_len,
                                             keys,
                                             signature_algorithm,
                                             signature, signature_len);
}
