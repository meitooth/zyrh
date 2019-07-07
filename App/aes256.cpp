/*************************************************************************
> File Name: test_aes.c
> Author: Filon
> Created Time: Wed 06 Dec 2017 05:23:08 PM CST
************************************************************************/
#include "stdafx.h"
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include<openssl/aes.h>
#include <string>
#define LEN 60*1024

unsigned char *userKey = (unsigned char *)"088fcef5c38d6bf01021c410b7689032";
static char encoding_table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
'w', 'x', 'y', 'z', '0', '1', '2', '3',
'4', '5', '6', '7', '8', '9', '+', '/' };
static char *decoding_table = NULL;
static int mod_table[] = { 0, 2, 1 };


void build_decoding_table(void) {

	if(!decoding_table)
		decoding_table = (char*)malloc(256);

	for (int i = 0; i < 64; i++)
		decoding_table[(unsigned char)encoding_table[i]] = i;
}

void base64_cleanup(void) {
	free(decoding_table);
}


size_t base64_encode(const unsigned char *data,
	size_t input_length,
	unsigned char *encoded_data) {
	size_t output_length;
	output_length = 4 * ((input_length + 2) / 3);

	for (int i = 0, j = 0; i < input_length;) {

		uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (int i = 0; i < mod_table[input_length % 3]; i++)
		encoded_data[output_length - 1 - i] = '=';

	return output_length;
}


size_t base64_decode(const unsigned char *data,
	size_t input_length,
	unsigned char *decoded_data) {

	size_t output_length;
	if (decoding_table == NULL) build_decoding_table();

	if (input_length % 4 != 0) return -1;

	output_length = input_length / 4 * 3;
	if (data[input_length - 1] == '=') (output_length)--;
	if (data[input_length - 2] == '=') (output_length)--;

	for (int i = 0, j = 0; i < input_length;) {

		uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

		uint32_t triple = (sextet_a << 3 * 6)
			+ (sextet_b << 2 * 6)
			+ (sextet_c << 1 * 6)
			+ (sextet_d << 0 * 6);

		if (j < output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}
	return output_length;
}



std::string aes128encoder(unsigned char * raw_in)
{
	int len = LEN, out_len = 0;
	//unsigned char *raw_in = "0123456789abcdef0123456789abcdef";
	unsigned char input[LEN];
	
	//  unsigned char *userKey = "fedcba9876543210";
	unsigned char out[LEN];
	unsigned char out_b64[LEN];
	unsigned char out_b64_src[LEN];
	unsigned char plain[LEN];
	AES_KEY aes_enc_ctx;

	memset(input, 0, LEN);
	memset(out, 0, LEN);
	memset(out_b64, 0, LEN);
	memset(out_b64_src, 0, LEN);
	memset(plain, 0, LEN);

	build_decoding_table();

#define PCKS7 0
	
#if PCKS7 
	// pcks7padding
	int orglen = strlen((const char*)raw_in);
	len = (orglen + 15) / 16 * 16;
	memset(input, len - orglen, sizeof(input));
	memcpy(input, raw_in, orglen);
#else 
	//zeropadding
	len = (strlen((char*)raw_in) + 15) / 16 * 16;
	strcpy((char*)input, (char*)raw_in);
#endif
	printf("input = \n%s\n\n", input);

	//encrypt
	AES_set_encrypt_key(userKey, 256, &aes_enc_ctx);//这一步设置的密钥的长度位128，长则截断，短则用0补齐
	for (int i = 0; i < len / 16; ++i) {
		//AES_ecb_encrypt(input + i * 16, out + i * 16, &aes_enc_ctx, AES_ENCRYPT);
		AES_ecb_encrypt(input + i * 16, out + i * 16, &aes_enc_ctx, AES_ENCRYPT);
	}
	
	//b64encode
	out_len = base64_encode(out, len, out_b64);
	printf("out_b64 = \n%s\n\n", out_b64);
	std::string ret((char*)out_b64, out_len);
	return ret;

}
std::string aes128decoder(unsigned char* out_b64,int out_len)
{
	if (out_len == 0)
	{
		return "";
	}
	int len = LEN;
	unsigned char input[LEN];
	unsigned char out[LEN];
	unsigned char out_b64_src[LEN];
	unsigned char plain[LEN];
	AES_KEY aes_enc_ctx;

	memset(input, 0, LEN);
	memset(out, 0, LEN);
	memset(out_b64_src, 0, LEN);
	memset(plain, 0, LEN);

	build_decoding_table();
	len = base64_decode(out_b64, out_len, out_b64_src);

	//decrypt
	AES_set_decrypt_key(userKey, 256, &aes_enc_ctx);
	for (int i = 0; i < len / 16; ++i) {
		AES_ecb_encrypt(out_b64_src + i * 16, plain + i * 16, &aes_enc_ctx, AES_DECRYPT);
	}
	std::string ret = (char*)plain;
	return ret;
}