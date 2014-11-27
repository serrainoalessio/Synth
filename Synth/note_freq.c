#include <stdio.h>
#include <math.h>

#define F 440
#define S 1024
#define P 8192
#define C 16000000

int main() {
    int x = 57, y;
    double MULT, TIME; // note frequency
    for (y=0;y<127;y++) {
        MULT=pow(2, (y-x)/12.0);
        TIME=MULT*F*S*P/C;
        printf("%.0f, ", TIME);
    }
    return 0;
}
