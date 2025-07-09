#pragma once

template <typename Fun>
class ScopeGuard {
public:
    explicit ScopeGuard(Fun&& f) : func_{std::move(f)}, dismiss_{} {}
    explicit ScopeGuard(const Fun& f) : func_{f}, dismiss_{} {}

    ~ScopeGuard() {
        if (!dismiss_)
            func_();
    }

    ScopeGuard(ScopeGuard&& rhs) : func_{std::move(rhs.func_)}, dismiss_{rhs.dismiss_} {
        rhs.dismiss();
    }

    void dismiss() {
        dismiss_ = true;
    }

private:
    Fun func_;
    bool dismiss_;

    ScopeGuard() = delete;
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
};

template <typename Fun>
ScopeGuard<typename std::decay<Fun>::type> MakeGuard(Fun&& f) {
    return ScopeGuard<typename std::decay<Fun>::type>(std::forward<Fun>(f));
}
