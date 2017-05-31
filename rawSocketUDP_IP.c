#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct ip_header {
    
    unsigned char       ihl:4;
    unsigned char       ver:4;
    unsigned char       tos;
    unsigned short int  tlen;
    unsigned short int  id;
    unsigned char       flags:3;
    unsigned short int  frag_off:13;
    unsigned char       ttl;
    unsigned char       proto;
    unsigned short int  hdr_cksum;
    unsigned int        src_ip;
    unsigned int        dst_ip;
};

struct udp_header {

    unsigned short      src_port;
    unsigned short      dst_port;
    unsigned short      len;
    unsigned short      cksum;
};

#define BUFLEN 512

int main() {
    
    char buf[BUFLEN];   
    struct ip_header *iph = (struct ip_header *) buf;
    struct udp_header *udph = (struct udp_header *) (buf + sizeof(struct ip_header));
    struct sockaddr_in saddr;
    int one = 1;

    char source_ip[] = "4.4.4.4";
    char dest_ip[] = "172.16.112.1";
    int source_port = 1337;
    int dest_port = 42;

/*  SOCK_RAW: raw, use a valid IANA IP protocol (RFC1700)
    SOCK_DGRAM: udp, use with IPPROTO_UDP
    SOCK_STREAM: tcp, use with IPPROTO_TCP */
    int s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    //int s = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

/*  tell the kernel that we provide our own IP header 
    implied when using IPPROTO_RAW above 
    
    if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) == -1) {
        perror("setsockopt() failed");
        exit(1);
    }
*/

    iph->ver = 4;
    iph->ihl = 5;
    iph->tos = 0;
    iph->tlen = sizeof(struct ip_header) + sizeof(struct udp_header);
    iph->id = 1337;
    iph->flags = 0;
    iph->frag_off = 0;
    iph->ttl = 64;
    iph->proto = 17; //udp
    iph->hdr_cksum = 0;
    iph->src_ip = inet_addr(source_ip);
    iph->dst_ip = inet_addr(dest_ip);

    udph->src_port = htons(source_port);
    udph->dst_port = htons(dest_port);
    udph->len = htons(sizeof(struct udp_header));
    udph->cksum = 0;

    saddr.sin_family = AF_INET;
    inet_pton(AF_INET, dest_ip, &saddr.sin_addr);
   // saddr.sin_port = htons(dest_port);

    sendto(s, buf, iph->tlen, 0, (struct sockaddr*) &saddr, sizeof(saddr));
}

