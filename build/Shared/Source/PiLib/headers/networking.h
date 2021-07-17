#pragma once

#include "include.h"

#include "utility.h"
#include "info.h"

namespace pilib {
	void sendHtml(std::string& request, std::ostream& output); //ignores request, sends payload no matter what
	void sendBinary(std::string& request, std::ostream& output);
	void errorPage(const char* file, const char* code, std::ostream& output); //return an html error page with inserted error code
	void simpleRequest(std::string& request, std::ostream& output); //supports GET and HEAD requests only

	class Server {
	private:
		sockaddr_in address;
		int sock, nsock, max_users, addrlen;
	public:
		typedef void(*responsehandler)(std::string&, std::ostream&);
		std::string root;

		Server(int connections = 5, int domain = AF_INET, int service = SOCK_STREAM, int protocol = 0, int port = 80, ulong interface = INADDR_ANY);
		void bindServer();
		void startListen();
		void prep();
		void launch(std::atomic_bool& condition, std::ostream& output, Server::responsehandler handler);

		std::string fullPath(const char* file);
		void basicHeader(std::ostream& output, const char* type, std::ostringstream& resource);

		void internalHandler(std::string& request, std::ostream& output);
	};
}
