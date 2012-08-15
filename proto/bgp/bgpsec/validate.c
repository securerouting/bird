/*
 *	bgpsec: validation functions
 *
 *	Copyright 2011 SPARTA, Inc.
 *
 *	XXX: license
 *
 */

#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <openssl/bn.h>
#include "validate.h"

/* XXX: this is copied from openssl's (1.0.1c) ec_lcl.h which could change
        But they don't provide an API to get to the private key section
        choices: keep going as is, or try to switch to X509 cert signing
        functions
*/
struct ec_key_st {
        int version;

        EC_GROUP *group;

        EC_POINT *pub_key;
        BIGNUM   *priv_key;

        unsigned int enc_flag;
        point_conversion_form_t conv_form;

        int     references;
        int     flags;

   void *method_data; /* was EC_EXTRA_DATA */
} /* EC_KEY */;

#ifdef LOG_TO_STDERR
#define ERROR(errmsg) do { fprintf(stderr, "Error: %s\n", errmsg); return(BGPSEC_FAILURE); } while(0);
#else
#define ERROR(errmsg) do { log(L_ERR, "Error: %s\n", errmsg); return(BGPSEC_FAILURE); } while(0);
#endif

int bgpsec_sign_data_with_bin_ski(byte *octets, int octets_len,
                                  char *ski, size_t ski_len,
                                  int algorithm, byte *signature,
                                  int in_signature_len) {
    bgpsec_key_data key;

    if (BGPSEC_SUCCESS !=
        bgpsec_load_key_from_bin_ski(ski, ski_len,
                                     &key, BGPSEC_DEFAULT_CURVE, 1)) {
        ERROR("Failed to load a bgpsec key from a binary SKI");
    }


    return bgpsec_sign_data_with_cert(octets, octets_len, key,
                                      algorithm, signature, in_signature_len);
}

