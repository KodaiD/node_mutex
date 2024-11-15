#pragma once



#include <atomic>

class TestNodeMutex;

class NodeMutex {
  private:
    friend TestNodeMutex;

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
    enum class LockStatus {
        SUCCESS,
        VERSION_CHANGED,
        SHLOCKED,
        SIXLOCKED,
        XLOCKED,
        UNLOCKED
    };

    NodeMutex() : lock_word_{0} {}

    void LockS() {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.xlock_) {
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

    void LockSIX() {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.sixlock_ || expected.xlock_) {
                expected.obj_ = lock_word_.load(std::memory_order_acquire);
                continue;
            }
            desired.obj_ = expected.obj_;
            desired.sixlock_ = 1;
            if (lock_word_.compare_exchange_weak(expected.obj_, desired.obj_, std::memory_order_acquire)) {
                return;
            }
        }
    }

    void LockX() {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.slock_ > 0 || expected.sixlock_ || expected.xlock_) {
                expected.obj_ = lock_word_.load(std::memory_order_acquire);
                continue;
            }
            desired.obj_ = expected.obj_;
            desired.xlock_ = 1;
            if (lock_word_.compare_exchange_weak(expected.obj_, desired.obj_, std::memory_order_acquire)) {
                return;
            }
        }
    }

    bool TryLockS() {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.xlock_) {
                return false;
            }
            desired.obj_ = expected.obj_;
            desired.slock_++;
            if (lock_word_.compare_exchange_weak(expected.obj_, desired.obj_, std::memory_order_acquire)) {
                return true;
            }
        }
    }

    bool TryLockSIX() {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.sixlock_ || expected.xlock_) {
                return false;
            }
            desired.obj_ = expected.obj_;
            desired.sixlock_ = 1;
            if (lock_word_.compare_exchange_weak(expected.obj_, desired.obj_, std::memory_order_acquire)) {
                return true;
            }
        }
    }

    bool TryLockX() {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.xlock_) {
                expected.obj_ = lock_word_.load(std::memory_order_acquire);
                continue;
            }
            if (expected.sixlock_) {
                return false;
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

    bool TryLockS(uint64_t ver) {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.version_ != (ver >> 18)) {
                return false;
            }
            if (expected.xlock_) {
                return false;
            }
            desired.obj_ = expected.obj_;
            desired.slock_++;
            if (lock_word_.compare_exchange_weak(expected.obj_, desired.obj_, std::memory_order_acquire)) {
                return true;
            }
        }
    }

    bool TryLockSIX(uint64_t ver) {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.version_ != (ver >> 18)) {
                return false;
            }
            if (expected.sixlock_ || expected.xlock_) {
                return false;
            }
            desired.obj_ = expected.obj_;
            desired.sixlock_ = 1;
            if (lock_word_.compare_exchange_weak(expected.obj_, desired.obj_, std::memory_order_acquire)) {
                return true;
            }
        }
    }

    bool TryLockX(uint64_t ver) {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.version_ != (ver >> 18)) {
                return false;
            }
            if (expected.xlock_) {
                expected.obj_ = lock_word_.load(std::memory_order_acquire);
                continue;
            }
            if (expected.sixlock_) {
                return false;
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

    LockStatus TryLockXwithVer(uint64_t ver) {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.version_ != (ver >> 18)) {
                return LockStatus::VERSION_CHANGED;
            }
            if (expected.xlock_) {
                expected.obj_ = lock_word_.load(std::memory_order_acquire);
                continue;
            }
            if (expected.sixlock_) {
                return LockStatus::SIXLOCKED;
            }
            if (expected.slock_ > 0) {
                return LockStatus::SHLOCKED;
            }
            desired.obj_ = expected.obj_;
            desired.xlock_ = 1;
            if (lock_word_.compare_exchange_weak(expected.obj_, desired.obj_, std::memory_order_acquire)) {
                return LockStatus::SUCCESS;
            }
        }
    }

    void UpgradeToX() {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.slock_ > 0) {
                expected.obj_ = lock_word_.load(std::memory_order_acquire);
                continue;
            }
            desired.obj_ = expected.obj_;
            desired.sixlock_ = 0;
            desired.xlock_ = 1;
            if (lock_word_.compare_exchange_weak(expected.obj_, desired.obj_, std::memory_order_acquire)) {
                return;
            }
        }
    }

    bool TryUpgradeFromSToX() {
        LockWord expected, desired;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (true) {
            if (expected.slock_ > 1 || expected.sixlock_) {
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

    void DowngradeFromXToS() {
        LockWord desired;
        desired.obj_ = lock_word_.load(std::memory_order_relaxed);
        desired.slock_ = 1;
        desired.xlock_ = 0;
        desired.version_++;
        lock_word_.store(desired.obj_, std::memory_order_release);
    }

    void UnlockS() {
        lock_word_.fetch_sub(1, std::memory_order_release);
    }

    void UnlockSIX() {
        LockWord desired;
        desired.obj_ = lock_word_.load(std::memory_order_relaxed);
        desired.sixlock_ = 0;
        lock_word_.store(desired.obj_, std::memory_order_release);
    }

    uint64_t UnlockX() {
        LockWord desired;
        desired.obj_ = lock_word_.load(std::memory_order_relaxed);
        desired.xlock_ = 0;
        desired.version_++;
        lock_word_.store(desired.obj_, std::memory_order_release);
        return desired.obj_;
    }

    uint64_t GetVersion() const {
        LockWord expected;
        expected.obj_ = lock_word_.load(std::memory_order_acquire);
        while (expected.xlock_) {
            expected.obj_ = lock_word_.load(std::memory_order_acquire);        
        }
        expected.slock_ = 0;
        expected.sixlock_ = 0;
        return expected.obj_;
    }

    bool HasSameVersion(uint64_t ver) const {
        LockWord desired;
        desired.obj_ = lock_word_.load(std::memory_order_acquire);
        desired.slock_ = 0;
        desired.sixlock_ = 0;
        return (ver == desired.obj_);
    }
};
