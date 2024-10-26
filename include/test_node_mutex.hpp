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
};