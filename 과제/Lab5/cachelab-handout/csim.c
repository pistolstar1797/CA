#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include "cachelab.h"

struct Parameter {
	int s;
	int b;
	int E;
	int S;
	int B;
	int hitNum;
	int missNum;
	int evictNum;
};

struct Line {
	int latest;
	int valid;
	long long int tag;
	char *block;
};

struct Set {
	struct Line *lines;
};

struct Cache {
	 struct Set *sets;
};

struct Cache initCache(long long int setNum, int lineNum, long long int blockSize) {    
	struct Cache newCache;	 
	struct Set set;
	struct Line line;
	int set_i;
	int line_i;

	newCache.sets = (struct Set*) malloc(sizeof(struct Set) * setNum);

	for (set_i = 0 ; set_i < setNum ; set_i++) {   
		set.lines =  (struct Line *) malloc(sizeof(struct Line) * lineNum);
		newCache.sets[set_i] = set;

		for (line_i = 0 ; line_i < lineNum ; line_i++) {
			line.latest = 0;
			line.valid = 0;
			line.tag = 0;
			set.lines[line_i] = line;
        }	
	}

	return newCache;
}

void clean(struct Cache myCache, long long int setNum, int lineNum, long long int blockSize) {   
    int set_i;

	for (set_i = 0 ; set_i < setNum ; set_i++)	{
		struct Set set = myCache.sets[set_i];
		
		if (set.lines != NULL) {	
			free(set.lines);
		}
	} 

	if (myCache.sets != NULL) {
		free(myCache.sets);
	}
}

int findEmpty(struct Set exS, struct Parameter exP) {
	int lineNum = exP.E;
	int index;
	struct Line line;

	for (index = 0 ; index < lineNum ; index++) {
		line = exS.lines[index];
		if (line.valid == 0) {
			return index;
		}
	}

	return -1;
}

int findEvict(struct Set exS, struct Parameter exP, int *lineUsed) {
	int lineNum = exP.E;
	int maxUse = exS.lines[0].latest;
	int minUse = exS.lines[0].latest;
	int minUse_index = 0;
	int line_i;

	for (line_i = 1 ; line_i < lineNum ; line_i++) {
		if (minUse > exS.lines[line_i].latest) {
			minUse_index = line_i;	
			minUse = exS.lines[line_i].latest;
		}

		if (maxUse < exS.lines[line_i].latest) {
			maxUse = exS.lines[line_i].latest;
		}
	}

	lineUsed[0] = minUse;
	lineUsed[1] = maxUse;
	return minUse_index;
}

struct Parameter access(struct Cache myCache, struct Parameter exP, long long int addr) {
		int line_i;
		int isCacheFull = 1;

		int lineNum = exP.E;
		int hitPrev = exP.hitNum;

		int tagSize = (64 - (exP.s + exP.b));
		long long int tagInput = addr >> (exP.s + exP.b);
		unsigned long long int temp = addr << (tagSize);
		unsigned long long int set_i = temp >> (tagSize + exP.b);
		
  		struct Set exS = myCache.sets[set_i];

		for (line_i = 0 ; line_i < lineNum ; line_i++) 	{
			if (exS.lines[line_i].valid) {
				if (exS.lines[line_i].tag == tagInput) {
					exS.lines[line_i].latest++;
					exP.hitNum++;
				}
			} else if (!(exS.lines[line_i].valid) && (isCacheFull)) {
				isCacheFull = 0;
			}
		}	

		if (hitPrev == exP.hitNum) {
			exP.missNum++;
		} else {
			return exP;
		}

		int *lineUsed = (int*) malloc(sizeof(int) * 2);
		int minUse_index = findEvict(exS, exP, lineUsed);	

		if (isCacheFull) {
			exP.evictNum++;
			exS.lines[minUse_index].tag = tagInput;
			exS.lines[minUse_index].latest = lineUsed[1] + 1;
		}

		else {
            int empty_index = findEmpty(exS, exP);

			exS.lines[empty_index].tag = tagInput;
			exS.lines[empty_index].valid = 1;
			exS.lines[empty_index].latest = lineUsed[1] + 1;
		}						

		free(lineUsed);
		return exP;
}

int main(int argc, char **argv)
{
	
	struct Cache myCache;
	struct Parameter exP;
	bzero(&exP, sizeof(exP));
	long long int setNum;
	long long int blockSize;
	FILE *openTrace;
	char inst;
	long long int addr;
	int size;
	char *fileTrace;
	char c;
	
    while( (c=getopt(argc,argv,"hvs:E:b:t:")) != -1) {
        switch(c) {
        case 's':
            exP.s = atoi(optarg);
            break;
        case 'E':
            exP.E = atoi(optarg);
            break;
        case 'b':
            exP.b = atoi(optarg);
            break;
        case 't':
            fileTrace = optarg;
            break;
        case 'h':
            exit(0);
        default:
            exit(1);
        }
    }

 	setNum = 1 << exP.s;
	blockSize = 1 << exP.b;
	exP.hitNum = 0;
	exP.missNum = 0;
	exP.evictNum = 0;
	myCache = initCache(setNum, exP.E, blockSize);

	openTrace  = fopen(fileTrace, "r");
    
	if (openTrace != NULL) {
		while (fscanf(openTrace, " %c %llx,%d", &inst, &addr, &size) == 3) {
			switch(inst) {
				case 'I':
					break;
				case 'L':
					exP = access(myCache, exP, addr);
					break;
				case 'S':
					exP = access(myCache, exP, addr);
					break;
				case 'M':
					exP = access(myCache, exP, addr);
					exP = access(myCache, exP, addr);	
					break;
				default:
					break;
			}
		}
	}
	
    printSummary(exP.hitNum, exP.missNum, exP.evictNum);
	clean(myCache, setNum, exP.E, blockSize);
	fclose(openTrace);
    return 0;
}
