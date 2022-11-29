#pragma once
#include <chrono>
#include <string>
#include <cstdint>

class timer
{
public:
	timer() :start_{ std::chrono::high_resolution_clock::now() } {}

	template<typename unit = std::milli>
	double restart() {
		const auto end{ std::chrono::high_resolution_clock::now() };
		const auto value{ std::chrono::duration<double, unit>(end - start_).count() };
		start_ = end;
		return value;
	}

	template<typename unit = std::milli>
	double elapsed() {
		const auto end{ std::chrono::high_resolution_clock::now() };
		const auto value{ std::chrono::duration<double, unit>(end - start_).count() };
		return value;
	}
private:
	std::chrono::high_resolution_clock::time_point start_;
};

template<typename unit = std::milli>
class func_time
{
public:
	func_time() : t_{} {}
	~func_time() { printf("time:%lf\n", t_.elapsed<unit>()); }
	func_time(const func_time&) = delete;
	func_time(const func_time&&) = delete;
	const func_time& operator=(const func_time&) = delete;
private:
	timer t_;
};
