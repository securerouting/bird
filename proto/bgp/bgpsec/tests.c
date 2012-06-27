#include "validate.h"

#define RESULT(test, is_success) {                                      \
        printf("%7.7s: ", ((is_success) ? "ok" : "not ok"));             \
        printf test;                                                    \
        printf("\n");                                                   \
        if (is_success) good++;                                         \
        else bad++;                                                     \
    }

int main(int argc, char **argv) {
    byte signature[1024];
    int  signature_len = sizeof(signature);
    char strBuffer[1024];
    bgpsec_key_data key_data;
    char *ski = "fakefingerprint";
    int  signature_algorithms[] = { BGPSEC_ALGORITHM_SHA256_ECDSA_P_256, -1 };
    byte data_to_sign[] = { 1,2,3,4,5,6,7,8 };

    BIGNUM newbignum;
    EC_POINT *new_point;

    printf("Testing:\n");

    int good = 0, bad = 0;

    /* test whether we can sign a block of text */
    int algorithm_count = 0;

    int ret;
    int curveId;

    while(signature_algorithms[algorithm_count] > 0) {
        bgpsec_key_data key_data;

        switch (signature_algorithms[algorithm_count]) {
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

        /* generate a signature using a certificate */
        signature_len =
            bgpsec_sign_data_with_cert(data_to_sign, sizeof(data_to_sign),
                                       key_data,
                                       signature_algorithms[algorithm_count],
                                       signature, sizeof(signature));

        RESULT(("cert sign: algorithm %d, signature length (%d) is not negative",
                signature_algorithms[algorithm_count], signature_len),
               signature_len > -1);
        RESULT(("cert sign: algorithm %d, signature length (%d) has at least a byte", signature_algorithms[algorithm_count], signature_len), signature_len > 0);

        /* save the existing key */
        ret = bgpsec_save_key("/tmp/testkey", &key_data, curveId, 1);
        RESULT(("cert sign: saving key function returned: %d (should be %d)",
                ret, BGPSEC_SUCCESS), ret == BGPSEC_SUCCESS);

        ret = bgpsec_save_key("/tmp/testkey-public", &key_data, curveId, 0);
        RESULT(("cert sign: saving pubkey function returned: %d (should be %d)",
                ret, BGPSEC_SUCCESS), ret == BGPSEC_SUCCESS);

        /* save the key as a fake fingerprint */
        mkdir(KEY_REPO_PATH, 0777);
        strBuffer[sizeof(strBuffer)-1] = '\0';
        snprintf(strBuffer, sizeof(strBuffer)-1, "%s/%s",
                 KEY_REPO_PATH, ski);
        ret = bgpsec_save_key(strBuffer, &key_data, curveId, 1);

        /* modify the private key so it can't be part of the verification */
        BN_init(&newbignum);
        EC_KEY_set_private_key(key_data.ecdsa_key, &newbignum);

        /* verify that the signature matches */
        ret = bgpsec_verify_signature_with_cert(data_to_sign,
                                                sizeof(data_to_sign),
                                                key_data,
                                                signature_algorithms[algorithm_count],
                                                signature, signature_len);
        RESULT(("cert sign: verify signature result: %d (should be %d)",
                ret, BGPSEC_SIGNATURE_MATCH),
               ret == BGPSEC_SIGNATURE_MATCH);

        /* modify the public key so it can't be part of the verification */
        /* (which should make the verification fail now) */
        new_point = EC_POINT_new(EC_GROUP_new_by_curve_name(curveId));
        EC_KEY_set_public_key(key_data.ecdsa_key, new_point);
        EC_POINT_free(new_point);

        /* verify that the signature no longer matches */
        ret = bgpsec_verify_signature_with_cert(data_to_sign,
                                                sizeof(data_to_sign),
                                                key_data,
                                                signature_algorithms[algorithm_count],
                                                signature, signature_len);
        RESULT(("cert sign: verify signature fail result: %d (should be %d)",
                ret, BGPSEC_SIGNATURE_MISMATCH),
               ret == BGPSEC_SIGNATURE_MISMATCH);
        
        /* completely get rid of the current key */
        EC_KEY_free(key_data.ecdsa_key);
        key_data.ecdsa_key = NULL;

        /* now reload the key from the files and use them to verify it */
        ret = bgpsec_load_key("/tmp/testkey", &key_data, curveId, 1);
        RESULT(("cert sign: loading key function returned: %d (should be %d)",
                ret, BGPSEC_SUCCESS), ret == BGPSEC_SUCCESS);
        
        /* verify that the signature matches again with the loaded key */
        ret = bgpsec_verify_signature_with_cert(data_to_sign,
                                                sizeof(data_to_sign),
                                                key_data,
                                                signature_algorithms[algorithm_count],
                                                signature, signature_len);
        RESULT(("cert sign: verify signature result: %d (should be %d)",
                ret, BGPSEC_SIGNATURE_MATCH),
               ret == BGPSEC_SIGNATURE_MATCH);

        /* completely get rid of the current key */
        EC_KEY_free(key_data.ecdsa_key);
        key_data.ecdsa_key = NULL;

        /* now reload just the public part of the key and test just it */
        ret = bgpsec_load_key("/tmp/testkey-public", &key_data, curveId, 0);
        RESULT(("cert sign: loading public key function returned: %d (should be %d)",
                ret, BGPSEC_SUCCESS), ret == BGPSEC_SUCCESS);
        
        /* verify that the signature matches again with the public key */
        ret = bgpsec_verify_signature_with_cert(data_to_sign,
                                                sizeof(data_to_sign),
                                                key_data,
                                                signature_algorithms[algorithm_count],
                                                signature, signature_len);
        RESULT(("cert sign: verify (pub) signature result: %d (should be %d)",
                ret, BGPSEC_SIGNATURE_MATCH),
               ret == BGPSEC_SIGNATURE_MATCH);
        
        /* generate a signature using a fingerprint */
        /* XXX: set test directory to search for matching ski->certs */
        signature_len =
            bgpsec_sign_data_with_ascii_ski(data_to_sign,
                                            sizeof(data_to_sign),
                                            ski, strlen(ski)+1,
                                            signature_algorithms[algorithm_count],
                                            signature, sizeof(signature));

        RESULT(("ski sign:   algorithm %d, signature length (%d) is not negative",
                signature_algorithms[algorithm_count], signature_len),
               signature_len > -1);
        RESULT(("ski sign:   algorithm %d, signature length (%d) has at least a byte", signature_algorithms[algorithm_count], signature_len), signature_len > 0);
        

        /* verify that the signature matches */
        ret = bgpsec_verify_signature_with_ascii_ski(data_to_sign,
                                                     sizeof(data_to_sign),
                                                     ski, strlen(ski)+1,
                                                     signature_algorithms[algorithm_count],
                                                     signature, sizeof(signature));
        RESULT(("ski sign:   verify signature result: %d (should be %d)",
                ret, BGPSEC_SIGNATURE_MATCH),
               ret == BGPSEC_SIGNATURE_MATCH);

        /* move on to the next algorithm */
        algorithm_count++;
    }
    
    printf("\nResults:\n");
    printf("  Good: %d\n", good);
    printf("   Bad: %d\n", bad);

    return 0;
}
