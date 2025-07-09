#pragma once
#include <chrono>
#include <cstdint>
#include <string>

template <typename unit = std::milli>
class elapsed_timer {
public:
    elapsed_timer() : start_{std::chrono::high_resolution_clock::now()} {}

    double restart() {
        const auto end{std::chrono::high_resolution_clock::now()};
        const auto value{std::chrono::duration<double, unit>(end - start_).count()};
        start_ = end;
        return value;
    }

    double elapsed() {
        const auto end{std::chrono::high_resolution_clock::now()};
        const auto value{std::chrono::duration<double, unit>(end - start_).count()};
        return value;
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
};

template <typename unit = std::milli>
class scoped_timer {
public:
    scoped_timer() : t_{} {}
    ~scoped_timer() { printf("time:%lf\n", t_.elapsed()); }
    scoped_timer(const scoped_timer&) = delete;
    scoped_timer(const scoped_timer&&) = delete;
    const scoped_timer& operator=(const scoped_timer&) = delete;

private:
    elapsed_timer<unit> t_;
};
