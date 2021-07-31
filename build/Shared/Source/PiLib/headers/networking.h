#pragma once

#define INCLUDE_ALL
#include "include.h"
#undef INCLUDE_ALL

#include "utility.h"
#include "info.h"
#include "../External/mimetype.h"

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
			Segment(std::pair<std::string, std::string> pair);

			std::string getKey();
			std::string getValue();

			std::string* intKey();
			std::string* intValue();

			std::string getSerialized();
			static std::string getSerialized(const std::string& key, const std::string& value);
			static Segment getDeserialized(const std::string& segment);
		};
		//create inherited classes that define common headers ^

		class HeaderList {
		private:
			std::vector<Segment> headers;
		public:
			HeaderList(){}
			HeaderList(const std::vector<Segment>& list) : headers(list) {}
			HeaderList(const size_t size);
			HeaderList(const HeaderList& other) : headers(other.headers) {}
			HeaderList(const std::string& body);

			void add(const Segment& header);
			void add(const std::string& key, const std::string& value);
			void add(const std::vector<Segment>& list);
			void add(const std::string& body);

			void reserve(size_t reserve);
			void reset();
			std::string find(std::string key);
			std::string allHeaders();

			std::vector<Segment>* intHeaders();

			static std::unordered_map<std::string, std::string> headerMap(std::vector<Segment>& headers);
		};

		class Request {
		private:
			Version version;
			Method method;
			std::string resource;
			HeaderList headers;
		public:
			//create helper method for making bulk {Segments} ~somewhere ~~ >> HeaderList << 
			Request(Method method, const std::string& resource, const HeaderList& headers, Version version = Version::HTTP_1_1)
				: version(version), method(method), resource(resource), headers(headers) {}
			Request(const std::string& reqest);

			std::string getSerialized();

			Version* intVersion();
			Method* intMethod();
			std::string* intResource();
			HeaderList* intHeaders();

			static std::string getSerialized(Method method, const std::string& resource, std::vector<Segment>& headers, Version version = Version::HTTP_1_1);	//add overload with {move}
			static void getSerialized(std::ostream& buffer, Method method, const std::string& resource, std::vector<Segment>& headers, Version version = Version::HTTP_1_1);
			//static (deserialize) {=constructor} methods here?
		};

		class Response {
		private:
			Code responsecode;
			Version version;
			HeaderList headers;	//std::map<std::string, std::vector<Segment> > -> for complete RFC standard
			std::string body;
		public:
			Response(Version version = Version::HTTP_1_1) : version(version) {}
			Response(Code responsecode, const HeaderList& headers, const std::string& body = std::string(), Version version = Version::HTTP_1_1)
				: responsecode(responsecode), version(version), headers(headers), body(body) {}	//add overload with {move}
			Response(const std::string& response);

			void update(Code responsecode, const std::vector<Segment>& headers, const std::string& body = std::string());
			void update(Code responsecode, HeaderList& headers, const std::string& body = std::string());
			std::string getSerialized();

			Code* intCode();
			Version* intVersion();
			std::string* intBody();
			HeaderList* intHeaders();

			uint bodyLen();

			static std::string getSerialized(Code responsecode, std::vector<Segment>& headers, const std::string& body = std::string(), Version version = Version::HTTP_1_1);
			static void getSerialized(std::ostream& buffer, Code responsecode, std::vector<Segment>& headers, const std::string& body = std::string(), Version version = Version::HTTP_1_1);
			//static (deserialize) {=constructor} methods here?
		};

		class HttpHandler {
		private:
			/*struct State {
				int sent;
				std::string bbuff;

			};*/

			std::string root;
			Version version;
			//Request request;
			Response response;
			int sent;

			std::string rPath(const char* item);
		public:
			HttpHandler(const char* root = resources::root, Version version = Version::HTTP_1_1) : root(root), version(version), response(version) /*, request(version)*/ {}

			//server
			void respond(const int& socket, const std::string& input);
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
			void serve_beta(const std::atomic_bool& rc, std::ostream& out = std::cout);
		};
	}
}