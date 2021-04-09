#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono;

int i;
int j;
char ch;

int main(int argc, char *args[])
{
    auto _start = steady_clock::now();

    
    for (i = 1; i <= 5; i++)
	
printf("i = %d\n", i);
    cout << endl;
    
    for (i = 1; i <= 3; i++)
	
{
            
            for (j = 4; j >= 1; j--)
	
{
                    printf("i = %d, j = %d\n", i, j);
                }
        }
    cout << endl;
    
    for (ch = 'a'; ch <= 'z'; ch++)
	
printf("%c", ch);
    cout << endl;
    
    for (ch = 'Z'; ch >= 'A'; ch--)
	
printf("%c", ch);
    cout << endl;

    auto _end = steady_clock::now();
    long _elapsed = duration_cast<milliseconds>(_end - _start).count();
    printf("\n[%ld milliseconds execution time.]\n", _elapsed);
}
