#include "node_mutex.hpp"

#include <iostream>

int main() {
    NodeMutex mutex;

    if (!mutex.TryLockX()) {
        exit(1);
    }

    mutex.UnlockX();

    mutex.SLock();

    if (mutex.TryLockX()) {
        exit(1);
    }

    if (!mutex.TryUpgrade()) {
        exit(1);
    }

    mutex.UnlockX();

    mutex.SLock();

    mutex.SLock();

    if (mutex.TryUpgrade()) {
        exit(1);
    }

    mutex.UnlockS();

    if (!mutex.TryUpgrade()) {
        exit(1);
    }

    mutex.UnlockX();

    std::cout << "TEST PASSED." << std::endl;
}