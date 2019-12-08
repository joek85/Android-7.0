/* Written by Markus Koetter (nepenthesdev@gmail.com) for the OpenSSL
 * project.
 */
/* ====================================================================
 * Copyright (c) 2011 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <memory.h>
#include <errno.h>

#include <openssl/aes.h>
#include <openssl/engine.h>
#include <sys/socket.h>
#include <linux/if_alg.h>
#include <unistd.h>
#include <sys/param.h>
#include <ctype.h>
#include <stdbool.h>

#include "openssl/md5.h"
#include "openssl/des.h"
#include "openssl/ecdh.h"
#include "openssl/ossl_typ.h"
#include "../crypto/ecdh/ech_locl.h"
#include "../crypto/ec/ec_lcl.h"
#include "../crypto/ecdsa/ecs_locl.h"

#ifdef SUPPORT_CE_V3_2
#pragma message("The version of CE is V3.2.")
#endif

/* Define the capability of SS controller. */

#define SS_CTR_MODE_ENABLE		1
//#define SS_CTS_MODE_ENABLE		1
#define SS_OFB_MODE_ENABLE		1
#define SS_CFB_MODE_ENABLE		1
#define SS_XTS_MODE_ENABLE		1
#define SS_SHA224_ENABLE		1
#define SS_SHA256_ENABLE		1
#define SS_SHA384_ENABLE		1
#define SS_SHA512_ENABLE		1
#define SS_HMAC_SHA1_ENABLE		1
#define SS_HMAC_SHA256_ENABLE	1
#define SS_RSA_ENABLE			1
#define SS_DH_ENABLE			1
#define SS_ECC_ENABLE			1

