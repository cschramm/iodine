/*
 * Copyright (c) 2006-2009 Bjorn Andersson <flex@kryo.se>, Erik Ekman <yarrick@kryo.se>
 * Copyright (c) 2011-2012 Julian Kranz <julian@juliankranz.de>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

/* Last byte of raw header is the command */
#define RAW_HDR_LEN 4
#define RAW_HDR_IDENT_LEN 3
#define RAW_HDR_CMD 3
#define RAW_HDR_CMD_LOGIN 0x10
#define RAW_HDR_CMD_DATA  0x20
#define RAW_HDR_CMD_PING  0x30

#define RAW_HDR_CMD_MASK  0xF0
#define RAW_HDR_USR_MASK  0x0F
#define RAW_HDR_GET_CMD(x) ((x)[RAW_HDR_CMD] & RAW_HDR_CMD_MASK)
#define RAW_HDR_GET_USR(x) ((x)[RAW_HDR_CMD] & RAW_HDR_USR_MASK)
extern const unsigned char raw_header[RAW_HDR_LEN];

#ifdef WINDOWS32
#include "windows.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <err.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#endif

#define DNS_PORT 53

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define QUERY_NAME_SIZE 256

#if defined IP_RECVDSTADDR 
# define DSTADDR_SOCKOPT IP_RECVDSTADDR 
# define dstaddr(x) ((struct in_addr *) CMSG_DATA(x)) 
#elif defined IP_PKTINFO 
# define DSTADDR_SOCKOPT IP_PKTINFO 
# define dstaddr(x) (&(((struct in_pktinfo *)(CMSG_DATA(x)))->ipi_addr))
# define dstaddr6(x) (&(((struct in6_pktinfo *)(CMSG_DATA(x)))->ipi6_addr))
#endif

#if defined IP_MTU_DISCOVER
  /* Linux */
# define IP_OPT_DONT_FRAG IP_MTU_DISCOVER
# define DONT_FRAG_VALUE IP_PMTUDISC_DO
#elif defined IP_DONTFRAG
  /* FreeBSD */
# define IP_OPT_DONT_FRAG IP_DONTFRAG
# define DONT_FRAG_VALUE 1
#elif defined IP_DONTFRAGMENT
  /* Winsock2 */
# define IP_OPT_DONT_FRAG IP_DONTFRAGMENT
# define DONT_FRAG_VALUE 1
#endif

#define T_UNSET 65432
/* Unused RR type; "private use" range, see http://www.bind9.net/dns-parameters */

struct packet 
{
	int len;		/* Total packet length */
	int sentlen;		/* Length of chunk currently transmitted */
	int offset;		/* Current offset */
	char data[64*1024];	/* The data */
	char seqno;		/* The packet sequence number */
	char fragment;		/* Fragment index */
};

struct query {
	char name[QUERY_NAME_SIZE];
	unsigned short type;
	unsigned short rcode;
	unsigned short id;
	union {
		struct in_addr v4;
		struct in6_addr v6;
	} destination;
	union {
		struct sockaddr v4;
		struct sockaddr_in6 v6;
	} from;
	int fromlen;
	unsigned short id2;
	struct sockaddr from2;
	int fromlen2;
};

enum connection {
	CONN_RAW_UDP,
	CONN_DNS_NULL,
	CONN_MAX
};

void check_superuser(void (*usage_fn)(void));
int open_dns(int, in_addr_t);
int open_dns_ipv6(int localport, struct in6_addr listen_ip6);
void close_dns(int);

void do_chroot(char *);
void do_setcon(char *);
void do_detach();
void do_pidfile(char *);

void read_password(char*, size_t);

int check_topdomain(char *);

#if defined(WINDOWS32) || defined(ANDROID)
#ifndef ANDROID
int inet_aton(const char *cp, struct in_addr *inp);
#endif

void err(int eval, const char *fmt, ...);
void warn(const char *fmt, ...);
void errx(int eval, const char *fmt, ...);
void warnx(const char *fmt, ...);
#endif

int recent_seqno(int , int);

#ifdef LINUX
void ipv6_addr_add(struct in6_addr *addr, uint8_t amount);
char ipv6_addr_equals(struct in6_addr *a, struct in6_addr *b);
char ipv6_net_check(struct in6_addr *net, char netmask);
void ipv6_print(struct in6_addr *ip, char netmask6);
char *ipv6_str(struct in6_addr *ip);
#endif

#endif
