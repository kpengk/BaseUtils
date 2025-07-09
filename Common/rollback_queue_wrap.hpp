#pragma once
#include <optional>

template<typename Q>
class rollback_queue_wrap {
public:
    using value_type = Q::value_type;

    rollback_queue_wrap(Q& queue) : queue_{queue} {}

    void clear() {
        queue_.clear();
        rollback_data_.reset();
    }
    
    Q::value_type get_profile() {
        if (rollback_data_) {
            Q::value_type tmp_data = std::move(rollback_data_.value());
            rollback_data_.reset();
            return tmp_data;
        }

        Q::value_type res;
        if (!queue_.dequeue_for(res, std::chrono::seconds(1))) {
        }
        return res;
    }

    void rollback(Q::value_type&& data) {
        rollback_data_ = data;
    }

private:
    Q& queue_;
    std::optional<Q::value_type> rollback_data_;
};