#if 1
#define E_DEBUG_FLAG
#define E_DBG(string, args...) \
	do { \
		printf("%s()%u---", __func__, __LINE__); \
		printf(string, ##args); \
	} while (0)
#else
#define E_DBG(string, args...)  \
	do { \
	} while (0)	
#endif

#define E_ERR(string, args...) \
	do { \
		printf("%s()%u---", __func__, __LINE__); \
		printf(string, ##args); \
	} while (0)

#ifndef AF_ALG
#define AF_ALG 38
#endif

#ifndef SOL_ALG
#define SOL_ALG 279
#endif

/* Socket options */
#define ALG_SET_KEY			1
#define ALG_SET_IV			2
#define ALG_SET_OP			3

/* Operations */
#define ALG_OP_DECRYPT			0
#define ALG_OP_ENCRYPT			1

#define AES_KEY_SIZE_128        16
#define AES_KEY_SIZE_192        24
#define AES_KEY_SIZE_256        32

#define RSA_MAX_LEN			4096

static const EVP_MD af_alg_md_hmac_sha1;
static const EVP_MD af_alg_md_hmac_sha256;

static int af_alg_ciphers(ENGINE *e, const EVP_CIPHER **cipher, const int **nids, int nid);
static int af_alg_digests(ENGINE *e, const EVP_MD **digest, const int **nids, int nid);

#define DYNAMIC_ENGINE
#define AF_ALG_ENGINE_ID	"af_alg"
#define AF_ALG_ENGINE_NAME	"use AF_ALG for AES crypto"

void print_hex(void *_data, int _len, int _addr)
{
#ifdef E_DEBUG_FLAG
	int i;
	unsigned char *data = (unsigned char *)_data;

	printf("-------------------- The valid len = %d ----------------------- \n", _len);
	for (i=0; i<(_len+7)/8; i++) {
		printf("0x%08X: %02X %02X %02X %02X %02X %02X %02X %02X \n", i*8 + _addr,
			data[i*8+0], data[i*8+1], data[i*8+2], data[i*8+3],
			data[i*8+4], data[i*8+5], data[i*8+6], data[i*8+7]);
	}
	printf("-------------------------------------------------------------- \n");
#endif
}

static bool nid_in_nids(int nid, int nids[], int num)
{
	int i=0;
	for( i=0;i<num;i++ )
		if( nids[i] == nid )
			return true;
	return false;
}

struct af_alg_cipher_data
{
	int tfmfd;
	int op;
	__u32 type;
};

struct af_alg_digest_data
{
#if defined(SS_HMAC_SHA1_ENABLE) || defined(SS_HMAC_SHA256_ENABLE)
	char key[SHA_CBLOCK];
	int keylen;
#endif
	int tfmfd;
	int opfd;
};

static int af_alg_cipher_all_nids[] = {
	NID_aes_128_ecb,
	NID_aes_192_ecb,
	NID_aes_256_ecb,
	NID_aes_128_cbc,
	NID_aes_192_cbc,
	NID_aes_256_cbc,
#ifdef SS_CTR_MODE_ENABLE
	NID_aes_128_ctr,
	NID_aes_192_ctr,
	NID_aes_256_ctr,
#endif
#ifdef SS_CTS_MODE_ENABLE
	NID_aes_128_cts,
	NID_aes_192_cts,
	NID_aes_256_cts,
#endif
#ifdef SS_XTS_MODE_ENABLE
	NID_aes_128_xts,
	NID_aes_256_xts,
#endif
#ifdef SS_CFB_MODE_ENABLE
	NID_aes_128_cfb1,
	NID_aes_192_cfb1,
	NID_aes_256_cfb1,
	NID_aes_128_cfb8,
	NID_aes_192_cfb8,
	NID_aes_256_cfb8,
	NID_aes_128_cfb128,
	NID_aes_192_cfb128,
	NID_aes_256_cfb128,
#endif
#ifdef SS_OFB_MODE_ENABLE
	NID_aes_128_ofb128,
	NID_aes_192_ofb128,
	NID_aes_256_ofb128,
#endif
	NID_des_ecb,
	NID_des_cbc,
	NID_des_ede3_ecb,
	NID_des_ede3_cbc,
};
static int af_alg_cipher_all_nids_num = (sizeof(af_alg_cipher_all_nids)/sizeof(af_alg_cipher_all_nids[0]));
static int *af_alg_cipher_nids = NULL;
static int af_alg_cipher_nids_num = 0;

static int af_alg_digest_all_nids[] = {
	NID_sha1,
	NID_sha224,
	NID_sha256,
	NID_sha384,
	NID_sha512,
	NID_md5,
	NID_hmac_sha1,
	NID_hmacWithSHA256,
};
static int af_alg_digest_all_nids_num = sizeof(af_alg_digest_all_nids)/sizeof(af_alg_digest_all_nids[0]);
static int *af_alg_digest_nids = NULL;
static int af_alg_digest_nids_num = 0;

static  void e_afalg_dump(char *str,unsigned char *data,\
	int len,int align)
{
	int i = 0;
	if(str)
		printf("\n%s: ",str);
	for(i = 0;i<len;i++)
	{
		if((i%align) == 0)
		{
			printf("\n");
		}
		printf("%x",*(data++));
	}
	printf("\n");
}

#define SS_RAND_SEED_DEFAULT_LEN	24
struct af_alg_digest_data af_alg_rand_ctx;

void af_alg_rand_init(char *buf, int num)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "rng",
		.salg_name = "trng",
	};

	memset(&af_alg_rand_ctx, 0, sizeof(struct af_alg_digest_data));
	af_alg_rand_ctx.tfmfd = socket(AF_ALG, SOCK_SEQPACKET, 0);
	if(af_alg_rand_ctx.tfmfd == -1) {
		E_ERR("socket() failed! [%d]: %s\n", errno, strerror(errno));
		return;
	}

	if (bind(af_alg_rand_ctx.tfmfd, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
		E_ERR("bind() failed! [%d]: %s\n", errno, strerror(errno));
		return;
	}

	if (buf != NULL)
		if (setsockopt(af_alg_rand_ctx.tfmfd, SOL_ALG, ALG_SET_KEY, buf, num) == -1) {
			E_ERR("setsockopt() failed! [%d]: %s\n", errno, strerror(errno));
			return;
		}

	af_alg_rand_ctx.opfd = accept(af_alg_rand_ctx.tfmfd, NULL, 0);
	if (af_alg_rand_ctx.opfd == -1 ) {
		E_ERR("accept() failed! [%d]: %s\n", errno, strerror(errno));
		return;
	}

}

void af_alg_rand_exit(void)
{
	close(af_alg_rand_ctx.opfd);
	af_alg_rand_ctx.opfd = -1;

	close(af_alg_rand_ctx.tfmfd);
	af_alg_rand_ctx.tfmfd = -1;
}

void af_alg_seed(const void *buf, int num)
{
	E_DBG("Seed len = %d\n", num);
	if (af_alg_rand_ctx.opfd < 2)
		af_alg_rand_init((char *)buf, num);
}

static int af_alg_rand(unsigned char *buf, int len)
{
	ssize_t r = 0;

	if (af_alg_rand_ctx.opfd < 2)
		af_alg_rand_init(NULL, SS_RAND_SEED_DEFAULT_LEN);

	r = read(af_alg_rand_ctx.opfd, buf, len);
	E_DBG("read(%d) return %d \n", len, r);
	if (r != len) {
		E_ERR("read() return %d. [%d]: %s\n", (int)r, errno, strerror(errno));
	}
	E_DBG("%02x %02x %02x %02x   %02x %02x %02x %02x \n",
			buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);

	af_alg_rand_exit();
	return 1;
}

static int af_alg_rand_status(void)
{
	return 1;
}

static RAND_METHOD af_alg_random =
{
	af_alg_seed,
	af_alg_rand,
	NULL,
	NULL,
	af_alg_rand,
	af_alg_rand_status,
};

#if defined(SS_RSA_ENABLE) || defined(SS_DH_ENABLE) || defined(SS_ECC_ENABLE)

#define ECC521_SIZE	(((521+31)/32)*4)
#define ECC256_SIZE	(((256+31)/32)*4)
#define ECC224_SIZE	(((224+31)/32)*4)
#define ECC160_SIZE	(((160+31)/32)*4)

void af_alg_convert_byte(unsigned char *data, int len)
{
	int i;
	unsigned char *buf = NULL;

	buf = OPENSSL_malloc(len);
	if (buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", len);
		return;
	}

	for (i=0; i<len; i++)
		buf[i] = data[len - 1 - i];

	memcpy(data, buf, len);
	OPENSSL_free(buf);
}

int af_alg_asym_sock_init(struct af_alg_cipher_data *ctx, const BIGNUM *key, int size, char *name, int type)
{
	char key_str[RSA_MAX_LEN/8] = {0};

	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
	};

	E_DBG("name = %s, size = %d, type = %d \n", name, size, type);
	if (size > RSA_MAX_LEN/8) {
		E_ERR("The len is too large: %d\n", size);
		return -1;
	}

	if (type == NID_secp521r1)
		snprintf((char *)sa.salg_name, 20, "%s(%d)", name, 521);
	else
		snprintf((char *)sa.salg_name, 20, "%s(%d)", name, size*8);

	ctx->tfmfd = socket(AF_ALG, SOCK_SEQPACKET, 0);
	if(ctx->tfmfd == -1) {
		E_ERR("socket() failed! [%d]: %s\n", errno, strerror(errno));
		return 0;
	}

	if (bind(ctx->tfmfd, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
		E_ERR("bind(%s) failed! [%d]: %s\n", sa.salg_name, errno, strerror(errno));
		return 0;
	}

	if (key != NULL) {
		memcpy(key_str, (char *)key->d, BN_num_bytes(key));
		if (setsockopt(ctx->tfmfd, SOL_ALG, ALG_SET_KEY, key_str, size) == -1) {
			E_ERR("setsockopt() failed! [%d]: %s\n", errno, strerror(errno));
			return 0;
		}
	}

	ctx->op = accept(ctx->tfmfd, NULL, 0);
	if (ctx->op == -1) {
		E_ERR("accept() failed! [%d]: %s\n", errno, strerror(errno));
		return 0;
	}

	return 1;
}
#endif

#ifdef SS_RSA_ENABLE

struct af_alg_cipher_data af_alg_rsa_ctx = {0, 0, 0};

int af_alg_rsa_padding(unsigned char *out, int olen,
			const unsigned char *in, int ilen, int padding)
{
	int ret = 0;

	memset(out, 0, olen);
	switch (padding) {
	case RSA_PKCS1_PADDING:
		ret = RSA_padding_add_PKCS1_type_2(out, olen, in, ilen);
		break;
#ifndef OPENSSL_NO_SHA
	case RSA_PKCS1_OAEP_PADDING:
		ret = RSA_padding_add_PKCS1_OAEP(out, olen, in, ilen, NULL, 0);
		break;
#endif
	case RSA_SSLV23_PADDING:
		ret = RSA_padding_add_SSLv23(out, olen, in, ilen);
		break;
	case RSA_NO_PADDING:
		ret = RSA_padding_add_none(out, ilen, in, ilen);
		break;
	default:
		E_ERR("Invalid padding type: %d \n", padding);
		break;
	}

	if (ret == 1)
		af_alg_convert_byte(out, olen);

	return ret;
}

int af_alg_rsa_unpadding(unsigned char *out, int olen,
			const unsigned char *in, int ilen, int padding)
{
	int ret = 0;

	switch (padding) {
	case RSA_PKCS1_PADDING:
		ret = RSA_padding_check_PKCS1_type_2(out, olen, in, ilen, olen);
		break;
#ifndef OPENSSL_NO_SHA
	case RSA_PKCS1_OAEP_PADDING:
		ret = RSA_padding_check_PKCS1_OAEP(out, olen, in, ilen, olen,NULL,0);
			break;
#endif
	case RSA_SSLV23_PADDING:
		ret = RSA_padding_check_SSLv23(out, olen, in, ilen, olen);
		break;
	case RSA_NO_PADDING:
		ret = RSA_padding_check_none(out, olen, in, ilen, olen);
		break;
	default:
		E_ERR("Invalid padding type: %d \n", padding);
		break;
	}

	return ret;
}

#ifdef SUPPORT_CE_V3_2

/* to = (from ^ rsa->e) mod rsa->n */
int af_alg_rsa_pub_enc(int flen, const unsigned char *from,
		   unsigned char *to, RSA *rsa, int padding)
{
	int ret = 0;
	int n_len = 0;
	int n_len_align = 0;
	unsigned char *inbuf = NULL;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct iovec iov;
	char ivbuf[CMSG_SPACE(sizeof(af_alg_rsa_ctx.type))];

	if ((rsa->e == NULL) || (rsa->n == NULL)) {
		E_ERR("Invalid e 0x%p or n 0x%p\n", rsa->e, rsa->n);
		return 0;
	}

	n_len = BN_num_bytes(rsa->n);
	n_len_align = ((n_len + 3)/4)*4;
	if (flen > n_len) {
		E_ERR("The input data is too long: %d/%d\n", flen, n_len);
		return 0;
	}

	/* Key: NULL */
	if (af_alg_asym_sock_init(&af_alg_rsa_ctx, NULL, n_len_align, "rsa", 0) == 0)
		return 0;

	/* Src: e-n-from */
	inbuf = OPENSSL_malloc(n_len_align*3);
	if (inbuf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", n_len_align*3);
		return 0;
	}
	memset(inbuf, 0, n_len_align*3);
	memcpy(inbuf, (char *)rsa->e->d, BN_num_bytes(rsa->e));
	memcpy(&inbuf[n_len_align], (char *)rsa->n->d, BN_num_bytes(rsa->n));

	af_alg_rsa_padding(&inbuf[n_len_align*2], n_len_align, from, flen, padding);

	af_alg_rsa_ctx.type = ALG_OP_ENCRYPT;
	memset(ivbuf, 0, sizeof(ivbuf));
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = ivbuf;
	msg.msg_controllen = sizeof(ivbuf);

	/* Set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg),&af_alg_rsa_ctx.type, 4);

	/* IV: NULL */

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	iov.iov_base = (void *)inbuf;
	iov.iov_len = n_len_align*3;

	ret = sendmsg(af_alg_rsa_ctx.op, &msg, 0);
	if (ret == -1) {
		OPENSSL_free(inbuf);
		E_ERR("sendmsg(%d) failed! return %d\n", af_alg_rsa_ctx.op, ret);
		return 0;
	}

	OPENSSL_free(inbuf);
	ret = read(af_alg_rsa_ctx.op, to, n_len);
	if (ret != n_len) {
		E_ERR("read() failed! return %d / %d\n", ret, n_len);
		return 0;
	}

	af_alg_convert_byte(to, n_len);
	return ret;
}

/* to = (from ^ rsa->d) mod rsa->n */
int af_alg_rsa_priv_dec(int flen,const unsigned char *from,
			unsigned char *to, RSA *rsa, int padding)
{
	int ret = 0;
	int n_len = 0;
	int n_len_align = 0;
	unsigned char *inbuf = NULL;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct iovec iov;
	char ivbuf[CMSG_SPACE(sizeof(af_alg_rsa_ctx.type))];

	if ((rsa->d == NULL) || (rsa->n == NULL)) {
		E_ERR("Invalid e 0x%p or n 0x%p\n", rsa->e, rsa->n);
		return 0;
	}

	n_len = BN_num_bytes(rsa->n);
	n_len_align = ((n_len + 3)/4)*4;
	if (flen > n_len) {
		E_ERR("The input data is too long: %d/%d\n", flen, n_len);
		return 0;
	}

	/* Key: NULL */
	if (af_alg_asym_sock_init(&af_alg_rsa_ctx, NULL, n_len_align, "rsa", 0) == 0)
		return 0;

	/* Src: d-n-from */
	inbuf = OPENSSL_malloc(n_len_align*3);
	if (inbuf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", n_len_align*3);
		return 0;
	}
	memset(inbuf, 0, n_len_align*3);
	memcpy(inbuf, (char *)rsa->d->d, BN_num_bytes(rsa->d));
	memcpy(&inbuf[n_len_align], (char *)rsa->n->d, BN_num_bytes(rsa->n));
	memcpy(&inbuf[n_len_align*2], from, n_len_align);
	af_alg_convert_byte(&inbuf[n_len_align*2], n_len_align);

	af_alg_rsa_ctx.type = ALG_OP_DECRYPT;
	memset(ivbuf, 0, sizeof(ivbuf));
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = ivbuf;
	msg.msg_controllen = sizeof(ivbuf);

	/* Set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg),&af_alg_rsa_ctx.type, 4);

	/* IV: NULL */

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	iov.iov_base = (void *)inbuf;
	iov.iov_len = n_len_align*3;

	ret = sendmsg(af_alg_rsa_ctx.op, &msg, 0);
	if (ret == -1) {
		OPENSSL_free(inbuf);
		E_ERR("sendmsg(%d) failed! return %d\n", af_alg_rsa_ctx.op, ret);
		return 0;
	}

	OPENSSL_free(inbuf);
	ret = read(af_alg_rsa_ctx.op, to, n_len);
	if (ret != n_len) {
		E_ERR("read() failed! return %d / %d\n", ret, n_len);
		return 0;
	}

	af_alg_convert_byte(to, n_len);
	return ret;
}

#else

/* to = (from ^ rsa->e) mod rsa->n */
int af_alg_rsa_pub_enc(int flen, const unsigned char *from,
		   unsigned char *to, RSA *rsa, int padding)
{
	int ret = 0;
	int n_len = 0;
	unsigned char *inbuf = NULL;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct af_alg_iv *ivm;
	struct iovec iov;
	int iv_len = RSA_MAX_LEN/8; /* Maximum length of RSA key. */
	char ivbuf[CMSG_SPACE(sizeof(af_alg_rsa_ctx.type)) + CMSG_SPACE(offsetof(struct af_alg_iv, iv) + iv_len)];

	if ((rsa->e == NULL) || (rsa->n == NULL)) {
		E_ERR("Invalid e 0x%p or n 0x%p\n", rsa->e, rsa->n);
		return 0;
	}

	n_len = BN_num_bytes(rsa->n);
	if (flen > n_len) {
		E_ERR("The input data is too long: %d/%d\n", flen, n_len);
		return 0;
	}

	/* Key: rsa->e */
	if (af_alg_asym_sock_init(&af_alg_rsa_ctx, rsa->e, n_len, "rsa", 0) == 0)
		return 0;

	/* Src: from */
	inbuf = OPENSSL_malloc(n_len);
	if (inbuf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", n_len);
		return 0;
	}
	af_alg_rsa_padding(inbuf, n_len, from, flen, padding);

	af_alg_rsa_ctx.type = ALG_OP_ENCRYPT;
	memset(ivbuf, 0, sizeof(ivbuf));
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = ivbuf;
	msg.msg_controllen = sizeof(ivbuf) - iv_len + n_len;

	/* Set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg),&af_alg_rsa_ctx.type, 4);

	/* Set IV: rsa->n */
	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(offsetof(struct af_alg_iv, iv) + n_len);
	ivm = (void*)CMSG_DATA(cmsg);
	ivm->ivlen = n_len;
	memcpy(ivm->iv, rsa->n->d, n_len);

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	iov.iov_base = (void *)inbuf;
	iov.iov_len = n_len;

	ret = sendmsg(af_alg_rsa_ctx.op, &msg, 0);
	if (ret == -1) {
		OPENSSL_free(inbuf);
		E_ERR("sendmsg(%d) failed! return %d\n", af_alg_rsa_ctx.op, ret);
		return 0;
	}

	OPENSSL_free(inbuf);
	ret = read(af_alg_rsa_ctx.op, to, n_len);
	if (ret != n_len) {
		E_ERR("read() failed! return %d / %d\n", ret, n_len);
		return 0;
	}

	af_alg_convert_byte(to, n_len);
	return ret;
}

/* to = (from ^ rsa->d) mod rsa->n */
int af_alg_rsa_priv_dec(int flen,const unsigned char *from,
			unsigned char *to, RSA *rsa, int padding)
{
	int ret = 0;
	int n_len = 0;
	unsigned char *inbuf = NULL;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct af_alg_iv *ivm;
	struct iovec iov;
	int iv_len = RSA_MAX_LEN/8; /* Maximum length of RSA key. */
	char ivbuf[CMSG_SPACE(sizeof(af_alg_rsa_ctx.type)) + CMSG_SPACE(offsetof(struct af_alg_iv, iv) + iv_len)];

	if ((rsa->d == NULL) || (rsa->n == NULL)) {
		E_ERR("Invalid e 0x%p or n 0x%p\n", rsa->e, rsa->n);
		return 0;
	}

	n_len = BN_num_bytes(rsa->n);
	if (flen > n_len) {
		E_ERR("The input data is too long: %d/%d\n", flen, n_len);
		return 0;
	}

	/* Key: rsa->d */
	if (af_alg_asym_sock_init(&af_alg_rsa_ctx, rsa->d, n_len, "rsa", 0) == 0)
		return 0;

	/* Src: from */
	inbuf = OPENSSL_malloc(n_len);
	if (inbuf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", n_len);
		return 0;
	}
	memcpy(inbuf, from, n_len);
	af_alg_convert_byte(inbuf, n_len);

	af_alg_rsa_ctx.type = ALG_OP_DECRYPT;
	memset(ivbuf, 0, sizeof(ivbuf));
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = ivbuf;
	msg.msg_controllen = sizeof(ivbuf) - iv_len + n_len;

	/* Set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg),&af_alg_rsa_ctx.type, 4);

	/* Set IV: rsa->n */
	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(offsetof(struct af_alg_iv, iv) + n_len);
	ivm = (void*)CMSG_DATA(cmsg);
	ivm->ivlen = n_len;
	memcpy(ivm->iv, rsa->n->d, n_len);

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	iov.iov_base = (void *)inbuf;
	iov.iov_len = n_len;

	ret = sendmsg(af_alg_rsa_ctx.op, &msg, 0);
	if (ret == -1) {
		OPENSSL_free(inbuf);
		E_ERR("sendmsg(%d) failed! return %d\n", af_alg_rsa_ctx.op, ret);
		return 0;
	}

	OPENSSL_free(inbuf);
	ret = read(af_alg_rsa_ctx.op, to, n_len);
	if (ret != n_len) {
		E_ERR("read() failed! return %d / %d\n", ret, n_len);
		return 0;
	}
	af_alg_convert_byte(to, n_len);

	return ret;
}
#endif

int af_alg_rsa_init(RSA *rsa)
{
	memset(&af_alg_rsa_ctx, 0, sizeof(struct af_alg_cipher_data));
	return 1;
}

int af_alg_rsa_finish(RSA *rsa)
{
	if (af_alg_rsa_ctx.tfmfd > 0)
		close(af_alg_rsa_ctx.tfmfd);
	if (af_alg_rsa_ctx.op > 0)
		close(af_alg_rsa_ctx.op);
	return 1;
}

static RSA_METHOD af_alg_rsa =
	{
	"Af_alg RSA method",
	af_alg_rsa_pub_enc,
	NULL,
	NULL,
	af_alg_rsa_priv_dec,
	NULL,
	NULL,
	af_alg_rsa_init,
	af_alg_rsa_finish,
	0,
	NULL,
	NULL,
	NULL,
	NULL
	};
#endif

#ifdef SS_DH_ENABLE

struct af_alg_cipher_data af_alg_dh_ctx = {0, 0, 0};

static int af_alg_dh_generate_key(DH *dh)
{
	DH_METHOD *dh_pub = (DH_METHOD *)DH_OpenSSL();
	return dh_pub->generate_key(dh);
}

static int af_alg_dh_compute_key(unsigned char *key, const BIGNUM *pub_key, DH *dh)
{
	DH_METHOD *dh_pub = (DH_METHOD *)DH_OpenSSL();
	return dh_pub->compute_key(key, pub_key, dh);
}

int af_alg_dh_init(DH *dh)
{
	memset(&af_alg_dh_ctx, 0, sizeof(struct af_alg_cipher_data));
	return 1;
}

int af_alg_dh_finish(DH *dh)
{
	if (af_alg_dh_ctx.tfmfd > 0) {
		close(af_alg_dh_ctx.tfmfd);
		af_alg_dh_ctx.tfmfd = 0;
	}
	if (af_alg_dh_ctx.op > 0) {
		close(af_alg_dh_ctx.op);
		af_alg_dh_ctx.op = 0;
	}
	return 1;
}

/* r = (a ^ p) mod m */
static int af_alg_dh_bn_mod_exp(const DH *dh, BIGNUM *r, const BIGNUM *a,
	const BIGNUM *p, const BIGNUM *m, BN_CTX *ctx,
	BN_MONT_CTX *m_ctx)
{
	int ret = 0;
	int p_len = 0;
	unsigned char *a_buf = NULL;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct af_alg_iv *ivm;
	struct iovec iov;
	int iv_len = RSA_MAX_LEN/8; /* Maximum length of RSA key. */
	char ivbuf[CMSG_SPACE(sizeof(af_alg_dh_ctx.type)) + CMSG_SPACE(offsetof(struct af_alg_iv, iv) + iv_len)];

	if ((r == NULL) || (a == NULL) || (p == NULL) || (m == NULL)) {
		E_ERR("Invalid parameter: r: 0x%p, a: 0x%p, p: 0x%p, m: 0x%p\n", r, a, p, m);
		return 0;
	}
	E_DBG("Len: r %d, a %d, p %d, m %d \n", BN_num_bytes(r), BN_num_bytes(a),
											BN_num_bytes(p), BN_num_bytes(m));

	/* Key: p */
	p_len = BN_num_bytes(p);
	if (af_alg_asym_sock_init(&af_alg_dh_ctx, p, p_len, "dh", 0) == 0)
		return 0;

	/* Src: a */
	a_buf = OPENSSL_malloc(p_len);
	if (a_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", p_len);
		return 0;
	}
	memset(a_buf, 0, p_len);
	memcpy(a_buf, a->d, BN_num_bytes(a));

	af_alg_dh_ctx.type = ALG_OP_ENCRYPT;
	memset(ivbuf, 0, sizeof(ivbuf));
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = ivbuf;
	msg.msg_controllen = sizeof(ivbuf) - iv_len + p_len;

	/* Set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg), &af_alg_dh_ctx.type, 4);

	/* Set IV: m */
	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(offsetof(struct af_alg_iv, iv) + p_len);
	ivm = (void*)CMSG_DATA(cmsg);
	ivm->ivlen = p_len;
	memcpy(ivm->iv, m->d, p_len);

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	iov.iov_base = (void *)a_buf;
	iov.iov_len = p_len;

	ret = sendmsg(af_alg_dh_ctx.op, &msg, 0);
	if (ret == -1) {
		E_ERR("sendmsg() failed! return %d \n", ret);
		OPENSSL_free(a_buf);
		return 0;
	}

	BN_copy(r, p);
	ret = read(af_alg_dh_ctx.op, r->d, p_len);
	if (ret != p_len) {
		E_ERR("read() failed! return %d / %d\n", ret, p_len);
		return 0;
	}

	af_alg_dh_finish(NULL);

	OPENSSL_free(a_buf);
	return 1;
}

static DH_METHOD af_alg_dh = {
	"Af_alg DH method",
	af_alg_dh_generate_key,
	af_alg_dh_compute_key,
	af_alg_dh_bn_mod_exp,
	af_alg_dh_init,
	af_alg_dh_finish,
	0,
	NULL,
	NULL
};
#endif

#ifdef SS_ECC_ENABLE

struct af_alg_cipher_data af_alg_ecc_ctx = {0, 0, 0};

static void af_alg_point2bin(const EC_GROUP *group, unsigned char *buf, const EC_POINT *p, BN_CTX *ctx, int step)
{
	BIGNUM *x_p = NULL;
	BIGNUM *y_p = NULL;

	x_p = BN_new();
	y_p = BN_new();
	if ((x_p == NULL) || (y_p == NULL)) {
		E_ERR("Failed to malloc BN: x 0x%p, y 0x%p\n", x_p, y_p);
		return;
	}

	if (!EC_POINT_get_affine_coordinates_GF2m(group, p, x_p, y_p, ctx)) {
		E_ERR("Failed to get (X,Y) coordinate.\n");
		return;
	}
	memcpy(buf, x_p->d, BN_num_bytes(x_p));
	memcpy(buf+step, y_p->d, BN_num_bytes(y_p));

	BN_free(x_p);
	BN_free(y_p);
}

void af_alg_ecc_finish(void)
{
	if (af_alg_ecc_ctx.tfmfd > 0) {
		close(af_alg_ecc_ctx.tfmfd);
		af_alg_ecc_ctx.tfmfd = 0;
	}
	if (af_alg_ecc_ctx.op > 0) {
		close(af_alg_ecc_ctx.op);
		af_alg_ecc_ctx.op = 0;
	}
}

int af_alg_get_ecc_size(int nid)
{
	switch (nid) {
	case NID_secp160k1:
	case NID_secp160r1:
	case NID_secp160r2:
		return ECC160_SIZE;
	case NID_secp224k1:
	case NID_secp224r1:
		return ECC224_SIZE;
	case NID_secp256k1:
	case NID_X9_62_prime256v1:
		return ECC256_SIZE;
	case NID_secp521r1:
		return ECC521_SIZE;
	default:
		E_ERR("Unsupported curve NID: %d\n", nid);
		return 13;	// make it failure quickly
	}
}

#ifdef SUPPORT_CE_V3_2
/* r = point * scalar */
static int af_alg_POINT_mul(const EC_GROUP *group, EC_POINT *r,
					const EC_POINT *point, const BIGNUM *scalar, BN_CTX *ctx)
{
	int ret = 1;
	int ecc_size = 0;
	unsigned char *src_buf = NULL;
	unsigned char *dst_buf = NULL;
	int src_len = 0;
	int dst_len = 0;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct iovec iov;
	char ivbuf[CMSG_SPACE(sizeof(af_alg_ecc_ctx.type))];

	/* Key: NULL */
	ecc_size = af_alg_get_ecc_size(group->curve_name);
	if (af_alg_asym_sock_init(&af_alg_ecc_ctx, NULL, ecc_size, "ecdh", group->curve_name) == 0)
			return 0;

	/* Src buf: field + scalar + group->a + point->x + point->y */
	src_len = ecc_size*5;
	src_buf = OPENSSL_malloc(src_len);
	if (src_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", src_len);
		af_alg_ecc_finish();
		return 0;
	}
	memset(src_buf, 0, src_len);
	memcpy(src_buf, group->field.d, BN_num_bytes(&group->field));
	memcpy(src_buf+ecc_size, scalar->d, BN_num_bytes(scalar));
	memcpy(src_buf+ecc_size*2, group->a.d, BN_num_bytes(&group->a));
	af_alg_point2bin(group, src_buf+ecc_size*3, point, ctx, ecc_size);

	/* Dst buf: r->x + r->y */
	dst_len = ecc_size*2;
	dst_buf = OPENSSL_malloc(dst_len);
	if (dst_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", dst_len);
		OPENSSL_free(src_buf);
		af_alg_ecc_finish();
		return 0;
	}
	memset(dst_buf, 0, dst_len);

	af_alg_ecc_ctx.type = ALG_OP_ENCRYPT;
	memset(ivbuf, 0, sizeof(ivbuf));
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = ivbuf;
	msg.msg_controllen = sizeof(ivbuf);

	/* Set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg), &af_alg_ecc_ctx.type, 4);

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	iov.iov_base = (void *)src_buf;
	iov.iov_len = src_len*5;

	ret = sendmsg(af_alg_ecc_ctx.op, &msg, 0);
	if (ret == -1) {
		E_ERR("sendmsg() failed! return %d \n", ret);
		OPENSSL_free(src_buf);
		OPENSSL_free(dst_buf);
		af_alg_ecc_finish();
		return 0;
	}

	ret = read(af_alg_ecc_ctx.op, dst_buf, dst_len);
	if (ret != dst_len)
		E_ERR("read() failed! return %d / %d\n", ret, dst_len);

	af_alg_convert_byte(dst_buf, ecc_size);
	BN_bin2bn(dst_buf, ecc_size, &r->X);
	af_alg_convert_byte(dst_buf+ecc_size, ecc_size);
	BN_bin2bn(dst_buf+ecc_size, ecc_size, &r->Y);

	OPENSSL_free(src_buf);
	OPENSSL_free(dst_buf);
	af_alg_ecc_finish();
	return 1;
}

#else
/* r = point * scalar */
static int af_alg_POINT_mul(const EC_GROUP *group, EC_POINT *r,
					const EC_POINT *point, const BIGNUM *scalar, BN_CTX *ctx)
{
	int ret = 1;
	int ecc_size = 0;
	unsigned char *src_buf = NULL;
	unsigned char *dst_buf = NULL;
	int src_len = 0;
	int dst_len = 0;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct af_alg_iv *ivm;
	struct iovec iov;
	int iv_len = RSA_MAX_LEN/8; /* Maximum length of RSA key. */
	char ivbuf[CMSG_SPACE(sizeof(af_alg_ecc_ctx.type)) + CMSG_SPACE(offsetof(struct af_alg_iv, iv) + iv_len)];

	/* Key: p_scalar */
	ecc_size = af_alg_get_ecc_size(group->curve_name);
	if (af_alg_asym_sock_init(&af_alg_ecc_ctx, scalar, ecc_size, "ecdh", group->curve_name) == 0)
			return 0;

	/* Src buf: group->a + point->x + point->y */
	src_len = ecc_size*3;
	src_buf = OPENSSL_malloc(src_len);
	if (src_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", src_len);
		af_alg_ecc_finish();
		return 0;
	}
	memset(src_buf, 0, src_len);
	memcpy(src_buf, group->a.d, BN_num_bytes(&group->a));
	af_alg_point2bin(group, src_buf+ecc_size, point, ctx, ecc_size);

	/* Dst buf: r->x + r->y */
	dst_len = ecc_size*2;
	dst_buf = OPENSSL_malloc(dst_len);
	if (dst_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", dst_len);
		OPENSSL_free(src_buf);
		af_alg_ecc_finish();
		return 0;
	}
	memset(dst_buf, 0, dst_len);

	af_alg_ecc_ctx.type = ALG_OP_ENCRYPT;
	memset(ivbuf, 0, sizeof(ivbuf));
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = ivbuf;
	msg.msg_controllen = sizeof(ivbuf) - iv_len + ecc_size;

	/* Set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg), &af_alg_ecc_ctx.type, 4);

	/* Set IV: group->field */
	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(offsetof(struct af_alg_iv, iv) + ecc_size);
	ivm = (void*)CMSG_DATA(cmsg);
	ivm->ivlen = ecc_size;
	memcpy(ivm->iv, group->field.d, BN_num_bytes(&group->field));

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	iov.iov_base = (void *)src_buf;
	iov.iov_len = src_len;

	ret = sendmsg(af_alg_ecc_ctx.op, &msg, 0);
	if (ret == -1) {
		E_ERR("sendmsg() failed! return %d \n", ret);
		OPENSSL_free(src_buf);
		OPENSSL_free(dst_buf);
		af_alg_ecc_finish();
		return 0;
	}

	ret = read(af_alg_ecc_ctx.op, dst_buf, dst_len);
	if (ret != dst_len)
		E_ERR("read() failed! return %d / %d\n", ret, dst_len);

	af_alg_convert_byte(dst_buf, ecc_size);
	BN_bin2bn(dst_buf, ecc_size, &r->X);
	af_alg_convert_byte(dst_buf+ecc_size, ecc_size);
	BN_bin2bn(dst_buf+ecc_size, ecc_size, &r->Y);

	OPENSSL_free(src_buf);
	OPENSSL_free(dst_buf);
	af_alg_ecc_finish();
	return 1;
}
#endif

/** Computes r = generator * g_scalar + point * p_scalar
 *           r = point * p_scalar, if g_scalar = 0
 *  \param  group    underlying EC_GROUP object
 *  \param  r        EC_POINT object for the result
 *  \param  g_scalar BIGNUM with the multiplier for the group generator (optional)
 *  \param  point    EC_POINT object with the first factor of the second summand
 *  \param  p_scalar BIGNUM with the second factor of the second summand
 *  \param  ctx      BN_CTX object (optional)
 *  \return 1 on success and 0 if an error occured
 */
static int af_alg_POINTs_mul(const EC_GROUP *group, EC_POINT *r, const BIGNUM *g_scalar,
	const EC_POINT *point, const BIGNUM *p_scalar, BN_CTX *ctx)
{
	int ret = 1;
	EC_POINT *tmp = NULL;

	if ((group == NULL) || (r == NULL) || (point == NULL) || (p_scalar == NULL)) {
		E_ERR("Invalid parameter: group: 0x%p, r: 0x%p, point: 0x%p, p_scalar: 0x%p\n",
			group, r, point, p_scalar);
		return 0;
	}
	E_DBG("len: a: %d, b: %d, order: %d, point(%d, %d, %d), p_scalar: %d\n",
		BN_num_bytes(&group->a), BN_num_bytes(&group->b), BN_num_bytes(&group->order),
		BN_num_bytes(&point->X), BN_num_bytes(&point->Y), BN_num_bytes(&point->Z),
		BN_num_bytes(p_scalar));

	if ((tmp = EC_POINT_new(group)) == NULL) {
		E_ERR("Failed to create point.\n");
		return 1;
	}

	/* 1. tmp = point * p_scalar */
	ret = af_alg_POINT_mul(group, tmp, point, p_scalar, ctx);
	if (ret != 1)
		return 0;

	if (g_scalar == NULL) {
		EC_POINT_copy(r, tmp);
		EC_POINT_free(tmp);
		return 1;
	}

	/* 2. r = group->generator * g_scalar */
	ret = af_alg_POINT_mul(group, r, group->generator, g_scalar, ctx);
	if (ret != 1)
		goto af_alg_POINTs_mul_err;

	EC_POINT_set_affine_coordinates_GF2m(group, r, &r->X, &r->Y, ctx);
	EC_POINT_set_affine_coordinates_GF2m(group, tmp, &tmp->X, &tmp->Y, ctx);

	/* 3. r = r + tmp */
	ret = EC_POINT_add(group, r, r, tmp, ctx);
	if (ret != 1)
		goto af_alg_POINTs_mul_err;

	EC_POINT_free(tmp);
	return 1;

af_alg_POINTs_mul_err:
	EC_POINT_free(tmp);
	return 0;
}

static int af_alg_ecdh_compute_key(void *out, size_t outlen, const EC_POINT *pub_key,
	EC_KEY *ecdh,
	void *(*KDF)(const void *in, size_t inlen, void *out, size_t *outlen))
{
	int ret = 1;
	BN_CTX *ctx;
	EC_POINT *tmp=NULL;
	BIGNUM *x=NULL, *y=NULL;
	const BIGNUM *priv_key;
	const EC_GROUP* group;
	size_t buflen, len;
	unsigned char *buf=NULL;

	if (outlen > INT_MAX)
	{
		E_ERR("The outlen is too large: %zd\n", outlen);
		return -1;
	}

	if ((ctx = BN_CTX_new()) == NULL)
		goto ecdh_err;
	BN_CTX_start(ctx);
	x = BN_CTX_get(ctx);
	y = BN_CTX_get(ctx);

	priv_key = EC_KEY_get0_private_key(ecdh);
	if (priv_key == NULL) {
		E_ERR("Fail to call EC_KEY_get0_private_key()\n");
		goto ecdh_err;
	}

	group = EC_KEY_get0_group(ecdh);
	if ((tmp=EC_POINT_new(group)) == NULL) {
		E_ERR("Failed to create point.\n");
		goto ecdh_err;
	}

	if (!af_alg_POINTs_mul(group, tmp, NULL, pub_key, priv_key, ctx))
	{
		E_ERR("---\n");
		goto ecdh_err;
	}
	BN_copy(x, &tmp->X);
	BN_copy(y, &tmp->Y);

	buflen = (EC_GROUP_get_degree(group) + 7)/8;
	len = BN_num_bytes(x);
	if (len > buflen)
		{
			E_ERR("---\n");
			goto ecdh_err;
		}
	if ((buf = OPENSSL_malloc(buflen)) == NULL)
		{
			E_ERR("---\n");
			goto ecdh_err;
		}

	memset(buf, 0, buflen - len);
	if (len != (size_t)BN_bn2bin(x, buf + buflen - len))
		{
			E_ERR("---\n");
			goto ecdh_err;
		}

	if (KDF != 0)
		{
		if (KDF(buf, buflen, out, &outlen) == NULL)
			{
				E_ERR("---\n");
				goto ecdh_err;
			}
		ret = outlen;
		}
	else
		{
		/* no KDF, just copy as much as we can */
		if (outlen > buflen)
			outlen = buflen;
		memcpy(out, buf, outlen);
		ret = outlen;
		}

ecdh_err:
	if (tmp) EC_POINT_free(tmp);
	if (ctx) BN_CTX_end(ctx);
	if (ctx) BN_CTX_free(ctx);
	if (buf) OPENSSL_free(buf);
	return ret;
}

static ECDH_METHOD af_alg_ecdh = {
	"Af_alg ECDH method",
	af_alg_ecdh_compute_key,
	0,
	NULL
};

#ifdef SUPPORT_CE_V3_2

/* r = (a * b) mod m */
int af_alg_BN_mod_mul(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, const BIGNUM *m,
	BN_CTX *ctx)
{
	int ret = 0;
	int mod_mul_size = 0;
	int src_len = 0;
	unsigned char *src_buf = NULL;
	unsigned char *dst_buf = NULL;

	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct iovec iov;
	char ivbuf[CMSG_SPACE(sizeof(af_alg_ecc_ctx.type))];

	if ((r == NULL) || (a == NULL) || (b == NULL) || (m == NULL)) {
		E_ERR("Invalid parameter: r: 0x%p, a: 0x%p, b: 0x%p, m: 0x%p\n", r, a, b, m);
		return 0;
	}
	E_DBG("Len: r %d, a %d, b %d, m %d \n", BN_num_bytes(r), BN_num_bytes(a),
											BN_num_bytes(b), BN_num_bytes(m));
	bn_check_top(a);
	bn_check_top(b);
	bn_check_top(m);

	mod_mul_size = BN_num_bytes(m) > 64 ? 128 : 64;

	/* Key: NULL */
	if (af_alg_asym_sock_init(&af_alg_ecc_ctx, NULL, mod_mul_size, "ecc_verify", 0) == 0)
		return 0;

	af_alg_ecc_ctx.type = ALG_OP_ENCRYPT;
	memset(ivbuf, 0, sizeof(ivbuf));
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = ivbuf;
	msg.msg_controllen = sizeof(ivbuf);

	/* Set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg), &af_alg_ecc_ctx.type, 4);

	/* Src: m + a + b */
	src_len = mod_mul_size*3;
	src_buf = OPENSSL_malloc(src_len);
	if (src_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", src_len);
		return 0;
	}
	memset(src_buf, 0, src_len);
	memcpy(src_buf, m->d, BN_num_bytes(m));
	memcpy(src_buf+mod_mul_size, a->d, BN_num_bytes(a));
	memcpy(src_buf+mod_mul_size*2, b->d, BN_num_bytes(b));

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	iov.iov_base = (void *)src_buf;
	iov.iov_len = src_len;

	/* Dst: r */
	dst_buf = OPENSSL_malloc(mod_mul_size);
	if (dst_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", mod_mul_size);
		return 0;
	}
	memset(dst_buf, 0, mod_mul_size);

	ret = sendmsg(af_alg_ecc_ctx.op, &msg, 0);
	if (ret == -1) {
		E_ERR("sendmsg() failed! return %d \n", ret);
		return 0;
	}

	ret = read(af_alg_ecc_ctx.op, dst_buf, mod_mul_size);
	if (ret != mod_mul_size) {
		E_ERR("read() failed! return %d / %d\n", ret, mod_mul_size);
		return 0;
	}

	BN_copy(r, m);
	memcpy(r->d, dst_buf, BN_num_bytes(r));

	OPENSSL_free(src_buf);
	OPENSSL_free(dst_buf);
	af_alg_ecc_finish();
	bn_check_top(r);
	return 1;
}

static ECDSA_SIG *af_alg_ecdsa_do_sign(const unsigned char *dgst, int dgst_len,
		const BIGNUM *in_kinv, const BIGNUM *in_r, EC_KEY *eckey)
{
	int ok = 0, i;
	BIGNUM *kinv=NULL, *m=NULL,*tmp=NULL,*order=NULL;
	const BIGNUM *ckinv;
	BN_CTX	   *ctx = NULL;
	const EC_GROUP *group = NULL;
	ECDSA_SIG  *sig;
	ECDSA_DATA *ecdsa;
	const BIGNUM *priv_key;

	int ret = 0;
	int ecc_size = 0;
	int src_len = 0;
	int dst_len = 0;
	unsigned char *src_buf = NULL;
	unsigned char *dst_buf = NULL;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct iovec iov;
	char ivbuf[CMSG_SPACE(sizeof(af_alg_ecc_ctx.type))];

	E_DBG("dgst: 0x%p, dgst_len: %d, in_kinv: 0x%p, in_r: 0x%p, eckey: 0x%p\n",
		dgst, dgst_len, in_kinv, in_r, eckey);

	ecdsa	 = ecdsa_check(eckey);
	group	 = EC_KEY_get0_group(eckey);
	priv_key = EC_KEY_get0_private_key(eckey);

	if (group == NULL || priv_key == NULL || ecdsa == NULL)
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_PASSED_NULL_PARAMETER);
		return NULL;
	}
	E_DBG("Len: priv_key %d, generator %d, a %d, order %d, field %d \n",
		BN_num_bytes(priv_key), BN_num_bytes(&group->generator->X),
		BN_num_bytes(&group->a), BN_num_bytes(&group->order), BN_num_bytes(&group->field));

	sig = ECDSA_SIG_new();
	if (!sig)
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_MALLOC_FAILURE);
		return NULL;
	}

	if ((ctx = BN_CTX_new()) == NULL || (order = BN_new()) == NULL
		|| (tmp = BN_new()) == NULL || (m = BN_new()) == NULL
		|| (kinv = BN_new()) == NULL)
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_MALLOC_FAILURE);
		goto err;
	}

	if (!EC_GROUP_get_order(group, order, ctx))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_EC_LIB);
		goto err;
	}
	i = BN_num_bits(order);
	E_DBG("order bits: %d\n", i);
	/* Need to truncate digest if it is too long: first truncate whole
	 * bytes.
	 */
	if (8 * dgst_len > i)
		dgst_len = (i + 7)/8;
	if (!BN_bin2bn(dgst, dgst_len, m))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_BN_LIB);
		goto err;
	}

	/* If still too long truncate remaining bits with a shift */
	if ((8 * dgst_len > i) && !BN_rshift(m, m, 8 - (i & 0x7)))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_BN_LIB);
		goto err;
	}

	if (in_kinv == NULL || in_r == NULL) {
		/* get random k */
		do {
			if (!BN_rand_range(kinv, order)) {
				ECDSAerr(ECDSA_F_ECDSA_SIGN_SETUP,
				 ECDSA_R_RANDOM_NUMBER_GENERATION_FAILED);
				goto err;
			}
		} while (BN_is_zero(kinv));
		ckinv = kinv;
	}
	else {
		ckinv = in_kinv;
		if (BN_copy(sig->r, in_r) == NULL)
		{
			ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_MALLOC_FAILURE);
			goto err;
		}
	}

	ecc_size = af_alg_get_ecc_size(group->curve_name);

	/* Key: NULL */
	if (af_alg_asym_sock_init(&af_alg_ecc_ctx, NULL, ecc_size, "ecc_sign", group->curve_name) == 0)
			return 0;

	/* Src buf: kinv + group->field + group->a + generator->x + generator->y
			+ group->order + priv_key + dgst */
	src_len = ecc_size*8;
	src_buf = OPENSSL_malloc(src_len);
	if (src_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", src_len);
		goto err;
	}
	memset(src_buf, 0, src_len);
	memcpy(src_buf, ckinv->d, BN_num_bytes(ckinv));
	memcpy(src_buf+ecc_size, group->field.d, BN_num_bytes(&group->field));
	memcpy(src_buf+ecc_size*2, group->a.d, BN_num_bytes(&group->a));
	af_alg_point2bin(group, src_buf+ecc_size*3, group->generator, ctx, ecc_size);
	memcpy(src_buf+ecc_size*5, order->d, BN_num_bytes(order));
	memcpy(src_buf+ecc_size*6, priv_key->d, BN_num_bytes(priv_key));
	if (ecc_size > dgst_len)
		memcpy(src_buf+ecc_size*7, m->d, dgst_len); // Zero the high bit
	else
		memcpy(src_buf+ecc_size*7, m->d, ecc_size);

	/* Dst buf: sig->r + sig->s */
	dst_len = ecc_size*2;
	dst_buf = OPENSSL_malloc(dst_len);
	if (dst_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", dst_len);
		goto err;
	}
	memset(dst_buf, 0, dst_len);

	af_alg_ecc_ctx.type = ALG_OP_ENCRYPT;
	memset(ivbuf, 0, sizeof(ivbuf));
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = ivbuf;
	msg.msg_controllen = sizeof(ivbuf);

	/* Set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg), &af_alg_ecc_ctx.type, 4);

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	iov.iov_base = (void *)src_buf;
	iov.iov_len = src_len;

	ret = sendmsg(af_alg_ecc_ctx.op, &msg, 0);
	if (ret == -1) {
		E_ERR("sendmsg() failed! return %d \n", ret);
		goto err;
	}

	ret = read(af_alg_ecc_ctx.op, dst_buf, dst_len);
	if (ret != dst_len) {
		E_ERR("read() failed! return %d / %d\n", ret, dst_len);
		goto err;
	}

	E_DBG("The dst data: \n");
	print_hex(dst_buf, dst_len, 0);

	af_alg_convert_byte(dst_buf, ecc_size);
	BN_bin2bn(dst_buf, ecc_size, sig->r);
	af_alg_convert_byte(dst_buf+ecc_size, ecc_size);
	BN_bin2bn(dst_buf+ecc_size, ecc_size, sig->s);

	ok = 1;
err:
	if (src_buf)
		OPENSSL_free(src_buf);
	if (dst_buf)
		OPENSSL_free(dst_buf);
	af_alg_ecc_finish();

	if (!ok)
	{
		ECDSA_SIG_free(sig);
		sig = NULL;
	}
	if (ctx)
		BN_CTX_free(ctx);
	if (m)
		BN_clear_free(m);
	if (tmp)
		BN_clear_free(tmp);
	if (order)
		BN_free(order);
	if (kinv)
		BN_clear_free(kinv);
	return sig;
}

#else

/* r = (a * b) mod m */
int af_alg_BN_mod_mul(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, const BIGNUM *m,
	BN_CTX *ctx)
{
	int ret = 0;
	int mod_mul_size = 0;
	int src_len = 0;
	unsigned char *src_buf = NULL;
	unsigned char *dst_buf = NULL;

	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct af_alg_iv *ivm;
	struct iovec iov;
	int iv_len = RSA_MAX_LEN/8; /* Maximum length of RSA key. */
	char ivbuf[CMSG_SPACE(sizeof(af_alg_ecc_ctx.type)) + CMSG_SPACE(offsetof(struct af_alg_iv, iv) + iv_len)];

	if ((r == NULL) || (a == NULL) || (b == NULL) || (m == NULL)) {
		E_ERR("Invalid parameter: r: 0x%p, a: 0x%p, b: 0x%p, m: 0x%p\n", r, a, b, m);
		return 0;
	}
	E_DBG("Len: r %d, a %d, b %d, m %d \n", BN_num_bytes(r), BN_num_bytes(a),
											BN_num_bytes(b), BN_num_bytes(m));
	bn_check_top(a);
	bn_check_top(b);
	bn_check_top(m);

	mod_mul_size = BN_num_bytes(m) > 64 ? 128 : 64;

	/* Key: NULL */
	if (af_alg_asym_sock_init(&af_alg_ecc_ctx, NULL, mod_mul_size, "ecc_verify", 0) == 0)
		return 0;

	af_alg_ecc_ctx.type = ALG_OP_ENCRYPT;
	memset(ivbuf, 0, sizeof(ivbuf));
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = ivbuf;
	msg.msg_controllen = sizeof(ivbuf) - iv_len + mod_mul_size;

	/* Set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg), &af_alg_ecc_ctx.type, 4);

	/* Set IV: m */
	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(offsetof(struct af_alg_iv, iv) + mod_mul_size);
	ivm = (void*)CMSG_DATA(cmsg);
	ivm->ivlen = mod_mul_size;
	memcpy(ivm->iv, m->d, BN_num_bytes(m));

	/* Src: a + b */
	src_len = mod_mul_size*2;
	src_buf = OPENSSL_malloc(src_len);
	if (src_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", src_len);
		return 0;
	}
	memset(src_buf, 0, src_len);
	memcpy(src_buf, a->d, BN_num_bytes(a));
	memcpy(src_buf+mod_mul_size, b->d, BN_num_bytes(b));

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	iov.iov_base = (void *)src_buf;
	iov.iov_len = src_len;

	/* Dst: r */
	dst_buf = OPENSSL_malloc(mod_mul_size);
	if (dst_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", mod_mul_size);
		return 0;
	}
	memset(dst_buf, 0, mod_mul_size);

	ret = sendmsg(af_alg_ecc_ctx.op, &msg, 0);
	if (ret == -1) {
		E_ERR("sendmsg() failed! return %d \n", ret);
		return 0;
	}

	ret = read(af_alg_ecc_ctx.op, dst_buf, mod_mul_size);
	if (ret != mod_mul_size) {
		E_ERR("read() failed! return %d / %d\n", ret, mod_mul_size);
		return 0;
	}

	BN_copy(r, m);
	memcpy(r->d, dst_buf, BN_num_bytes(r));

	OPENSSL_free(src_buf);
	OPENSSL_free(dst_buf);
	af_alg_ecc_finish();
	bn_check_top(r);
	return 1;
}

