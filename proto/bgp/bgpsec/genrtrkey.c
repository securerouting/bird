#include <stdio.h>
#include <sys/param.h>

#include "validate.h"

#define ERROR(errmsg) do { fprintf(stderr, "Error: %s\n", errmsg); exit(1); } while(0);

int main(int argc, char **argv) {
    byte signature[1024];
    int  signature_len = sizeof(signature);
    bgpsec_key_data key_data;
    char *ski = 0;
    int  signature_algorithm = BGPSEC_ALGORITHM_SHA256_ECDSA_P_256;
    char filenamebuf[MAXPATHLEN];
    char octetBuffer[4096];
    FILE *saveTo;
    const EC_POINT *publicKey;
    size_t len;
    int curveId;
    const BIGNUM *keydata;

    if (argc < 2) {
        ERROR("Usage: genrtrkey FILEPREFIX");
    }

    switch (signature_algorithm) {
    case BGPSEC_ALGORITHM_SHA256_ECDSA_P_256:
        /* XXX: need the right group */
        curveId = BGPSEC_DEFAULT_CURVE;
        key_data.ecdsa_key = EC_KEY_new_by_curve_name(curveId);
        EC_KEY_generate_key(key_data.ecdsa_key);
        break;
    default:
        printf("key setup failure; unknown algorithm\n");
        exit(42);
    }

    return bgpsec_save_key(argv[1], &key_data, curveId, 1);
}
