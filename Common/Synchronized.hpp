/**
 * This module implements a Synchronized abstraction useful in mutex-based concurrency.
 *
 * The Synchronized<T, Mutex> class is the primary public API exposed by this module.
 */

#pragma once

#include <array>
#include <mutex>
#include <shared_mutex>
#include <tuple>
#include <type_traits>
#include <utility>

// always inline
#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
#define ALWAYS_INLINE inline
#endif

namespace folly {

namespace detail {
    template <typename, typename Mutex>
    inline constexpr bool kSynchronizedMutexIsUnique = false;
    template <typename Mutex>
    inline constexpr bool kSynchronizedMutexIsUnique<decltype(void(std::declval<Mutex&>().lock())), Mutex> = true;

    template <typename, typename Mutex>
    inline constexpr bool kSynchronizedMutexIsShared = false;
    template <typename Mutex>
    inline constexpr bool kSynchronizedMutexIsShared<decltype(void(std::declval<Mutex&>().lock_shared())), Mutex> = true;


    /**
     * 用于描述互斥锁“级别”的枚举。支持的级别包括
     *  Unique - 仅支持独占锁定的普通互斥锁
     *  Shared - 具有共享锁定和解锁功能的共享互斥锁；
     */
    enum class SynchronizedMutexLevel { Unique, Shared };

    template <typename Mutex>
    inline constexpr SynchronizedMutexLevel kSynchronizedMutexLevel =
        kSynchronizedMutexIsShared<void, Mutex> ? SynchronizedMutexLevel::Shared : SynchronizedMutexLevel::Unique;

    enum class SynchronizedMutexMethod { Lock, TryLock };

    template <SynchronizedMutexLevel Level, SynchronizedMutexMethod Method>
    struct SynchronizedLockPolicy {
        static constexpr SynchronizedMutexLevel level = Level;
        static constexpr SynchronizedMutexMethod method = Method;
    };

    using SynchronizedLockPolicyExclusive = SynchronizedLockPolicy<SynchronizedMutexLevel::Unique, SynchronizedMutexMethod::Lock>;
    using SynchronizedLockPolicyTryExclusive = SynchronizedLockPolicy<SynchronizedMutexLevel::Unique, SynchronizedMutexMethod::TryLock>;
    using SynchronizedLockPolicyShared = SynchronizedLockPolicy<SynchronizedMutexLevel::Shared, SynchronizedMutexMethod::Lock>;
    using SynchronizedLockPolicyTryShared = SynchronizedLockPolicy<SynchronizedMutexLevel::Shared, SynchronizedMutexMethod::TryLock>;

    template <SynchronizedMutexLevel>
    struct SynchronizedLockType_ {};

    template <>
    struct SynchronizedLockType_<SynchronizedMutexLevel::Unique> {
        template <typename Mutex>
        using apply = std::unique_lock<Mutex>;
    };

    template <>
    struct SynchronizedLockType_<SynchronizedMutexLevel::Shared> {
        template <typename Mutex>
        using apply = std::shared_lock<Mutex>;
    };

    template <SynchronizedMutexLevel Level, typename MutexType>
    using SynchronizedLockType = typename SynchronizedLockType_<Level>::template apply<MutexType>;
} // namespace detail

/**
 * SynchronizedBase 是 Synchronized<T> 的辅助父类。
 *
 * 它为共享互斥类型提供 wlock() 和 rlock() 方法，或为纯独占互斥类型提供 lock() 方法。
 */
template <typename Subclass, detail::SynchronizedMutexLevel level>
class SynchronizedBase;

template <typename LockedType, typename Mutex, typename LockPolicy>
class LockedPtrBase;
template <typename LockedType, typename LockPolicy>
class LockedPtr;

/**
 * SynchronizedBase 专门用于共享互斥类型。
 *
 * 此类提供 wlock() 和 rlock() 方法来获取锁和访问数据。
 */
template <typename Subclass>
class SynchronizedBase<Subclass, detail::SynchronizedMutexLevel::Shared> {
private:
    template <typename T, typename P>
    using LockedPtr_ = ::folly::LockedPtr<T, P>;

public:
    using LockPolicyExclusive = detail::SynchronizedLockPolicyExclusive;
    using LockPolicyShared = detail::SynchronizedLockPolicyShared;
    using LockPolicyTryExclusive = detail::SynchronizedLockPolicyTryExclusive;
    using LockPolicyTryShared = detail::SynchronizedLockPolicyTryShared;

    using WLockedPtr = LockedPtr_<Subclass, LockPolicyExclusive>;
    using ConstWLockedPtr = LockedPtr_<const Subclass, LockPolicyExclusive>;

