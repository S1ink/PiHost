#define INCLUDE_ALL
#define PILIB_FULL
#include "pilib.h"

#include <csignal>

std::atomic_bool run = { true };
//std::atomic_bool run2 = { true };

const std::array<std::string, 31> sigmap = {
	"console hangup", "keyboard interrupt", "quit request",
	"illegal CPU instruction", "trace request", "abort",
	"invalid bus address", "floating point arithmetic error, likely div/0",
	"program killed (unblockable)", "custom signal, not from os",
	"segmentation violation - check for bad '*/&'", "custom signal - not an os error",
	"broken pipe", "alarm", "request to terminate", "stack fault",
	"child status has changed", "command to continue",
	"program stopped (unblockable)", "keyboard stop", "backround terminal read",
	"backround terminal write", "urgent socket condition", "CPU limit exceeded",
	"file size limit exceeded", "virtual alarm", "profiling alarm",
	"window size change", "IO now possible", "power restart (terminate)", "bad system call",
};
const std::array<int, 17> int_sigs = {
	1, 5, 10, 12, 13, 14, 17, 18, 20, 21, 22, 23, 26, 27, 28, 29, 31
};
const std::array<int, 14> exit_sigs = {
	2, 3, 4, 6, 7, 8, 11, 15, 16, 24, 25, 30
};

static void endCondition() {
	std::cin.ignore();
	run = false;
	/*std::cin.ignore();
	run2 = false;*/
}

static void callback(int gpio, int level, uint32_t tick) {
	return;
}

void sigIgnore(int signum) {
	pilib::StopWatch scope;
	std::string desc = sigmap[signum-1];
	std::cout << pilib::dateStamp() << " : Signal {" << signum << "} caught [" << desc << "] - ignored";
}

void sigTerminate(int signum) {
	pilib::StopWatch scope;
	std::string desc = sigmap[signum-1];
	std::cout << pilib::dateStamp() << " : Signal {" << signum << "} caught [" << desc << "] - calling exit\n";
	run = false;
}

int main(int argc, char* argv[]) {
	//pilib::StopWatch runtime;
	//std::thread end(endCondition);

	/*pilib::http::HttpServer server(pilib::http::resources::root, 5, pilib::http::Version::HTTP_1_1);
	server.serve_beta(run);*/
	
	{
		pilib::StopWatch scope;

		for (uint i = 0; i < int_sigs.size(); i++) {
			signal(int_sigs[i], sigIgnore);
		}
		for (uint i = 0; i < exit_sigs.size(); i++) {
			signal(exit_sigs[i], sigTerminate);
		}
	}

	while (run) {
		std::cout << "loop run!" << newline;
		std::this_thread::sleep_for(CHRONO::seconds(5));
	}

	//end.join();
	//th.join();
	return 0;
}

//SIGNALS
/*
9 - KILL >>>>>>>>>>>>>>>>>>>> {unblockable}
19 - Stop >>>>>>>>>>>>>>>>>>> {unblockable}

Handle/Ignore/Log: 
1 - Hangup
5 - Trace trap
13 - Broken pipe
17 - Child status has changed
18 - Continue
20 - Keyboard stop
21 - Backround read from "tty"
22 - Backround write to "tty"
23 - Urgent condition on socket
28 - Window size change
29 - IO now possible
31 - Bad system call

Exit:
2 - Interrupt
3 - Quit
4 - Illegal instruction *
6 - Abort
7 - Bus error **
8 - FPAE
11 - Segmentation violation
15 - Termination
16 - Stack fault *
24 - CPU limit exceeded *
25 - File size limit exceeded *
30 - Power failure restart

Conditional:
10 - user defined
12 - user defined #2
14 - Alarm
26 - Virtual time alarm
27 - Profiling alarm clock

*/

//NOTES

/*while (true) {
		std::string inp;
		std::cout << "Enter a file path to open: ";
		std::cin >> inp;
		if (inp == "exit") {
			break;
		}
		uint size = 0;
		std::string buffer;
		std::fstream file;
		file.open(inp, std::ios::in | std::ios::binary);
		if (!file) {
			std::cout << "File is not readable or does not exist" << newline;
		}
		else {
			file.seekg(0, std::ios::end);
			size = file.tellg();
			buffer.resize(size);
			file.seekg(0, std::ios::beg);
			file.read(&buffer[0], size);
			file.close();
			std::cout << buffer.size() << newline;
			std::cout << buffer << newline;
		}
	}*/