#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono;

int number;

double root(double x)
{
    double root;

    double r;
    double prev;
    double diff;
    r = 1;
    prev = 0;
    do
    {
        r = (x/r + r)/2;
        diff = r - prev;
        
        if(diff < 0) {
diff = -diff;
}
        prev = r;
    }
    while (!(diff < 1.0e-10));
    root = r;

    return root;
}

void print(int n, double root)
{
    printf("The square root of %4d is %8.4f\n", n, root);
}

int main(int argc, char *args[])
{
    auto _start = steady_clock::now();

    
    for (number = 1; number <= 25; number++)
{
            print(number, root(number));
        }

    auto _end = steady_clock::now();
    long _elapsed = duration_cast<milliseconds>(_end - _start).count();
    printf("\n[%ld milliseconds execution time.]\n", _elapsed);
}
