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

#include "validate.h"

#define ERROR(errmsg) do { fprintf(stderr, "Error: %s\n", errmsg); return(BGPSEC_FAILURE); } while(0);


int bgpsec_sign_data_with_bin_ski(byte *octets, int octets_len,
                                  char *ski, size_t ski_len,
                                  int algorithm, byte *signature,
                                  int in_signature_len) {
    bgpsec_key_data key;

    if (BGPSEC_SUCCESS !=
        bgpsec_load_key_from_bin_ski(ski, ski_len,
                                     &key, BGPSEC_DEFAULT_CURVE, 1)) {
        ERROR("");
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
        ERROR("");
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
        ERROR("");
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
        ERROR("");
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
            ERROR("failed to extract the private key");
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
        ERROR("failed to extract the public key");
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
    EC_POINT *publicKey;
    FILE *loadFrom;
    size_t len;
    int ret;
    EC_GROUP *ecGroup;
    

    filenamebuf[sizeof(filenamebuf)-1] = '\0';

    /* create the basic key structure */
    key_data->ecdsa_key = EC_KEY_new_by_curve_name(curveId);

    if (loadPrivateKey) {
        /* load the private key */
        snprintf(filenamebuf, sizeof(filenamebuf)-1,
                 "%s.private", filePrefix);
        loadFrom = fopen(filenamebuf, "r");
        if (loadFrom == NULL)
            ERROR("failed to open the private key file");

        len = fread(octetBuffer, sizeof(octetBuffer), 1, loadFrom);
        if (len < 0)
            ERROR("failed to read the private key file");

        fclose(loadFrom);

        BN_hex2bn(&privateData, octetBuffer);
        EC_KEY_set_private_key(key_data->ecdsa_key, privateData);
    }

    /* load the public key */
    snprintf(filenamebuf, sizeof(filenamebuf)-1, "%s.public", filePrefix);

    /* find the size of the file */
    
    loadFrom = fopen(filenamebuf, "r"); 
    len = bgpsec_get_filesize(filenamebuf);
    if (fread(octetBuffer, len, 1, loadFrom) != 1) {
        ERROR("failed to read the public key file");
    }
    fclose(loadFrom);

    /* XXX: leaks the curve object */
    ecGroup = EC_GROUP_new_by_curve_name(curveId);
    if (NULL == ecGroup) {
        ERROR("Failed to create a EC_GROUP");
    }
    publicKey = EC_POINT_new(ecGroup);
    if (!publicKey) {
        ERROR("failed to create a new EC_POINT");
    }
    EC_POINT_oct2point(ecGroup, publicKey, octetBuffer, len, NULL);

    ret = EC_KEY_set_public_key(key_data->ecdsa_key, publicKey);
    if (0 == publicKey) {
        ERROR("failed to load the public key");
    }

    if (0 == EC_KEY_check_key(key_data->ecdsa_key)) {
        ERROR("newly loaded EC key not ok");
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
            ERROR("invalid incoming SKI length");
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
