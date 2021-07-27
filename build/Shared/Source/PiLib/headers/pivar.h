//GLOBAL COMPILE-TIME VARS
#pragma once

#define CHRONO std::chrono
#define CE_STR constexpr char const*

constexpr char newline = 10;	// ('\n')
constexpr char space = 32;		// (' ')
constexpr char csvd = 44;		// (',')
constexpr char cr = 13;			// ('\r')
constexpr char null = 0;		// ('\0')

CE_STR endline = "\r\n";

namespace locations {
	namespace stats {
		CE_STR cpu = "/proc/stat";
		CE_STR network = "/proc/net/dev";
		CE_STR load = "/proc/loadavg";
		CE_STR disk = "/proc/diskstats";
		CE_STR mem = "/proc/meminfo";
	}

	namespace external {

		CE_STR winbackup = "/data/pihost/resources/rsyncbackup.csv";
		//CE_STR r_winbackup = "../external/rsyncbackup.csv";
		CE_STR tasks = "/data/pihost/resources/tasks.csv";
		//CE_STR r_tasks = "../external/tasks.csv";
	}
}

namespace gpin {
	constexpr unsigned short pi_fan = 18;
	constexpr unsigned short pi_power = 3;

	constexpr unsigned short pc_power = 20;
	constexpr unsigned short pc_reset = 16;
	constexpr unsigned short pc_status = 12;
}

namespace pilib {
	//https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol
	namespace http {
		enum class Version {
			HTTP_1_0,
			HTTP_1_1,
			HTTP_2_0
		};
		enum class Method {
			GET, HEAD, POST,
			PUT, DELETE, TRACE,
			OPTIONS, CONNECT, PATCH
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
			NETWORK_AUTHICATION_REQIRED = 511
		};

		namespace resources {
			CE_STR root = "/data/pihost/resources";
			CE_STR relative_root = "../";
			CE_STR home = "index.html";
			CE_STR error_page = "error.html";
		}
	}
}