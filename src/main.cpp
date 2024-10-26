#include "test_node_mutex.hpp"

#include <iostream>

int main() {
    NodeMutex mutex;

    if (!mutex.TryLockX()) {
        exit(1);
    }

    mutex.UnlockX();

    mutex.LockS();

    if (mutex.TryLockX()) {
        exit(1);
    }

    if (!mutex.TryUpgradeFromSToX()) {
        exit(1);
    }

    mutex.UnlockX();

    mutex.LockS();

    mutex.LockS();

    if (mutex.TryUpgradeFromSToX()) {
        exit(1);
    }

    mutex.UnlockS();

    if (!mutex.TryUpgradeFromSToX()) {
        exit(1);
    }

    mutex.UnlockX();

    TestNodeMutex::TestLockS();
    TestNodeMutex::TestLockSIX();
    TestNodeMutex::TestLockX();

    TestNodeMutex::TestTryLockS();
    TestNodeMutex::TestTryLockSIX();
    TestNodeMutex::TestTryLockX();

    std::cout << "TEST PASSED." << std::endl;
}