#include "header.h"
#include <openssl/evp.h>

void computeSHA256(const char *data, unsigned char *hash)
{
	EVP_MD_CTX *mdctx;
	const EVP_MD *md;
	unsigned int hash_len;

	// Initialize the EVP context
	mdctx = EVP_MD_CTX_new();
	md = EVP_sha256();

	if (EVP_DigestInit_ex(mdctx, md, NULL) != 1)
	{
		fprintf(stderr, "EVP_DigestInit_ex failed\n");
		EVP_MD_CTX_free(mdctx);
		exit(EXIT_FAILURE);
	}

	if (EVP_DigestUpdate(mdctx, data, strlen(data)) != 1)
	{
		fprintf(stderr, "EVP_DigestUpdate failed\n");
		EVP_MD_CTX_free(mdctx);
		exit(EXIT_FAILURE);
	}

	if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1)
	{
		fprintf(stderr, "EVP_DigestFinal_ex failed\n");
		EVP_MD_CTX_free(mdctx);
		exit(EXIT_FAILURE);
	}

	// Clean up
	EVP_MD_CTX_free(mdctx);
}

void printHash(unsigned char *hash)
{
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		printf("%x", hash[i]);
	}
	printf("\n");
}