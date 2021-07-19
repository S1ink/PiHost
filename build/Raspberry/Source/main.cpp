#define STD_FULL
#define PILIB_FULL
#include "pilib.h"

//typedef void(*function)(std::ostream&);
//
//void streamWrap(pilib::lstream& fout, function func) {
//	fout.openOutput();
//	func(fout);
//	fout.close();
//
//	fout.openInput();
//	std::cout << fout.rdbuf() << newline;
//	fout.close();
//}
//
//void work(std::ostream& output) {
//	output << pilib::dateStamp() << ": work done?" << newline;
//}

void aptRun(const char* message, std::ostream& output) {
	pilib::StopWatch ptime(false);
	output << "Pihost internal APT updater initialized. (" << pilib::dateStamp() << ")" << newline;
	int updates = pilib::aptUpdate(output);
	output << newline << newline << " * * * * * * * * " << newline << newline;
	int upgrades = pilib::aptUpgrade(output);
	output << "Updates: " << updates << newline << "Upgrades: " << upgrades << newline;
	output << "Process finished at: " << pilib::dateStamp() << newline;
	ptime.pTotal(output);
}

void streamWrap(const char* message, pilib::lstream output, pilib::templatefunc func) {
	std::cout << output.getFile() << newline;
	output.openOutput();
	func(message, output);
	output.close();
}

void pTasks(
	const char* filepath,
	std::ostream& output,
	std::atomic_bool& control,
	time_t th_uintv,
	std::vector<std::thread>& threads,
	std::map<std::string,
	pilib::templatefunc>& funcmap
) {
	std::ifstream reader(filepath);
	std::string linebuffer, numbuff;
	pilib::TaskFile databuffer;
	pilib::templatefunc funcbuff;
	std::getline(reader, linebuffer);
	char dlm = pilib::clearEnd(linebuffer);
	if (linebuffer != "name,command,output,mode,hr,min,sec") {
		output << "Task file is not in the correct format.\n";
	}
	else {
		while (std::getline(reader, linebuffer)) {
			std::istringstream linestream(linebuffer);
			std::getline(linestream, databuffer.name, csvd);
			std::getline(linestream, databuffer.command, csvd);
			std::getline(linestream, databuffer.output, csvd);
			std::getline(linestream, databuffer.mode, csvd);
			std::getline(linestream, numbuff, csvd);
			{std::istringstream numstream(numbuff); numstream >> databuffer.tme.hr; }
			std::getline(linestream, numbuff, csvd);
			{std::istringstream numstream(numbuff); numstream >> databuffer.tme.min; }
			std::getline(linestream, numbuff, dlm);
			{std::istringstream numstream(numbuff); numstream >> databuffer.tme.sec; }
			auto search = funcmap.find(databuffer.name);
			funcbuff = search->second;
			std::ios_base::openmode mode;
			if (databuffer.mode == "a") {
				mode = std::ios::app;
			}
			else if (databuffer.mode == "o") {
				mode = std::ios::trunc;
			}
			else {
				mode = std::ios_base::openmode::_S_ios_openmode_end;
			}
			pilib::lstream stream(databuffer.output.c_str(), mode);
			std::thread th(
				pilib::routineThread<void, const char*, pilib::lstream,
				pilib::templatefunc>,
				std::ref(control),
				std::move(databuffer.tme),
				th_uintv,
				static_cast<void(*)(const char*, pilib::lstream, pilib::templatefunc)>(streamWrap),
				databuffer.command.c_str(),
				stream,
				funcbuff
			);
			threads.emplace_back(std::move(th));
		}
	}
}

std::atomic_bool run = { true };

static void endCondition() {
	std::cin.ignore();
	run = false;
}

int main(int argc, char* argv[]) {
	pilib::StopWatch runtime;
	std::thread end(endCondition);

	std::map<std::string, pilib::templatefunc> functions;
	std::vector<std::thread> threads;
	functions.insert(std::pair<std::string, pilib::templatefunc>("apt", aptRun));

	pTasks("/data/pihost/tasks.csv", std::cout, run, 10, threads, functions);

	/*netstat::transfer nets;
	while (run) {
		netstat::ethernet(nets);
		std::cout << bytePrefixes(nets.down) << "/s down" << newline << bytePrefixes(nets.up) << "/s up" << newline << newline;
	}*/

	/*networking::Server webserver;
	webserver.prep();
	webserver.launch(std::ref(run), std::cout, networking::sendBinary);*/

	/*file.open("/PiSHARE/Media/Audio/MCU/06 The First Bite Is the Deepest.m4a", std::ios::out | std::ios::binary);
	file.write(&buffer[0], size);
	file.close();*/

	//pilib::lstream stream("/data/logs/work.txt", std::ios::trunc);
	//std::thread th(pilib::loopingThread < CHRONO::seconds::rep, CHRONO::seconds::period, void, pilib::lstream&, function >, std::ref(run), CHRONO::seconds(5), CHRONO::seconds(5), streamWrap, std::ref(stream), work);

	end.join();
	unsigned int i;
	for (i = 0; i < threads.size(); i++) {
		threads[i].join();
	}
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