#pragma once
#ifdef WIN32
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <netinet/in.h>
#endif

#define MAX_PACKET_LEN 1024 * 100
#define MAX_UDP_PACKET_LEN 1024 * 2

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef long long int64;
typedef int64 guid;
inline int64 htonll(int64 n) {
	return (int64)(((int64)htonl(n)) << 32) | htonl(n >> 32);
}
inline int64 ntohll(int64 n) {
	return (int64)(((int64)ntohl(n)) << 32) | ntohl(n >> 32);
}
inline double ntoh_double(double net_double) {
	int64 host_int64;
	host_int64 = ntohll(*((int64 *) &net_double));
	return *((double *) &host_int64);
}
inline double hton_double(double host_double) {
	int64 net_int64;
	net_int64 = htonll(*((int64 *) &host_double));
	return *((double *) &net_int64);
}