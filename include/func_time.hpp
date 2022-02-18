#pragma once
#include <chrono>
#include <string>

#define TIME_TYPE std::milli
//#define TIME_TYPE std::micro

class func_time
{
public:
	func_time() {
		start_ = std::chrono::steady_clock::now();
	}

	func_time(const std::string& title) : title_{ title } {
		start_ = std::chrono::steady_clock::now();
	}

	~func_time() {
		elapsed();
	}

	double elapsed() {
		const auto end{ std::chrono::steady_clock::now() };
		const auto value{ std::chrono::duration<double, TIME_TYPE>(end - start_).count() };

		if (title_.empty())
			printf("time: %lf ms\n", value);
		else
			printf("%s, time: %lf ms\n", title_.c_str(), value);

		return value;
	}

	double restart() {
		const auto end{ std::chrono::steady_clock::now() };
		const auto value{ std::chrono::duration<double, TIME_TYPE>(end - start_).count() };

		if (title_.empty())
			printf("time: %lf ms\n", value);
		else
			printf("%s, time: %lf ms\n", title_.c_str(), value);

		start_ = end;
		return value;
	}

private:
	std::chrono::steady_clock::time_point start_;
	std::string title_;
};