    using RLockedPtr = LockedPtr_<Subclass, LockPolicyShared>;
    using ConstRLockedPtr = LockedPtr_<const Subclass, LockPolicyShared>;

    using TryWLockedPtr = LockedPtr_<Subclass, LockPolicyTryExclusive>;
    using ConstTryWLockedPtr = LockedPtr_<const Subclass, LockPolicyTryExclusive>;

    using TryRLockedPtr = LockedPtr_<Subclass, LockPolicyTryShared>;
    using ConstTryRLockedPtr = LockedPtr_<const Subclass, LockPolicyTryShared>;

    using LockedPtr = WLockedPtr;
    using ConstLockedPtr = ConstRLockedPtr;

    /**
     * @brief 获取独占锁。
     *
     * 获取独占锁，并返回可用于安全访问数据的 LockedPtr。
     *
     * LockedPtr 提供操作符 -> 和 * 来访问数据。当LockedPtr被销毁时，锁将被释放。
     *
     * @methodset Exclusive lock
     */
    LockedPtr wlock() {
        return LockedPtr(static_cast<Subclass*>(this));
    }

    ConstWLockedPtr wlock() const {
        return ConstWLockedPtr(static_cast<const Subclass*>(this));
    }

    /**
     * @brief 获取独占锁，或者为空。
     *
     * 尝试以独占模式获取锁。如果获取不成功，则返回的 LockedPtr 将为 null。
     *
     * （使用 LockedPtr::operator bool() 或 LockedPtr::isNull() 来检查有效性。）
     *
     * @methodset Exclusive lock
     */
    TryWLockedPtr try_wlock() {
        return TryWLockedPtr{static_cast<Subclass*>(this)};
    }

    ConstTryWLockedPtr try_wlock() const {
        return ConstTryWLockedPtr{static_cast<const Subclass*>(this)};
    }

    /**
     * @brief 获取读锁。
     *
     * 返回的 LockedPtr 将强制对数据进行 const 访问，除非在非常量上下文中获取锁并使用 asNonConstUnsafe()。
     *
     * @methodset Shared lock
     */
    RLockedPtr rlock() {
        return RLockedPtr(static_cast<Subclass*>(this));
    }

    ConstLockedPtr rlock() const {
        return ConstLockedPtr(static_cast<const Subclass*>(this));
    }

    /**
     * @brief 获取读锁，或者为空。
     *
     * 尝试以共享模式获取锁。如果获取不成功，则返回的 LockedPtr 将为 null。
     *
     * （使用 LockedPtr::operator bool() 或 LockedPtr::isNull() 来检查有效性。）
     *
     * @methodset Shared lock
     */
    TryRLockedPtr try_rlock() {
        return TryRLockedPtr{static_cast<Subclass*>(this)};
    }

    ConstTryRLockedPtr try_rlock() const {
        return ConstTryRLockedPtr{static_cast<const Subclass*>(this)};
    }

    /**
     * 尝试获取锁，如果先超时则失败。如果获取不成功，则返回的 LockedPtr 将为 null。
     *
     * （使用 LockedPtr::operator bool() 或 LockedPtr::isNull() 来检查有效性。）
     *
     * @methodset Exclusive lock
     */
    template <typename Rep, typename Period>
    LockedPtr wlock(const std::chrono::duration<Rep, Period>& timeout) {
        return LockedPtr(static_cast<Subclass*>(this), timeout);
    }

    template <typename Rep, typename Period>
    LockedPtr wlock(const std::chrono::duration<Rep, Period>& timeout) const {
        return LockedPtr(static_cast<const Subclass*>(this), timeout);
    }

    /**
     * 尝试获取锁，如果先超时则失败。如果获取不成功，则返回的 LockedPtr 将为 null。
     *
     * （使用 LockedPtr::operator bool() 或 LockedPtr::isNull() 来检查有效性。）
     *
     * @methodset Shared lock
     */
    template <typename Rep, typename Period>
    RLockedPtr rlock(const std::chrono::duration<Rep, Period>& timeout) {
        return RLockedPtr(static_cast<Subclass*>(this), timeout);
    }

    template <typename Rep, typename Period>
    ConstRLockedPtr rlock(const std::chrono::duration<Rep, Period>& timeout) const {
        return ConstRLockedPtr(static_cast<const Subclass*>(this), timeout);
    }

    /**
     * 在独占锁定的情况下调用一个函数。
     *
     * 对数据的引用将作为其唯一参数传递到函数中。
     *
     * 这可以与 lambda 参数一起使用，以便轻松在代码中定义小的临界区。例如：
     *
     *   auto value = obj.withWLock([](auto& data) {
     *     data.doStuff();
     *     return data.getValue();
     *   });
     *
     * @methodset Exclusive lock
     */
    template <typename Function>
    auto with_wlock(Function&& function) {
        return function(*wlock());
    }

