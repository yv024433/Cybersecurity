// ----rawudp.c------
// Must be run by root lol! Just datagram, no payload/data
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include <arpa/inet.h>

// The packet length
#define PCKT_LEN 8192
 
// Function for checksum calculation. From the RFC,
// the checksum algorithm is:
//  "The checksum field is the 16 bit one's complement of the one's
//  complement sum of all 16 bit words in the header.  For purposes of
//  computing the checksum, the value of the checksum field is zero."
unsigned short csum(unsigned short *buf, int nwords)
{       //
        unsigned long sum;
        for(sum=0; nwords>0; nwords--)
                sum += *buf++;
        sum = (sum >> 16) + (sum &0xffff);
        sum += (sum >> 16);
        return (unsigned short)(~sum);
}
// Source IP, source port, target IP, target port from the command line arguments
int main(int argc, char *argv[])
{
  int sd;
	// No data/payload just datagram
	char buffer[PCKT_LEN];
	// Our own headers' structures
	struct ip *iph = (struct ip *) buffer;
	struct udphdr *udph = (struct udphdr *) (buffer + sizeof(struct ip));
	// Source and destination addresses: IP and port
	struct sockaddr_in sin, din;
	const int one = 1;
 
	memset(buffer, 0, PCKT_LEN);
 
	if(argc != 5)
	{
		printf("- Invalid parameters!!!\n");
		printf("- Usage %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);
		exit(-1);
	}
 
	// Create a raw socket with UDP protocol
	sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
	if(sd < 0)
	{
		perror("socket() error");
		// If something wrong just exit
		exit(-1);
	}
	else
		printf("socket() - Using SOCK_RAW socket and UDP protocol is OK.\n");
 
	// The source is redundant, may be used later if needed
	// The address family
	sin.sin_family = AF_INET;
	din.sin_family = AF_INET;
	// Port numbers
	sin.sin_port = htons(atoi(argv[2]));
	din.sin_port = htons(atoi(argv[4]));
	// IP addresses
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	din.sin_addr.s_addr = inet_addr(argv[3]);
 
	// Fabricate the IP header or we can use the
	// standard header structures but assign our own values.
	iph->ip_hl=5;
	iph->ip_v=4;
	iph->ip_tos = 16; // Low delay
	iph->ip_len = sizeof(struct ip) + sizeof(struct udphdr);
	iph->ip_id = htons(54321);
	iph->ip_ttl = 64; // hops
	iph->ip_p = 17; // UDP
	// Source IP address, can use spoofed address here!!!
	(iph->ip_src).s_addr = inet_addr(argv[1]);
	// The destination IP address
	(iph->ip_dst).s_addr = inet_addr(argv[3]);
	 
	// Fabricate the UDP header. Source port number, redundant
	udph->source = htons(atoi(argv[2]));
	// Destination port number
	udph->dest = htons(atoi(argv[4]));
	udph->len = htons(sizeof(struct udphdr));
	// Calculate the checksum for integrity
	//iph->ip_sum = csum((unsigned short *)buffer, sizeof(struct ip) + sizeof(struct udphdr));
	// Inform the kernel do not fill up the packet structure. we will build our own...
	if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0)
	{
		perror("setsockopt() error");
		exit(-1);
	}
	else
		printf("setsockopt() is OK.\n");
	 
	// Send loop, send for every 2 second for 100 count
	printf("Trying...\n");
	printf("Using raw socket and UDP protocol\n");
	printf("Using Source IP: %s port: %u, Target IP: %s port: %u.\n", argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));
	 
	int count;
	for(count = 1; count <=20; count++)
	{
		if(sendto(sd, buffer, iph->ip_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		// Verify
		{
			perror("sendto() error");
			exit(-1);
		}
		else
		{
			printf("Count #%u - sendto() is OK.\n", count);
			sleep(2);
		}
	}
	close(sd);
	return 0;
}