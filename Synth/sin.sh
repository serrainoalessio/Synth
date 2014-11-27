#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323

int main() {
    int i; 
    for (i = 0; i < 2048; i++) {
        printf("%d, ", round(sin(2*PI*i/2048)*127));
    }
    return 0;
}
