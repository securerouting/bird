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
    xxxunknown *cert;
    xxxunknown *fp;
    int  signature_algorithms[] = { BGPSEC_ALGORITHM_SHA256_ECDSA_P_256, -1 };
    byte data_to_sign[] = { 1,2,3,4,5,6,7,8 };

    printf("Testing:\n");

    int good = 0, bad = 0;

    /* test whether we can sign a block of text */
    int algorithm_count = 0;

    int ret;
    
    while(signature_algorithms[algorithm_count] > 0) {

        /* generate a signature using a certificate */
        signature_len =
            bgpsec_sign_data_with_cert(data_to_sign, sizeof(data_to_sign), cert,
                                       signature_algorithms[algorithm_count],
                                       signature, sizeof(signature));

        RESULT(("cert sign: algorithm %d, signature length (%d) is not negative",
                signature_algorithms[algorithm_count], signature_len),
               signature_len > -1);
        RESULT(("cert sign: algorithm %d, signature length (%d) has at least a byte", signature_algorithms[algorithm_count], signature_len), signature_len > 0);
        

        /* verify that the signature matches */
        ret = bgpsec_verify_signature_with_cert(data_to_sign,
                                                sizeof(data_to_sign),
                                                cert,
                                                signature_algorithms[algorithm_count],
                                                signature, sizeof(signature));
        RESULT(("cert sign: verify signature result: %d (should be %d)",
                ret, BGPSEC_SIGNATURE_MATCH),
               ret == BGPSEC_SIGNATURE_MATCH);

        /* generate a signature using a fingerprint */
        /* XXX: set test directory to search for matching fp->certs */
        signature_len =
            bgpsec_sign_data_with_fp(data_to_sign, sizeof(data_to_sign), fp,
                                     signature_algorithms[algorithm_count],
                                     signature, sizeof(signature));

        RESULT(("fp sign:   algorithm %d, signature length (%d) is not negative",
                signature_algorithms[algorithm_count], signature_len),
               signature_len > -1);
        RESULT(("fp sign:   algorithm %d, signature length (%d) has at least a byte", signature_algorithms[algorithm_count], signature_len), signature_len > 0);
        

        /* verify that the signature matches */
        ret = bgpsec_verify_signature_with_fp(data_to_sign,
                                              sizeof(data_to_sign),
                                              fp,
                                              signature_algorithms[algorithm_count],
                                              signature, sizeof(signature));
        RESULT(("fp sign:   verify signature result: %d (should be %d)",
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
