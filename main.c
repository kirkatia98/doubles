#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

union Data32 {
    unsigned u;
    float f;
    char bytes[4];
};

union Data64 {
    unsigned long u;
    double f;
    char bytes[8];
};

char* toBinary(unsigned n, int len)
{
    char* binary = (char*)malloc(sizeof(char) * len + 2);
    int k = 0;
    for (unsigned i = (1 << (len - 1)); i > 0; i = i / 2) {
        binary[k++] = (n & i) ? '1' : '0';
        if (k == 1 || k == 10) {
            binary[k++] = ' ';
        }
    }
    binary[k] = '\0';
    return binary;
}

void print32(union Data32* data) {
    printf("uint: %u\nfloat: %e\nbits: %s\n\n", data->u, data->f, toBinary(data->u, 32));
}

int main() {
    union Data32 a, b, c, d;

    a.bytes[0] = 0;
    a.bytes[1] = 0;
    a.bytes[2] = 0x80;
    a.bytes[3] = 0x3F;

    b.f = a.f * 2;
    c.f = a.f * 3;
    d.f = a.f * 4;

    print32(&a);
    print32(&b);
    print32(&c);
    print32(&d);
}


