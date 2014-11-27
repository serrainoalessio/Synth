#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323

int main() {
    int i; 
    for (i = 0; i < 8192; i++) {
        printf("%4d, ", (int)round((sin(2*PI*(float)i/8192.0) + .5*sin(4*PI*(float)i/8192.0) + .5*sin(8*PI*(float)i/8192.0) + 2)*65.5));
        if (i % 32 == 31)
            printf("\n");
    }
    return 0;
}
