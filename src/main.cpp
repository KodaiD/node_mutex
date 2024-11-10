#include "test_node_mutex.hpp"

#include <iostream>

int main() {
    NodeMutex mutex;

    TestNodeMutex::TestLockS();
    TestNodeMutex::TestLockSIX();
    TestNodeMutex::TestLockX();

    TestNodeMutex::TestTryLockS();
    TestNodeMutex::TestTryLockSIX();
    TestNodeMutex::TestTryLockX();

    TestNodeMutex::TestTryLockSwithVer();
    TestNodeMutex::TestTryLockSIXwithVer();
    TestNodeMutex::TestTryLockXwithVer();

    TestNodeMutex::TestUpgradeAndDowngrade();
    TestNodeMutex::TestGetVersionAndHasSameVersion();
    TestNodeMutex::TestLockCompatibility();
    TestNodeMutex::TestParallelExecution(4);

    TestNodeMutex::TestTryLockXwithVer2();

    std::cout << "TEST PASSED." << std::endl;
}