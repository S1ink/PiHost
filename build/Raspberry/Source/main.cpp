#define STD_FULL
#define PILIB_FULL
#include "pilib.h"

typedef void(*function)(std::ostream&);

void streamWrap(pilib::lstream& fout, function func) {
	fout.openOutput();
	func(fout);
	fout.close();

	fout.openInput();
	std::cout << fout.rdbuf() << newline;
	fout.close();
}

void work(std::ostream& output) {
	output << pilib::dateStamp() << ": work done?" << newline;
}

std::atomic_bool run = { true };

static void endCondition() {
	std::cin.ignore();
	run = false;
}

void winBackup(std::ostream& output) {
	output << "Pihost internal WinBackup initialized. (" << pilib::dateStamp() << ")" << newline << newline;
	pilib::winSync(locations::external::winbackup, output);
	output << "Process finished at: " << pilib::dateStamp() << newline;
}

int main(int argc, char* argv[]) {
	pilib::StopWatch runtime;

	//std::thread end(endCondition);

	/*netstat::transfer nets;
	while (run) {
		netstat::ethernet(nets);
		std::cout << bytePrefixes(nets.down) << "/s down" << newline << bytePrefixes(nets.up) << "/s up" << newline << newline;
	}*/

	/*networking::Server webserver;
	webserver.prep();
	webserver.launch(std::ref(run), std::cout, networking::sendBinary);*/

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
	/*file.open("/PiSHARE/Media/Audio/MCU/06 The First Bite Is the Deepest.m4a", std::ios::out | std::ios::binary);
	file.write(&buffer[0], size);
	file.close();*/

	//pilib::lstream stream("/data/logs/work.txt", std::ios::trunc);
	//std::thread th(pilib::loopingThread < CHRONO::seconds::rep, CHRONO::seconds::period, void, pilib::lstream&, function >, std::ref(run), CHRONO::seconds(5), CHRONO::seconds(5), streamWrap, std::ref(stream), work);

	winBackup(std::cout);

	//end.join();
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