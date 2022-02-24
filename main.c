#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 256
#define lf "%lf"
#define lf52 "%.52lf"

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

int cmpfunc (const void * ap, const void * bp) {
    double a = *(double *)ap;
    double b = *(double *)bp;
    if (b == a) {
        return 0;
    }
    return (a > b ? 1 : -1);
}

double avg_naive(const double* data, int n){
    double sum = 0.0;
    for (int i = 0; i < n; i++){
        sum += data[i];
    }
    return sum / (double ) n;
}



int main(int argc, char *argv[]) {
    int n;
    double sum, avg, val;
    double * data;

    FILE * fp;
    char* filename;

    errno_t err;
    char errbuff[256];

    if (argc < 2) {
        printf("Please provide a filename.");
        return 0;
    }

    filename = argv[1];
    if((err = fopen_s(&fp, filename, "r")) != 0){
        strerror_s(errbuff,255 + 1,err);
        fprintf(stderr, "cannot open file '%s': %s\n", filename, errbuff);
        return 0;
    }

    fscanf_s(fp, "n: %d\n", &n);
    fscanf_s(fp, "sum: " lf "\n", &sum);
    fscanf_s(fp, "avg: " lf "\n", &avg);
    fscanf_s(fp, "\n");

    printf("n: %d\n", n);
    printf("sum: " lf52 "\n", sum);
    printf("avg: " lf52 "\n", avg);
    printf("\n");

    data = malloc(n * sizeof(double));

    for(int i = 0; i < n; i++){
        fscanf_s(fp, lf, &val);
        printf(lf52 ", %la\n", val, val);
        data[i] = val;
    }
    fclose(fp);

    double avg1 = avg_naive(data, n);

    qsort(data, n, sizeof(double),cmpfunc);


    double avg2 = avg_naive(data, n);

    printf("\n");
    printf("Naive: " lf52 "," lf52 "\n", avg1, avg - avg1 );\
    printf("Sorted: " lf52 "," lf52 "\n", avg2, avg - avg2 );
    printf("\n");

    return(0);
}


