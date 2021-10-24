//You can modify the below code to get started, or you are welcome to write from scratch

#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

char wordx[17];
char filex[17];
FILE *fp;


char engword[17];
char encryp[32] = {0};

void KeyGen(char* in, char* out)
{
	int i = 0;
	char* temp = malloc(17);

        memcpy(temp,in,strlen(in));
        i = strlen(in);
        while (i < 16){
                temp[i] = 0x20;
                i++;
        }

        memcpy(out,temp,17);

}

int do_crypt(char *outfile)
{
        unsigned char outbuf[1024];
        int outlen, tmplen;
        /* Bogus key and IV: we'd normally set these from
         * another source.
         */
        unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
        unsigned char iv[] = {1,2,3,4,5,6,7,8};
        char intext[] = "Some Crypto Text";
        EVP_CIPHER_CTX ctx;
        FILE *out;
        EVP_CIPHER_CTX_init(&ctx);
	
        EVP_EncryptInit_ex(&ctx, EVP_idea_cbc(), NULL, key, iv);

        if(!EVP_EncryptUpdate(&ctx, outbuf, &outlen, intext, strlen(intext))) {
                /* Error */
                return 0;
	}
        /* Buffer passed to EVP_EncryptFinal() must be after data just
         * encrypted to avoid overwriting it.
         */
        if(!EVP_EncryptFinal_ex(&ctx, outbuf + outlen, &tmplen)) {
                /* Error */
                return 0;
	}
        outlen += tmplen;
        EVP_CIPHER_CTX_cleanup(&ctx);
        /* Need binary mode for fopen because encrypted data is
         * binary data. Also cannot use strlen() on it because
         * it wont be null terminated and may contain embedded
         * nulls.
         */
        out = fopen(outfile, "wb");
        fwrite(outbuf, 1, outlen, out);
        fclose(out);
        return 1;
}

int main()
{
	//create the main function
	//create a char length 17 (our limit)
	//open a file containing the list of words
	//for each word, use it as a key to see if the ciphertext matches
	//once you found a word that matches the ciphertext, print

	//your code goes here
	return 0;
}

