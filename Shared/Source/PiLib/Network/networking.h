#pragma once

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

namespace pilib {
	//network utilities
	void getSockIp(int socket, char* ip);

	class BaseServer {
	private:
		const char *ip, *port;
		bool reuse;
		int family, socktype, flags, connections;
	protected:
		//friend class http::HttpServer;
		addrinfo* list;
		int sockmain;
	public:
		BaseServer(
			const char* ip, const char* port, int connections = 5, bool reusable = true,
			int family = AF_INET, int socktype = SOCK_STREAM, int flags = AI_PASSIVE
		) : ip(ip), port(port), reuse(reusable), family(family), socktype(socktype), flags(flags), connections(connections) {}

		~BaseServer();

		void getServerAddr();
		void getSock();
		void bindSock();
		void startListen();

		//general
		static void getAddrs(const char* ip, const char* port, addrinfo* hints, addrinfo** list);
		static void getAddrs(addrinfo** list, const char* ip, const char* port, int family = AF_UNSPEC, int socktype = SOCK_STREAM, int flags = AI_PASSIVE);
		static void getAddr4(addrinfo** list, const char* ip, const char* port, int socktype = SOCK_STREAM, int flags = AI_PASSIVE);
		static void getAddr6(addrinfo** list, const char* ip, const char* port, int socktype = SOCK_STREAM, int flags = AI_PASSIVE);
		static void getServerAddr(addrinfo** list, const char* port, int family = AF_INET, int socktype = SOCK_STREAM);
		static void getServerAddr(addrinfo** list);

		static void addrListVec(addrinfo* list, std::vector<addrinfo>& vec);

		static int getSock(addrinfo* addr, bool reuse = true);
		static int getSock(const addrinfo& addr, bool reuse = true);
		static int bindNewSock(addrinfo* addr, bool reuse = true);
		static void bindSock(int sock, addrinfo* addr);

		static void startListen(int sock, int connections);
	};
}