    template <typename Function>
    auto with_wlock(Function&& function) const {
        return function(*wlock());
    }

    /**
     * 在独占锁定的情况下调用一个函数。
     *
     * 这与 withWLock() 类似，但该函数将传递 LockedPtr 而不是对数据本身的引用。
     *
     * 如果需要的话，这允许在 LockedPtr 参数上调用 scopedUnlock()。
     *
     * @methodset Exclusive lock
     */
    template <typename Function>
    auto with_wlock_ptr(Function&& function) {
        return function(wlock());
    }

    template <typename Function>
    auto with_wlock_ptr(Function&& function) const {
        return function(wlock());
    }

    /**
     * 在共享模式下持有锁并调用函数。
     *
     * 对数据的 const 引用将作为其唯一参数传递到函数中。
     *
     * @methodset Shared lock
     */
    template <typename Function>
    auto with_rlock(Function&& function) const {
        return function(*rlock());
    }

    /**
     * 在共享模式下持有锁并调用函数。
     *
     * 这与 withRLock() 类似，但该函数将传递 LockedPtr 而不是对数据本身的引用。
     *
     * 如果需要的话，这允许在 LockedPtr 参数上调用 scopedUnlock()。
     *
     * @methodset Shared lock
     */
    template <typename Function>
    auto with_rlock_ptr(Function&& function) {
        return function(rlock());
    }

    template <typename Function>
    auto with_rlock_ptr(Function&& function) const {
        return function(rlock());
    }
};

/**
 * SynchronizedBase 专门用于非共享互斥类型。
 *
 * 此类提供lock（）方法来获取锁并访问数据。
 */
template <typename Subclass>
class SynchronizedBase<Subclass, detail::SynchronizedMutexLevel::Unique> {
private:
    template <typename T, typename P>
    using LockedPtr_ = ::folly::LockedPtr<T, P>;

public:
    using LockPolicyExclusive = detail::SynchronizedLockPolicyExclusive;
    using LockPolicyTryExclusive = detail::SynchronizedLockPolicyTryExclusive;

    using LockedPtr = LockedPtr_<Subclass, LockPolicyExclusive>;
    using ConstLockedPtr = LockedPtr_<const Subclass, LockPolicyExclusive>;

    using TryLockedPtr = LockedPtr_<Subclass, LockPolicyTryExclusive>;
    using ConstTryLockedPtr = LockedPtr_<const Subclass, LockPolicyTryExclusive>;

    /**
     * @brief 获取锁。
     *
     * 返回一个可以用来安全访问数据的 LockedPtr。
     *
     * @methodset Non-shareable lock
     */
    LockedPtr lock() {
        return LockedPtr(static_cast<Subclass*>(this));
    }

    /**
     * 获取锁，并返回一个可用于安全访问数据的 ConstLockedPtr。
     *
     * @methodset Non-shareable lock
     */
    ConstLockedPtr lock() const {
        return ConstLockedPtr(static_cast<const Subclass*>(this));
    }

    /**
     * @brief 获取锁，或者为空。
     *
     * 尝试以独占模式获取锁。如果获取不成功，则返回的 LockedPtr 将为 null。
     *
     * （使用 LockedPtr::operator bool() 或 LockedPtr::isNull() 来检查有效性。）
     *
     * @methodset Non-shareable lock
     */
    TryLockedPtr try_lock() {
        return TryLockedPtr{static_cast<Subclass*>(this)};
    }

    ConstTryLockedPtr try_lock() const {
        return ConstTryLockedPtr{static_cast<const Subclass*>(this)};
    }

    /**
     * 尝试获取锁，如果先超时则失败。如果获取不成功，则返回的 LockedPtr 将为 null。
     *
     * @methodset Non-shareable lock
     */
    template <typename Rep, typename Period>
    LockedPtr lock(const std::chrono::duration<Rep, Period>& timeout) {
        return LockedPtr(static_cast<Subclass*>(this), timeout);
    }

    /**
     * 尝试获取锁，如果先超时则失败。如果获取不成功，则返回的 LockedPtr 将为 null。
     *
     * @methodset Non-shareable lock
     */
    template <typename Rep, typename Period>
    ConstLockedPtr lock(const std::chrono::duration<Rep, Period>& timeout) const {
        return ConstLockedPtr(static_cast<const Subclass*>(this), timeout);
    }