static ECDSA_SIG *af_alg_ecdsa_do_sign(const unsigned char *dgst, int dgst_len,
		const BIGNUM *in_kinv, const BIGNUM *in_r, EC_KEY *eckey)
{
	int ok = 0, i;
	BIGNUM *kinv=NULL, *m=NULL,*tmp=NULL,*order=NULL;
	const BIGNUM *ckinv;
	BN_CTX	   *ctx = NULL;
	const EC_GROUP *group = NULL;
	ECDSA_SIG  *sig;
	ECDSA_DATA *ecdsa;
	const BIGNUM *priv_key;

	int ret = 0;
	int ecc_size = 0;
	int src_len = 0;
	int dst_len = 0;
	unsigned char *src_buf = NULL;
	unsigned char *dst_buf = NULL;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct af_alg_iv *ivm;
	struct iovec iov;
	int iv_len = RSA_MAX_LEN/8; /* Maximum length of RSA key. */
	char ivbuf[CMSG_SPACE(sizeof(af_alg_ecc_ctx.type)) + CMSG_SPACE(offsetof(struct af_alg_iv, iv) + iv_len)];

	E_DBG("dgst: 0x%p, dgst_len: %d, in_kinv: 0x%p, in_r: 0x%p, eckey: 0x%p\n",
		dgst, dgst_len, in_kinv, in_r, eckey);

	ecdsa	 = ecdsa_check(eckey);
	group	 = EC_KEY_get0_group(eckey);
	priv_key = EC_KEY_get0_private_key(eckey);

	if (group == NULL || priv_key == NULL || ecdsa == NULL)
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_PASSED_NULL_PARAMETER);
		return NULL;
	}
	E_DBG("Len: priv_key %d, generator %d, a %d, order %d, field %d \n",
		BN_num_bytes(priv_key), BN_num_bytes(&group->generator->X),
		BN_num_bytes(&group->a), BN_num_bytes(&group->order), BN_num_bytes(&group->field));

	sig = ECDSA_SIG_new();
	if (!sig)
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_MALLOC_FAILURE);
		return NULL;
	}

	if ((ctx = BN_CTX_new()) == NULL || (order = BN_new()) == NULL
		|| (tmp = BN_new()) == NULL || (m = BN_new()) == NULL
		|| (kinv = BN_new()) == NULL)
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_MALLOC_FAILURE);
		goto err;
	}

	if (!EC_GROUP_get_order(group, order, ctx))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_EC_LIB);
		goto err;
	}
	i = BN_num_bits(order);
	E_DBG("order bits: %d\n", i);
	/* Need to truncate digest if it is too long: first truncate whole
	 * bytes.
	 */
	if (8 * dgst_len > i)
		dgst_len = (i + 7)/8;
	if (!BN_bin2bn(dgst, dgst_len, m))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_BN_LIB);
		goto err;
	}

	/* If still too long truncate remaining bits with a shift */
	if ((8 * dgst_len > i) && !BN_rshift(m, m, 8 - (i & 0x7)))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_BN_LIB);
		goto err;
	}

	if (in_kinv == NULL || in_r == NULL) {
		/* get random k */
		do {
			if (!BN_rand_range(kinv, order)) {
				ECDSAerr(ECDSA_F_ECDSA_SIGN_SETUP,
				 ECDSA_R_RANDOM_NUMBER_GENERATION_FAILED);
				goto err;
			}
		} while (BN_is_zero(kinv));
		ckinv = kinv;
	}
	else {
		ckinv = in_kinv;
		if (BN_copy(sig->r, in_r) == NULL)
		{
			ECDSAerr(ECDSA_F_ECDSA_DO_SIGN, ERR_R_MALLOC_FAILURE);
			goto err;
		}
	}

	ecc_size = af_alg_get_ecc_size(group->curve_name);

	/* Key: ckinv */
	if (af_alg_asym_sock_init(&af_alg_ecc_ctx, ckinv, ecc_size, "ecc_sign", group->curve_name) == 0)
			return 0;

	/* Src buf: group->a + generator->x + generator->y + dgst + priv_key */
	src_len = ecc_size*5;
	src_buf = OPENSSL_malloc(src_len);
	if (src_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", src_len);
		goto err;
	}
	memset(src_buf, 0, src_len);
	memcpy(src_buf, group->a.d, BN_num_bytes(&group->a));
	af_alg_point2bin(group, src_buf+ecc_size, group->generator, ctx, ecc_size);
	if (ecc_size > dgst_len)
		memcpy(src_buf+ecc_size*3, m->d, dgst_len); // Zero the high bit
	else
		memcpy(src_buf+ecc_size*3, m->d, ecc_size);

	memcpy(src_buf+ecc_size*4, priv_key->d, BN_num_bytes(priv_key));

	/* Dst buf: (ckinv*generator)->x + (ckinv*generator)->y + sig->r + sig->s */
	dst_len = ecc_size*4;
	dst_buf = OPENSSL_malloc(dst_len);
	if (dst_buf == NULL) {
		E_ERR("Failed to OPENSSL_malloc(%d). \n", dst_len);
		goto err;
	}
	memset(dst_buf, 0, dst_len);

	af_alg_ecc_ctx.type = ALG_OP_ENCRYPT;
	memset(ivbuf, 0, sizeof(ivbuf));
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = ivbuf;
	msg.msg_controllen = sizeof(ivbuf) - iv_len + ecc_size*2;

	/* Set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg), &af_alg_ecc_ctx.type, 4);

	/* IV: group->order + group->field */
	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(offsetof(struct af_alg_iv, iv) + ecc_size*2);
	ivm = (void*)CMSG_DATA(cmsg);
	ivm->ivlen = ecc_size*2;
	memcpy(ivm->iv, order->d, BN_num_bytes(order));
	memcpy(ivm->iv+ecc_size, group->field.d, BN_num_bytes(&group->field));

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	iov.iov_base = (void *)src_buf;
	iov.iov_len = src_len;

	ret = sendmsg(af_alg_ecc_ctx.op, &msg, 0);
	if (ret == -1) {
		E_ERR("sendmsg() failed! return %d \n", ret);
		goto err;
	}

	ret = read(af_alg_ecc_ctx.op, dst_buf, dst_len);
	if (ret != dst_len) {
		E_ERR("read() failed! return %d / %d\n", ret, dst_len);
		goto err;
	}

	E_DBG("The dst data: \n");
	print_hex(dst_buf, dst_len, 0);

	af_alg_convert_byte(dst_buf+ecc_size*2, ecc_size);
	BN_bin2bn(dst_buf+ecc_size*2, ecc_size, sig->r);
	af_alg_convert_byte(dst_buf+ecc_size*3, ecc_size);
	BN_bin2bn(dst_buf+ecc_size*3, ecc_size, sig->s);

	ok = 1;
