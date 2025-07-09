#pragma once

template<typename Q>
class rollback_queue{
public:
    using value_type = Q::value_type;

    rollback_queue(Q& queue) : queue_{queue} {}

protected:

private:
    Q& queue_;
};
