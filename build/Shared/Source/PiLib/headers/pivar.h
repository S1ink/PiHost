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
	namespace http {
		//https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol
		/*HEADER FORMAT:
		* version_{status-code}_{status-message}
		* date_{date}
		* server_{name}
		* type_{type}
		* len_{length}
		* ~~~~~~~~~>
		* (\n)
		* CONTENT */

		CE_STR http1 = "HTTP/1.0 ";
		CE_STR http11 = "HTTP/1.1 ";
		CE_STR date = "Date: ";
		CE_STR server = "Server: ";
		CE_STR type = "Content-Type: ";
		CE_STR len = "Content-Length: ";

		namespace req {
			CE_STR get = "GET";
			CE_STR head = "HEAD";
			CE_STR post = "POST";
			CE_STR put = "PUT";
			CE_STR del = "DELETE";
			CE_STR connect = "CONNECT";
			CE_STR options = "OPTIONS";
			CE_STR trace = "TRACE";
			CE_STR patch = "PATCH";
		}

		namespace codes {
			//https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
			CE_STR continu = "100 Continue";
			CE_STR ok = "200 OK";
			CE_STR created = "201 Created";
			CE_STR accepted = "202 Accepted";
			CE_STR no_content = "204 No Content";
			CE_STR reset_content = "205 Reset Content";
			CE_STR partial_content = "206 Partial Content";
			CE_STR mult_choices = "300 Multiple Choices";
			CE_STR moved_perm = "301 Moved Permanently";
			CE_STR found = "302 Found";
			CE_STR bad_request = "400 Bad Request";
			CE_STR unauthorized = "401 Unauthorized";
			CE_STR pay_up = "402 Payment Required";
			CE_STR forbidden = "403 Forbidden";
			CE_STR not_found = "404 Not Found";
			CE_STR method_na = "405 Method Not Allowed";
			CE_STR not_acc = "406 Not Acceptable";
			CE_STR request_timeout = "408 Request Timeout";
			CE_STR conflict = "409 Conflict";
			CE_STR gone = "410 Gone";
			CE_STR len_req = "411 Length Required";
			CE_STR url_too_l = "414 URI Too Long";
			CE_STR unsupported_media = "415 Unsupported Media Type";
			CE_STR tp_egg = "418 I'm a teapot"; //*
			CE_STR internal = "500 Internal Server Error";
			CE_STR not_impl = "501 Not Implemented";
			CE_STR bad_gate = "502 Bad Gateway";
			CE_STR service_unav = "503 Service Unavailable";
			CE_STR gate_timo = "504 Gateway Timeout";
			CE_STR http_unsup = "505 HTTP Version Not Supported";
			CE_STR not_ext = "510 Not Extended";
			CE_STR auth_req = "511 Network Authentication Required";
		}

		namespace types {
			//https://stackoverflow.com/questions/23714383/what-are-all-the-possible-values-for-http-content-type-header
			namespace text {
				CE_STR plain = "text/plain";
				CE_STR html = "text/html";
				CE_STR csv = "text/csv";
				CE_STR css = "text/css";
				CE_STR xml = "text/xml";
			}
			namespace image {
				CE_STR gif = "image/gif";
				CE_STR jpeg = "image/jpeg";
				CE_STR png = "image/png";
			}
			namespace audio {}
			namespace video {}
			namespace app {}
			namespace multi {}
			namespace vnd {}
		}

		namespace resources {
			CE_STR root = "/data/pihost/resources";
			CE_STR relative_root = "../";
			CE_STR home = "index.html";
			CE_STR error_page = "error.html";
		}
	}
}