
/*YESSIN MOAKHER yessin.moakher*/

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cachelab.h"
#include <limits.h>

/* Globals set by command line args */
int verbosity = 0; /* print trace if set */
int s = 0;         /* set index bits */
int b = 0;         /* block offset bits */
int E = 0;         /* associativity */
char* trace_file = NULL;

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

int S; 
int B; 
int timer = 0; 

typedef struct {
    int valid;          
    unsigned long tag;  
    int lru;           
} cache_line;

cache_line** cache;

void initCache() {
    S = 1 << s; 
    B = 1 << b; 

    cache = (cache_line**)malloc(S * sizeof(cache_line*));
    for (int i = 0; i < S; i++) {
        cache[i] = (cache_line*)malloc(E * sizeof(cache_line));
        for (int j = 0; j < E; j++) {
            cache[i][j].valid = 0;
            cache[i][j].tag = 0;
            cache[i][j].lru = 0;
        }
    }
}

void freeCache() {
    for (int i = 0; i < S; i++) {
        free(cache[i]);
    }
    free(cache);
}

void accessCache(unsigned long address, int* hits, int* misses, int* evictions) {

    unsigned long setIndex = (address >> b) & ((1 << s) - 1);
    unsigned long tag = address >> (s + b);

    cache_line* set = cache[setIndex];

    int hitIndex = -1;
    int emptyIndex = -1;
    int lruIndex = -1;
    int minLRU = INT_MAX;

    for (int i = 0; i < E; i++) {
        if (set[i].valid) {
            if (set[i].tag == tag) {
                hitIndex = i;
                break;
            }
            if (set[i].lru < minLRU) {
                minLRU = set[i].lru;
                lruIndex = i;
            }
        } else if (emptyIndex == -1) {
            emptyIndex = i;
        }
    }

    if (hitIndex != -1) { 
        (*hits)++;
        set[hitIndex].lru = timer++;
        if (verbosity) printf("hit ");
    } else { 
        (*misses)++;
        if (verbosity) printf("miss ");

        if (emptyIndex != -1) { 
            set[emptyIndex].valid = 1;
            set[emptyIndex].tag = tag;
            set[emptyIndex].lru = timer++;
        } else { 
            (*evictions)++;
            if (verbosity) printf("eviction ");
            set[lruIndex].tag = tag;
            set[lruIndex].lru = timer++;
        }
    }
}


void parse(char* trace_file, int* hits, int* misses, int* evictions) {
    FILE* file = fopen(trace_file, "r");

    char operation;
    unsigned long address;
    int size;

    while (fscanf(file, " %c %lx,%d", &operation, &address, &size) == 3) {
        if (operation == 'I') continue;

        if (verbosity) printf("%c %lx,%d ", operation, address, size);

        switch (operation) {
            case 'L': 
            case 'S': 
                accessCache(address, hits, misses, evictions);
                break;
            case 'M': 
                accessCache(address, hits, misses, evictions);
                accessCache(address, hits, misses, evictions);
                break;
        }

        if (verbosity) printf("\n");
    }

    fclose(file);
}




/*
 * main - Main routine 
 */
int main(int argc, char* argv[])
{
  char c;
  
  while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
    switch(c){
    case 's':
      s = atoi(optarg);
      break;
    case 'E':
      E = atoi(optarg);
      break;
    case 'b':
      b = atoi(optarg);
      break;
    case 't':
      trace_file = optarg;
      break;
    case 'v':
      verbosity = 1;
      break;
    case 'h':
      printUsage(argv);
      exit(0);
    default:
      printUsage(argv);
      exit(1);
    }
  }

  /* Make sure that all required command line args were specified */
  if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
    printf("%s: Missing required command line argument\n", argv[0]);
    printUsage(argv);
    exit(1);
  }

  initCache();

  int hits = 0, misses = 0, evictions = 0;
  parse(trace_file, &hits, &misses, &evictions);

  freeCache();

  printSummary(hits, misses, evictions);

  return 0;
}
