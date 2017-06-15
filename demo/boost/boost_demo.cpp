//
// Created by v on 17-4-6.
//

#include "boost/thread.hpp"
using namespace boost::this_thread::hiden;
int test_sleep_until() {
    timespec ts;
    sleep_until(ts);

    return 0;
}

int main() {
    test_sleep_until();
    return 0;
}