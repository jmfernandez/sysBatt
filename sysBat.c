#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#define READBUFSIZE	4096

const char power_supply_basedir[] = "/sys/class/power_supply";
const char battery_present_entry[] = "present";
const char power_supply_type_entry[] = "type";
const char capacity_entry[] = "capacity";
const char battery_type[] = "Battery";

struct BatStats {
	char name[NAME_MAX+1];
	int capacity;
};

struct BatStats * getBatStats() {
	unsigned int numBats = 0;
	struct BatStats * batStats = NULL;
	
	/* First, we are going to learn the available resources */
	DIR * psDir = NULL;
	if((psDir = opendir(power_supply_basedir)) != NULL) {
		struct dirent * entry = NULL;
		while((entry = readdir(psDir)) != NULL) {
			/* The directory is interesting. Now we are going to open it */
			if(entry->d_name[0] != '.' && entry->d_type != DT_REG) {
				/* saving the position */
				long nextPos = telldir(psDir);
				closedir(psDir);
				
				size_t batDirPathSize = sizeof(power_supply_basedir)+1+strlen(entry->d_name);
				char * batName = strdup(entry->d_name);
				char * batDirPath = (char *)malloc(batDirPathSize);
				sprintf(batDirPath,"%s/%s",power_supply_basedir,entry->d_name);
				
				DIR * batDir = NULL;
				if((batDir = opendir(batDirPath))!=NULL) {
					struct dirent * batEntry = NULL;
					int isPresent = 0;
					int isBattery = 0;
					int capacity = 0;
					
					/* reading buffer */
					char * content = (char *)malloc(READBUFSIZE);
					/* sys path buffer */
					char * batEntryPath = (char *)malloc(batDirPathSize+NAME_MAX);
					/* The file being opened */
					FILE * batEntryFile = NULL;
					int op = 0;
					
					while((batEntry = readdir(batDir)) != NULL) {
						if(strcmp(batEntry->d_name,power_supply_type_entry)==0) {
							/* Is this a battery? */
							op = 1;
						} else if(strcmp(batEntry->d_name,battery_present_entry)==0) {
							/* Is the device present? */
							op = 2;
						} else if(strcmp(batEntry->d_name,capacity_entry)==0) {
							/* The charging level */
							op = 3;
						} else {
							op = 0;
						}
						
						if(op > 0) {
							sprintf(batEntryPath,"%s/%s",batDirPath,batEntry->d_name);
							
							if((batEntryFile = fopen(batEntryPath,"r")) != NULL) {
								switch(op) {
									case 1:
										/* Is this a battery? */
										fscanf(batEntryFile,"%s",content);
										isBattery = strcmp(battery_type,content) == 0;
										break;
									case 2:
										/* Is the device present? */
										fscanf(batEntryFile,"%d",&isPresent);
										break;
									case 3:
										/* The charging level */
										fscanf(batEntryFile,"%d",&capacity);
										break;
								}
								fclose(batEntryFile);
							}
						}
					}
					closedir(batDir);
					if(content!=NULL) {
						free((void *)content);
					}
					if(batEntryPath!=NULL) {
						free((void *)batEntryPath);
					}
					
					/* We found a present battery, so record it */
					if(isBattery > 0 && isPresent > 0) {
						batStats = (struct BatStats *)realloc((void *)batStats,sizeof(struct BatStats *)*(numBats+2));
						
						batStats[numBats].capacity = capacity;
						strcpy(batStats[numBats].name,batName);
						numBats++;
						batStats[numBats].name[0] = '\0';
					}
				}
				free((void *)batName);
				if((psDir = opendir(power_supply_basedir)) != NULL) {
					seekdir(psDir,nextPos);
				} else {
					break;
				}
			}
		}
		closedir(psDir);
	}
	
	return batStats;
}

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