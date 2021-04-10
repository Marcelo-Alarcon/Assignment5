#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono;

int i;
int j;

int main(int argc, char *args[])
{
    auto _start = steady_clock::now();

    i = 1;
    
    while (i <= 5)
{
        printf("i = %d\n", i);
        i = i + 1;
    }
    cout << endl;
    i = 1;
    
    while (i <= 5)
{
        j = 10;
        
        while (j <= 30)
{
            printf("i = %d, j = %d\n", i, j);
            j = j + 10;
        }
        i = i + 1;
    }

    auto _end = steady_clock::now();
    long _elapsed = duration_cast<milliseconds>(_end - _start).count();
    printf("\n[%ld milliseconds execution time.]\n", _elapsed);
}
