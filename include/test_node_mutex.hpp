#pragma once

#include "node_mutex.hpp"

#include <cassert>
#include <thread>
#include <vector>
#include <cstdio>

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

        printf("TestLockS passed\n");
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

        printf("TestLockSIX passed\n");
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

        printf("TestLockX passed\n");
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

        printf("TestTryLockS passed\n");
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

        printf("TestTryLockSIX passed\n");
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

        printf("TestTryLockX passed\n");
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

        printf("TestTryLockSwithVer passed\n");
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

        printf("TestTryLockSIXwithVer passed\n");
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

        printf("TestTryLockXwithVer passed\n");
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

        printf("TestUpgradeAndDowngrade passed\n");
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

        printf("TestGetVersionAndHasSameVersion passed\n");
    }

    static void TestLockCompatibility() {
        NodeMutex mutex;
        NodeMutex::LockWord lock_word;

        mutex.LockS();
        assert(!mutex.TryLockX());
        assert(mutex.TryLockSIX());
        assert(mutex.TryLockS());
        assert(!mutex.TryUpgradeFromSToX());
        mutex.UnlockS();
        mutex.UnlockSIX();
        assert(mutex.TryUpgradeFromSToX());
        mutex.UnlockX();

        mutex.LockSIX();
        assert(!mutex.TryLockX());
        assert(!mutex.TryLockSIX());
        assert(mutex.TryLockS());
        mutex.UnlockS();
        mutex.UpgradeToX();
        mutex.DowngradeFromXToS();
        mutex.UnlockS();

        mutex.LockX();
        assert(!mutex.TryLockX());
        assert(!mutex.TryLockSIX());
        assert(!mutex.TryLockS());
        mutex.UnlockX();

        printf("TestLockCompatibility passed\n");
    }

    static void TestParallelExecution(size_t num_threads) {
        NodeMutex mutex;
        std::vector<std::thread> threads;

        for (int i = 0; i < num_threads; i++) {
            threads.push_back(std::thread([&mutex]() {
                mutex.LockS();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                mutex.UnlockS();
           
                mutex.LockSIX();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                mutex.UnlockSIX();
            
                mutex.LockX();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                mutex.UnlockX();
            
                while (!mutex.TryLockS()) ;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                mutex.UnlockS();
            
                while (!mutex.TryLockSIX()) ;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                mutex.UnlockSIX();
            
                while (!mutex.TryLockX()) ;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                mutex.UnlockX();
            
                mutex.LockS();
                while (!mutex.TryUpgradeFromSToX()) ;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                mutex.UnlockX();
            
                mutex.LockSIX();
                mutex.UpgradeToX();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                mutex.UnlockX();
            
                mutex.LockX();
                mutex.DowngradeFromXToS();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                mutex.UnlockS();
            }));
        }

        for (auto& thread : threads) {
            thread.join();
        }

        printf("TestParallelExecution passed\n");
    }
};