int bgpsec_sign_data_with_ascii_ski(byte *octets, int octets_len,
                                    char *ski, size_t ski_len,
                                    int algorithm, byte *signature,
                                    int in_signature_len) {
    bgpsec_key_data key;

    if (BGPSEC_SUCCESS !=
        bgpsec_load_key_from_ascii_ski(ski, ski_len,
                                       &key, BGPSEC_DEFAULT_CURVE, 1)) {
        ERROR("Failed to load a bgpsec key from an ascii SKI");
    }


    return bgpsec_sign_data_with_cert(octets, octets_len, key,
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
    
int bgpsec_verify_signature_with_ascii_ski(byte *octets, int octets_len,
                                           char *ski, size_t ski_len,
                                           int signature_algorithm,
                                           byte *signature, int signature_len) {
    bgpsec_key_data key;

    if (BGPSEC_SUCCESS !=
        bgpsec_load_key_from_ascii_ski(ski, ski_len,
                                       &key, BGPSEC_DEFAULT_CURVE, 0)) {
        ERROR("Failed to load a bgpsec key from an ascii SKI");
    }


    return bgpsec_verify_signature_with_cert(octets, octets_len, key,
                                             signature_algorithm,
                                             signature, signature_len);
}

int bgpsec_verify_signature_with_bin_ski(byte *octets, int octets_len,
                                           char *ski, size_t ski_len,
                                           int signature_algorithm,
                                           byte *signature, int signature_len) {
    bgpsec_key_data key;

    if (BGPSEC_SUCCESS !=
        bgpsec_load_key_from_bin_ski(ski, ski_len,
                                     &key, BGPSEC_DEFAULT_CURVE, 0)) {
        ERROR("Failed to load a bgpsec key from an binary SKI");
    }


    return bgpsec_verify_signature_with_cert(octets, octets_len, key,
                                             signature_algorithm,
                                             signature, signature_len);
}

int bgpsec_save_key(const char *filePrefix, bgpsec_key_data *key_data,
                    int curveId, int savePrivateKey) {
    char filenamebuf[MAXPATHLEN];
    char octetBuffer[4096];
    const BIGNUM *keydata;
    const EC_POINT *publicKey;
    FILE *saveTo;
    size_t len;
    mode_t oldUmask;

    oldUmask = umask(066);

    filenamebuf[sizeof(filenamebuf)-1] = '\0';

    if (savePrivateKey) {
        /* extract the private key */
        keydata = EC_KEY_get0_private_key(key_data->ecdsa_key);
        if (NULL == keydata) {
            umask(oldUmask);
            ERROR("Failed to extract the private key");
        }

        /* save the private key */
        snprintf(filenamebuf, sizeof(filenamebuf)-1,
                 "%s.private", filePrefix);
        saveTo = fopen(filenamebuf, "w");
        BN_print_fp(saveTo, keydata);
        fclose(saveTo);
    }

    /* extract the public key */
    publicKey = EC_KEY_get0_public_key(key_data->ecdsa_key);
    if (NULL == publicKey) {
        umask(oldUmask);
        ERROR("Failed to extract the public key");
    }

    /* save the public key */
    /* XXX: leaks the curve object */
    len = EC_POINT_point2oct(EC_GROUP_new_by_curve_name(curveId),
                             publicKey, POINT_CONVERSION_COMPRESSED,
                             octetBuffer, sizeof(octetBuffer), NULL);

    snprintf(filenamebuf, sizeof(filenamebuf)-1, "%s.public", filePrefix);
    saveTo = fopen(filenamebuf, "w");
    fwrite(octetBuffer, len, 1, saveTo);
    fclose(saveTo);

    umask(oldUmask);

    return BGPSEC_SUCCESS;
}

int bgpsec_load_key(const char *filePrefix, bgpsec_key_data *key_data,
                    int curveId, int loadPrivateKey) {
    char filenamebuf[MAXPATHLEN];
    char octetBuffer[4096];
    BIGNUM *privateData = NULL;
    EC_POINT *publicKey = NULL;
    FILE *loadFrom = NULL;
    size_t len;
    int ret;
    EC_GROUP *ecGroup;
    unsigned char *cbuf = NULL;
    
    BIO            *input_bio = NULL;
    EVP_PKEY       *private_key = NULL;
    X509           *x509_cert = NULL;

    filenamebuf[sizeof(filenamebuf)-1] = '\0';

    switch (curveId) {
    case BGPSEC_ALGORITHM_SHA256_ECDSA_P_256:
        curveId = BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256;
        break;
    default:
        ERROR("Unkown curve ID");
    }

    /* create the basic key structure */
    key_data->ecdsa_key = EC_KEY_new_by_curve_name(curveId);

    /* load the public key */
    input_bio = BIO_new(BIO_s_file());
    if (NULL == input_bio) {
        ERROR("Error creating OpenSSL file BIO");
    }

    snprintf(filenamebuf, sizeof(filenamebuf)-1, "%s.pub", filePrefix);

    if (BIO_read_filename(input_bio, filenamebuf) <=0) {
        BIO_vfree(input_bio);
        ERROR("Error reading public certificate from BIO");
    }

/* #define BGPSEC_USE_PEM_CERTS 1 */
#ifdef BGPSEC_USE_PEM_CERTS
    x509_cert = PEM_read_bio_X509_AUX(input_bio, NULL, NULL, NULL);
    if (NULL == x509_cert) {
        ERROR("Failed to load the x509 cert");
    }
#else /* following is !BGPSEC_USE_PEM_CERTS, which is DER */
    x509_cert = X509_new();
    if (NULL == x509_cert) {
        ERROR("Failed to create an empty x509 certificate");
    }
    x509_cert = d2i_X509_bio(input_bio, &x509_cert); /* DER/ASN1 */
    if (NULL == x509_cert) {
        ERROR("Failed to load the (DER) x509 cert");
    }
    BIO_reset(input_bio); /* XXX: actually needed? */
#endif /* BGPSEC_USE_PEM_CERTS */




    /* XXX: key is in x509_cert->cert_info->key */
    /* type:
               X509_PUBKEY *key;
               ->
                         ASN1_BIT_STRING *public_key;
                         EVP_PKEY *pkey;
    */


    /* ...cert_info->key->pkey->ec */

    /* XXX: leaks the curve object */
    ecGroup = EC_GROUP_new_by_curve_name(curveId);
    if (NULL == ecGroup) {
        ERROR("Failed to create a EC_GROUP");
    }
    publicKey = EC_POINT_new(ecGroup);
    if (!publicKey) {
        ERROR("Failed to create a new EC_POINT");
    }

    memcpy(octetBuffer,
           ASN1_STRING_data(x509_cert->cert_info->key->public_key),
           len = ASN1_STRING_length(x509_cert->cert_info->key->public_key));

    EC_POINT_oct2point(ecGroup, publicKey, octetBuffer, len, NULL);

    ret = EC_KEY_set_public_key(key_data->ecdsa_key, publicKey);
    if (0 == publicKey) {
        ERROR("Failed to load the public key");
    }

    if (0 == EC_KEY_check_key(key_data->ecdsa_key)) {
        ERROR("Newly loaded public EC key not ok");
    }

    if (loadPrivateKey) {
        /* load the private key */
        snprintf(filenamebuf, sizeof(filenamebuf)-1,
                 "%s.private", filePrefix);

        input_bio = BIO_new(BIO_s_file());
        if (NULL == input_bio) {
            ERROR("Failed to load a private key");
        }

        if (BIO_read_filename(input_bio, filenamebuf) <=0) {
            BIO_vfree(input_bio);
            ERROR("Error reading a private key from a BIO");
        }

        private_key = PEM_read_bio_PrivateKey(input_bio, NULL, NULL, NULL);
        if (NULL == private_key)
            ERROR("Failed to load the private key from the bio");

        BIO_vfree(input_bio);
        
        if (!EC_KEY_set_private_key(key_data->ecdsa_key,
                                    private_key->pkey.ec->priv_key))
            ERROR("Failed to set the private key into the EC key object");

        if (0 == EC_KEY_check_key(key_data->ecdsa_key)) {
            ERROR("Newly loaded public/private EC key not ok");
        }
    }

    return BGPSEC_SUCCESS;
}

int bgpsec_load_key_from_ascii_ski(const char *ski, size_t ski_len,
                                   bgpsec_key_data *key_data,
                                   int curveId, int loadPrivateKey) {
    char filenamebuf[MAXPATHLEN];
    char octetBuffer[4096];
    BIGNUM *privateData = NULL;
    EC_POINT *publicKey;
    FILE *loadFrom;
    size_t len;
    int ret;
    EC_GROUP *ecGroup;

    /* just in case buffers */
    char ascii_buf[MAXPATHLEN];

    /* verify that the incoming data is appropriate */

    if (ski[ski_len]-1 != 0) {
        /* the ski isn't null terminated, so we'll need to replace it
           with a string that is */
        if (ski_len > sizeof(ascii_buf)) {
            ERROR("Invalid incoming SKI length");
        }
        memcpy(ascii_buf, ski, ski_len);
        ascii_buf[ski_len] = '\0';
        ski = ascii_buf;
    }

    /* XXX: build a hash tree directory structure instead? */
    filenamebuf[sizeof(filenamebuf)-1] = '\0';
    snprintf(filenamebuf, sizeof(filenamebuf)-1, "%s/%s",
             KEY_REPO_PATH, ski);

    return bgpsec_load_key(filenamebuf, key_data, curveId, loadPrivateKey);
}

int bgpsec_load_key_from_bin_ski(const char *ski, size_t ski_len,
                                 bgpsec_key_data *key_data,
                                 int curveId, int loadPrivateKey) {
    char filenamebuf[MAXPATHLEN];
    char octetBuffer[4096];
    BIGNUM *privateData = NULL;
    EC_POINT *publicKey;
    FILE *loadFrom;
    size_t len;
    int ret;
    EC_GROUP *ecGroup;

    char ascii_ski_buf[ski_len * 2 + 1], *cp;
    int i;
    
    cp = ascii_ski_buf;
    for(i = 0; i < ski_len; i++) {
        sprintf(cp, "%02x", ski[i]);
        cp += 2;
    }

    /* null terminate it */
    ascii_ski_buf[sizeof(ascii_ski_buf)-1] = '\0';

    return
        bgpsec_load_key_from_ascii_ski(ascii_ski_buf, sizeof(ascii_ski_buf),
                                       key_data, curveId, loadPrivateKey);
}

int bgpsec_get_filesize(const char *filename) {
    struct stat statbuf;

    stat(filename, &statbuf);
    return statbuf.st_size;
}
