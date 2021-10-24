#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<openssl/evp.h>
#include<string.h>

void add(unsigned int *data, int len)
{
	//treat 4*len bytes memory as one large number and perform add 1
	int p, carry;
	p = len - 1;
	carry = 1;

	while(carry ==1 && p >= 0)
	{
		if(data[p]!=0xffffffff)
		{
			data[p]++;
			carry = 0;
		}
		else
		{
			data[p]=0x00000000;
			carry = 1;
			p--;
		}
	}
	return;
}

int compare(const char *a, const char *b, int len)
{
	int i;
	//compare first len bytes of a and b
	for(i=0; i<len; i++)
	{
		if(a[i] != b[i])
			return 1;
	}
	return 0; //if they are the same
}

double avg(const int *data, int count)
{
	//calculate count numbers' average value
	int i;
	double avg = 0;
	for(i=0;i<count;i++)
	{
		avg = avg*((double)i/(double)(i+1))+(double)(data[i])/(double)(i+1);
	}
	return avg;
}

void display(char *data, int len)
{
	//print out len bytes memory
	int i = 0;
	for(;i<len;i++)
	{
		printf("%02X ", (unsigned char)data[i]);
	}
	return;
}


///You are to write this function below

int compareAll(const char *list, const char *data, int total, int unit_len, int len)
{
	//(list) contains multiple history entries appended at the end without any overhead
	//(data) is the current entry you are comparing with the (list) items
	//(total) is the length of (list)
	//(unit_len) is the length of each entry
	//(len) is the length of bytes you are comparing between (list)'s entries and (data)
	
	//TASK
	//write a code to compare (data) and all entries in (list)'s first (len) bytes
	//return the index of item matched, otherwise return -1
	//hint: use a for-loop until you reach the end of the list
	//hint: use the function compare already given to you to compare bytes of length (len)
	

	/* YOUR CODE GOES HERE */


	return -1; // no match, return -1
}

int main(int argc, char** argv)
{
	if(argc <2)
	{
		printf("Usage: cfb AttemptCount\n");
		return 0;
	}
	srand(time(NULL));
	char data[16];
	char dataBackup[16];
	char hashHistory[1024*1024];
	char data_hash[16];
	int i, j;

	int attempt = atoi(argv[1]);
	int count = 0;
	int *statistics;
	statistics = (int *)malloc(attempt * sizeof(int));
	
	EVP_MD_CTX ctx_d;

	while(count<attempt)
	{
		for(i=0;i<4;i++)
		{	
			((int *)dataBackup)[i] = ((int *)data)[i] = rand();
		}
	

		int c, index;
		c = 0; //store how many attemps has done for one specific hash value
		
		while(index=compareAll(hashHistory, data_hash, c-1, 16, 3)<0) //keep looping until there is a collision
		{
			if (c > 0)
			{
				bcopy(data_hash, &hashHistory[16*(c-1)], 16);
			} //not match, so store the previous result

			add((unsigned int*)data, 4);
			c++;

			int p, q, r, r_len;
			EVP_MD_CTX_init(&ctx_d);
			p = EVP_DigestInit_ex(&ctx_d, EVP_md5(), NULL);
			q = EVP_DigestUpdate(&ctx_d, data, 16);
			r = EVP_DigestFinal_ex(&ctx_d, data_hash, &r_len);
			EVP_MD_CTX_cleanup(&ctx_d);
		}

		for(j=0; j<= index; j++)
		{
			add((unsigned int *)dataBackup, 4); //reconstruct the history message
		}

		printf("\n* Hash value Match:  calculated for %d times\n", c);
		printf("   Message One:      ");
		display(dataBackup, 16);
		printf("\n   Message Two:      ");
		display(data, 16);
		printf("\n   Hash Value:       ");
		display(data_hash,3);

		statistics[count]=c;
		count++;
	}
	
	double avg_time = avg(statistics, attempt); //calculate average number
	printf("\n===================================================\n");
	printf("Tried %lf times on average for each hash value. \n", avg_time);
	return 0;
}
	
