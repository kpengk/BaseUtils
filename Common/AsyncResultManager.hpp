#pragma once
#include <cassert>
#include <future>
#include <memory>
#include <mutex>
#include <unordered_map>


// 用于管理异步返回结果，统一处理多个异步调用的返回结果。
template <typename ResultType = void, typename IDType = std::size_t>
class AsyncResultManager {
public:
    AsyncResultManager() = default;
    ~AsyncResultManager() = default;

    std::future<ResultType> async_task(IDType id) {
        auto p = std::make_shared<std::promise<ResultType>>();
        std::future<ResultType> future = p->get_future();
        {
            std::lock_guard<std::mutex> lock(mtx_);
            futures_.emplace(id, std::move(p));
        }
        return future;
    }

    template <typename Fun>
    std::future<ResultType> async_task(IDType id, const Fun& fn) {
        auto p = std::make_shared<std::promise<ResultType>>();
        std::future<ResultType> future = p->get_future();
        {
            std::lock_guard<std::mutex> lock(mtx_);
            futures_.emplace(id, std::move(p));
        }
        fn();
        return future;
    }

    // void 特化版本的 callback
    template <typename T = ResultType, typename std::enable_if_t<std::is_same_v<T, void>>* = nullptr>
    void call_back(IDType id) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = futures_.find(id);
        if (it == futures_.end()) {
            printf("Error: ID not found.\n");
            return;
        }
        it->second->set_value();
        futures_.erase(it);
        futures_.erase(id);
    }

    // 非 void 版本的 callback
    template <typename T = ResultType, typename std::enable_if_t<!std::is_same_v<T, void>>* = nullptr>
    void call_back(IDType id, T&& data) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = futures_.find(id);
        if (it == futures_.end()) {
            printf("Error: ID not found.\n");
            return;
        }
        it->second->set_value(std::forward<T>(data));
        futures_.erase(it);
    }

private:
    std::mutex mtx_;
    std::unordered_map<IDType, std::shared_ptr<std::promise<ResultType>>> futures_;
};