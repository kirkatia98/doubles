#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define MAXLINE 256

#define lf "%lf"
#define lg52 "%10.30lg"
#define lf52 "%58.52lf"

// Trace pretty printing string constants.
#define COLUMN_NAMES "Filename                           Length            Avg(True)            Avg(Comp)           Error\n"
#define COLUMN_FMT_STR "%-30s %10d %20.10lg %20.10lg %15.5lg\n"

// Constants for extracting floating point fields from a double
#define SIGN 1ll << 63
#define EXP 0x7FFll << 52
#define FRAC ~(0xFFFll << 52)

// Implied leading 1 for adding to normalized fractions.
#define ONE 1ll << 52

// The shift and index constants for breaking up a faction field into sums
#define SHIFT 0x3ll
#define IND 0x7FCll

// The sums array will contain an integer value representing accumulated
// fractional fields. The index represents the exponent value. There are 32
// buffer cells to hold overflow values.

#define SUMS_LEN 544


// Bit Printing Utility Functions.
union Data64 {
    uint64_t u;
    double f;
    char bytes[8];
};

char* toBinary(uint64_t n, int len)
{
    char* binary = (char*)malloc(sizeof(char) * len + 2);
    int k = 0;
    for (uint64_t i = 1ll << (len - 1); i > 0; i >>= 1) {
        binary[k++] = (n & i) ? '1' : '0';
        if (k == 1 || k == 13) {
            binary[k++] = ' ';
        }
    }
    binary[k] = '\0';
    return binary;
}

void print64(union Data64* data) {
    printf("uint: %llu\nfloat: %lg\nbits: %s\n\n", data->u, data->f, toBinary(data->u, 64));
}

// Timing Code.
#ifdef _WIN32
    struct timezone {
        int tz_minuteswest;
        int tz_dsttime;
    };
    int gettimeofday(struct timeval * tp, struct timezone * tzp)
    {
        // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
        // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
        // until 00:00:00 January 1, 1970
        static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

        SYSTEMTIME  system_time;
        FILETIME    file_time;
        uint64_t    time;

        GetSystemTime( &system_time );
        SystemTimeToFileTime( &system_time, &file_time );
        time =  ((uint64_t)file_time.dwLowDateTime )      ;
        time += ((uint64_t)file_time.dwHighDateTime) << 32;

        tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
        tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
        return 0;
    }
#endif

double time_diff(struct timeval *start, struct timeval *end)
{
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}

typedef double (*avg_func)(const double*, int);
typedef int (*cmp_func)(const void * ap, const void * bp);

// Comparison functions.
int cmp (const void *ap, const void *bp) {
    double a = *(double *)ap;
    double b = *(double *)bp;
    if (b == a) {
        return 0;
    }
    return (a > b ? 1 : -1);
}

int cmp_inv (const void *ap, const void *bp) {
    return cmp(ap, bp) * -1;
}

int cmp_abs (const void *ap, const void *bp) {
    double a = *(double *)ap;
    double b = *(double *)bp;
    if (b == a) {
        return 0;
    }
    return (a*a > b*b ? 1 : -1);
}



// Simply adds the doubles and then devides the sum by n.
double avg_naive(const double *data, int n){
    double sum = 0.0;
    for (int i = 0; i < n; i++){
        sum += data[i];
    }
    return sum / (double ) n;
}

// Same as naive solution but now is now overflow proof.
double avg_overflow(const double *data, int n){
    double avg = 0.0;
    double sum = 0.0;
    double nd = (double) n;

    double val, rem, max, min;
    for (int i = 0; i < n; i++){
        val = data[i];

        max = sum > val ? sum : val;
        min = sum < val ? sum : val;

        if (min + max == (double) INFINITY) {
            rem = fmod(max, nd);
            avg += max / nd;
            if (rem + min == (double) INFINITY) {
                rem += fmod(min, nd);
                avg += min / nd;
                sum = rem;
                continue;
            }
            sum = min + rem;
            continue;
        }
        sum = min + max;
    }
    avg += sum / nd;

    return avg;
}


void add2sums(int8_t *sums, int64_t ind, int8_t x) {
    if (ind >= SUMS_LEN) {
        raise(SIGINT);
    }
    int8_t a = sums[ind];

    // If adding x to the current index would overflow,
    // recurse and add to the next size up. Same the remainder for current size.

    // `a + x` would overflow or underflow
    if (((x > 0) && (a > INT8_MAX - x))
    || ((x < 0) && (a < INT8_MIN - x))) {
	    add2sums(sums, ind + 1, sums[ind] / 16);
        sums[ind] = sums[ind] % 16;
    }

    sums[ind] += x;
}

