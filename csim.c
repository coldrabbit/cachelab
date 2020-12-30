#include "cachelab.h"

int hit_count, miss_count, eviction_count;//需要输出的三个参数，分别为命中数、缺失数、清出数

typedef struct{
    int valid;
    int tag;
    int time;
}cache_line;

int main(int argc, char* argv[])
{
    hit_count = miss_count = eviction_count = 0;
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
