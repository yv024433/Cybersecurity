//---cat rawtcp.c---
// Run as root or SUID 0, just datagram no data/payload
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>

// Packet length
#define PCKT_LEN 8192
 
// Simple checksum function, may use others such as Cyclic Redundancy Check, CRC
unsigned short csum(unsigned short *buf, int len)
{
        unsigned long sum;
        for(sum=0; len>0; len--)
                sum += *buf++;
        sum = (sum >> 16) + (sum &0xffff);
        sum += (sum >> 16);
        return (unsigned short)(~sum);
}
 
int main(int argc, char *argv[])
{
  int sd;
	// No data, just datagram
	char buffer[PCKT_LEN];
	// The size of the headers
	struct ip *iph = (struct ip *) buffer;
	struct tcphdr *tcp = (struct tcphdr *) (buffer + sizeof(struct ip));
	struct sockaddr_in sin, din;
	const int one = 1;
	 
	memset(buffer, 0, PCKT_LEN);
	 
	if(argc != 5)
	{
		printf("- Invalid parameters!!!\n");
		printf("- Usage: %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);
		exit(-1);
	}
	 
	sd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
	if(sd < 0)
	{
	   perror("socket() error");
	   exit(-1);
	}
	else
		printf("socket()-SOCK_RAW and tcp protocol is OK.\n");
	 
	// The source is redundant, may be used later if needed
	// Address family
	sin.sin_family = AF_INET;
	din.sin_family = AF_INET;
	// Source port, can be any, modify as needed
	sin.sin_port = htons(atoi(argv[2]));
	din.sin_port = htons(atoi(argv[4]));
	// Source IP, can be any, modify as needed
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	din.sin_addr.s_addr = inet_addr(argv[3]);
	// IP structure
	iph->ip_hl=5;
	iph->ip_v=4;
	iph->ip_tos = 16; // Low delay
	iph->ip_len = sizeof(struct ip) + sizeof(struct tcphdr);
	iph->ip_id = htons(54321);
	iph->ip_off = 0;
	iph->ip_ttl = 64;
	iph->ip_p = 6; // TCP
	iph->ip_sum = 0; // Done by kernel
	 
	// Source IP, modify as needed, spoofed, we accept through command line argument
	(iph->ip_src).s_addr = inet_addr(argv[1]);
	// Destination IP, modify as needed, but here we accept through command line argument
	(iph->ip_dst).s_addr = inet_addr(argv[3]);
	 
	// The TCP structure. The source port, spoofed, we accept through the command line
	tcp->source = htons(atoi(argv[2]));
	// The destination port, we accept through command line
	tcp->dest = htons(atoi(argv[4]));
	tcp->seq = htonl(1);
	tcp->ack_seq = 0;
	tcp->doff = 5;
	tcp->syn = 1;
	tcp->ack = 0;
	tcp->window = htons(32767);
	tcp->check = 0; // Done by kernel
	tcp->urg_ptr = 0;
	// IP checksum calculation
	//ip->ip_sum = csum((unsigned short *) buffer, (sizeof(struct ip) + sizeof(struct tcphdr)));
	 
	// Inform the kernel do not fill up the headers' structure, we fabricated our own
	if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0)
	{
	    perror("setsockopt() error");
	    exit(-1);
	}
	else
	   printf("setsockopt() is OK\n");
	 
	printf("Using:::::Source IP: %s port: %u, Target IP: %s port: %u.\n", argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));
	 
	// sendto() loop, send every 2 second for 50 counts
	unsigned int count;
	for(count = 0; count < 20; count++)
	{
	if(sendto(sd, buffer, iph->ip_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	// Verify
	{
	   perror("sendto() error");
	   exit(-1);
	}
	else
	   printf("Count #%u - sendto() is OK\n", count);
	sleep(2);
	}
	close(sd);
	return 0;
}