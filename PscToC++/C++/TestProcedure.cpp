#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono;

int i;
int j;
double x;
double y;
bool p;

void alpha(int m, int n, double r, int k, int o, double w, double z, 
           bool q)
{
    double a;
    double b;
    double c;
    int i;
    a = 1;
    b = 2;
    c = a*b - w/z;
    i = 3;
    k = k + i + j + n;
    q = (!p) || ((i == j) && (w > 2) && (w/z == 1.5)) ;
    z = r;
    printf("alpha parms:  m = %d, n = %d, r = %4.2f, k = %d, o = %d, w = %4.2f, z = %4.2f, q = %d\n", 
           m, n, r, k, o, w, z, q);
    printf("alpha locals: a = %4.2f, b = %4.2f, c = %4.2f, i = %d\n", 
           a, b, c, i);
}

void beta()
{
    double i;
    double j;
    i = 22;
    j = 44;
    printf("beta locals:  Hello, world! %4.2f\n", i + j);
}

void gamma(int i, int n, double x, double r)
{
    double a;
    double b;
    double c;
    a = i + n;
    b = i*x + r;
    c = i;
    i = i + n;
    printf("gamma parms:  i = %d, n = %d, x = %4.2f, r = %4.2f\n", 
           i, n, x, r);
    printf("gamma locals: a = %4.2f, b = %4.2f, c = %4.2f\n", 
           a, b, c);
}

int main(int argc, char *args[])
{
    auto _start = steady_clock::now();

    i = 5;
    j = 7;
    x = 3;
    y = 2;
    p = false;
    printf("main:         i = %d, j = %d, x = %4.2f, y = %4.2f, p = %d\n", 
           i, j, x, y, p);
    alpha(6, i-3, x+y/i, i, j, x, y, p);
    printf("main:         i = %d, j = %d, x = %4.2f, y = %4.2f, p = %d\n", 
           i, j, x, y, p);
    beta();
    printf("main:         i = %d, j = %d, x = %4.2f, y = %4.2f, p = %d\n", 
           i, j, x, y, p);
    gamma(i, j, i/x, i*j);
    printf("main:         i = %d, j = %d, x = %4.2f, y = %4.2f, p = %d\n", 
           i, j, x, y, p);

    auto _end = steady_clock::now();
    long _elapsed = duration_cast<milliseconds>(_end - _start).count();
    printf("\n[%ld milliseconds execution time.]\n", _elapsed);
}