    /**
     * 在持有锁的情况下调用一个函数。
     *
     * 对数据的引用将作为其唯一参数传递到函数中。
     *
     * 这可以与 lambda 参数一起使用，以便轻松在代码中定义小的临界区。例如：
     *
     *   auto value = obj.withLock([](auto& data) {
     *     data.doStuff();
     *     return data.getValue();
     *   });
     *
     * @methodset Non-shareable lock
     */
    template <typename Function>
    auto with_lock(Function&& function) {
        return function(*lock());
    }

    template <typename Function>
    auto with_lock(Function&& function) const {
        return function(*lock());
    }

    /**
     * 在独占锁定的情况下调用一个函数。
     *
     * 这与 withWLock() 类似，但该函数将传递 LockedPtr 而不是对数据本身的引用。
     *
     * 这允许在 LockedPtr 参数上调用 scopedUnlock() 和 as_lock()。
     *
     * @methodset Non-shareable lock
     */
    template <typename Function>
    auto with_lock_ptr(Function&& function) {
        return function(lock());
    }

    template <typename Function>
    auto with_lock_ptr(Function&& function) const {
        return function(lock());
    }
};

/**
 * `Synchronized` 将数据与互斥锁配对。数据只能通过 `LockedPtr` 访问，通常通过 `.rlock()` 或 `.wlock()` 获取；
 * 互斥锁在 `LockedPtr` 的整个生命周期内保持。
 *
 * 建议在获取“LockedPtr”对象时明确打开一个新的嵌套范围，以帮助明显地划分关键部分并确保“LockedPtr”在不再需要时立即被销毁。
 *
 * @tparam T  要存储的数据的类型。
 * @tparam Mutex  保护数据的互斥类型。必须是可锁定的。
 */
template <typename T, typename Mutex = std::shared_mutex>
struct Synchronized : public SynchronizedBase<Synchronized<T, Mutex>, detail::kSynchronizedMutexLevel<Mutex>> {
private:
    using Base = SynchronizedBase<Synchronized<T, Mutex>, detail::kSynchronizedMutexLevel<Mutex>>;
    static constexpr bool nxCopyCtor{std::is_nothrow_copy_constructible_v<T>};
    static constexpr bool nxMoveCtor{std::is_nothrow_move_constructible_v<T>};

    // 用于禁用复制构造函数和赋值
    class NonImplementedType;

public:
    using LockedPtr = typename Base::LockedPtr;
    using ConstLockedPtr = typename Base::ConstLockedPtr;
    using DataType = T;
    using MutexType = Mutex;

    /**
     * 默认构造函数让两个成员都调用它们自己的默认构造函数。
     */
    constexpr Synchronized() = default;

public:
    /**
     * 构造函数以数据为参数复制该数据。无需锁定构造对象。
     */
    explicit Synchronized(const T& rhs) noexcept(nxCopyCtor) : data_(rhs) {}

    /**
     * 构造函数以数据右值作为参数并移动它。无需锁定构造对象。
     */
    explicit Synchronized(T&& rhs) noexcept(nxMoveCtor) : data_(std::move(rhs)) {}

    /**
     * 允许您就地构造不可移动类型。使用 constexpr 实例“in_place”作为第一个参数。
     */
    template <typename... Args>
    explicit constexpr Synchronized(std::in_place_t, Args&&... args) : data_(std::forward<Args>(args)...) {}

    /**
     * 允许您构造同步对象，并根据需要将构造参数传递给底层互斥锁
     */
    template <typename... DatumArgs, typename... MutexArgs>
    Synchronized(std::piecewise_construct_t, std::tuple<DatumArgs...> datumArgs, std::tuple<MutexArgs...> mutexArgs)
        : Synchronized{std::piecewise_construct, std::move(datumArgs), std::move(mutexArgs),
            std::make_index_sequence<sizeof...(DatumArgs)>{}, std::make_index_sequence<sizeof...(MutexArgs)>{}} {}

    /**
     * 锁定对象，分配数据。
     */
    Synchronized& operator=(const T& rhs) {
        if (&data_ != &rhs) {
            auto guard = LockedPtr{this};
            data_ = rhs;
        }
        return *this;
    }

    /**
     * 锁定对象，移动-分配数据。
     */
    Synchronized& operator=(T&& rhs) {
        if (&data_ != &rhs) {
            auto guard = LockedPtr{this};
            data_ = std::move(rhs);
        }
        return *this;
    }

    /**
     * @brief 获得一些锁。
     *
     * 如果互斥量是共享互斥量，并且 Synchronized 实例是 const，则这将获取共享锁。否则，这将获取独占锁。
     *
     * 一般来说，对于读写锁，倾向于使用显式的 rlock() 和 wlock() 方法，对于纯独占锁，倾向于使用 lock()。
     *
     * contextual_lock() 主要用于那些不一定知道锁类型的其他模板函数。
     */
    LockedPtr contextual_lock() {
        return LockedPtr(this);
    }
    ConstLockedPtr contextual_lock() const {
        return ConstLockedPtr(this);
    }

