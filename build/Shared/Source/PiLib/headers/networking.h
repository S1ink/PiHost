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
		//enums moved to {pivar.h}

		/*template<typename type>	//implement for less lines?
		class HDB {
		private:
			static const std::unordered_map<std::string, type> typemap;
			static const std::unordered_map<type, std::string> stringmap;
		public:
			static type getType(const std::string& str) {
				auto search = typemap.find(str);
				return search->second;
			}
			static std::string getString(type type) {
				auto search = stringmap.find(type);
				return search->second;
			}
		};*/

		//implement checks on conversion functions
		struct Versions {
		private:
			static const std::unordered_map<std::string, Version> typemap;
			static const std::unordered_map<Version, std::string> stringmap;
		public:
			static Version getType(const std::string& str);
			static std::string getString(Version version);
		};
		struct Methods {
		private:
			static const std::unordered_map<std::string, Method> typemap;
			static const std::unordered_map<Method, std::string> stringmap;
		public:
			static Method getType(const std::string& str);
			static std::string getString(Method method);
		};
		struct Codes {
		private:
			static const std::unordered_map<std::string, Code> typemap;
			static const std::unordered_map<Code, std::string> stringmap;
		public:
			static Code getType(const std::string& str);
			static Code getType(int code);
			static std::string getString(Code code);
			static std::string getString(int code);
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
			//static (deserialize) {=constructor} methods here?
		};

		class Response {
		private:
			Code responsecode;
			Version version;
			std::map<std::string, std::string> headers;	//std::map<std::string, std::vector<Segment> > -> for complete RFC standard
			std::string body;
		public:
			Response(Version version = Version::HTTP_1_1) : version(version) {}
			Response(Code responsecode, std::vector<Segment>& headers, const std::string& body, Version version = Version::HTTP_1_1);	//add overload with {move}
			Response(const std::string& response);

			std::string getSerialized();

			void lateConstruct(Code responsecode, std::vector<Segment>& headers, const std::string& body);

			Code* intCode();
			Version* intVersion();
			std::string* intBody();
			std::map<std::string, std::string>* intHeaders();

			std::vector<Segment> getHeaders();

			uint bodyLen();

			static std::string getSerialized(Code responsecode, std::vector<Segment>& headers, const std::string& body, Version version = Version::HTTP_1_1);
			static void getSerialized(std::ostream& buffer, Code responsecode, std::vector<Segment>& headers, const std::string& body, Version version = Version::HTTP_1_1);
			//static (deserialize) {=constructor} methods here?
		};

		class HttpHandler {
		private:
			struct State {
				int sent;
				std::string bbuff;

			};

			std::string root;
			Version version;
			State state;

			std::string rPath(const char* item);
		public:
			HttpHandler(const char* root, Version version = Version::HTTP_1_1) : root(root), version(version) {}

			//server
			void respond(int socket, const std::string& input);
			//client
			//void request(const std::string& input, std::ostream& output, bool init = false); 
		};

		class HttpServer : public BaseServer {
		private:
			HttpHandler handler;
			Version version;
			void prepServer();
		public:
			HttpServer(const char* root = resources::root, int max_accepts = 5, Version version = Version::HTTP_1_1);

			void serve(const std::atomic_bool& rc, std::ostream& out = std::cout);
			void serve1_0(const std::atomic_bool& rc, std::ostream& out = std::cout);
			void serve1_1(const std::atomic_bool& rc, std::ostream& out = std::cout);
		};
	}
}