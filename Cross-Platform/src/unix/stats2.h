#pragma once

#ifdef __unix__
#include <array>
#include <vector>
#include <chrono>
#include <initializer_list>

class Utilization {
public:
	struct CoreState {
		friend class Utilization;
	private:
		enum class States {
			USER, NICE, SYSTEM,
			IDLE, IOWAIT, IRQ,
			SOFTIRQ, STEAL,
			GUEST, GUEST_NICE,
			TOTAL
		};
		static const std::array<States, 2> idle_states;
		static const std::array<States, 8> active_states;

		std::array<uint64_t, (size_t)States::TOTAL> raw;

	public:
		uint64_t getIdle();
		uint64_t getActive();
		uint64_t getTotal();
		uint64_t getState(States s);
		uint64_t getStates(States states[], size_t size);
		uint64_t getStates(std::initializer_list<States> states);

		// calculation methods between two 'CoreState's

	};

private:

};

#else

#endif