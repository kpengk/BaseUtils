//
// Created by Glasssix-KP on 2025-03-12.
//

#pragma once

#include <list>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <unordered_map>


/**
 * @brief  无锁
 */
class NullLock {
public:
    inline void lock() {}
    inline void unlock() {}
};


/**
 * @brief  互斥锁
 */
class MutexLock {
public:
    inline void lock() {
        mutex_.lock();
    }
    inline void unlock() {
        mutex_.unlock();
    }

private:
    std::mutex mutex_;
};


/**
 * @brief  LRU 缓存
 * @tparam Key 键类型
 * @tparam Value 值类型
 */
template <typename Key, typename Value, typename LockType = NullLock>
class LRUCache {
public:
    /**
     * @brief  构造函数
     * @param  capacity 缓存容量
     */
    explicit LRUCache(size_t capacity = 4) : capacity_{capacity} {
        if (capacity <= 0) {
            throw std::logic_error("Capacity must be positive");
        }
    }

    /**
     * @brief  获取缓存值（若存在则移至最近使用位置）
     * @param  key 键
     * @return 缓存值
     */
    [[nodiscard]] std::optional<Value> get(const Key& key) {
        std::lock_guard<LockType> lock(lock_);
        const auto iter = cache_map_.find(key);
        if (iter == cache_map_.end()) {
            return std::nullopt;
        }
        // 移动节点到链表头部
        cache_list_.splice(cache_list_.begin(), cache_list_, iter->second);
        return iter->second->second;
    }

    /**
     * @brief  插入或更新键值对
     * @param  key 键
     * @param  value 值
     */
    void put(const Key& key, const Value& value) {
        std::lock_guard<LockType> lock(lock_);
        const auto iter = cache_map_.find(key);
        if (iter != cache_map_.end()) {
            // 更新值并移至头部
            iter->second->second = value;
            cache_list_.splice(cache_list_.begin(), cache_list_, iter->second);
            return;
        }
        // 容量已满，淘汰尾部节点
        if (cache_list_.size() >= capacity_) {
            const auto last_node = std::prev(cache_list_.end());
            cache_map_.erase(last_node->first);
            cache_list_.pop_back();
        }
        // 插入新节点到头部
        cache_list_.emplace_front(key, value);
        cache_map_[key] = cache_list_.begin();
    }

    /**
     * @brief  删除缓存中带特定键的元素
     * @param  key 要删除的元素键值
     */
    void erase(const Key& key) {
        std::lock_guard<LockType> lock(lock_);
        const auto iter = cache_map_.find(key);
        if (iter != cache_map_.end()) {
            cache_list_.erase(iter->second);
            cache_map_.erase(iter);
        }
    }

    /**
     * @brief  检查缓存是否含有带特定键的元素
     * @param  key 要搜索的元素键值
     * @return 若有这种元素则为 true，否则为 false。
     */
    bool exists(const Key& key) const {
        std::lock_guard<LockType> lock(lock_);
        return cache_map_.find(key) != cache_map_.end();
    }

    /**
     * @brief  返回缓存中元素数
     * @return 缓存中的元素数量
     */
    std::size_t size() const {
        std::lock_guard<LockType> lock(lock_);
        return cache_map_.size();
    }

    /**
     * @brief  返回缓存最大容量
     * @return 缓存的最大容量
     */
    std::size_t capacity() const {
        return capacity_;
    }

private:
    using ListType = std::list<std::pair<Key, Value>>;

    const std::size_t capacity_;
    mutable LockType lock_;
    ListType cache_list_;
    std::unordered_map<Key, typename ListType::iterator> cache_map_;
};
