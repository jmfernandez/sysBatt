#include <stdio.h>
#include <stdlib.h>
#include "sysBat.h"

int main(int argc,char* argv) {
	struct BatStats * batStats = getBatStats();
	struct BatStats * currBatStats = batStats;
	
	if(batStats!=NULL) {
		for(currBatStats = batStats;currBatStats->name[0] != '\0';currBatStats++) {
			fprintf(stderr,"Bat %s, level %d\n",currBatStats->name,currBatStats->capacity);
		}
		
		free((void *)batStats);
	}
	
	return 0;
}