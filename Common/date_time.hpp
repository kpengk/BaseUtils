#pragma once
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

namespace detail {
/*!
 Counts the number of identical leading characters in s.

 If s is empty, returns 0. Otherwise, returns the number of
 consecutive {s.front()} characters at the start of s.

 repeat_count("a");   // == 1
 repeat_count("ab");  // == 1
 repeat_count("aab"); // == 2
*/
inline int repeat_count(std::string_view s) {
    if (s.empty())
        return 0;
    const char c{s[0]};
    std::size_t j{1};
    for (; j < s.size() && s[j] == c; ++j) {}
    return static_cast<int>(j);
}

inline std::string to_string_width(int val, int width = -1) {
    char buf[16]{};
    switch (width) {
        case 1: sprintf(buf, "%01d", val); return buf;
        case 2: sprintf(buf, "%02d", val); return buf;
        case 3: sprintf(buf, "%03d", val); return buf;
        case 4: sprintf(buf, "%04d", val); return buf;
        case 5: sprintf(buf, "%05d", val); return buf;
        case 6: sprintf(buf, "%06d", val); return buf;
        case 7: sprintf(buf, "%07d", val); return buf;
        case 8: sprintf(buf, "%08d", val); return buf;
        case 9: sprintf(buf, "%09d", val); return buf;
        default: break;
    }
    return std::to_string(val);
}
} // namespace

enum class time_spec
{
    utc,
    local
};

/*
| Expression | Output                                                                |
| :--------: | :-------------------------------------------------------------------- |
|     yy     | The year as a two digit number (00 to 99)                             |
|    yyyy    | The year as a four digit number. If the year is negative, a minus sign is prepended, making five characters. |
|     M      | The month as a number without a leading zero (1 to 12)                |
|     MM     | The month as a number with a leading zero (01 to 12)                  |
|     d      | The day as a number without a leading zero (1 to 31)                  |
|     dd     | The day as a number with a leading zero (01 to 31)                    |
|     H      | The hour without a leading zero (0 to 23)                             |
|     HH     | The hour with a leading zero (00 to 23)                               |
|     m      | The minute without a leading zero (0 to 59)                           |
|     mm     | The minute with a leading zero (00 to 59)                             |
|     s      | The whole second, without any leading zero (0 to 59)                  |
|     ss     | The whole second, with a leading zero where applicable (00 to 59)     |
|     z      | The fractional part of the second, to go after a decimal point, without trailing zeroes (0 to 999). |
|    zzz     | The fractional part of the second, to millisecond precision, including trailing zeroes where applicable (000 to 999). |
*/

class date_time {
public:
    date_time()
        : timestamp_{} {}

    explicit date_time(std::chrono::system_clock::time_point timestamp, time_spec spec = time_spec::utc)
        : timestamp_{timestamp} {
        if (spec == time_spec::local) {
            timestamp_ -= std::chrono::hours{8};
        }
    }

    date_time(const date_time& other) = default;
    date_time(date_time&& other) = default;
    date_time& operator=(const date_time& other) = default;
    ~date_time() = default;

#if __cplusplus >= 202002L
    std::chrono::year_month_day date() const {
        return std::chrono::floor<std::chrono::days>(timestamp_);
    }

    template <typename T = std::chrono::milliseconds>
    std::chrono::hh_mm_ss<T> time() const {
        const std::chrono::hh_mm_ss hms{std::chrono::duration_cast<T>(
            timestamp_.time_since_epoch() -
            std::chrono::duration_cast<std::chrono::days>(timestamp_.time_since_epoch()))};
        return hms;
    }
#endif

