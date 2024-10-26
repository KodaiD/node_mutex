#pragma once

#include "node_mutex.hpp"

#include <cassert>

class TestNodeMutex {
  public:
    static void TestLockS() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        mutex.LockS();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 1);
        mutex.LockS();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 2);
        mutex.UnlockS();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 1);
        mutex.UnlockS();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 0);
    }

    static void TestLockSIX() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        mutex.LockSIX();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.sixlock_ == 1);
        mutex.UnlockSIX();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.sixlock_ == 0);
    }

    static void TestLockX() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        mutex.LockX();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.xlock_ == 1);
        mutex.UnlockX();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.xlock_ == 0);
    }

    static void TestTryLockS() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        assert(mutex.TryLockS());
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 1);
        assert(mutex.TryLockS());
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 2);
        mutex.UnlockS();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 1);
        mutex.UnlockS();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 0);
    }

    static void TestTryLockSIX() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        assert(mutex.TryLockSIX());
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.sixlock_ == 1);
        mutex.UnlockSIX();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.sixlock_ == 0);
    }

    static void TestTryLockX() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        assert(mutex.TryLockX());
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.xlock_ == 1);
        mutex.UnlockX();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.xlock_ == 0);
    }

    static void TestTryLockSwithVer() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        assert(mutex.TryLockS(lock_word.obj_));
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 1);
        assert(mutex.TryLockS(lock_word.obj_));
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 2);
        mutex.UnlockS();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 1);
        mutex.UnlockS();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 0);
    }

    static void TestTryLockSIXwithVer() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        mutex.TryLockSIX(lock_word.obj_);
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.sixlock_ == 1);
        mutex.UnlockSIX();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.sixlock_ == 0);
    }

    static void TestTryLockXwithVer() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        mutex.TryLockX(lock_word.obj_);
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.xlock_ == 1);
        mutex.UnlockX();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.xlock_ == 0);
    }

    static void TestUpgradeAndDowngrade() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        mutex.LockSIX();
        mutex.UpgradeToX();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.xlock_ == 1);
        mutex.UnlockX();
        mutex.LockS();
        assert(mutex.TryUpgradeFromSToX());
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.xlock_ == 1);
        mutex.DowngradeFromXToS();
        lock_word.obj_ = mutex.lock_word_;
        assert(lock_word.slock_ == 1);
        mutex.UnlockS();
    }

    static void TestGetVersionAndHasSameVersion() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        mutex.LockS();
        assert(mutex.GetVersion() == 0);
        mutex.LockSIX();
        assert(mutex.GetVersion() == 0);
        mutex.UnlockS();
        mutex.UnlockSIX();
        lock_word.obj_ = mutex.lock_word_;
        assert(mutex.HasSameVersion(0));
        mutex.LockX();
        mutex.UnlockX();
        lock_word.obj_ = 0;
        lock_word.version_++;
        assert(mutex.HasSameVersion(lock_word.obj_));
    }
};