err:
	if (src_buf)
		OPENSSL_free(src_buf);
	if (dst_buf)
		OPENSSL_free(dst_buf);
	af_alg_ecc_finish();

	if (!ok)
	{
		ECDSA_SIG_free(sig);
		sig = NULL;
	}
	if (ctx)
		BN_CTX_free(ctx);
	if (m)
		BN_clear_free(m);
	if (tmp)
		BN_clear_free(tmp);
	if (order)
		BN_free(order);
	if (kinv)
		BN_clear_free(kinv);
	return sig;
}

#endif

static int af_alg_ecdsa_do_verify(const unsigned char *dgst, int dgst_len,
		const ECDSA_SIG *sig, EC_KEY *eckey)
{
	int ret = -1, i;
	BN_CTX	 *ctx;
	BIGNUM	 *order, *u1, *u2, *m, *X;
	EC_POINT *point = NULL;
	const EC_GROUP *group;
	const EC_POINT *pub_key;

	E_DBG("Enter \n");

	/* check input values */
	if (eckey == NULL || (group = EC_KEY_get0_group(eckey)) == NULL ||
		(pub_key = EC_KEY_get0_public_key(eckey)) == NULL || sig == NULL)
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ECDSA_R_MISSING_PARAMETERS);
		return -1;
	}

	ctx = BN_CTX_new();
	if (!ctx)
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_MALLOC_FAILURE);
		return -1;
	}
	BN_CTX_start(ctx);
	order = BN_CTX_get(ctx);
	u1	  = BN_CTX_get(ctx);
	u2	  = BN_CTX_get(ctx);
	m	  = BN_CTX_get(ctx);
	X	  = BN_CTX_get(ctx);
	if (!X)
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_BN_LIB);
		goto err;
	}

	if (!EC_GROUP_get_order(group, order, ctx))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_EC_LIB);
		goto err;
	}

	if (BN_is_zero(sig->r)			|| BN_is_negative(sig->r) ||
		BN_ucmp(sig->r, order) >= 0 || BN_is_zero(sig->s)  ||
		BN_is_negative(sig->s)		|| BN_ucmp(sig->s, order) >= 0)
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ECDSA_R_BAD_SIGNATURE);
		ret = 0;	/* signature is invalid */
		goto err;
	}
	/* calculate tmp1 = inv(S) mod order */
	if (!BN_mod_inverse(u2, sig->s, order, ctx))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_BN_LIB);
		goto err;
	}
	/* digest -> m */
	i = BN_num_bits(order);
	/* Need to truncate digest if it is too long: first truncate whole
	 * bytes.
	 */
	if (8 * dgst_len > i)
		dgst_len = (i + 7)/8;
	if (!BN_bin2bn(dgst, dgst_len, m))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_BN_LIB);
		goto err;
	}
	/* If still too long truncate remaining bits with a shift */
	if ((8 * dgst_len > i) && !BN_rshift(m, m, 8 - (i & 0x7)))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_BN_LIB);
		goto err;
	}

	/* u1 = m * tmp mod order */
	if (!BN_mod_mul(u1, m, u2, order, ctx))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_BN_LIB);
		goto err;
	}

	/* u2 = r * w mod q */
	if (!BN_mod_mul(u2, sig->r, u2, order, ctx))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_BN_LIB);
		goto err;
	}

	if ((point = EC_POINT_new(group)) == NULL)
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_MALLOC_FAILURE);
		goto err;
	}
	if (!af_alg_POINTs_mul(group, point, u1, pub_key, u2, ctx))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_EC_LIB);
		goto err;
	}

	if (EC_METHOD_get_field_type(EC_GROUP_method_of(group)) == NID_X9_62_prime_field)
	{
		if (!EC_POINT_get_affine_coordinates_GFp(group,
			point, X, NULL, ctx))
		{
			ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_EC_LIB);
			goto err;
		}
	}
