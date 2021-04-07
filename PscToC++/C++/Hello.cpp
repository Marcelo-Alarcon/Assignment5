#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono;

int i;

int hello(int argc, char *args[])
{
    auto _start = steady_clock::now();

    printf("%d Hello, world!\n", i);

    auto _end = steady_clock::now();
    long _elapsed = duration_cast<milliseconds>(_end - _start).count();
    printf("\n[%ld milliseconds execution time.]\n", _elapsed);
}
