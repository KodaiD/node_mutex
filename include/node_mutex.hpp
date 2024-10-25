#pragma once

#include <atomic>

class NodeMutex {
  public:
    struct LockWord {
        union {
            uint64_t obj_;
            struct {
                uint16_t slock_ : 16;
                bool sixlock_ : 1;
                bool xlock_ : 1;
                uint64_t version_ : 46;
            };
        };

        LockWord() : obj_(0) {}
    };

    std::atomic<uint64_t> lock_word_;

  public:
    NodeMutex() : lock_word_{0} {}

    void SLock() {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.sixlock_ || expected.xlock_) {
                expected.obj_ = lock_word_.load(std::memory_order_acquire);
                continue;
            }
            desired.obj_ = expected.obj_;
            desired.slock_++;
            if (lock_word_.compare_exchange_weak(expected.obj_, desired.obj_, std::memory_order_acquire)) {
                return;
            }
        }
    }

    bool TryLockX() {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.sixlock_ != 0 || expected.xlock_ != 0) {
                expected.obj_ = lock_word_.load(std::memory_order_acquire);
                continue;
            }
            if (expected.slock_ > 0) {
                return false;
            }
            desired.obj_ = expected.obj_;
            desired.xlock_ = 1;
            if (lock_word_.compare_exchange_weak(expected.obj_, desired.obj_, std::memory_order_acquire)) {
                return true;
            }
        }
    }

    bool TryUpgrade() {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.slock_ > 1) {
                return false;
            }
            desired.obj_ = expected.obj_;
            desired.slock_ = 0;
            desired.xlock_ = 1;
            if (lock_word_.compare_exchange_weak(expected.obj_, desired.obj_, std::memory_order_acquire)) {
                return true;
            }
        }
    }

    void UnlockS() {
        lock_word_.fetch_sub(1, std::memory_order_release);
    }

    void UnlockX() {
        LockWord desired;
        desired.obj_ = lock_word_.load(std::memory_order_relaxed);
        desired.xlock_ = 0;
        desired.version_++;
        lock_word_.store(desired.obj_, std::memory_order_release);
    }
};
