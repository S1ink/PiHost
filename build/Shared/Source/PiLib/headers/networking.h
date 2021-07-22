#pragma once

#include "include.h"

#include "utility.h"
#include "info.h"

/* SERVER PROCESS
* - set up "addrinfo"'s base on type of server -> getaddrinfo()
* - create main socket from ^ -> socket()
* - bind the socket -> bind()
* - start listening -> listen()
* - send()/receive()
* 
*  VARIABLES
* addrinfo:
* - "family" -> ipv4 or ipv6
* - "socktype" -> TCP or UDP (stream or dgram)
* - ""
*/

namespace pilib {
	class BaseServer {
	private:
		addrinfo* list;
		const char* ip, *port;
		bool reuse;
		int family, socktype, flags, connections, sockmain;
	public:
		BaseServer(
			const char* ip, const char* port, int connections = 5, bool reusable = true,
			int family = AF_INET, int socktype = SOCK_STREAM, int flags = AI_PASSIVE
		) : ip(ip), port(port), connections(connections), reuse(reusable), family(family), socktype(socktype), flags(flags) {}

		~BaseServer();

		void getServerAddr();
		void getSock();
		void bindSock();
		void startListen();

		addrinfo* intAddr();
		int intSock();

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

	namespace http {
		enum class Method {
			GET, HEAD, POST,
			PUT, DELETE, TRACE,
			OPTIONS, CONNECT, PATCH
		};

		class Methods {
		private:
			static const std::map<std::string, Method> typemap;
			static const std::map<Method, std::string> stringmap;
		public:
			static Method getType(const std::string& str);
			static std::string getString(Method method);
		};

		enum class Version {
			HTTP_1_0,
			HTTP_1_1,
			HTTP_2_0
		};

		class Versions {
		private:
			static const std::map<std::string, Version> typemap;
			static const std::map<Version, std::string> stringmap;
		public:
			static Version getType(const std::string& str);
			static std::string getString(Version version);
		};
		

		//(header)
		//This class represents the header segments (a newline-terminated line) found within both HTTP requests and responses
		class Segment {
		private:
			std::string key;
			std::string value;
		public:
			Segment(const std::string& key, const std::string& value) : key(key), value(value) {}
			Segment(const std::string&& key, const std::string&& value) : key(std::move(key)), value(std::move(value)) {}
			Segment(const std::string& segment);
			Segment(const std::string&& segment);

			std::string getKey();
			std::string getValue();

			std::string* intKey();
			std::string* intValue();

			std::string getSerialized();
			static std::string getSerialized(const std::string& key, const std::string& value);
			static Segment getDeserialized(const std::string& segment);
		};

		class Request {
		private:
			Version version;
			Method method;
			std::string resource;
			std::map<std::string, std::string> headers;
		public:
			//create helper method for making bulk {Segments} ~somewhere
			Request(Method method, const std::string& resource, std::vector<Segment>& headers, Version version = Version::HTTP_1_1);
			Request(const std::string& reqest);

			std::string getSerialized();

			Version* intVersion();
			Method* intMethod();
			std::string* intResource();
			std::map<std::string, std::string>* intHeaders();

			std::vector<Segment> getHeaders();

			static std::string getSerialized(Method method, const std::string& resource, std::vector<Segment>& headers, Version version = Version::HTTP_1_1);	//add overload with {move}
			static void getSerialized(std::ostream& buffer, Method method, const std::string& resource, std::vector<Segment> headers, Version version = Version::HTTP_1_1);
			//static {constructor} methods here?
		};

		class Response {
		private:

		public:

		};
	}

	class HttpHandler {
	private:
		std::string root;
	public:
		HttpHandler(const char* root) : root(root) {}

		std::string fullPath(const char* item);
	};

	class HttpServer : public BaseServer {
	private:
		HttpHandler handler;
		void prepServer();
	public:
		HttpServer(const char* root = http::resources::root, int max_accepts = 5);

		void server1_0(const std::atomic_bool& rc, std::ostream& out = std::cout);
		void serve(const std::atomic_bool& rc, std::ostream& out = std::cout);
	};
}