    template <typename Rep, typename Period>
    LockedPtr contextual_lock(const std::chrono::duration<Rep, Period>& timeout) {
        return LockedPtr(this, timeout);
    }

    template <typename Rep, typename Period>
    ConstLockedPtr contextual_lock(const std::chrono::duration<Rep, Period>& timeout) const {
        return ConstLockedPtr(this, timeout);
    }

    /**
     * @brief 获取读取锁。
     *
     * 如果互斥类型是共享的，则 contextualRLock() 获取读取锁；如果互斥类型是非共享的，则获取常规排他锁。
     *
     * contextualRLock() 当您知道您更喜欢读锁（如果可用）时，即使 Synchronized<T> 对象本身是非 const 的。
     */
    ConstLockedPtr contextual_rlock() const {
        return ConstLockedPtr(this);
    }

    template <typename Rep, typename Period>
    ConstLockedPtr contextual_rlock(const std::chrono::duration<Rep, Period>& timeout) const {
        return ConstLockedPtr(this, timeout);
    }

    /**
     * @brief 交换数据。
     *
     * 与另一个 Synchronized 交换。防止自我交换。仅交换数据。按地址递增顺序获取锁。
     */
    void swap(Synchronized& rhs) noexcept {
        if (this == &rhs) {
            return;
        }
        if (this > &rhs) {
            return rhs.swap(*this);
        }
        auto guard1 = LockedPtr{this};
        auto guard2 = LockedPtr{&rhs};

        using std::swap;
        swap(data_, rhs.data_);
    }

    /**
     * 与另一个数据交换。建议这样做，因为它只会短暂保持互斥。
     */
    void swap(T& rhs) {
        LockedPtr guard(this);

        using std::swap;
        swap(data_, rhs);
    }

    /**
     * @brief 交换数据。
     *
     * 分配另一个数据并返回原始值。建议这样做，因为它只会短暂保持互斥。
     */
    T exchange(T&& rhs) {
        swap(rhs);
        return std::move(rhs);
    }

    /**
     * 将数据复制到给定目标。
     */
    void copy_into(T& target) const {
        ConstLockedPtr guard(this);
        target = data_;
    }

    /**
     * 返回数据的最新副本。
     */
    T copy() const {
        ConstLockedPtr guard(this);
        return data_;
    }

    /**
     * @brief 无需锁定即可访问数据。
     *
     * 返回对数据的引用而不获取锁。
     *
     * 为已知可以安全使用的调用站点提供后门。例如，当已知只有一个线程可以访问 Synchronized 实例时。
     *
     * 要小心使用 - 此方法明确覆盖了其余 Synchronized API 提供的正常安全保证。
     */
    T& unsafe_get_unlocked() {
        return data_;
    }
    const T& unsafe_get_unlocked() const {
        return data_;
    }

    /**
     * @brief 直接访问底层 mutex_。
     *
     * 为在不同调用中锁定和解锁配对的调用站点提供后门。例如，在 fork 处理程序中。
     * 请谨慎使用，因为调用者有责任确保它与解锁配对，并且两者之间没有任何其他尝试再次隐式或显式获取锁定的内容。
     */
    Mutex& unsafe_get_mutex() {
        return mutex_;
    }

private:
    template <typename LockedType, typename MutexType, typename LockPolicy>
    friend class ::folly::LockedPtrBase;
    template <typename LockedType, typename LockPolicy>
    friend class ::folly::LockedPtr;

    /**
     * 辅助构造函数为非默认可构造类型 T 启用 Synchronized。
     * 守卫在实际的公共构造函数中创建，并在构造对象所需的时间内保持活动状态
     */
    Synchronized(const Synchronized& rhs, const ConstLockedPtr& /*guard*/) noexcept(nxCopyCtor) : data_(rhs.data_) {}

    Synchronized(Synchronized&& rhs, const LockedPtr& /*guard*/) noexcept(nxMoveCtor) : data_(std::move(rhs.data_)) {}

    template <typename... DatumArgs, typename... MutexArgs, std::size_t... IndicesOne, std::size_t... IndicesTwo>
    Synchronized(std::piecewise_construct_t, std::tuple<DatumArgs...> datumArgs, std::tuple<MutexArgs...> mutexArgs,
        std::index_sequence<IndicesOne...>, std::index_sequence<IndicesTwo...>)
        : data_{std::get<IndicesOne>(std::move(datumArgs))...}, mutex_{std::get<IndicesTwo>(std::move(mutexArgs))...} {
    }