#ifndef OPENSSL_NO_EC2M
	else /* NID_X9_62_characteristic_two_field */
	{
		if (!EC_POINT_get_affine_coordinates_GF2m(group,
			point, X, NULL, ctx))
		{
			ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_EC_LIB);
			goto err;
		}
	}
#endif
	if (!BN_nnmod(u1, X, order, ctx))
	{
		ECDSAerr(ECDSA_F_ECDSA_DO_VERIFY, ERR_R_BN_LIB);
		goto err;
	}
	/*	if the signature is correct u1 is equal to sig->r */
	ret = (BN_ucmp(u1, sig->r) == 0);
err:
	BN_CTX_end(ctx);
	BN_CTX_free(ctx);
	if (point)
		EC_POINT_free(point);
	return ret;
}

static ECDSA_METHOD af_alg_ecdsa = {
	"Af_alg ECDSA method",
	af_alg_ecdsa_do_sign,
	NULL,
	af_alg_ecdsa_do_verify,
	0,
	NULL
};

#endif

int af_alg_init(ENGINE * engine)
{
	int sock;
	if((sock = socket(AF_ALG, SOCK_SEQPACKET, 0)) == -1)
		return 0;
	close(sock);

	return 1;
}

int af_alg_finish(ENGINE * engine)
{
	return 1;
}
/* The definitions for control commands specific to this engine */
#define AF_ALG_CMD_CIPHERS	ENGINE_CMD_BASE
#define AF_ALG_CMD_DIGESTS	(ENGINE_CMD_BASE + 1)
#define AF_ALG_CMD_RANDS	(ENGINE_CMD_BASE + 2)
#define AF_ALG_CMD_RSA		(ENGINE_CMD_BASE + 3)
#define AF_ALG_CMD_DH		(ENGINE_CMD_BASE + 4)
#define AF_ALG_CMD_ECDH		(ENGINE_CMD_BASE + 5)