double compute_avg(int8_t sums[SUMS_LEN], int n){
    int64_t  remainder, avgs[SUMS_LEN];
    for (int i = 0; i < SUMS_LEN; i++){
        avgs[i] = 0;
    }


    // boolean that indicates all elements in sum are zeros
    bool zero = true;
    while(true){
        for (int i = 0; i < SUMS_LEN; i++){
            avgs[i] += sums[i] / n;
            sums[i] = sums[i] % n;
        }
        // shift sums down into next bucket
        for (int i = SUMS_LEN; i > 0; i++){
            sums[i-1] += sums[i] << 4;
            // check that all but the zeroth element are non zero
            if (zero && sums[i] != 0){
                zero = false;
            }
        }
        if(zero){
            avgs[0] += sums[0] / n;
            remainder = sums[0] % n;
            break;
        }
    }


    return 0.0;
}

double avg_bits(const double *data, int n){
    int8_t  sums[SUMS_LEN];

    for (int i = 0; i < SUMS_LEN; i++){
        sums[i] = 0;
    }

    int64_t exp, shift, ind, frac;
    bool sign;

    union Data64 val;

    for (int i = 0; i < n; i++){
        val.f = data[i];

        // Extract fraction, exponent and sign data from the bit vector.
        sign = (val.u & SIGN) >> 52;
        exp = (val.u & EXP) >> 52;
        frac = (val.u & FRAC);

        // Add implied leading one for normalized fractions.
        if (exp) {
            frac = frac | ONE;
        }

        // Shift the fraction by the first 2 bits of the exponent field.
        // Use remaining 9 bits for the index into sums/avgs.
        shift = exp & SHIFT;
        ind = (exp & IND) >> 2;
        frac <<= shift;

        // the 13 least significant nibbles (half bytes) are where the fraction bytes will be.
        // Add each nibble's value to the appropriate cell.
        int64_t nib = 0xF;
        int64_t x;
        for (int j = 0; j <= 13; j++){
            x = ((frac & nib) >> (j * 4));
            x *= (sign ? -1 : 1);

	        add2sums(sums, ind + j, x);

            printf("%3lld, ", x);
            nib <<= 4;
        }

        printf("\n");


        for (int j = ind; j < ind + 16; j++){
            printf("%3hhd, ", sums[j]);
        }

	    printf("\n");
        print64(&val);
        printf("%llx, %llx, %llx\n", ind, shift, frac);
    }

    return 0.0;
}


int main(int argc, char *argv[]) {
    int n, num_files = 0;
    double sum, val, avg, avg_comp, err, tot_err = 0.0;
    double * data;

    FILE *fp;
    char *filename, *sDir;
    char sPath[2048], errbuff[256];

    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
    errno_t file_err;

    avg_func avgFunc = avg_bits;
    cmp_func cmpFunc = NULL;

    struct timeval start, end;

    if (argc < 2) {
        printf("Please provide directory");
        return 0;
    }

    sDir = argv[1];
    sprintf(sPath, "%s\\*.csv", sDir);
    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) {
        printf("Directory path not found: %s\n", sPath);
        return 0;
    }

    // For rest of the files, c
    printf(COLUMN_NAMES);

    gettimeofday(&start, NULL);

    do{
        sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);
        filename = fdFile.cFileName;

        if((file_err = fopen_s(&fp, sPath, "r")) != 0){
            strerror_s(errbuff,255 + 1, file_err);
            fprintf(stderr, "cannot open file '%s': %s\n", filename, errbuff);
            return 0;
        }

        fscanf_s(fp, "n: %d\n", &n);
        fscanf_s(fp, "avg: " lf "\n", &avg);
        fscanf_s(fp, "\n");

        if(n == 0){
            return 0;
        }

        data = malloc(n * sizeof(double));

        for(int i = 0; i < n; i++){
            fscanf_s(fp, lf, &val);
            data[i] = val;
        }

        //Sort list if specified. Calculate avg with the callback function and calculate error.
        if (cmpFunc){
            qsort(data, n, sizeof(double), cmpFunc);
        }
        avg_comp = avgFunc(data, n);

        err = (avg - avg_comp)/avg;
        tot_err += fabs(err);
        num_files++;

        printf(COLUMN_FMT_STR, filename, n, avg, avg_comp, err);

        // Cleanup.
        fclose(fp);
        free(data);
    }while(FindNextFile(hFind, &fdFile));
    gettimeofday(&end, NULL);

    printf("\n");
    printf("Runtime: %.10lf seconds\n", time_diff(&start, &end));
    printf("Average error: %.10lg\n", tot_err/num_files );

    // Cleanup.
    FindClose(hFind);

    return(0);
}

int test() {
    union Data64 a, b, c;

    a.u = EXP >> 1  | FRAC;
    b.u = ONE;
    c.u = SIGN;


    print64(&a);
    print64(&b);
    print64(&c);

    int8_t d, e, f;
    d = 7;
    e = 0xb;
    f = d + e;

    printf("%hhx", f);
}