    // 数据成员的模拟 - 保持数据成员同步！
    // LockedPtr 需要 offsetof()，它只为标准布局类型指定，而 Synchronized 没有，因此我们为 offsetof 定义了一个模拟
    struct Simulacrum {
        std::aligned_storage_t<sizeof(DataType), alignof(DataType)> data_;
        std::aligned_storage_t<sizeof(MutexType), alignof(MutexType)> mutex_;
    };

    // 数据成员 - 保持数据成员的模拟同步！
    T data_;
    mutable Mutex mutex_;
};

template <typename SynchronizedType, typename LockPolicy>
class ScopedUnlocker;

namespace detail {
    /*
     * 如果模板参数是 const Synchronized<T>，则解析为“const T”的辅助别名；如果参数不是 const，则解析为“T”。
     */
    template <typename SynchronizedType, bool AllowsConcurrentAccess>
    using SynchronizedDataType = std::conditional_t<AllowsConcurrentAccess || std::is_const_v<SynchronizedType>,
            typename SynchronizedType::DataType const, typename SynchronizedType::DataType>;
    /*
     * 如果模板参数是 const Synchronized<T>，则辅助别名解析为 ConstLockedPtr；如果参数不是 const，则辅助别名解析为 LockedPtr。
     */
    template <typename SynchronizedType>
    using LockedPtrType = std::conditional_t<std::is_const_v<SynchronizedType>,
        typename SynchronizedType::ConstLockedPtr, typename SynchronizedType::LockedPtr>;

    template <typename Synchronized, typename LockFunc, typename TryLockFunc, typename... Args>
    class SynchronizedLocker {
    public:
        using LockedPtr = std::invoke_result_t<LockFunc&, Synchronized&, const Args&...>;

        template <typename LockFuncType, typename TryLockFuncType, typename... As>
        SynchronizedLocker(Synchronized& sync, LockFuncType&& lock_func, TryLockFuncType try_lock_func, As&&... as)
            : synchronized{sync}, lock_func_{std::forward<LockFuncType>(lock_func)},
              try_lock_func_{std::forward<TryLockFuncType>(try_lock_func)}, args_{std::forward<As>(as)...} {}

        auto lock() const {
            auto args = std::tuple<const Args&...>{args_};
            return std::apply(lock_func_, std::tuple_cat(std::tie(synchronized), args));
        }

        auto try_lock() const {
            return try_lock_func_(synchronized);
        }

    private:
        Synchronized& synchronized;
        LockFunc lock_func_;
        TryLockFunc try_lock_func_;
        std::tuple<Args...> args_;
    };

    template <typename Synchronized, typename LockFunc, typename TryLockFunc, typename... Args>
    auto makeSynchronizedLocker(
        Synchronized& synchronized, LockFunc&& lockFunc, TryLockFunc&& tryLockFunc, Args&&... args) {
        using LockFuncType = std::decay_t<LockFunc>;
        using TryLockFuncType = std::decay_t<TryLockFunc>;
        return SynchronizedLocker<Synchronized, LockFuncType, TryLockFuncType, std::decay_t<Args>...>{synchronized,
            std::forward<LockFunc>(lockFunc), std::forward<TryLockFunc>(tryLockFunc), std::forward<Args>(args)...};
    }

    template <typename Synchronized, typename... Args>
    auto wlock(Synchronized& synchronized, Args&&... args) {
        return detail::makeSynchronizedLocker(
            synchronized, [](auto& s, auto&&... a) { return s.wlock(std::forward<decltype(a)>(a)...); },
            [](auto& s) { return s.tryWLock(); }, std::forward<Args>(args)...);
    }

    template <typename Synchronized, typename... Args>
    auto rlock(Synchronized& synchronized, Args&&... args) {
        return detail::makeSynchronizedLocker(
            synchronized, [](auto& s, auto&&... a) { return s.rlock(std::forward<decltype(a)>(a)...); },
            [](auto& s) { return s.tryRLock(); }, std::forward<Args>(args)...);
    }

    template <typename Synchronized, typename... Args>
    auto ulock(Synchronized& synchronized, Args&&... args) {
        return detail::makeSynchronizedLocker(
            synchronized, [](auto& s, auto&&... a) { return s.ulock(std::forward<decltype(a)>(a)...); },
            [](auto& s) { return s.tryULock(); }, std::forward<Args>(args)...);
    }

    template <typename Synchronized, typename... Args>
    auto lock(Synchronized& synchronized, Args&&... args) {
        return detail::makeSynchronizedLocker(
            synchronized, [](auto& s, auto&&... a) { return s.lock(std::forward<decltype(a)>(a)...); },
            [](auto& s) { return s.tryLock(); }, std::forward<Args>(args)...);
    }
} // namespace detail

/**
 * 此类以范围方式暂时解锁 LockedPtr。
 */
template <typename SynchronizedType, typename LockPolicy>
class ScopedUnlocker {
public:
    explicit ScopedUnlocker(LockedPtr<SynchronizedType, LockPolicy>* p) noexcept : ptr_(p), parent_(p->parent()) {
        ptr_->releaseLock();
    }

