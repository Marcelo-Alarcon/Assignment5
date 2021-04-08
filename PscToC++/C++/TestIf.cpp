#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono;

int i;
int j;
int k;
double x;
double y;
bool p;
bool q;
string str;
string txt;

int main(int argc, char *args[])
{
    auto _start = steady_clock::now();

    i = 1;
    j = 2;

    if(i == j) {
x = 3.14;
    } else {x = -5;
}

    if(i != j) {
y = 3.14;
    } else {y = -5;
}
    printf("i = %d, j = %d, x = %5.2f, y = %5.2f\n", i, j, x, y);

    if(i == j) {
{
            x = -7;
        }
    } else {{
            x = 8;
        }
}

    if(i != j) {
{
            y = 14;
        }
    } else {{
            y = -2;
        }
}
    printf("i = %d, j = %d, x = %5.2f, y = %5.2f\n", i, j, x, y);
    p = i == j;

    if(p) {
x = 55.55;
    } else {{
            x = 77.77;
            y = 88.88;
        }
}
    printf("i = %d, j = %d, x = %5.2f, y = %5.2f\n", i, j, x, y);
    q = i <= j;
    k = 10;

    if(p) {
i = 33;
    } else {
        if(!q) {
i = 44;
        } else {
            if(i == j) {
i = 55;
            } else {i = 6;
}
}
}

    if(!p) {
        if(q) {
j = 9;
        } else {j = -9;
}
    } else {}

    if(p) {
        if(q) {
k = 11;
        } else {k = 12;
}
    } else {}
    printf("i = %d, j = %d, x = %5.2f, y = %5.2f\n", i, j, x, y);
    printf("k = %d\n", k);
    cout << endl;

    if(!p && q && (i != j) && (x < y)) {
printf("Good-bye");
    } else {}

    if(!p) {
        if(q) {
            if(i != j) {
                if(x < y) {
printf(", world!\n");
                } else {}
            } else {}
        } else {}
    } else {}

    auto _end = steady_clock::now();
    long _elapsed = duration_cast<milliseconds>(_end - _start).count();
    printf("\n[%ld milliseconds execution time.]\n", _elapsed);
}
