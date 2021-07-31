#define INCLUDE_ALL
#define PILIB_FULL
#include "pilib.h"

std::atomic_bool run = { true };
//std::atomic_bool run2 = { true };

static void endCondition() {
	std::cin.ignore();
	run = false;
	/*std::cin.ignore();
	run2 = false;*/
}

int main(int argc, char* argv[]) {
	pilib::StopWatch runtime;
	std::thread end(endCondition);

	pilib::http::HttpServer server(pilib::http::resources::root, 5, pilib::http::Version::HTTP_1_1);
	server.serve_beta(run);

	end.join();
	//th.join();
	return 0;
}

//NOTES
/* CPU States
user – time spent in user mode.
nice – time spent in user mode with low priority.
system – time spent in system mode.
idle – time spent in the idle task.
iowait –  time waiting for I/O to complete.
irq – time servicing hardware interrupts.
softirq – time servicing software interrupts.
steal – time spent in other operating systems when running in a virtualized environment.
guest – time spent running a virtual CPU for guest operating systems.
guest_nice – time spent running a low priority virtual CPU for guest operating systems.
* * * *
idle + iowait = time doing nothing (usr_hz)
all others = time spend utilized
*/

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