    ScopedUnlocker(const ScopedUnlocker&) = delete;

    ScopedUnlocker& operator=(const ScopedUnlocker&) = delete;

    ScopedUnlocker(ScopedUnlocker&& other) noexcept
        : ptr_(std::exchange(other.ptr_, nullptr)), parent_(std::exchange(other.parent_, nullptr)) {}

    ScopedUnlocker& operator=(ScopedUnlocker&& other) = delete;

    ~ScopedUnlocker() noexcept(false) {
        if (ptr_) {
            ptr_->reacquireLock(parent_);
        }
    }

private:
    LockedPtr<SynchronizedType, LockPolicy>* ptr_{nullptr};
    SynchronizedType* parent_{nullptr};
};

/**
 * LockedPtr 在 LockedPtr 存在期间保持 Synchronized<T> 对象锁定。
 * 它可以通过使用 operator->() 和 operator*() 直接访问数据的成员。
 * LockPolicy 参数控制是否以独占模式或共享模式获取锁。
 */
template <typename SynchronizedType, typename LockPolicy>
class LockedPtr {
private:
    constexpr static bool AllowsConcurrentAccess = LockPolicy::level != detail::SynchronizedMutexLevel::Unique;
    // 具有适当 const 限定的 DataType
    using CDataType = detail::SynchronizedDataType<SynchronizedType, AllowsConcurrentAccess>;

    template <typename LockPolicyOther>
    using EnableIfSameLevel = std::enable_if_t<LockPolicy::level == LockPolicyOther::level>;

    template <typename, typename>
    friend class LockedPtr;

    friend class ScopedUnlocker<SynchronizedType, LockPolicy>;

public:
    using DataType = typename SynchronizedType::DataType;
    using MutexType = typename SynchronizedType::MutexType;
    using Synchronized = std::remove_const_t<SynchronizedType>;
    using LockType = detail::SynchronizedLockType<LockPolicy::level, MutexType>;

    /**
     * 创建一个未初始化的LockedPtr。
     *
     * 不允许取消引用未初始化的 LockedPtr。
     */
    LockedPtr() = default;

    /**
     * 采用 Synchronized<T> 并锁定它。
     */
    explicit LockedPtr(SynchronizedType* parent) : lock_{!parent ? LockType{} : do_lock(parent->mutex_)} {}

    /**
     * 采用 Synchronized<T> 并在指定的超时时间内尝试锁定它。
     *
     * 阻塞直到获取锁或指定的超时时间到期。如果超时时间到期而未获取锁，则 LockedPtr 将为空，并且 LockedPtr::isNull() 将返回 true。
     */
    template <typename Rep, typename Period>
    LockedPtr(SynchronizedType* parent, const std::chrono::duration<Rep, Period>& timeout)
        : lock_{parent ? LockType{parent->mutex_, timeout} : LockType{}} {}

    /**
     * 移动构造函数。
     */
    LockedPtr(LockedPtr&& rhs) noexcept = default;

    template <typename Type = SynchronizedType, std::enable_if_t<std::is_const_v<Type>, int> = 0>
    /* implicit */ LockedPtr(LockedPtr<Synchronized, LockPolicy>&& rhs) noexcept : lock_{std::move(rhs.lock_)} {}

    template <typename LockPolicyType, EnableIfSameLevel<LockPolicyType>* = nullptr>
    explicit LockedPtr(LockedPtr<SynchronizedType, LockPolicyType>&& other) noexcept : lock_{std::move(other.lock_)} {}

    template <typename Type = SynchronizedType, typename LockPolicyType,
        std::enable_if_t<std::is_const_v<Type>, int> = 0, EnableIfSameLevel<LockPolicyType>* = nullptr>
    explicit LockedPtr(LockedPtr<Synchronized, LockPolicyType>&& rhs) noexcept : lock_{std::move(rhs.lock_)} {}

    /**
     * 移动赋值运算符。
     */
    LockedPtr& operator=(LockedPtr&& rhs) noexcept = default;

    template <typename LockPolicyType, EnableIfSameLevel<LockPolicyType>* = nullptr>
    LockedPtr& operator=(LockedPtr<SynchronizedType, LockPolicyType>&& other) noexcept {
        lock_ = std::move(other.lock_);
        return *this;
    }

