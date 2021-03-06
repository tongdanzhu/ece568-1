#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "lib/encoding.h"

#define HEX_LEN		20
#define BYTE_LEN	10
#define HOTP_BASE_LEN	40	// Size of given base of hotp url + 1
#define TOTP_BASE_LEN	42	// Size of given base of totp url + 1


 uint8_t CharVal(char c)
{
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'f') return c - 'a' + 10;
	if(c >= 'A' && c <= 'F') return c - 'A' + 10;
	return 255;
}

void gethex(char *input, uint8_t * sol)
{
	uint8_t *p;
	int len, i;
	
    len = strlen(input) / 2;
	for(i=0, p = (uint8_t *) input; i<len; i++) {
		sol[i] = (CharVal(*p) << 4) | CharVal(*(p+1));
		p += 2;
	}
    sol[len] = 0;
}


int
main(int argc, char * argv[])
{
	if ( argc != 4 ) {
		printf("Usage: %s [issuer] [accountName] [secretHex]\n", argv[0]);
		return(-1);
	}

	char *	issuer = argv[1];
	char *	accountName = argv[2];
	char *	secret_hex = argv[3];
	char * hotp_url;
	char * totp_url;
	int 	j, i, prelen, secret_hex_len, hotp_len, totp_len;

	secret_hex_len = strlen(secret_hex);

	assert (secret_hex_len <= 20);

	printf("\nIssuer: %s\nAccount Name: %s\nSecret (Hex): %s\n\n",
		issuer, accountName, secret_hex);
        j = 0;
	char padded_secret_hex[secret_hex_len/2];
	
	// Left-pad secret hex with zeros if its less than 20 hex characters
	prelen = 20 - secret_hex_len;
	for (i = 0; i < prelen; i++)
		padded_secret_hex[i] = '0';
	for ( ; i < 20; i++) 
		padded_secret_hex[i] = secret_hex[i - prelen];
  	padded_secret_hex[i] = '\0';
	gethex(padded_secret_hex, padded_secret_hex);
	const char *encoded_accountName = urlEncode(accountName);
        const char *encoded_issuer = urlEncode(issuer);
        char encoded_secret[100];	// Store only 10 bytes of the base32 encoding so that we have an 80 bit secret

	int l = base32_encode((const uint8_t *) padded_secret_hex, 10, (uint8_t *) encoded_secret, 100);
	//encoded_secret[17] = '\0';
	// Prepare buffers for hotp and totp uris 
	hotp_len = 100;
	totp_len = 100;
	hotp_url = (char*) malloc(hotp_len*sizeof(char));
	totp_url = (char*) malloc(totp_len*sizeof(char));

	// Create an otpauth:// URI and display a QR code that's compatible
	// with Google Authenticator
	snprintf(hotp_url, hotp_len, "otpauth://hotp/%s?issuer=%s&secret=%s&counter=1", encoded_accountName, encoded_issuer, encoded_secret);
	//printf(hotp_url);
	displayQRcode(hotp_url);

	snprintf(totp_url, totp_len, "otpauth://totp/%s?issuer=%s&secret=%s&period=30", encoded_accountName, encoded_issuer, encoded_secret);
	displayQRcode(totp_url);
	free(hotp_url);
	free(totp_url);

	return (0);
}

