#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <atomic>

#include "http.h"
#include "mimetype.h"
#include "../networking.h"
#include "../output.h"
#include "../program.h"
#include "../resources.h"

#ifdef _WIN32
#pragma push_macro("DELETE")
#pragma push_macro("ERROR")

#undef DELETE
#undef ERROR
#endif


class HttpServer : public BaseServer {
public:
	class HttpHandler;
	class HttpFormatter;

protected:
	std::atomic<bool> l_online;		//backup for if external control not specified
	const char* root;
	Version version;
	std::atomic<bool>* online;
	olstream logs;

	void prepServer();
	inline bool defLog();
	void lerror(const char* message);
	void ex_lerror(const char* message);

	template<class handler_t = HttpHandler, class formatter_t = HttpFormatter>
	static void serveWorker1_0(HttpServer* inst);
	template<class handler_t = HttpHandler, class formatter_t = HttpFormatter>
	static void serveWorker1_1(HttpServer* inst);

public:
	class HttpFormatter {
		friend class HttpServer;
	protected:
		HttpServer* that;
		olstream* getStream();
		Version* getVer();

		HttpFormatter() : that(nullptr) {}
		HttpFormatter(HttpServer* outer) : that(outer) {}
		virtual void update(HttpServer* outer);

	public:
		HttpFormatter(HttpServer& server) : that(&server) {}

		virtual void onServerStart();
		virtual void onConnect(int fd, const char* ip);
		virtual void onRequest(int fd, const char* ip, int readlen, const Request* req = nullptr, const char* resource = nullptr);
		virtual void onResponse(int fd, const char* ip, int sent, const Response* resp = nullptr, const char* resource = nullptr);
		virtual void onDisconnect(int fd, const char* ip);
		virtual void onServerEnd();


	};
	class HttpHandler {
		friend class HttpServer;

	protected:
		HttpServer* that;	//surrounding instance
		Response response;
		Request request_buff;

		HttpHandler() = delete;
		HttpHandler(HttpServer* outer) : that(outer), response(that->version) {}
		~HttpHandler() = default;

		virtual std::string resourceMapper(std::string&& root, const std::string& requested);
		//virtual std::string resourceSupplier(const std::string& path);

	public:
		HttpHandler(HttpServer& server) : that(&server), response(server.version) {}

		static const char* safeMime(const char* path);
		virtual void respond(const int socket, const char* ip, const int readlen, const std::string& input, HttpFormatter* formatter);	//server
		//void request(const std::string& input, std::ostream& output, bool init = false);	//client


	};

	HttpServer() = delete;
	HttpServer(
		const olstream& logger,
		const char* root = progdir.getDir(),
		std::atomic<bool>* control = nullptr,
		Version version = Version::HTTP_1_1,
		const char* port = "http",
		int max_clients = 5
	);
	HttpServer(
		olstream&& logger = &std::cout,
		const char* root = progdir.getDir(),
		std::atomic<bool>* control = nullptr,
		Version version = Version::HTTP_1_1,
		const char* port = "http",
		int max_clients = 5
	);
	~HttpServer() = default;

	inline const char* getRoot() const { return this->root; }
	inline Version getVersion() const { return this->version; }

	void setLog(const std::ios::openmode modes);
	void setLog(const char* file);
	void setLog(const char* file, const std::ios::openmode modes);
	void setLog(std::ostream* stream);
	void setLog(const olstream& target);
	void setLog(olstream&& target);

	void stop();
	void estop();

	template<class handler_t = HttpHandler, class formatter_t = HttpFormatter>
	void serve();
	template<class handler_t = HttpHandler, class formatter_t = HttpFormatter>
	void serve1_0();
	template<class handler_t = HttpHandler, class formatter_t = HttpFormatter>
	void serve1_1();
	template<class handler_t = HttpHandler, class formatter_t = HttpFormatter>
	void serveThread();
	template<class handler_t = HttpHandler, class formatter_t = HttpFormatter>
	void serveThread1_0();
	template<class handler_t = HttpHandler, class formatter_t = HttpFormatter>
	void serveThread1_1();


};


#include "server.inc"

#ifdef _WIN32
#pragma pop_macro("ERROR")
#pragma pop_macro("DELETE")
#endif