static const ENGINE_CMD_DEFN af_alg_cmd_defns[] = {
	{AF_ALG_CMD_CIPHERS,"CIPHERS","which ciphers to run",ENGINE_CMD_FLAG_STRING},
	{AF_ALG_CMD_DIGESTS,"DIGESTS","which digests to run",ENGINE_CMD_FLAG_STRING},
	{AF_ALG_CMD_RANDS,"RAND","which rand to run",ENGINE_CMD_FLAG_STRING},
	{AF_ALG_CMD_RSA,"RSA","RSA of af_alg engine",ENGINE_CMD_FLAG_STRING},
	{AF_ALG_CMD_DH,"DH","DH of af_alg engine",ENGINE_CMD_FLAG_STRING},
	{AF_ALG_CMD_ECDH,"ECDH","ECDH of af_alg engine",ENGINE_CMD_FLAG_STRING},
	{0, NULL, NULL, 0}
};
static int cipher_nid(const EVP_CIPHER *c)
{
	return EVP_CIPHER_nid(c);
}
static int digest_nid(const EVP_MD *d)
{
	return EVP_MD_type(d);
}
static bool names_to_nids(const char *names, const void*(*by_name)(const char *), int (*to_nid)(const void *), int **rnids, int *rnum, int *nids, int num)
{
	char *str, *r;
	char *c = NULL;
	r = str = strdup(names);
	while( (c = strtok_r(r, " ", &r)) != NULL )
	{
		const void *ec = by_name(c);
		if (strncmp(c, "hmac-sha1", 9) == 0)
			ec = &af_alg_md_hmac_sha1;
		if (strncmp(c, "hmac-sha256", 11) == 0)
			ec = &af_alg_md_hmac_sha256;
		if (ec == NULL)
			{
			E_DBG("no such evp :ec is NULL please check the names string item\n");
			/* the cipher/digest is unknown */
			return false;
			}
			
		if( nid_in_nids(to_nid(ec), nids, num) == false )
			{
				E_DBG("nid not in nids[num] :to_nid(ec) is %d  num is %d",to_nid(ec),num);
				/* we do not support the cipher */
				return false;
			}


		if((*rnids = realloc(*rnids, (*rnum+1)*sizeof(int))) == NULL)
			return false;
		(*rnids)[*rnum]=to_nid(ec);
		*rnum = *rnum+1;
	}
	return true;
}

static int af_alg_ctrl(ENGINE *e, int cmd, long i, void *p, void (*f)())
{
	OpenSSL_add_all_algorithms();
	switch( cmd )
	{
	case AF_ALG_CMD_CIPHERS:
		if( p == NULL )
			return 1;
		if( names_to_nids(p, (void *)EVP_get_cipherbyname, (void *)cipher_nid,\
			&af_alg_cipher_nids, &af_alg_cipher_nids_num, af_alg_cipher_all_nids,\
			af_alg_cipher_all_nids_num) == false )
			{
				E_DBG("p:%s has not supported cipher algorithm\n",(unsigned char*)p);
				return 0;
			}	
		ENGINE_unregister_ciphers(e);
		ENGINE_register_ciphers(e);
		return 1;
	case AF_ALG_CMD_DIGESTS:
		if( p == NULL )
			return 1;
		if( names_to_nids(p, (void *)EVP_get_digestbyname, (void *)digest_nid, &af_alg_digest_nids, &af_alg_digest_nids_num, af_alg_digest_all_nids, af_alg_digest_all_nids_num) == false )
			return 0;
		ENGINE_unregister_digests(e);
		ENGINE_register_digests(e);
		return 1;
	case AF_ALG_CMD_RANDS:
		ENGINE_unregister_digests(e);
		ENGINE_register_digests(e);
		return 1;
	case AF_ALG_CMD_RSA:
	case AF_ALG_CMD_DH:
	case AF_ALG_CMD_ECDH:
		ENGINE_unregister_digests(e);
		ENGINE_register_digests(e);
		return 1;
	default:
		break;
	}
	return 0;
}

static int af_alg_bind_helper(ENGINE * e)
{
	if( !ENGINE_set_id(e, AF_ALG_ENGINE_ID) ||
		!ENGINE_set_init_function(e, af_alg_init) ||
		!ENGINE_set_finish_function(e, af_alg_finish) ||
		!ENGINE_set_name(e, AF_ALG_ENGINE_NAME) ||
		!ENGINE_set_ciphers (e, af_alg_ciphers) ||
		!ENGINE_set_digests (e, af_alg_digests) ||
		!ENGINE_set_RAND(e, &af_alg_random) ||
#ifdef SS_RSA_ENABLE
		!ENGINE_set_RSA(e, &af_alg_rsa) ||
#endif
#ifdef SS_DH_ENABLE
		!ENGINE_set_DH(e, &af_alg_dh) ||
#endif
#ifdef SS_ECC_ENABLE
		!ENGINE_set_ECDH(e, &af_alg_ecdh) ||
		!ENGINE_set_ECDSA(e, &af_alg_ecdsa) ||
#endif
		!ENGINE_set_ctrl_function(e, af_alg_ctrl) ||
		!ENGINE_set_cmd_defns(e, af_alg_cmd_defns))
		return 0;
	else
		return 1;
}

ENGINE *ENGINE_af_alg(void)
{
	ENGINE *eng = ENGINE_new();
	if( !eng )
		return NULL;

	if( !af_alg_bind_helper(eng) )
	{
		ENGINE_free(eng);
		return NULL;
	}
	return eng;
}


void ENGINE_load_af_alg(void)
{
	ENGINE *toadd = ENGINE_af_alg();
	E_DBG("toadd = %p\n", toadd);
	
	if(!toadd) return;
	ENGINE_add(toadd);
	ENGINE_free(toadd);
	ERR_clear_error();
}


static int af_alg_bind_fn(ENGINE *e, const char *id)
{
	E_DBG("\n");
	if( id && (strcmp(id, AF_ALG_ENGINE_ID) != 0) )
		return 0;

	if( !af_alg_bind_helper(e) )
		return 0;

	return 1;
}

IMPLEMENT_DYNAMIC_CHECK_FN();
IMPLEMENT_DYNAMIC_BIND_FN(af_alg_bind_fn);

static int af_alg_aes_init_key (EVP_CIPHER_CTX *ctx, const unsigned char *key, struct sockaddr_alg *sa)
{
	int keylen = EVP_CIPHER_CTX_key_length(ctx);
	struct af_alg_cipher_data *acd = (struct af_alg_cipher_data *)ctx->cipher_data;

	E_DBG("keylen = %d\n", keylen);
	acd->op = -1;

	if( ctx->encrypt )
		acd->type = ALG_OP_ENCRYPT;
	else
		acd->type = ALG_OP_DECRYPT;

	acd->tfmfd = socket(AF_ALG, SOCK_SEQPACKET, 0);
	if (acd->tfmfd == -1) {
		E_ERR("socket() failed! [%d]: %s\n", errno, strerror(errno));
		return 0;
	}
	
	if (bind(acd->tfmfd, (struct sockaddr*)sa, sizeof(*sa)) == -1) {
		E_ERR("bind() failed! [%d]: %s\n", errno, strerror(errno));
		return 0;
	}

	if (setsockopt(acd->tfmfd, SOL_ALG, ALG_SET_KEY, key, keylen) == -1) {
		E_ERR("setsockopt() failed! [%d]: %s\n", errno, strerror(errno));
		return 0;
	}

	return 1;
}

static int af_alg_aes_cbc_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "cbc(aes)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}

static int af_alg_aes_ecb_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "ecb(aes)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}

#ifdef SS_CTR_MODE_ENABLE
static int af_alg_aes_ctr_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "ctr(aes)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}
#endif

#ifdef SS_CTS_MODE_ENABLE
static int af_alg_aes_cts_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "cts(aes)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}
#endif

#ifdef SS_XTS_MODE_ENABLE
static int af_alg_aes_xts_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "xts(aes)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}
#endif

#ifdef SS_OFB_MODE_ENABLE
static int af_alg_aes_ofb128_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "ofb(aes)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}
#endif

#ifdef SS_CFB_MODE_ENABLE
static int af_alg_aes_cfb1_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "cfb1(aes)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}

static int af_alg_aes_cfb8_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "cfb8(aes)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}

static int af_alg_aes_cfb128_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "cfb128(aes)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}
#endif

int af_alg_aes_cleanup_key(EVP_CIPHER_CTX *ctx)
{
	struct af_alg_cipher_data *acd = (struct af_alg_cipher_data *)ctx->cipher_data;
	E_DBG("\n");
	if (acd->tfmfd > 0)
		close(acd->tfmfd);
	if (acd->op > 0)
		close(acd->op);
	return 1;
}

static int af_alg_aes_cbc_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	struct af_alg_cipher_data *acd = (struct af_alg_cipher_data *)ctx->cipher_data;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct af_alg_iv *ivm;
	struct iovec iov;
	int iv_len = EVP_CIPHER_CTX_iv_length(ctx);
	char buf[CMSG_SPACE(sizeof(acd->type)) + CMSG_SPACE(offsetof(struct af_alg_iv, iv) + iv_len)];
	ssize_t len;
	unsigned char save_iv[iv_len];

	memset(buf, 0, sizeof(buf));
	E_DBG("nbytes = %d\n", nbytes);

	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = buf;
	msg.msg_controllen = sizeof(buf);
	if( acd->op == -1 )
	{
		if((acd->op = accept(acd->tfmfd, NULL, 0)) == -1)
			return 0;
	}
	/* set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg),&acd->type, 4);

	/* set IV - or update if it was set before */
	if(!ctx->encrypt)
		memcpy(save_iv, in_arg + nbytes - iv_len, iv_len);

	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(offsetof(struct af_alg_iv, iv) + iv_len);
	ivm = (void*)CMSG_DATA(cmsg);
	ivm->ivlen = iv_len;
	memcpy(ivm->iv, ctx->iv, iv_len);

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	unsigned int todo = nbytes;
	unsigned int done = 0;
	while( todo-done > 0 )
	{
		iov.iov_base = (void *)(in_arg + done);
		iov.iov_len = todo-done;

		if((len = sendmsg(acd->op, &msg, 0)) == -1)
			return 0;

		if (read(acd->op, out_arg+done, len) != len)
			return 0;
		
		/* do not update IV for following chunks */
		msg.msg_controllen = 0;
		done += len;
	}

	/* copy IV for next iteration */
	if(ctx->encrypt)
		memcpy(ctx->iv, out_arg + done - iv_len, iv_len);
	else
		memcpy(ctx->iv, save_iv, iv_len);
	return 1;
}

