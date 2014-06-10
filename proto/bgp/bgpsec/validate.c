/*
 *	bgpsec: validation functions
 *
 *
 *      Parsons, Inc.
 *      (c) 2013-2013
 *
 *	Code can be used under either license:
 *      - Freely distributed and used under the terms of the GNU GPLv2.
 *      - Freely distributed and used under a BSD license, See README.bgpsec.
 */

#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <openssl/ec.h>
#include "validate.h"

/* define if the openssl errors should be logged */
/* #define LOG_OPENSSL_ERRORS */

/* define if the logging should go to STDERR instead of the bird log file */
/* #define LOG_TO_STDERR  */

#ifdef LOG_TO_STDERR
#define ERRORMSG(errmsg) do { fprintf(stderr, "Error: %s\n", errmsg); } while(0)
#else
#define ERRORMSG(errmsg) do { log(L_ERR "Error: %s\n", errmsg); } while(0)
#endif

#define ERROR(errmsg) do { ERRORMSG(errmsg); return(BGPSEC_FAILURE); } while(0)

void print_openssl_errors() {
#if defined(LOG_OPENSSL_ERRORS) && defined(LOG_TO_STDERR)
    ERR_print_errors_fp(stderr);
#endif
}

int bgpsec_sign_data_with_bin_ski(struct bgp_config *conf,
                                  byte *octets, int octets_len,
                                  char *ski, size_t ski_len,
				  int asn,
                                  int algorithm, byte *signature,
                                  int in_signature_len) {
    bgpsec_key_data key = { NULL };
  
    if (BGPSEC_SUCCESS !=
        bgpsec_load_key_from_bin_ski(conf, ski, ski_len, asn,
                                     &key, BGPSEC_DEFAULT_CURVE, 1)) {
        log(L_ERR "Failed to load a bgpsec key from a binary SKI");
	return(BGPSEC_FAILURE);
    }


    return bgpsec_sign_data_with_cert(conf, octets, octets_len, key,
                                      algorithm, signature, in_signature_len);
}

int bgpsec_sign_data_with_ascii_ski(struct bgp_config *conf,
                                    byte *octets, int octets_len,
                                    char *ski, size_t ski_len,
				    int asn,
                                    int algorithm, byte *signature,
                                    int in_signature_len) {
    bgpsec_key_data key = { NULL };

    if (BGPSEC_SUCCESS !=
        bgpsec_load_key_from_ascii_ski(conf, ski, ski_len, asn,
                                       &key, BGPSEC_DEFAULT_CURVE, 1)) {
        log(L_ERR "Failed to load a bgpsec key from an ascii SKI");
	return(BGPSEC_FAILURE);
    }

    return bgpsec_sign_data_with_cert(conf, octets, octets_len, key,
                                      algorithm, signature, in_signature_len);
}

/* Might need to call OpenSSL_add_all_digests() somewhere */

int bgpsec_sign_data_with_cert(struct bgp_config *conf,
                               byte *octets, int octets_len,
                               bgpsec_key_data cert,
                               int signature_algorithm,
                               byte *signature, int signature_len) {

    EVP_PKEY_CTX *ctx = NULL;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    size_t md_len;
    size_t sig_len = signature_len;
    int result = -1;

    switch (signature_algorithm) {
    case BGPSEC_ALGORITHM_SHA256_ECDSA_P_256:

	if (EVP_Digest(octets, octets_len, md_value, &md_len, EVP_sha256(), NULL) &&
	    (ctx = EVP_PKEY_CTX_new(cert.pkey, NULL)) != NULL &&
	    EVP_PKEY_sign_init(ctx) > 0 &&
	    EVP_PKEY_CTX_set_signature_md(ctx, EVP_sha256()) > 0 &&
	    EVP_PKEY_sign(ctx, signature, &sig_len, md_value, md_len) > 0)
	{
	    result = sig_len;
	}
	else
	{
	  log(L_ERR "validate: Failed to create digest/sign");
          return(BGPSEC_FAILURE);
	}

    default:
        break;
    }

    EVP_PKEY_CTX_free(ctx);
    return result;
}