    int millisecond() const {
        const auto since_epoch = timestamp_.time_since_epoch();
        const auto sec = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch - sec);
        return ms.count();
    }

    template <typename T = std::chrono::milliseconds>
    std::int64_t to_since_epoch() const {
        return std::chrono::duration_cast<T>(timestamp_.time_since_epoch()).count();
    }

    std::string to_string(std::string_view format, time_spec spec = time_spec::local) const {
        std::string result;
        result.reserve(24);

        date_time dt = *this;
        if (spec == time_spec::local) {
            dt.timestamp_ += std::chrono::hours{8};
        }
#if __cplusplus >= 202002L
        const auto ymd{dt.date()};
        const auto hms{dt.time()};

        const auto year{static_cast<int>(ymd.year())};
        const auto month{static_cast<unsigned int>(ymd.month())};
        const auto day{static_cast<unsigned int>(ymd.day())};
        const auto hour{static_cast<unsigned int>(hms.hours().count())};
        const auto minute{static_cast<unsigned int>(hms.minutes().count())};
        const auto second{static_cast<unsigned int>(hms.seconds().count())};
#else
        const std::time_t tt = std::chrono::system_clock::to_time_t(dt.timestamp_);
        const std::tm* tm = gmtime(&tt);

        const int year{tm->tm_year + 1900};
        const int month{tm->tm_mon + 1};
        const int day{tm->tm_mday};
        const int hour = tm->tm_hour;
        const int minute = tm->tm_min;
        const int second = tm->tm_sec;
#endif

        int i{};
        while (i < format.size()) {
            const char c{format[i]};
            int repeat{detail::repeat_count(format.substr(i))};

            switch (c) {
                case 'y':
                    if (repeat >= 4) {
                        repeat = 4;
                        const int len = (year < 0) ? 5 : 4;
                        result.append(detail::to_string_width(year, len));
                    } else if (repeat >= 2) {
                        repeat = 2;
                        result.append(detail::to_string_width(year % 100, 2));
                    } else {
                        repeat = 1;
                        result.push_back(c);
                    }
                    break;
                case 'M':
                    repeat = std::min(repeat, 2);
                    switch (repeat) {
                        case 1: result.append(detail::to_string_width(month)); break;
                        case 2: result.append(detail::to_string_width(month, 2)); break;
                        default: break;
                    }
                    break;
                case 'd':
                    repeat = std::min(repeat, 2);
                    switch (repeat) {
                        case 1: result.append(detail::to_string_width(day)); break;
                        case 2: result.append(detail::to_string_width(day, 2)); break;
                        default: break;
                    }
                    break;
                case 'H':
                    repeat = std::min(repeat, 2);
                    switch (repeat) {
                        case 1: result.append(detail::to_string_width(hour)); break;
                        case 2: result.append(detail::to_string_width(hour, 2)); break;
                        default: break;
                    }
                    break;
                case 'm':
                    repeat = std::min(repeat, 2);
                    switch (repeat) {
                        case 1: result.append(detail::to_string_width(minute)); break;
                        case 2: result.append(detail::to_string_width(minute, 2)); break;
                        default: break;
                    }
                    break;
                case 's':
                    repeat = std::min(repeat, 2);
                    switch (repeat) {
                        case 1: result.append(detail::to_string_width(second)); break;
                        case 2: result.append(detail::to_string_width(second, 2)); break;
                        default: break;
                    }
                    break;
                case 'z':
                    {
                    repeat = (repeat >= 3) ? 3 : 1;
                    // note: the millisecond component is treated like the
                    // decimal part of the seconds so ms == 2 is always
                    // printed as "002", but ms == 200 can be either "2" or
                    // "200"
#if __cplusplus >= 202002L
                    const int ms =
                        std::chrono::duration_cast<std::chrono::milliseconds>(hms.subseconds()).count();
#else
                    const int ms = dt.millisecond();
#endif
                    result.append(detail::to_string_width(ms, 3));
                    if (repeat == 1) {
                        if (result.back() == '0')
                            result.pop_back();
                        if (result.back() == '0')
                            result.pop_back();
                    }
                    break;
                    }
                default: result.append(std::string(repeat, c)); break;
            }
            i += repeat;
        }

        return result;
    }

    static date_time now() { return date_time{std::chrono::system_clock::now()}; }

    template <typename T = std::chrono::milliseconds>
    static date_time from_since_epoch(std::int64_t msecs) {
        return date_time{std::chrono::system_clock::time_point{T{msecs}}};
    }

private:
    std::chrono::system_clock::time_point timestamp_;
};
