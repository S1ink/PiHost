#pragma once

#ifdef __unix__
#include <array>
#include <vector>
#include <chrono>
#include <fstream>
#include <thread>

#ifndef CORE_STATISTICS_SRC
#define CORE_STATISTICS_SRC	"/proc/stat"
#endif
#ifndef CORE_STATISTICS_STRICT_PARSING
#define CORE_STATISTICS_STRICT_PARSING	0
#endif


template<typename t>
inline size_t operator~(t v) { return static_cast<size_t>(v); }

class CoreStats {
public:
	typedef uint64_t	_jiffies;
	enum class State {
		USER, NICE, SYSTEM,
		IDLE, IOWAIT, IRQ,
		SOFTIRQ, STEAL,
		GUEST, GUEST_NICE,
		TOTAL
	};
	// static inline const std::array<State, 2> idle_states{
	// 	State::IDLE, State::IOWAIT
	// };
	// static inline const std::array<State, 8> active_states{
	// 	State::USER, State::NICE, State::SYSTEM,
	// 	State::IRQ, State::SOFTIRQ, State::STEAL,
	// 	State::GUEST, State::GUEST_NICE
	// };

	inline CoreStats(bool p_all = false) : parse_all(p_all) {}
	CoreStats(const CoreStats&) = delete;
	CoreStats(CoreStats&&) = default;

	inline void configParseAll(bool val) { this->parse_all = val; }
	inline bool validCore(uint16_t c) const { return c >= 0 && c < this->individual.size(); }

	inline _jiffies getIdle(size_t i = IMMEDIATE) const
		{ return getStates<State::IDLE, State::IOWAIT>(i); }
	inline _jiffies getCoreIdle(uint16_t c, size_t i = IMMEDIATE) const
		{ return getCoreStates<State::IDLE, State::IOWAIT>(c, i); }
	inline _jiffies getActive(size_t i = IMMEDIATE) const
		{ return getStates<State::USER, State::NICE, State::SYSTEM, State::IRQ, State::SOFTIRQ, State::STEAL, State::GUEST, State::GUEST_NICE>(i); }
	inline _jiffies getCoreActive(uint16_t c, size_t i = IMMEDIATE) const
		{ return getCoreStates<State::USER, State::NICE, State::SYSTEM, State::IRQ, State::SOFTIRQ, State::STEAL, State::GUEST, State::GUEST_NICE>(c, i); }
	inline _jiffies getTotal(size_t i = IMMEDIATE) const
		{ return getIdle(i) + getActive(i); }
	inline _jiffies getCoreTotal(uint16_t c, size_t i = IMMEDIATE) const
		{ return getCoreIdle(c, i) + getCoreActive(c, i); }
	template<State s>
	_jiffies getState(size_t = IMMEDIATE) const;
	template<State s>
	_jiffies getCoreState(uint16_t c, size_t = IMMEDIATE) const;
	template<State s = State::TOTAL, State... states>
	_jiffies getStates(size_t = IMMEDIATE) const;
	template<State s = State::TOTAL, State... states>
	_jiffies getCoreStates(uint16_t c, size_t = IMMEDIATE) const;

	// ** CURRENTLY OUTPUTS RATIO NOT PERCENT **
	float fromLast();
	float fromLastCore(uint16_t c);
	void fromLastAll(std::vector<float>&);

	template<typename rep, typename period>
	float inlineAverage(std::chrono::duration<rep,period>);
	template<typename rep, typename period>
	float inlineAverageCore(std::chrono::duration<rep,period>, uint16_t c);
	template<typename rep, typename period>
	void inlineAverageAll(std::chrono::duration<rep,period>, std::vector<float>&);

	static float temp();


private:
	typedef std::array<_jiffies, (size_t)State::TOTAL>	CoreBuff;
	enum {
		IMMEDIATE = 0,
		REFERENCE = 1
	};

	std::array<CoreBuff, 2> main;
	std::vector<std::array<CoreBuff, 2> > individual;
	std::ifstream reader;
	char head[4];
	bool parse_all;

	void updateBuff();


};


template<CoreStats::State s>
CoreStats::_jiffies CoreStats::getState(size_t i) const {
	// if constexpr(s == State::TOTAL) {
	// 	return this->getTotal(i);
	// }
	return this->main[i][~s];
}
template<CoreStats::State s>
CoreStats::_jiffies CoreStats::getCoreState(uint16_t c, size_t i) const {
	// if constexpr(s == State::TOTAL) {
	// 	return this->getCoreTotal(c, i);
	// }
	if(this->validCore(c)) {
		return this->individual[c][i][~s];
	}
	return 0U;
}
template<CoreStats::State s, CoreStats::State... states>
CoreStats::_jiffies CoreStats::getStates(size_t i) const {
	if constexpr(s != State::TOTAL) {
		return this->getState<s>(i) + this->getStates<states...>(i);
	}
	return 0U;
}
template<CoreStats::State s, CoreStats::State... states>
CoreStats::_jiffies CoreStats::getCoreStates(uint16_t c, size_t i) const {
	if constexpr(s != State::TOTAL) {
		return this->getCoreState<s>(c, i) + this->getCoreStates<states...>(c, i);
	}
	return 0U;
}

template<typename rep, typename period>
float CoreStats::inlineAverage(std::chrono::duration<rep,period> dt) {
	this->updateBuff();
	std::this_thread::sleep_for(dt);
	return this->fromLast();
}
template<typename rep, typename period>
float CoreStats::inlineAverageCore(std::chrono::duration<rep,period> dt, uint16_t c) {
	this->updateBuff();
	std::this_thread::sleep_for(dt);
	return this->fromLastCore(c);
}
template<typename rep, typename period>
void CoreStats::inlineAverageAll(std::chrono::duration<rep,period> dt, std::vector<float>& out) {
	this->updateBuff();
	std::this_thread::sleep_for(dt);
	this->fromLastAll(out);
}

#else
#endif