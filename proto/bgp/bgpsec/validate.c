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
        length += i2d_ASN1_INTEGER(as, &cp);

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
    int  result;
    ECDSA_SIG *ecdsa_signature = ECDSA_SIG_new();
    ASN1_INTEGER *ar = NULL;
    ASN1_INTEGER *as = NULL;
    const unsigned char *cp = signature+2;
    
    /* extract the signature object from the DER encoded structure */
    d2i_ASN1_INTEGER(&ar, &cp, signature_len - (cp - signature));
    d2i_ASN1_INTEGER(&as, &cp, signature_len - (cp - signature));
    ASN1_INTEGER_to_BN(ar, ecdsa_signature->r);
    ASN1_INTEGER_to_BN(as, ecdsa_signature->s);

    result = ECDSA_do_verify(octets, octets_len, ecdsa_signature,
                             cert.ecdsa_key);

    if (result == 1)
        return BGPSEC_SIGNATURE_MATCH;

    return BGPSEC_SIGNATURE_MISMATCH;
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
