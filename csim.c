/***
 * editor: songjie
 * time: 2020.1.4
 * function: cache simulator
 ***/

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>

int hit_count = 0,
    miss_count = 0,
    eviction_count = 0;//arguments required to output

typedef struct{
    int h;
    int v;
    int s;
    int E;
    int b;
    char t[100];
}arguments;//the arguments input from order

arguments args;//use global argument for arguments from order

typedef struct{
    int validBit;
    int tag;
    int LRU_counter;
}cache_line, *cache_set, **cache;//cache's structure

cache myCache = NULL;//use global argument for cache

void initCache(){//initial cache
    int S = 1 << args.s;//cache has S = 2^s cache lines

    myCache = (cache)malloc(sizeof(cache_set)*S);
    for(int i = 0; i < S; i++){//malloc for each cache line
        myCache[i] = (cache_set)malloc(sizeof(cache_line)*args.E);
        for(int j = 0; j < args.E; j++){
            myCache[i][j].validBit = 0;
            myCache[i][j].tag = -1;
            myCache[i][j].LRU_counter = 0;
        }
    }
}

void usage(){//-h print the help info
    printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n"
           "Options:\n"
           "  -h         Print this help message.\n"
           "  -v         Optional verbose flag.\n"
           "  -s <num>   Number of set index bits.\n"
           "  -E <num>   Number of lines per set.\n"
           "  -b <num>   Number of block offset bits.\n"
           "  -t <file>  Trace file.\n\n"
           "Examples:\n"
           "  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n"
           "  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

void parseArgs(int argc, char* argv[]){
    args.h = 0;
    args.v = 0;
    int opt;

    while((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1){//parse arguments from order
        switch(opt){
            case 'h':
                args.h = 1;
                usage();
                break;
            case 'v':
                args.v = 1;
                break;
            case 's':
                args.s = atoi(optarg);
                break;
            case 'E':
                args.E = atoi(optarg);
                break;
            case 'b':
                args.b = atoi(optarg);
                break;
            case 't':
                strcpy(args.t, optarg);
                break;
            default:
                break;
        }
    }
}

void update(int address){//update contents in cache by LRU
    int tag = address >> (args.b + args.s);
    int index = (address >> args.b) & ((1 << args.s) - 1);
    cache_set set = myCache[index];

    int i;

    for(i = 0; i < args.E; i++){
        if(set[i].validBit == 1)
            set[i].LRU_counter++;
    }

    for(i = 0; i < args.E; i++){//hit
        if(set[i].validBit == 1 && set[i].tag == tag){
            hit_count++;
            set[i].LRU_counter = 0;
            return;
        }
    }

    miss_count++;//miss
    for(i = 0; i < args.E; i++){//there is blank line
        if(set[i].validBit == 0){
            set[i].validBit = 1;
            set[i].tag = tag;
            set[i].LRU_counter = 0;
            return;
        }
    }

    eviction_count++;//there is no blank line, evict a cache line
    int max_LRU_counter = set[0].LRU_counter;
    int max_LRU_counter_index = 0;
    for(i = 0; i < args.E; i++){
        if(set[i].LRU_counter > max_LRU_counter){
            max_LRU_counter = set[i].LRU_counter;
            max_LRU_counter_index = i;
        }
    }
    set[max_LRU_counter_index].LRU_counter = 0;
    set[max_LRU_counter_index].tag = tag;
    return;
}

void freeCache(){//free the space of cache malloced in the past
    int S = 1 << args.s;//set numbers of cache

    for(int i = 0; i < S; i++){
        free(myCache[i]);
    }
    free(myCache);
}

void parseCache(){//parse the trace file and update the cache
    char operation;
    int address;
    int size;//three arguments of memory traces' order

    FILE *fp = fopen(args.t, "r");//open the trace file
    if(fp == NULL){
        printf("file open failed\n");
        exit(-1);
    }

    while(fscanf(fp, " %c %x,%d", &operation, &address, &size) > 0){
        switch(operation){//'I' denotes an instruction load, ignore it
            case 'L'://data load
                update(address);
                break;
            case 'M'://data modify(a data load followed by a data store)
                update(address);
            case 'S'://data store
                update(address);
                break;
            default:
                break;
        }
    }

    fclose(fp);

    freeCache();//free the space of cache
}



/*main*/
int main(int argc, char* argv[])
{
    parseArgs(argc, argv);
    initCache();
    parseCache();
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