    template <typename Type = SynchronizedType, typename LockPolicyType,
        std::enable_if_t<std::is_const_v<Type>, int> = 0, EnableIfSameLevel<LockPolicyType>* = nullptr>
    LockedPtr& operator=(LockedPtr<Synchronized, LockPolicyType>&& other) noexcept {
        lock_ = std::move(other.lock_);
        return *this;
    }

    /*
     * 删除复制构造函数和赋值运算符。
     */
    LockedPtr(const LockedPtr& rhs) = delete;

    LockedPtr& operator=(const LockedPtr& rhs) = delete;

    /**
     * 析构函数释放。
     */
    ~LockedPtr() = default;

    /**
     * 访问底层锁对象。
     */
    LockType& as_lock() noexcept {
        return lock_;
    }
    LockType const& as_lock() const noexcept {
        return lock_;
    }

    /**
     * 检查此 LockedPtr 是否未初始化，或者指向有效的锁定数据。
     *
     * 此方法可用于检查定时获取操作是否成功。如果获取操作超时，则会导致 LockedPtr 为空。
     *
     * LockedPtr 始终为 null，或持有对有效数据的锁定。诸如 scopedUnlock() 之类的方法会在解锁期间将 LockedPtr 重置为 null。
     */
    [[nodiscard]] bool is_null() const {
        return !lock_.owns_lock();
    }

    /**
     * 显式布尔转换。
     *
     * Returns !isNull()
     */
    explicit operator bool() const {
        return lock_.owns_lock();
    }

    /**
     * 访问锁定的数据。
     *
     * 仅当 LockedPtr 有效时才应使用此方法。
     */
    CDataType* operator->() const {
        return std::addressof(parent()->data_);
    }

    /**
     * 访问锁定的数据。
     *
     * 仅当 LockedPtr 有效时才应使用此方法。
     */
    CDataType& operator*() const {
        return parent()->data_;
    }

    void unlock() noexcept {
        lock_ = {};
    }

    /**
     * 允许并发访问（共享）的锁会强制使用标准访问器进行 const 访问，即使 Synchronized 对象是非 const 的。
     *
     * 在某些情况下可能需要非常量访问，例如：
     *   - 在读锁下，如果对数据的一些变异操作是线程安全的（例如，在具有引用稳定性的关联容器中变异值）。
     *
     * 如果父 Synchronized 对象在获取锁时是非 const 的，则 asNonConstUnsafe() 返回对数据的非 const 引用。
     */
    template <typename = void>
    DataType& as_non_const_unsafe() const {
        static_assert(AllowsConcurrentAccess && !std::is_const_v<SynchronizedType>,
            "asNonConstUnsafe() is only available on non-exclusive locks acquired in a non-const context");

        return parent()->data_;
    }

    /**
     * 暂时解锁LockedPtr，并将其重置为空。
     *
     * 返回一个辅助对象，当辅助对象被销毁时，该对象将重新锁定并恢复 LockedPtr。只要此辅助对象存在，就不能取消引用 LockedPtr。
     */
    ScopedUnlocker<SynchronizedType, LockPolicy> scoped_unlock() {
        return ScopedUnlocker<SynchronizedType, LockPolicy>(this);
    }

    SynchronizedType* parent() const {
        using simulacrum = typename SynchronizedType::Simulacrum;
        static_assert(sizeof(simulacrum) == sizeof(SynchronizedType), "mismatch");
        static_assert(alignof(simulacrum) == alignof(SynchronizedType), "mismatch");
        const auto off = offsetof(simulacrum, mutex_);
        const auto raw = reinterpret_cast<char*>(lock_.mutex());
        return reinterpret_cast<SynchronizedType*>(raw - (raw ? off : 0));
    }

private:
    /* implicit */ LockedPtr(LockType lock) noexcept : lock_{std::move(lock)} {}

    template <typename LP>
    static constexpr bool is_try = LP::method == detail::SynchronizedMutexMethod::TryLock;

    template <typename MT, typename LT = LockType, typename LP = LockPolicy, std::enable_if_t<is_try<LP>, int> = 0>
    ALWAYS_INLINE static LT do_lock(MT& mutex) {
        return LT{mutex, std::try_to_lock};
    }

    template <typename MT, typename LT = LockType, typename LP = LockPolicy, std::enable_if_t<!is_try<LP>, int> = 0>
    ALWAYS_INLINE static LT do_lock(MT& mutex) {
        return LT{mutex};
    }

    void release_lock() noexcept {
        assert(lock_.owns_lock());
        lock_ = {};
    }

    void reacquire_lock(SynchronizedType* parent) {
        assert(parent);
        assert(!lock_.owns_lock());
        lock_ = do_lock(parent->mutex_);
    }

    LockType lock_;
};
}