static int af_alg_aes_ecb_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	struct af_alg_cipher_data *acd = (struct af_alg_cipher_data *)ctx->cipher_data;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct iovec iov;
	char buf[CMSG_SPACE(sizeof(acd->type))];
	ssize_t len;

	memset(buf, 0, sizeof(buf));
	E_DBG("nbytes = %d\n", nbytes);

	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = buf;
	msg.msg_controllen = sizeof(buf);
	if( acd->op == -1 )
	{
		if((acd->op = accept(acd->tfmfd, NULL, 0)) == -1)
			return 0;
	}
	/* set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg),&acd->type, 4);

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	unsigned int todo = nbytes;
	unsigned int done = 0;
	while( todo-done > 0 )
	{
		iov.iov_base = (void *)(in_arg + done);
		iov.iov_len = todo-done;

		if((len = sendmsg(acd->op, &msg, 0)) == -1)
			return 0;
		E_DBG("sendmsg() return %d \n", len);

		if (read(acd->op, out_arg+done, len) != len)
			return 0;
		
		/* do not update IV for following chunks */
		msg.msg_controllen = 0;
		done += len;
	}

	return 1;
}

#ifdef SS_CTR_MODE_ENABLE
static int af_alg_aes_ctr_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	return af_alg_aes_cbc_ciphers(ctx, out_arg, in_arg, nbytes);
}
#endif

#ifdef SS_CTS_MODE_ENABLE
static int af_alg_aes_cts_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	return af_alg_aes_cbc_ciphers(ctx, out_arg, in_arg, nbytes);
}
#endif

#ifdef SS_XTS_MODE_ENABLE
static int af_alg_aes_xts_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	return af_alg_aes_cbc_ciphers(ctx, out_arg, in_arg, nbytes);
}
#endif

#ifdef SS_OFB_MODE_ENABLE
static int af_alg_aes_ofb128_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	return af_alg_aes_cbc_ciphers(ctx, out_arg, in_arg, nbytes);
}
#endif

#ifdef SS_CFB_MODE_ENABLE
static int af_alg_aes_cfb1_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	return af_alg_aes_cbc_ciphers(ctx, out_arg, in_arg, nbytes);
}

static int af_alg_aes_cfb8_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	return af_alg_aes_cbc_ciphers(ctx, out_arg, in_arg, nbytes);
}

static int af_alg_aes_cfb128_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	return af_alg_aes_cbc_ciphers(ctx, out_arg, in_arg, nbytes);
}
#endif

#define EVP_CIPHER_block_size_AES_ECB	AES_BLOCK_SIZE
#define EVP_CIPHER_block_size_AES_CBC	AES_BLOCK_SIZE
#define EVP_CIPHER_block_size_AES_OFB	1
#define EVP_CIPHER_block_size_AES_CFB	1
#define EVP_CIPHER_block_size_AES_CTR	1
#define EVP_CIPHER_block_size_AES_CTS	1
#define EVP_CIPHER_block_size_AES_XTS	1
#define EVP_CIPHER_block_size_AES_CBC_HMAC_SHA1	AES_BLOCK_SIZE

#define EVP_CIPH_CBC_HMAC_SHA1_MODE		EVP_CIPH_CBC_MODE

#define	DECLARE_AES_EVP(ksize,lmode,umode)                  \
static const EVP_CIPHER af_alg_aes_##ksize##_##lmode = {    \
	.nid = NID_aes_##ksize##_##lmode,                       \
	.block_size = EVP_CIPHER_block_size_AES_##umode,	    \
	.key_len = AES_KEY_SIZE_##ksize,                        \
	.iv_len = AES_BLOCK_SIZE,                     		    \
	.flags = 0 | EVP_CIPH_##umode##_MODE,                   \
	.init = af_alg_aes_##lmode##_init_key,                  \
	.do_cipher = af_alg_aes_##lmode##_ciphers,              \
	.cleanup = af_alg_aes_cleanup_key,                      \
	.ctx_size = sizeof(struct af_alg_cipher_data),          \
	.set_asn1_parameters = EVP_CIPHER_set_asn1_iv,          \
	.get_asn1_parameters = EVP_CIPHER_get_asn1_iv,          \
	.ctrl = NULL,                                           \
	.app_data = NULL                                        \
}

DECLARE_AES_EVP(128,ecb,ECB);
DECLARE_AES_EVP(192,ecb,ECB);
DECLARE_AES_EVP(256,ecb,ECB);
DECLARE_AES_EVP(128,cbc,CBC);
DECLARE_AES_EVP(192,cbc,CBC);
DECLARE_AES_EVP(256,cbc,CBC);
#ifdef SS_CTR_MODE_ENABLE
DECLARE_AES_EVP(128,ctr,CTR);
DECLARE_AES_EVP(192,ctr,CTR);
DECLARE_AES_EVP(256,ctr,CTR);
#endif
#ifdef SS_CTS_MODE_ENABLE
DECLARE_AES_EVP(128,cts,CTS);
DECLARE_AES_EVP(192,cts,CTS);
DECLARE_AES_EVP(256,cts,CTS);
#endif
#ifdef SS_XTS_MODE_ENABLE
DECLARE_AES_EVP(128, xts, XTS);
DECLARE_AES_EVP(256, xts, XTS);
#endif
#ifdef SS_OFB_MODE_ENABLE
DECLARE_AES_EVP(128, ofb128, OFB);
DECLARE_AES_EVP(192, ofb128, OFB);
DECLARE_AES_EVP(256, ofb128, OFB);
#endif
#ifdef SS_OFB_MODE_ENABLE
DECLARE_AES_EVP(128, cfb1, CFB);
DECLARE_AES_EVP(128, cfb8, CFB);
DECLARE_AES_EVP(128, cfb128, CFB);
DECLARE_AES_EVP(192, cfb1, CFB);
DECLARE_AES_EVP(192, cfb8, CFB);
DECLARE_AES_EVP(192, cfb128, CFB);
DECLARE_AES_EVP(256, cfb1, CFB);
DECLARE_AES_EVP(256, cfb8, CFB);
DECLARE_AES_EVP(256, cfb128, CFB);
#endif

static int af_alg_des_cbc_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "cbc(des)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}

static int af_alg_des_ecb_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "ecb(des)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}

static int af_alg_des_cbc_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	return af_alg_aes_cbc_ciphers(ctx, out_arg, in_arg, nbytes);
}

static int af_alg_des_ecb_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	return af_alg_aes_ecb_ciphers(ctx, out_arg, in_arg, nbytes);
}

static int af_alg_des_ede3_cbc_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	return af_alg_aes_cbc_ciphers(ctx, out_arg, in_arg, nbytes);
}

static int af_alg_des_ede3_ecb_ciphers(EVP_CIPHER_CTX *ctx, unsigned char *out_arg, const unsigned char *in_arg, size_t nbytes)
{
	return af_alg_aes_ecb_ciphers(ctx, out_arg, in_arg, nbytes);
}

static int af_alg_des_ede3_cbc_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "cbc(des3)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}

static int af_alg_des_ede3_ecb_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "ecb(des3)",
	};

	E_DBG("\n");
	return af_alg_aes_init_key(ctx, key, &sa);
}

#define	DECLARE_DES_EVP(lmode,umode,keylen)                 \
static const EVP_CIPHER af_alg_des_##lmode = {              \
	.nid = NID_des_##lmode,                                 \
	.block_size = DES_KEY_SZ,	                            \
	.key_len = keylen,                                      \
	.iv_len = DES_KEY_SZ,                                   \
	.flags = 0 | EVP_CIPH_##umode##_MODE,                   \
	.init = af_alg_des_##lmode##_init_key,                  \
	.do_cipher = af_alg_des_##lmode##_ciphers,              \
	.cleanup = af_alg_aes_cleanup_key,                      \
	.ctx_size = sizeof(struct af_alg_cipher_data),          \
	.set_asn1_parameters = EVP_CIPHER_set_asn1_iv,          \
	.get_asn1_parameters = EVP_CIPHER_get_asn1_iv,          \
	.ctrl = NULL,                                           \
	.app_data = NULL                                        \
}
DECLARE_DES_EVP(ecb,ECB,DES_KEY_SZ);
DECLARE_DES_EVP(cbc,CBC,DES_KEY_SZ);
DECLARE_DES_EVP(ede3_ecb,ECB,DES_KEY_SZ*3);
DECLARE_DES_EVP(ede3_cbc,CBC,DES_KEY_SZ*3);

typedef struct {
	int nid;
	const EVP_CIPHER *evp;
} nid2cipher_t;
static nid2cipher_t gs_nid2cipher[] = {
	{NID_aes_128_ecb, &af_alg_aes_128_ecb},
	{NID_aes_192_ecb, &af_alg_aes_192_ecb},
	{NID_aes_256_ecb, &af_alg_aes_256_ecb},
	{NID_aes_128_cbc, &af_alg_aes_128_cbc},
	{NID_aes_192_cbc, &af_alg_aes_192_cbc},
	{NID_aes_256_cbc, &af_alg_aes_256_cbc},
#ifdef SS_CTR_MODE_ENABLE
	{NID_aes_128_ctr, &af_alg_aes_128_ctr},
	{NID_aes_192_ctr, &af_alg_aes_192_ctr},
	{NID_aes_256_ctr, &af_alg_aes_256_ctr},
#endif
#ifdef SS_CTS_MODE_ENABLE
	{NID_aes_128_cts, &af_alg_aes_128_cts},
	{NID_aes_192_cts, &af_alg_aes_192_cts},
	{NID_aes_256_cts, &af_alg_aes_256_cts},
#endif
#ifdef SS_OFB_MODE_ENABLE
	{NID_aes_128_ofb128, &af_alg_aes_128_ofb128},
	{NID_aes_192_ofb128, &af_alg_aes_192_ofb128},
	{NID_aes_256_ofb128, &af_alg_aes_256_ofb128},
#endif
#ifdef SS_CFB_MODE_ENABLE
	{NID_aes_128_cfb1,	 &af_alg_aes_128_cfb1},
	{NID_aes_128_cfb8,	 &af_alg_aes_128_cfb8},
	{NID_aes_128_cfb128, &af_alg_aes_128_cfb128},
	{NID_aes_192_cfb1,	 &af_alg_aes_192_cfb1},
	{NID_aes_192_cfb8,	 &af_alg_aes_192_cfb8},
	{NID_aes_192_cfb128, &af_alg_aes_192_cfb128},
	{NID_aes_256_cfb1,	 &af_alg_aes_256_cfb1},
	{NID_aes_256_cfb8,	 &af_alg_aes_256_cfb8},
	{NID_aes_256_cfb128, &af_alg_aes_256_cfb128},
#endif
#ifdef SS_OFB_MODE_ENABLE
	{NID_aes_128_xts, &af_alg_aes_128_xts},
	{NID_aes_256_xts, &af_alg_aes_256_xts},
#endif
	{NID_des_ecb,      &af_alg_des_ecb},
	{NID_des_cbc,      &af_alg_des_cbc},
	{NID_des_ede3_ecb, &af_alg_des_ede3_ecb},
	{NID_des_ede3_cbc, &af_alg_des_ede3_cbc},
	{0xFFFF, NULL}
};
static int gs_nid2cipher_cnt = sizeof(gs_nid2cipher)/sizeof(nid2cipher_t);

static int af_alg_match(int nid, void *array, int cnt)
{
	int i;
	nid2cipher_t *cur = (nid2cipher_t *)array;

	for (i=0; i<cnt; i++, cur++) {
		if (nid == cur->nid)
			return i;
	}
	return cnt - 1;
}

static int af_alg_ciphers(ENGINE *e, const EVP_CIPHER **cipher, const int **nids, int nid)
{
	E_DBG("cipher = %p, nids = %p,nid = %d\n", cipher,nids, nid);
	if( !cipher )
	{
		if(nids)
		{
			*nids = af_alg_cipher_nids;
			return af_alg_cipher_nids_num;
		}
		else
		{
			return 0;
		}
	}

	if( ! nid_in_nids(nid, af_alg_cipher_nids, af_alg_cipher_nids_num) )
		return 0;

	*cipher = gs_nid2cipher[af_alg_match(nid, gs_nid2cipher, gs_nid2cipher_cnt)].evp;
	return(*cipher != 0);
}

#define DIGEST_DATA(ctx) ((struct af_alg_digest_data*)(ctx->md_data))

