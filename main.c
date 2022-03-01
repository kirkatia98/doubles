#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <stdint.h>

#define MAXLINE 256
#define lf "%lf"
#define lg52 "%10.30lg"
#define lf52 "%58.52lf"

#define COLUMN_NAMES "Filename            Length            Avg(True)            Avg(Comp)           Error\n"
#define COLUMN_FMT_STR "%-15s %10d %20.10lg %20.10lg %15.5lg\n"

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

typedef double (*avg_func)(const double*, int);
typedef int (*cmp_func)(const void * ap, const void * bp);

char* toBinary(unsigned n, int len)
{
    char* binary = (char*)malloc(sizeof(char) * len + 2);
    int k = 0;
    for (unsigned i = (1 << (len - 1)); i > 0; i = i / 2) {
        binary[k++] = (n & i) ? '1' : '0';
    }
    binary[k] = '\0';
    return binary;
}
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

int cmp (const void * ap, const void * bp) {
    double a = *(double *)ap;
    double b = *(double *)bp;
    if (b == a) {
        return 0;
    }
    return (a > b ? 1 : -1);
}

int cmp_inv (const void * ap, const void * bp) {
    return cmp(ap, bp) * -1;
}


double avg_naive(const double* data, int n){
    double sum = 0.0;
    for (int i = 0; i < n; i++){
        sum += data[i];
    }
    return sum / (double ) n;
}

// Same as naive solution but now is now overflow proof.
double avg_overflow(const double* data, int n){
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

    avg_func avgFunc = avg_overflow;
    cmp_func cmpFunc = NULL;

    struct timeval start, end;

    if (argc < 2) {
        printf("Please provide directory");
        return 0;
    }


    // Search for files in passed in directory. Find . and .. files and ignore them.
    sDir = argv[1];
    sprintf(sPath, "%s\\*.*", sDir);
    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) {
        printf("Directory path not found: %s\n", sDir);
        return 0;
    }
    FindNextFile(hFind, &fdFile);

    // For rest of the files, calculate avg with the callback function and calculate error.
    printf(COLUMN_NAMES);

    gettimeofday(&start, NULL);
    while(FindNextFile(hFind, &fdFile))
    {
        sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);
        filename = fdFile.cFileName;

        if((file_err = fopen_s(&fp, sPath, "r")) != 0){
            strerror_s(errbuff,255 + 1, file_err);
            fprintf(stderr, "cannot open file '%s': %s\n", filename, errbuff);
            return 0;
        }

        fscanf_s(fp, "n: %d\n", &n);
        fscanf_s(fp, "sum: " lf "\n", &sum);
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
    }
    gettimeofday(&end, NULL);

    printf("\n");
    printf("Runtime: %.10lf seconds\n", time_diff(&start, &end));
    printf("Total error: %.10lg\n", tot_err/num_files );

    // Cleanup.
    FindClose(hFind);

    return(0);
}


