#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono;

int i;
int j;
int even;
int odd;
int prime;
char ch;
char str;

int main(int argc, char *args[])
{
    auto _start = steady_clock::now();

    i = 3;
    ch = 'b';
    even = -990;
    odd = -999;
    prime = 0;
    switch(i + 1) {
        case 1:
j = i;
        break; 
        case -8:
j = 8*i;
        break; 
        case 5:
        case 7:
        case 4:
j = 574*i;
        break; 
}
        printf("j = %d\n", j);
        switch(ch) {
            case 'c':
            case 'b':
str = 'p';
            break; 
            case 'a':
str = 'q';
            break; 
}
            printf("str = '%c'\n", str);
            
            for (i = -5; i <= 15; i++)
{
                    switch(i) {
                        case 2:
prime = i;
                        break; 
                        case -4:
                        case -2:
                        case 0:
                        case 4:
                        case 6:
                        case 8:
                        case 10:
                        case 12:
even = i;
                        break; 
                        case -3:
                        case -1:
                        case 1:
                        case 3:
                        case 5:
                        case 7:
                        case 9:
                        case 11:
switch(i) {
                            case -3:
                            case -1:
                            case 1:
                            case 9:
odd = i;
                            break; 
                            case 2:
                            case 3:
                            case 5:
                            case 7:
                            case 11:
prime = i;
                            break; 
}
                            break; 
}
                            printf("i = %d, even = %d, odd = %d, prime = %d\n", 
                                   i, even, odd, prime);
                        }

                    auto _end = steady_clock::now();
                    long _elapsed = duration_cast<milliseconds>(_end - _start).count();
                    printf("\n[%ld milliseconds execution time.]\n", _elapsed);
                }