int bgpsec_verify_signature_with_cert(struct bgp_config *conf,
                                      byte *octets, int octets_len,
                                      bgpsec_key_data cert,
                                      int signature_algorithm,
                                      byte *signature, int signature_len) {
    EVP_PKEY_CTX *ctx = NULL;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    size_t md_len;
    int result = BGPSEC_SIGNATURE_ERROR;

    /*
     * Not sure that BGPSEC_SIGNATURE_MISMATCH makes sense given the API.
     */

    if (EVP_Digest(octets, octets_len, md_value, &md_len, EVP_sha256(), NULL) &&
	(ctx = EVP_PKEY_CTX_new(cert.pkey, NULL)) != NULL &&
	EVP_PKEY_verify_init(ctx) > 0 &&
	EVP_PKEY_CTX_set_signature_md(ctx, EVP_sha256()) > 0 &&
	EVP_PKEY_verify(ctx, signature, signature_len, md_value, md_len) > 0)
    {
        result = BGPSEC_SIGNATURE_MATCH;
    }

#if 0
    else
        print_openssl_errors();
#endif

    EVP_PKEY_CTX_free(ctx);
    return result;
}
    
int bgpsec_verify_signature_with_ascii_ski(struct bgp_config *conf,
                                           byte *octets, int octets_len,
                                           char *ski, size_t ski_len,
					   int asn,
                                           int signature_algorithm,
                                           byte *signature, int signature_len) {
    bgpsec_key_data key = { NULL };

    if (BGPSEC_SUCCESS !=
        bgpsec_load_key_from_ascii_ski(conf, ski, ski_len, asn,
                                       &key, BGPSEC_DEFAULT_CURVE, 0)) {
        log(L_ERR "Failed to load a bgpsec key from an ascii SKI");
        return(BGPSEC_FAILURE);
    }

    return bgpsec_verify_signature_with_cert(conf, octets, octets_len, key,
                                             signature_algorithm,
                                             signature, signature_len);
}

int bgpsec_verify_signature_with_bin_ski(struct bgp_config *conf,
                                         byte *octets, int octets_len,
                                         char *ski, size_t ski_len,
					 int asn,
                                         int signature_algorithm,
                                         byte *signature, int signature_len) {
    bgpsec_key_data key = { NULL };

    if (BGPSEC_SUCCESS !=
        bgpsec_load_key_from_bin_ski(conf, ski, ski_len, asn,
                                     &key, BGPSEC_DEFAULT_CURVE, 0)) {
        log(L_ERR "Failed to load a bgpsec key from an binary SKI");
        return(BGPSEC_FAILURE);
    }

    return bgpsec_verify_signature_with_cert(conf, octets, octets_len, key,
                                             signature_algorithm,
                                             signature, signature_len);
}

int bgpsec_save_key(struct bgp_config *conf,
                    const char *filePrefix, bgpsec_key_data *key_data,
                    int curveId, int savePrivateKey) {
    char filenamebuf[MAXPATHLEN];
    BIO *bio = NULL;
    mode_t oldUmask;

    oldUmask = umask(077);

    memset(filenamebuf, 0, sizeof(filenamebuf));

    /*
     * Might want to whack asn1_flag to force namedCurve.
     */

    if (savePrivateKey) {
        /* save the private key */
        snprintf(filenamebuf, sizeof(filenamebuf)-1,
                 "%s.bin_private", filePrefix);
	if ((bio = BIO_new_file(filenamebuf, "wb")) == NULL ||
	    !i2d_PrivateKey_bio(bio, key_data->pkey) ||
	    !!BIO_free(bio))
	  log(L_ERR "Couldn't save private key");
          return(BGPSEC_FAILURE);
	bio = NULL;
    }

    /* save the public key */
    snprintf(filenamebuf, sizeof(filenamebuf)-1, "%s.bin_pub", filePrefix);
    if ((bio = BIO_new_file(filenamebuf, "wb")) == NULL ||
	!i2d_PUBKEY_bio(bio, key_data->pkey) ||
	!BIO_free(bio))
      log(L_ERR "Couldn't save public key");
      return(BGPSEC_FAILURE);


    umask(oldUmask);

    return BGPSEC_SUCCESS;
}

/*
 * OK, this is a lot more complicated than it needs to be.
 *
 * All we really need is:
 *
 * - For private keys, read public/private keypair from a DER file.
 *
 * - For public keys, read public key from a DER file.
 *
 * Not clear that this code should be touching certificates at all, or
 * using PEM, although we could support both to be kind.  Let's not,
 * for now, to keep this simple.
 *
 * Given that "openssl req" takes an -inform flag, Mike's script could
 * easily be hacked to do everything in DER.  Similarly, since nobody
 * is likely to work out the filename SKI encoding by hand, the script
 * that does that for imported certificates could just extract the
 * public key and write it in DER.
 */

