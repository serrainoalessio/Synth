#include <stdio.h>

unsigned int Mult, Add; // mult = 53 Add = 3
unsigned char bit;

unsigned char random() {
    bit*=Mult;
    bit+=Add;
    return bit;
}

int main() {
    int r0;
    int i, rn, prec;

    Mult = 1;
    Add = 1;

    for (Mult = 1; Mult < 256; Mult+=4) 
    for (Add = 1; Add < 256; Add+=2) {

        bit = 0;
        r0 = random();
        i = 0;
        //printf("%d --> %d\n", i, r0);
        rn = random();
        prec = rn;
        i++;
        //printf("%d --> %d\n", i, rn);
        while (rn != r0) {
            prec = rn;
            rn = random();
            i++;
            //printf("%d --> %d\n", i, rn);
            if (rn == prec) {
                printf("Reached a stall for: M %d A %d\n", Mult, Add);
                break;
            }
        }
        if (i != 256)
            printf("M %d A %d\n", Mult, Add);
    }

    return 0;
}
