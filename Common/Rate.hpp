//
// Created by Glasssix-KP on 2024/5/31.
//

#pragma once
#include <chrono>
#include <thread>

class Rate {
public:
    /*!
     * @param frequency unit:Hz
     */
    explicit Rate(double frequency) : desired_cycle_time_(1000.0 / frequency) {
        start_time_ = std::chrono::steady_clock::now();
    }

    Rate(Rate&&) = default;
    Rate& operator=(Rate&&) = default;
    Rate& operator=(const Rate&) = delete;
    Rate(const Rate&) = delete;

    void sleep() {
        const MilliDuration cycle_duration{std::chrono::high_resolution_clock::now() - start_time_};
        const MilliDuration sleep_duration{desired_cycle_time_ - cycle_duration};
        if (sleep_duration > MilliDuration{}) {
            std::this_thread::sleep_for(sleep_duration);
        }
        start_time_ += std::chrono::duration_cast<std::chrono::nanoseconds>(desired_cycle_time_);
    }

private:
    using MilliDuration = std::chrono::duration<double, std::milli>;
    std::chrono::steady_clock::time_point start_time_;
    MilliDuration desired_cycle_time_;
};
