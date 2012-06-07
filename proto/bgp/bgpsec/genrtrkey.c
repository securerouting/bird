#include <stdio.h>

#include "validate.h"

#define ERROR(errmsg) do { fprintf(stderr, "Error: %s\n", errmsg); exit(1); } while(0);

int main(int argc, char **argv) {
    byte signature[1024];
    int  signature_len = sizeof(signature);
    bgpsec_key_data key_data;
    xxxunknown *fp = 0;
    int  signature_algorithms[] = { BGPSEC_ALGORITHM_SHA256_ECDSA_P_256, -1 };
    byte data_to_sign[] = { 1,2,3,4,5,6,7,8 };
    char filenamebuf[MAX_PATH_LEN];
    char octetBuffer[4096];
    FILE *saveTo;
    const EC_POINT *publicKey;
    size_t len;
    int curveId;

    if (argc < 2) {
        ERROR("Usage: genrtrkey FILEPREFIX");
    }

    bgpsec_key_data key_data;

    const BIGNUM *keydata;

    switch (signature_algorithms[algorithm_count]) {
    case BGPSEC_ALGORITHM_SHA256_ECDSA_P_256:
        curveId = NID_secp192k1;
        key_data.ecdsa_key = EC_KEY_new_by_curve_name(curveId);
        /* XXX: need the right group */
        // key_data.ecdsa_key->group = EC_GROUP_new_by_nid(NID_secp192k1);
        EC_KEY_generate_key(key_data.ecdsa_key);
        break;
    default:
        printf("key setup failure; unknown algorithm\n");
        exit(42);
    }

    if (NULL == key_data.ecdsa_key) {
        ERROR("failed to generate a key structure");
    }

    filenamebuf[sizeof(filenamebuf)-1] = '\0';


    /* extract the private key */
    keydata = EC_KEY_get0_private_key(key_data.ecdsa_key);
    if (NULL == keydata) {
        ERROR("failed to extract the private key");
    }

    /* save the private key */
    snprintf(filenamebuf, sizeof(filenamebuf)-1, "%s.private", argv[1]);
    saveTo = fopen(filenamebuf, "w");
    BN_print_fp(saveTo, keydata);
    fclose(saveTo);

    /* extract the public key */
    publicKey = EC_KEY_get0_private_key(key_data.ecdsa_key);
    if (NULL == publicKey) {
        ERROR("failed to extract the public key");
    }

    /* save the public key */
    len = EC_POINT_point2oct(EC_GROUP_new_by_curve_name(curveId),
                             NID_secp192k1, publicKey,
                             POINT_CONVERSION_COMPRESSED,
                             octetBuffer, sizeof(octetBuffer), NULL);

    snprintf(filenamebuf, sizeof(filenamebuf)-1, "%s.public", argv[1]);
    saveTo = fopen(filenamebuf, "w");
    fwrite(octetBuffer, len, 1, saveTo);
    fclose(saveTo);

    return 0;
}
