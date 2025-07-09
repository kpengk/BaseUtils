//
// Created by Glasssix-KP on 2024/6/26.
//

#ifndef THREAD_TRACE_HPP
#define THREAD_TRACE_HPP
#include <thread>
#include <mutex>
#include "circular_queue.hpp"
#include "date_time.hpp"

namespace thread_id_helper {
    namespace detail {
        template<typename T, auto... field>
        struct ThiefMember {
            friend auto steal_impl(const T &t) {
                return std::make_tuple(field...);
            }
        };

        auto steal_impl(const std::thread::id &);

        template struct ThiefMember<std::thread::id, &std::thread::id::_Id>;

        static const auto member_tp = detail::steal_impl(std::thread::id{});
    }

    inline uint32_t to_number(const std::thread::id &tid) {
        return tid.*(std::get<0>(detail::member_tp));
    }

    inline uint32_t tid() {
        return to_number(std::this_thread::get_id());
    }
}


class thread_trace {
public:
    explicit thread_trace(size_t count = 512) : elements_{count} {
    }

    void enter(uint16_t fun) {
        const uint32_t tid = thread_id_helper::tid();
        const uint64_t val = 1ull << 63 | static_cast<uint64_t>(fun) << 32 | tid;
        std::lock_guard lock(mutex_);
        elements_.push({val, date_time::now().to_since_epoch<std::chrono::milliseconds>()});
    }

    void leave(uint16_t fun) {
        const uint32_t tid = thread_id_helper::tid();
        const uint64_t val = static_cast<uint64_t>(fun) << 32 | tid;
        std::lock_guard lock(mutex_);
        elements_.push({val, date_time::now().to_since_epoch<std::chrono::milliseconds>()});
    }

    std::string dump() const {
        mutex_.lock();
        const size_t count = elements_.size();
        std::vector<std::pair<uint64_t, int64_t>> datas(count);
        for (size_t i = 0; i < count; ++i) {
            datas[i] = elements_.at(i);
        }
        mutex_.unlock();

        std::stringstream ss;
        ss << "time, function, enter, tid\n";
        for (const auto &[fst, snd]: datas) {
            const auto tp = unpack(fst);
            ss << date_time::from_since_epoch(snd).to_string("yyyy-MM-dd HH:mm:ss.zzz ");
            ss << std::left << std::setw(6) << std::get<1>(tp);
            ss << std::left << std::setw(2) << std::get<0>(tp);
            ss << std::left << std::get<2>(tp) << "\n";
        }
        return ss.str();
    }

private:
    /* 从低位到高位：
     *  0~31位：线程ID
     * 32~47位：函数
     *    63位：函数进入或离开标志
     */
    static std::tuple<bool, uint16_t, uint32_t> unpack(uint64_t val) {
        const uint32_t tid = val & 0xFFFFFFFF;
        val >>= 32;
        const uint16_t fun = val & 0xFFFF;
        val >>= 31;
        const bool flag = val;
        return {flag, fun, tid};
    }

    mutable std::mutex mutex_;
    circular_queue<std::pair<uint64_t, int64_t>> elements_;
};


class trace_func_guard {
public:
    trace_func_guard(thread_trace &trace, uint16_t func): trace_{trace}, func_{func} {
        trace_.enter(func_);
    }

    ~trace_func_guard() {
        trace_.leave(func_);
    }

private:
    thread_trace &trace_;
    const uint16_t func_;
};

#endif //THREAD_TRACE_HPP