int bgpsec_load_key(struct bgp_config *conf,
                    const char *filePrefix, bgpsec_key_data *key_data,
                    int curveId, int loadPrivateKey) {
    char filenamebuf[MAXPATHLEN];
    int ret = BGPSEC_FAILURE;
    BIO *bio = NULL;

    /* translate the curve ID to the OpenSSL identifier */
    switch (curveId) {
    case BGPSEC_ALGORITHM_SHA256_ECDSA_P_256:
        curveId = BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256;
        break;
    default:
        log(L_ERR "Unkown curve ID");
        return(BGPSEC_FAILURE);
    }

    memset(filenamebuf, 0, sizeof(filenamebuf));

    snprintf(filenamebuf, sizeof(filenamebuf) - 1, "%s.%s",
	     filePrefix, (loadPrivateKey ? "private" : "pub"));

    if ((bio = BIO_new_file(filenamebuf, "rb")) != NULL &&
	(loadPrivateKey
	 ? d2i_PrivateKey_bio(bio, &key_data->pkey)
	 : d2i_PUBKEY_bio(bio, &key_data->pkey)) &&
	EVP_PKEY_id(key_data->pkey) == EVP_PKEY_EC &&
	BIO_free(bio))
    {
	EC_KEY_set_asn1_flag(EVP_PKEY_get0(key_data->pkey), OPENSSL_EC_NAMED_CURVE);
        ret = BGPSEC_SUCCESS;
	bio = NULL;
    }

    BIO_free(bio);

    return ret;
}

int bgpsec_load_key_from_ascii_ski(struct bgp_config *conf,
                                   const char *ski, size_t ski_len,
				   int asn,
                                   bgpsec_key_data *key_data,
                                   int curveId, int loadPrivateKey) {
    char filenamebuf[MAXPATHLEN];

    if (!generate_ski_filename(filenamebuf, sizeof(filenamebuf),
                               ((conf && conf->bgpsec_key_repo_path) ?
                                conf->bgpsec_key_repo_path :
                                DEFAULT_KEY_REPO_PATH),
                               ski, ski_len, asn)) {
      
      log(L_ERR "failed to generate a file name from a ski");
      return(BGPSEC_FAILURE);
    }

    return bgpsec_load_key(conf, filenamebuf, key_data,
                           curveId, loadPrivateKey);
}

char *generate_ski_filename(char *filenamebuf, size_t filenamebufLen,
                            const char *rootPath,
                            const char *ski, size_t skiLen,
			    int asn) {

    int ret;

    /*
     * SKI is fixed length, so neither error case should never happen.
     */

    if (skiLen <= 6)
	return NULL;

    ret = snprintf(filenamebuf, filenamebufLen-1, "%s/%.2s/%.4s/%.*s",
		   rootPath, ski, ski + 2, (skiLen - 6), ski + 6);

    if (ret >= filenamebufLen)
	return NULL;

    return filenamebuf;
}    

int bgpsec_load_key_from_bin_ski(struct bgp_config *conf,
                                 const char *ski, size_t ski_len,
				 int asn,
                                 bgpsec_key_data *key_data,
                                 int curveId, int loadPrivateKey) {

    char ascii_ski_buf[MAXPATHLEN], *cp;
    int i;

    if (ski_len * 2 + 1 >= sizeof(ascii_ski_buf)) {
        log(L_ERR "validate: buffer to small for SKI length: %d", ski_len);
	return BGPSEC_FAILURE;
    }

    memset(ascii_ski_buf, 0, sizeof(ascii_ski_buf));

    cp = ascii_ski_buf;
    for (i = 0; i < ski_len; i++) {
      sprintf(cp, "%02X", (byte)ski[i]);
      cp += 2;
    }

    return
        bgpsec_load_key_from_ascii_ski(conf,
                                       ascii_ski_buf, sizeof(ascii_ski_buf),
				       asn,
                                       key_data, curveId, loadPrivateKey);
}

int bgpsec_get_filesize(const char *filename) {
    struct stat statbuf;

    stat(filename, &statbuf);
    return statbuf.st_size;
}
