/* smime.c */
/* Written by Dr Stephen N Henson (shenson@bigfoot.com) for the OpenSSL
 * project 1999.
 */
/* ====================================================================
 * Copyright (c) 1999 The OpenSSL Project.  All rights reserved.
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
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

/* S/MIME utility function */

#include <stdio.h>
#include <string.h>
#include "apps.h"
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#undef PROG
#define PROG smime_main
static X509_STORE *setup_verify(char *CAfile, char *CApath);
static int save_certs(char *signerfile, STACK_OF(X509) *signers);

#define SMIME_OP	0x10
#define SMIME_ENCRYPT	(1 | SMIME_OP)
#define SMIME_DECRYPT	2
#define SMIME_SIGN	(3 | SMIME_OP)
#define SMIME_VERIFY	4
#define SMIME_PK7OUT	5

int MAIN(int, char **);

int MAIN(int argc, char **argv)
{
	int operation = 0;
	int ret = 0;
	char **args;
	char *inmode = "r", *outmode = "w";
	char *infile = NULL, *outfile = NULL;
	char *signerfile = NULL, *recipfile = NULL;
	char *certfile = NULL, *keyfile = NULL, *contfile=NULL;
	EVP_CIPHER *cipher = NULL;
	PKCS7 *p7 = NULL;
	X509_STORE *store = NULL;
	X509 *cert = NULL, *recip = NULL, *signer = NULL;
	EVP_PKEY *key = NULL;
	STACK_OF(X509) *encerts = NULL, *other = NULL;
	BIO *in = NULL, *out = NULL, *indata = NULL;
	int badarg = 0;
	int flags = PKCS7_DETACHED;
	char *to = NULL, *from = NULL, *subject = NULL;
	char *CAfile = NULL, *CApath = NULL;
	char *passargin = NULL, *passin = NULL;
	char *inrand = NULL;
	int need_rand = 0;
	int informat = FORMAT_SMIME, outformat = FORMAT_SMIME;
	args = argv + 1;

	ret = 1;

	while (!badarg && *args && *args[0] == '-') {
		if (!strcmp (*args, "-encrypt")) operation = SMIME_ENCRYPT;
		else if (!strcmp (*args, "-decrypt")) operation = SMIME_DECRYPT;
		else if (!strcmp (*args, "-sign")) operation = SMIME_SIGN;
		else if (!strcmp (*args, "-verify")) operation = SMIME_VERIFY;
		else if (!strcmp (*args, "-pk7out")) operation = SMIME_PK7OUT;
#ifndef NO_DES
		else if (!strcmp (*args, "-des3")) 
				cipher = EVP_des_ede3_cbc();
		else if (!strcmp (*args, "-des")) 
				cipher = EVP_des_cbc();
#endif
#ifndef NO_RC2
		else if (!strcmp (*args, "-rc2-40")) 
				cipher = EVP_rc2_40_cbc();
		else if (!strcmp (*args, "-rc2-128")) 
				cipher = EVP_rc2_cbc();
		else if (!strcmp (*args, "-rc2-64")) 
				cipher = EVP_rc2_64_cbc();
#endif
		else if (!strcmp (*args, "-text")) 
				flags |= PKCS7_TEXT;
		else if (!strcmp (*args, "-nointern")) 
				flags |= PKCS7_NOINTERN;
		else if (!strcmp (*args, "-noverify")) 
				flags |= PKCS7_NOVERIFY;
		else if (!strcmp (*args, "-nochain")) 
				flags |= PKCS7_NOCHAIN;
		else if (!strcmp (*args, "-nocerts")) 
				flags |= PKCS7_NOCERTS;
		else if (!strcmp (*args, "-noattr")) 
				flags |= PKCS7_NOATTR;
		else if (!strcmp (*args, "-nodetach")) 
				flags &= ~PKCS7_DETACHED;
		else if (!strcmp (*args, "-nosmimecap"))
				flags |= PKCS7_NOSMIMECAP;
		else if (!strcmp (*args, "-binary"))
				flags |= PKCS7_BINARY;
		else if (!strcmp (*args, "-nosigs"))
				flags |= PKCS7_NOSIGS;
		else if (!strcmp(*args,"-rand")) {
			if (args[1]) {
				args++;
				inrand = *args;
			} else badarg = 1;
			need_rand = 1;
		} else if (!strcmp(*args,"-passin")) {
			if (args[1]) {
				args++;
				passargin = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-to")) {
			if (args[1]) {
				args++;
				to = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-from")) {
			if (args[1]) {
				args++;
				from = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-subject")) {
			if (args[1]) {
				args++;
				subject = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-signer")) {
			if (args[1]) {
				args++;
				signerfile = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-recip")) {
			if (args[1]) {
				args++;
				recipfile = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-inkey")) {
			if (args[1]) {
				args++;
				keyfile = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-certfile")) {
			if (args[1]) {
				args++;
				certfile = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-CAfile")) {
			if (args[1]) {
				args++;
				CAfile = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-CApath")) {
			if (args[1]) {
				args++;
				CApath = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-in")) {
			if (args[1]) {
				args++;
				infile = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-inform")) {
			if (args[1]) {
				args++;
				informat = str2fmt(*args);
			} else badarg = 1;
		} else if (!strcmp (*args, "-outform")) {
			if (args[1]) {
				args++;
				outformat = str2fmt(*args);
			} else badarg = 1;
		} else if (!strcmp (*args, "-out")) {
			if (args[1]) {
				args++;
				outfile = *args;
			} else badarg = 1;
		} else if (!strcmp (*args, "-content")) {
			if (args[1]) {
				args++;
				contfile = *args;
			} else badarg = 1;
		} else badarg = 1;
		args++;
	}

	if(operation == SMIME_SIGN) {
		if(!signerfile) {
			BIO_printf(bio_err, "No signer certificate specified\n");
			badarg = 1;
		}
		need_rand = 1;
	} else if(operation == SMIME_DECRYPT) {
		if(!recipfile) {
			BIO_printf(bio_err, "No recipient certificate and key specified\n");
			badarg = 1;
		}
	} else if(operation == SMIME_ENCRYPT) {
		if(!*args) {
			BIO_printf(bio_err, "No recipient(s) certificate(s) specified\n");
			badarg = 1;
		}
		need_rand = 1;
	} else if(!operation) badarg = 1;

	if (badarg) {
		BIO_printf (bio_err, "Usage smime [options] cert.pem ...\n");
		BIO_printf (bio_err, "where options are\n");
		BIO_printf (bio_err, "-encrypt       encrypt message\n");
		BIO_printf (bio_err, "-decrypt       decrypt encrypted message\n");
		BIO_printf (bio_err, "-sign          sign message\n");
		BIO_printf (bio_err, "-verify        verify signed message\n");
		BIO_printf (bio_err, "-pk7out        output PKCS#7 structure\n");
#ifndef NO_DES
		BIO_printf (bio_err, "-des3          encrypt with triple DES\n");
		BIO_printf (bio_err, "-des           encrypt with DES\n");
#endif
#ifndef NO_RC2
		BIO_printf (bio_err, "-rc2-40        encrypt with RC2-40 (default)\n");
		BIO_printf (bio_err, "-rc2-64        encrypt with RC2-64\n");
		BIO_printf (bio_err, "-rc2-128       encrypt with RC2-128\n");
#endif
		BIO_printf (bio_err, "-nointern      don't search certificates in message for signer\n");
		BIO_printf (bio_err, "-nosigs        don't verify message signature\n");
		BIO_printf (bio_err, "-noverify      don't verify signers certificate\n");
		BIO_printf (bio_err, "-nocerts       don't include signers certificate when signing\n");
		BIO_printf (bio_err, "-nodetach      use opaque signing\n");
		BIO_printf (bio_err, "-noattr        don't include any signed attributes\n");
		BIO_printf (bio_err, "-binary        don't translate message to text\n");
		BIO_printf (bio_err, "-certfile file other certificates file\n");
		BIO_printf (bio_err, "-signer file   signer certificate file\n");
		BIO_printf (bio_err, "-recip  file   recipient certificate file for decryption\n");
		BIO_printf (bio_err, "-in file       input file\n");
		BIO_printf (bio_err, "-inform arg    input format SMIME (default), PEM or DER\n");
		BIO_printf (bio_err, "-inkey file    input private key (if not signer or recipient)\n");
		BIO_printf (bio_err, "-out file      output file\n");
		BIO_printf (bio_err, "-outform arg   output format SMIME (default), PEM or DER\n");
		BIO_printf (bio_err, "-content file  supply or override content for detached signature\n");
		BIO_printf (bio_err, "-to addr       to address\n");
		BIO_printf (bio_err, "-from ad       from address\n");
		BIO_printf (bio_err, "-subject s     subject\n");
		BIO_printf (bio_err, "-text          include or delete text MIME headers\n");
		BIO_printf (bio_err, "-CApath dir    trusted certificates directory\n");
		BIO_printf (bio_err, "-CAfile file   trusted certificates file\n");
		BIO_printf (bio_err, "-passin arg    input file pass phrase source\n");
		BIO_printf(bio_err,  "-rand file%cfile%c...\n", LIST_SEPARATOR_CHAR, LIST_SEPARATOR_CHAR);
		BIO_printf(bio_err,  "               load the file (or the files in the directory) into\n");
		BIO_printf(bio_err,  "               the random number generator\n");
		BIO_printf (bio_err, "cert.pem       recipient certificate(s) for encryption\n");
		goto end;
	}

	if(!app_passwd(bio_err, passargin, NULL, &passin, NULL)) {
		BIO_printf(bio_err, "Error getting password\n");
		goto end;
	}

	if (need_rand) {
		app_RAND_load_file(NULL, bio_err, (inrand != NULL));
		if (inrand != NULL)
			BIO_printf(bio_err,"%ld semi-random bytes loaded\n",
				app_RAND_load_files(inrand));
	}

	ret = 2;

	if(operation != SMIME_SIGN) flags &= ~PKCS7_DETACHED;

	if(operation & SMIME_OP) {
		if(flags & PKCS7_BINARY) inmode = "rb";
		if(outformat == FORMAT_ASN1) outmode = "wb";
	} else {
		if(flags & PKCS7_BINARY) outmode = "wb";
		if(informat == FORMAT_ASN1) inmode = "rb";
	}

	if(operation == SMIME_ENCRYPT) {
		if (!cipher) {
#ifndef NO_RC2			
			cipher = EVP_rc2_40_cbc();
#else
			BIO_printf(bio_err, "No cipher selected\n");
			goto end;
#endif
		}
		encerts = sk_X509_new_null();
		while (*args) {
			if(!(cert = load_cert(bio_err,*args,FORMAT_PEM))) {
				BIO_printf(bio_err, "Can't read recipient certificate file %s\n", *args);
				goto end;
			}
			sk_X509_push(encerts, cert);
			cert = NULL;
			args++;
		}
	}

	if(signerfile && (operation == SMIME_SIGN)) {
		if(!(signer = load_cert(bio_err,signerfile,FORMAT_PEM))) {
			BIO_printf(bio_err, "Can't read signer certificate file %s\n", signerfile);
			goto end;
		}
	}

	if(certfile) {
		if(!(other = load_certs(bio_err,certfile,FORMAT_PEM))) {
			BIO_printf(bio_err, "Can't read certificate file %s\n", certfile);
			ERR_print_errors(bio_err);
			goto end;
		}
	}

	if(recipfile && (operation == SMIME_DECRYPT)) {
		if(!(recip = load_cert(bio_err,recipfile,FORMAT_PEM))) {
			BIO_printf(bio_err, "Can't read recipient certificate file %s\n", recipfile);
			ERR_print_errors(bio_err);
			goto end;
		}
	}

	if(operation == SMIME_DECRYPT) {
		if(!keyfile) keyfile = recipfile;
	} else if(operation == SMIME_SIGN) {
		if(!keyfile) keyfile = signerfile;
	} else keyfile = NULL;

	if(keyfile) {
		if(!(key = load_key(bio_err,keyfile, FORMAT_PEM, passin))) {
			BIO_printf(bio_err, "Can't read recipient certificate file %s\n", keyfile);
			ERR_print_errors(bio_err);
			goto end;
		}
	}

	if (infile) {
		if (!(in = BIO_new_file(infile, inmode))) {
			BIO_printf (bio_err,
				 "Can't open input file %s\n", infile);
			goto end;
		}
	} else in = BIO_new_fp(stdin, BIO_NOCLOSE);

	if (outfile) {
		if (!(out = BIO_new_file(outfile, outmode))) {
			BIO_printf (bio_err,
				 "Can't open output file %s\n", outfile);
			goto end;
		}
	} else {
		out = BIO_new_fp(stdout, BIO_NOCLOSE);
#ifdef VMS
		{
		    BIO *tmpbio = BIO_new(BIO_f_linebuffer());
		    out = BIO_push(tmpbio, out);
		}
#endif
	}

	if(operation == SMIME_VERIFY) {
		if(!(store = setup_verify(CAfile, CApath))) goto end;
	}

	ret = 3;

	if(operation == SMIME_ENCRYPT) {
		p7 = PKCS7_encrypt(encerts, in, cipher, flags);
	} else if(operation == SMIME_SIGN) {
		p7 = PKCS7_sign(signer, key, other, in, flags);
		BIO_reset(in);
	} else {
		if(informat == FORMAT_SMIME) 
			p7 = SMIME_read_PKCS7(in, &indata);
		else if(informat == FORMAT_PEM) 
			p7 = PEM_read_bio_PKCS7(in, NULL, NULL, NULL);
		else if(informat == FORMAT_ASN1) 
			p7 = d2i_PKCS7_bio(in, NULL);
		else {
			BIO_printf(bio_err, "Bad input format for PKCS#7 file\n");
			goto end;
		}

		if(!p7) {
			BIO_printf(bio_err, "Error reading S/MIME message\n");
			goto end;
		}
		if(contfile) {
			BIO_free(indata);
			if(!(indata = BIO_new_file(contfile, "rb"))) {
				BIO_printf(bio_err, "Can't read content file %s\n", contfile);
				goto end;
			}
		}
	}

	if(!p7) {
		BIO_printf(bio_err, "Error creating PKCS#7 structure\n");
		goto end;
	}

	ret = 4;
	if(operation == SMIME_DECRYPT) {
		if(!PKCS7_decrypt(p7, key, recip, out, flags)) {
			BIO_printf(bio_err, "Error decrypting PKCS#7 structure\n");
			goto end;
		}
	} else if(operation == SMIME_VERIFY) {
		STACK_OF(X509) *signers;
		if(PKCS7_verify(p7, other, store, indata, out, flags)) {
			BIO_printf(bio_err, "Verification Successful\n");
		} else {
			BIO_printf(bio_err, "Verification Failure\n");
			goto end;
		}
		signers = PKCS7_get0_signers(p7, other, flags);
		if(!save_certs(signerfile, signers)) {
			BIO_printf(bio_err, "Error writing signers to %s\n",
								signerfile);
			ret = 5;
			goto end;
		}
		sk_X509_free(signers);
	} else if(operation == SMIME_PK7OUT) {
		PEM_write_bio_PKCS7(out, p7);
	} else {
		if(to) BIO_printf(out, "To: %s\n", to);
		if(from) BIO_printf(out, "From: %s\n", from);
		if(subject) BIO_printf(out, "Subject: %s\n", subject);
		if(outformat == FORMAT_SMIME) 
			SMIME_write_PKCS7(out, p7, in, flags);
		else if(outformat == FORMAT_PEM) 
			PEM_write_bio_PKCS7(out,p7);
		else if(outformat == FORMAT_ASN1) 
			i2d_PKCS7_bio(out,p7);
		else {
			BIO_printf(bio_err, "Bad output format for PKCS#7 file\n");
			goto end;
		}
	}
	ret = 0;
end:
	if (need_rand)
		app_RAND_write_file(NULL, bio_err);
	if(ret) ERR_print_errors(bio_err);
	sk_X509_pop_free(encerts, X509_free);
	sk_X509_pop_free(other, X509_free);
	X509_STORE_free(store);
	X509_free(cert);
	X509_free(recip);
	X509_free(signer);
	EVP_PKEY_free(key);
	PKCS7_free(p7);
	BIO_free(in);
	BIO_free(indata);
	BIO_free_all(out);
	if(passin) OPENSSL_free(passin);
	return (ret);
}

static X509_STORE *setup_verify(char *CAfile, char *CApath)
{
	X509_STORE *store;
	X509_LOOKUP *lookup;
	if(!(store = X509_STORE_new())) goto end;
	lookup=X509_STORE_add_lookup(store,X509_LOOKUP_file());
	if (lookup == NULL) goto end;
	if (CAfile) {
		if(!X509_LOOKUP_load_file(lookup,CAfile,X509_FILETYPE_PEM)) {
			BIO_printf(bio_err, "Error loading file %s\n", CAfile);
			goto end;
		}
	} else X509_LOOKUP_load_file(lookup,NULL,X509_FILETYPE_DEFAULT);
		
	lookup=X509_STORE_add_lookup(store,X509_LOOKUP_hash_dir());
	if (lookup == NULL) goto end;
	if (CApath) {
		if(!X509_LOOKUP_add_dir(lookup,CApath,X509_FILETYPE_PEM)) {
			BIO_printf(bio_err, "Error loading directory %s\n", CApath);
			goto end;
		}
	} else X509_LOOKUP_add_dir(lookup,NULL,X509_FILETYPE_DEFAULT);

	ERR_clear_error();
	return store;
	end:
	X509_STORE_free(store);
	return NULL;
}

static int save_certs(char *signerfile, STACK_OF(X509) *signers)
{
	int i;
	BIO *tmp;
	if(!signerfile) return 1;
	tmp = BIO_new_file(signerfile, "w");
	if(!tmp) return 0;
	for(i = 0; i < sk_X509_num(signers); i++)
		PEM_write_bio_X509(tmp, sk_X509_value(signers, i));
	BIO_free(tmp);
	return 1;
}
	
