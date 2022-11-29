#pragma once
#include <cassert>
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

namespace {
/*!
 Counts the number of identical leading characters in s.

 If s is empty, returns 0. Otherwise, returns the number of
 consecutive {s.front()} characters at the start of s.

 repeat_count("a");   // == 1
 repeat_count("ab");  // == 1
 repeat_count("aab"); // == 2
*/
int repeat_count(std::string_view s) {
    if (s.empty())
        return 0;
    const char c{s[0]};
    std::size_t j{1};
    for (; j < s.size() && s[j] == c; ++j) {}
    return static_cast<int>(j);
}

std::string long_long_to_string(long long val, int width = -1) {
    char buf[24]{};
    switch (width) {
        case 1: sprintf(buf, "%01lld", val); return buf;
        case 2: sprintf(buf, "%02lld", val); return buf;
        case 3: sprintf(buf, "%03lld", val); return buf;
        case 4: sprintf(buf, "%04lld", val); return buf;
        case 5: sprintf(buf, "%05lld", val); return buf;
        case 6: sprintf(buf, "%06lld", val); return buf;
        case 7: sprintf(buf, "%07lld", val); return buf;
        case 8: sprintf(buf, "%08lld", val); return buf;
        case 9: sprintf(buf, "%09lld", val); return buf;
        default: break;
    }

    if (width <= 0) {
        return std::to_string(val);
    } else {
        const auto val_str = std::to_string(val >= 0 ? val : -val);
        if (val >= 0) {
            if (val_str.size() >= width)
                return val_str;
            else
                return std::string(width - val_str.size(), '0') + val_str;
        } else {
            if (val_str.size() + 1 >= width)
                return "-" + val_str;
            else
                return "-" + std::string(width - 1 - val_str.size(), '0') + val_str;
        }
    }
}

bool time_format_contains_ap(std::string_view format) {
    return std::find_if(format.cbegin(), format.cend(), [](char c) { return c == 'a' || c == 'A'; }) != format.cend();
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
|     H      | The hour without a leading zero (0 to 23, even with AM/PM display)    |
|     HH     | The hour with a leading zero (00 to 23, even with AM/PM display)      |
|     h      | The hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display) |
|     hh     | The hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)  |
|     m      | The minute without a leading zero (0 to 59)                           |
|     mm     | The minute with a leading zero (00 to 59)                             |
|     s      | The whole second, without any leading zero (0 to 59)                  |
|     ss     | The whole second, with a leading zero where applicable (00 to 59)     |
|     z      | The fractional part of the second, to go after a decimal point, without trailing zeroes (0 to 999). |
|    zzz     | The fractional part of the second, to millisecond precision, including trailing zeroes where applicable (000 to 999). |
|  AP or A   | Use AM/PM display. `A/AP` will be replaced by 'AM' or 'PM'.           |
|  ap or a   | Use am/pm display. `a/ap` will be replaced by 'am' or 'pm'.           |
|  aP or Ap  | Use AM/PM display. `aP/Ap` will be replaced by 'AM' or 'PM'.          |
*/

class date_time {
public:
    date_time()
        : timestamp_{} {}

    date_time(std::chrono::system_clock::time_point timestamp, time_spec spec = time_spec::utc)
        : timestamp_{timestamp} {
        if (spec == time_spec::local) {
            timestamp_ -= std::chrono::hours{8};
        }
    }

    template <typename T>
    date_time(std::chrono::year_month_day date, std::chrono::hh_mm_ss<T> time,
              time_spec spec = time_spec::utc)
        : timestamp_{static_cast<std::chrono::sys_days>(date) + time.to_duration()} {
        if (spec == time_spec::local) {
            timestamp_ -= std::chrono::hours{8};
        }
    }

    date_time(const date_time& other) = default;
    date_time(date_time&& other) = default;
    date_time& operator=(const date_time& other) = default;
    ~date_time() = default;

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
        const auto ymd{dt.date()};
        const auto hms{dt.time()};

        const auto year{static_cast<int>(ymd.year())};
        const auto month{static_cast<unsigned int>(ymd.month())};
        const auto day{static_cast<unsigned int>(ymd.day())};

        int i{};
        while (i < format.size()) {
            const char c{format[i]};
            int repeat{repeat_count(format.substr(i))};

            switch (c) {
                case 'y':
                    if (repeat >= 4) {
                        repeat = 4;
                        const int len = (year < 0) ? 5 : 4;
                        result.append(long_long_to_string(year, len));
                    } else if (repeat >= 2) {
                        repeat = 2;
                        result.append(long_long_to_string(year % 100, 2));
                    } else {
                        repeat = 1;
                        result.push_back(c);
                    }
                    break;
                case 'M':
                    repeat = std::min(repeat, 2);
                    switch (repeat) {
                        case 1: result.append(long_long_to_string(month)); break;
                        case 2: result.append(long_long_to_string(month, 2)); break;
                    }
                    break;
                case 'd':
                    repeat = std::min(repeat, 2);
                    switch (repeat) {
                        case 1: result.append(long_long_to_string(day)); break;
                        case 2: result.append(long_long_to_string(day, 2)); break;
                    }
                    break;
                case 'h': {
                    repeat = std::min(repeat, 2);
                    int hour = hms.hours().count();
                    if (time_format_contains_ap(format)) {
                        if (hour > 12)
                            hour -= 12;
                        else if (hour == 0)
                            hour = 12;
                    }
                    switch (repeat) {
                        case 1: result.append(long_long_to_string(hour)); break;
                        case 2: result.append(long_long_to_string(hour, 2)); break;
                    }
                    break;
                }
                case 'H': {
                    repeat = std::min(repeat, 2);
                    const int hour = hms.hours().count();
                    switch (repeat) {
                        case 1: result.append(long_long_to_string(hour)); break;
                        case 2: result.append(long_long_to_string(hour, 2)); break;
                    }
                    break;
                }
                case 'm':
                    repeat = std::min(repeat, 2);
                    switch (repeat) {
                        case 1: result.append(long_long_to_string(hms.minutes().count())); break;
                        case 2: result.append(long_long_to_string(hms.minutes().count(), 2)); break;
                    }
                    break;
                case 's':
                    repeat = std::min(repeat, 2);
                    switch (repeat) {
                        case 1: result.append(long_long_to_string(hms.seconds().count())); break;
                        case 2: result.append(long_long_to_string(hms.seconds().count(), 2)); break;
                    }
                    break;
                case 'a':
                    repeat = format.substr(i + 1).starts_with('p') ? 2 : 1;
                    result.append(hms.hours().count() < 12 ? "am" : "pm");
                    break;
                case 'A':
                    repeat = format.substr(i + 1).starts_with('P') ? 2 : 1;
                    result.append(hms.hours().count() < 12 ? "AM" : "PM");
                    break;
                case 'z':
                    repeat = (repeat >= 3) ? 3 : 1;
                    // note: the millisecond component is treated like the
                    // decimal part of the seconds so ms == 2 is always
                    // printed as "002", but ms == 200 can be either "2" or
                    // "200"
                    const std::chrono::milliseconds ms =
                        std::chrono::duration_cast<std::chrono::milliseconds>(hms.subseconds());
                    result.append(long_long_to_string(ms.count(), 3));
                    if (repeat == 1) {
                        if (result.ends_with('0'))
                            result.pop_back();
                        if (result.ends_with('0'))
                            result.pop_back();
                    }
                    break;

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
