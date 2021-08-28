#pragma once

#include "../STD.h"
#include "../IO.h"
#include "../Utility.h"

#include "../pivar.h"

//https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol

namespace pilib {
	enum class Version {
		HTTP_1_0,
		HTTP_1_1,
		HTTP_2_0,
		TOTAL,
		ERROR
	};
	enum class Method {
		GET, HEAD, POST,
		PUT, DELETE, TRACE,
		OPTIONS, CONNECT, PATCH,
		TOTAL, ERROR
	};
	//Does not include all possible codes, only ~relevant~ ones -> full list here: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
	enum class Code {
		CONTINUE = 100, OK = 200, CREATED = 201, ACCEPTED = 202,
		NO_CONTENT = 204, RESET_CONTENT = 205, PARTIAL_CONTENT = 206,
		MULTIPLE_CHOICES = 300, MOVED_PERMANENTLY = 301, FOUND = 302,
		BAD_REQUEST = 400, UNAUTHORIZED = 401, PAYMENT_REQUIRED = 402,
		FORBIDDEN = 403, NOT_FOUND = 404, METHOD_NOT_ALLOWED = 405,
		NOT_ACCEPTABLE = 406, REQUEST_TIMEOUT = 408, CONFLICT = 409,
		GONE = 410, LENGTH_REQUIRED = 411, URI_TOO_LONG = 414,
		UNSUPPORTED_MEDIA_TYPE = 415, IM_A_TEAPOT = 418,
		INTERNAL_SERVER_ERROR = 500, NOT_IMPLEMENTED = 501,
		BAD_GATEWAY = 502, SERVICE_UNAVAILABLE = 503, GATEWAY_TIMEOUT = 504,
		HTTP_VERSION_NOT_SUPPORTED = 505, NOT_EXTENDED = 510,
		NETWORK_AUTHICATION_REQIRED = 511, ERROR
	};

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
		HeaderList() {}
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
}