static int af_alg_hash_init(EVP_MD_CTX *ctx, struct sockaddr_alg *sa)
{
	struct af_alg_digest_data *ddata = DIGEST_DATA(ctx);

	E_DBG("%s.%s init ... \n", sa->salg_type, sa->salg_name);
	if( (ddata->tfmfd = socket(AF_ALG, SOCK_SEQPACKET, 0)) == -1 ) {
		E_ERR("socket() failed! [%d]: %s\n", errno, strerror(errno));
		return 0;
	}

	if( bind(ddata->tfmfd, (struct sockaddr *)sa, sizeof(*sa)) != 0 ) {
		E_ERR("bind() failed! [%d]: %s\n", errno, strerror(errno));
		return 0;
	}

	if( (ddata->opfd = accept(ddata->tfmfd,NULL,0)) == -1 ) {
		E_ERR("accept() failed! [%d]: %s\n", errno, strerror(errno));
		return 0;
	}

	return 1;
}

static int af_alg_sha1_init(EVP_MD_CTX *ctx)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "hash",
		.salg_name = "sha1"
	};

	return af_alg_hash_init(ctx, &sa);
}

#ifdef SS_SHA224_ENABLE
static int af_alg_sha224_init(EVP_MD_CTX *ctx)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "hash",
		.salg_name = "sha224"
	};

	return af_alg_hash_init(ctx, &sa);
}
#endif

#ifdef SS_SHA256_ENABLE
static int af_alg_sha256_init(EVP_MD_CTX *ctx)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "hash",
		.salg_name = "sha256"
	};

	return af_alg_hash_init(ctx, &sa);
}
#endif

#ifdef SS_SHA384_ENABLE
static int af_alg_sha384_init(EVP_MD_CTX *ctx)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "hash",
		.salg_name = "sha384"
	};

	return af_alg_hash_init(ctx, &sa);
}
#endif

#ifdef SS_SHA512_ENABLE
static int af_alg_sha512_init(EVP_MD_CTX *ctx)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "hash",
		.salg_name = "sha512"
	};

	return af_alg_hash_init(ctx, &sa);
}
#endif

#if defined(SS_HMAC_SHA1_ENABLE) || defined(SS_HMAC_SHA256_ENABLE)
#define SWAB32(x) 	((((x)&0x000000ffUL) << 24) | (((x)&0x0000ff00UL) << 8) \
					| (((x)&0x00ff0000UL) >> 8) | (((x)&0xff000000UL) >> 24))

int af_alg_hmac_sha_padding(char *dst, const char *src, int len, char *key, int keylen)
{
	int i;
	int n = len % SHA_CBLOCK;
	int total = len + SHA_CBLOCK;
	char *p = dst;

	//K^IPAD begin (IPAD == 0x36)
	/* Generate dst based on HMAC-SHA1 standard. :: K^IPAD */
    for (i=0; i<keylen; ++i)
		dst[i] = key[i] ^ 0x36;
	memset(&dst[keylen], 0x36, SHA_CBLOCK-keylen);
	//K^IPAD end

	//Config the area: [ &dst[SHA_CBLOCK] --- &dst[SHA_CBLOCK*3] ]
	memcpy(&dst[SHA_CBLOCK], src, len);
	memset(&dst[SHA_CBLOCK+len], 0, SHA_CBLOCK*2);

	/* Padding the tail based on SHA1 standard. */
	p += total - n; /* point to the last block */
	p[n] = 0x80;//p[SHA_CBLOCK*2 + len % SHA_CBLOCK]  = 0x80
	n++;

	if (n > (SHA_CBLOCK-8))
		p += SHA_CBLOCK*2 - 8;
	else
		p += SHA_CBLOCK - 8;

	//config the last 8Byte to notify total to kernel(total == len + SHA_CBLOCK).
	*(int *)p = SWAB32(total >> 29);
	*(int *)(p+4) = SWAB32(total << 3);

	E_DBG("After padding %d: %02x %02x %02x %02x	%02x %02x %02x %02x\n",
			p + 8 - dst,
			p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

	//return the total len including(==2 or 3 SHA_CBLOCK) : 
	//processed key && processed data to let kernel parsing and fetch the data.
	return p + 8 - dst;
}

static int af_alg_hmac_sha_update(EVP_MD_CTX *ctx, const void *data, size_t length)
{
	struct af_alg_digest_data *ddata = DIGEST_DATA(ctx);
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct iovec iov;
	int encrypt = ALG_OP_ENCRYPT;
	char buf[CMSG_SPACE(sizeof(encrypt))];
	char *padded = NULL;
	ssize_t ret;

	//set preprocessed key data(after processed :key_lenth<=SHA_CBLOCK)
	if (setsockopt(ddata->tfmfd, SOL_ALG, ALG_SET_KEY, ddata->key, SHA_CBLOCK) == -1) {
		E_ERR("setsockopt() failed! [%d]: %s\n", errno, strerror(errno));
		return 0;
	}

	padded = OPENSSL_malloc(length + SHA_CBLOCK*3);
	if (padded == NULL) {
		E_ERR("Failed to malloc()!\n");
		return 0;
	}

	memset(buf, 0, sizeof(buf));
	E_DBG("length = %d\n", length);

	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_control = buf;
	msg.msg_controllen = sizeof(buf);

	/* set operation type encrypt|decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	memcpy(CMSG_DATA(cmsg), &encrypt, 4);

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	ret = af_alg_hmac_sha_padding(padded, data, length, ddata->key, ddata->keylen);
	iov.iov_base = (void *)padded;
	iov.iov_len = ret;
 
	e_afalg_dump("hamc after processed data: ",iov.iov_base,iov.iov_len,16);
	if ((ret = sendmsg(ddata->opfd, &msg, 0)) == -1)
		return 0;
	E_DBG("sendmsg() return %d \n", ret);

	OPENSSL_free(padded);
	return 1;
}
#endif

#ifdef SS_HMAC_SHA1_ENABLE
static int af_alg_hmac_sha1_init(EVP_MD_CTX *ctx)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "hmac-sha1"
	};
	return af_alg_hash_init(ctx, &sa);
}

static int af_alg_hmac_sha1_final(EVP_MD_CTX *ctx, unsigned char *md)
{
	int ret = 0;
	struct af_alg_digest_data *ddata = DIGEST_DATA(ctx);

	ret = read(ddata->opfd, md, SHA_DIGEST_LENGTH);
	if (ret != SHA_DIGEST_LENGTH) {
		E_ERR("read(%d) return %d! \n", ddata->opfd, ret);
		return 0;
	}

	return 1;
}
#endif

#ifdef SS_HMAC_SHA256_ENABLE
static int af_alg_hmac_sha256_init(EVP_MD_CTX *ctx)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "hmac-sha256"
	};
	return af_alg_hash_init(ctx, &sa);
}

static int af_alg_hmac_sha256_final(EVP_MD_CTX *ctx, unsigned char *md)
{
	int ret = 0;
	struct af_alg_digest_data *ddata = DIGEST_DATA(ctx);

	ret = read(ddata->opfd, md, SHA256_DIGEST_LENGTH);
	if (ret != SHA256_DIGEST_LENGTH) {
		E_ERR("read(%d) return %d! \n", ddata->opfd, ret);
		return 0;
	}

	return 1;
}
#endif

static int af_alg_sha_update(EVP_MD_CTX *ctx, const void *data, size_t length)
{
	struct af_alg_digest_data *ddata = DIGEST_DATA(ctx);
	ssize_t r;

	r = send(ddata->opfd, data, length, MSG_MORE);
	E_DBG("send(%d) return %d \n", length, r);
	if( r < 0 || (size_t)r < length )
		return 0;
	return 1;
}

static int af_alg_sha_final(EVP_MD_CTX *ctx, unsigned char *md)
{
	struct af_alg_digest_data *ddata = DIGEST_DATA(ctx);
	ssize_t r = 0;
	int md_size = ctx->digest->md_size;

	r = read(ddata->opfd, md, md_size);
	E_DBG("read(%d) return %d \n", md_size, r);
	if (r != md_size) {
		E_ERR("read() return %d. [%d]: %s\n", (int)r, errno, strerror(errno));
		return 0;
	}

	return 1;
}

static int af_alg_sha_copy(EVP_MD_CTX *_to,const EVP_MD_CTX *_from)
{
	struct af_alg_digest_data *from = DIGEST_DATA(_from);
	struct af_alg_digest_data *to = DIGEST_DATA(_to);

	E_DBG("\n");
	if( (to->opfd = accept(from->opfd, NULL, 0)) == -1 )
		return 0;
	if( (to->tfmfd = accept(from->tfmfd, NULL, 0)) == -1 )
		return 0;
	return 1;
}

static int af_alg_sha_cleanup(EVP_MD_CTX *ctx)
{
	struct af_alg_digest_data *ddata = DIGEST_DATA(ctx);
	if( ddata->opfd != -1 )
		close(ddata->opfd);
	if( ddata->tfmfd != -1 )
		close(ddata->tfmfd);
	return 0;
}

static int af_alg_md5_init(EVP_MD_CTX *ctx)
{
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "hash",
		.salg_name = "md5"
	};

	return af_alg_hash_init(ctx, &sa);
}

#define NID_hmac_sha1WithRSAEncryption NID_sha1WithRSAEncryption

#define	DECLARE_MD_SHA(digest, udigest)			\
static const EVP_MD af_alg_md_##digest = {    \
	NID_##digest,                               \
	NID_##digest##WithRSAEncryption,            \
	udigest##_DIGEST_LENGTH,                    \
	0,                                          \
	af_alg_##digest##_init,                     \
	af_alg_sha_update,                   		\
	af_alg_sha_final,    		                \
	af_alg_sha_copy,                     		\
	af_alg_sha_cleanup,                  		\
	EVP_PKEY_RSA_method,                        \
	SHA_CBLOCK,                                 \
	sizeof(struct af_alg_digest_data),          \
	NULL,										\
}

DECLARE_MD_SHA(sha1, SHA);
#ifdef SS_SHA224_ENABLE
DECLARE_MD_SHA(sha224, SHA224);
#endif
#ifdef SS_SHA256_ENABLE
DECLARE_MD_SHA(sha256, SHA256);
#endif
#ifdef SS_SHA384_ENABLE
DECLARE_MD_SHA(sha384, SHA384);
#endif
#ifdef SS_SHA512_ENABLE
DECLARE_MD_SHA(sha512, SHA512);
#endif
#ifdef SS_HMAC_SHA1_ENABLE
static const EVP_MD af_alg_md_hmac_sha1 = {
	NID_hmac_sha1,
	NID_hmac_sha1WithRSAEncryption,
	SHA_DIGEST_LENGTH,
	0,
	af_alg_hmac_sha1_init,
	af_alg_hmac_sha_update,
	af_alg_hmac_sha1_final,
	af_alg_sha_copy,
	af_alg_sha_cleanup,
	EVP_PKEY_RSA_method,
	SHA_CBLOCK,
	sizeof(struct af_alg_digest_data),
	NULL
};
#endif
#ifdef SS_HMAC_SHA256_ENABLE
static const EVP_MD af_alg_md_hmac_sha256 = {
	NID_hmacWithSHA256,
	NID_hmac_sha1WithRSAEncryption,
	SHA256_DIGEST_LENGTH,
	0,
	af_alg_hmac_sha256_init,
	af_alg_hmac_sha_update,
	af_alg_hmac_sha256_final,
	af_alg_sha_copy,
	af_alg_sha_cleanup,
	EVP_PKEY_RSA_method,
	SHA_CBLOCK,
	sizeof(struct af_alg_digest_data),
	NULL
};
#endif
DECLARE_MD_SHA(md5, MD5);

typedef struct {
	int nid;
	const EVP_MD *evp;
} nid2md_t;
static nid2md_t gs_nid2md[] = {
	{NID_sha1, 		&af_alg_md_sha1},
#ifdef SS_SHA224_ENABLE
	{NID_sha224, 	&af_alg_md_sha224},
#endif
#ifdef SS_SHA256_ENABLE
	{NID_sha256,	&af_alg_md_sha256},
#endif
#ifdef SS_SHA384_ENABLE
	{NID_sha384,	&af_alg_md_sha384},
#endif
#ifdef SS_SHA512_ENABLE
	{NID_sha512, 	&af_alg_md_sha512},
#endif
	{NID_md5,		&af_alg_md_md5},
#ifdef SS_HMAC_SHA1_ENABLE
	{NID_hmac_sha1,	&af_alg_md_hmac_sha1},
#endif
#ifdef SS_HMAC_SHA256_ENABLE
	{NID_hmacWithSHA256, &af_alg_md_hmac_sha256},
#endif
	{0xFFFF, 		NULL}
};
static int gs_nid2md_cnt = sizeof(gs_nid2md)/sizeof(nid2md_t);

static int af_alg_digests(ENGINE *e, const EVP_MD **digest, const int **nids, int nid)
{
	E_DBG("digest = %p \n", digest);
	if( !digest )
	{
		if(nids)
		{
			*nids = af_alg_digest_nids;
			return af_alg_digest_nids_num;	
		}
		else
		{
			return 0;
		}
	}

	if( nid_in_nids(nid, af_alg_digest_nids, af_alg_digest_nids_num) == false )
		return 0;

	E_DBG("nid = %d \n", nid);
	*digest = gs_nid2md[af_alg_match(nid, gs_nid2md, gs_nid2md_cnt)].evp;
	return (*digest != NULL);
}

