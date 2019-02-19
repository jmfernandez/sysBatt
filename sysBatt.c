/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 * Author: Jose M. Fernandez (GitHub user: jmfernandez)
 */
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "sysBatt.h"

#define READBUFSIZE	4096

const static char power_supply_basedir[] = "/sys/class/power_supply";
const static char battery_present_entry[] = "present";
const static char power_supply_type_entry[] = "type";
const static char capacity_entry[] = "capacity";
const static char battery_type[] = "Battery";

struct BattStats * getBattStats() {
	unsigned int numBatts = 0;
	struct BattStats * battStats = NULL;
	
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
				
				/* Get the path to the battery */
				char * battName = strdup(entry->d_name);
				size_t battNameLen = strlen(battName);
				size_t battDirPathSize = sizeof(power_supply_basedir)+1+battNameLen;
				char * battDirPath = (char *)malloc(battDirPathSize);
				memcpy(battDirPath,power_supply_basedir,sizeof(power_supply_basedir)-1);
				battDirPath[sizeof(power_supply_basedir)-1] = '/';
				strcpy(battDirPath+sizeof(power_supply_basedir),entry->d_name);
				
				DIR * battDir = NULL;
				if((battDir = opendir(battDirPath))!=NULL) {
					struct dirent * battEntry = NULL;
					int isPresent = 0;
					int isBattery = 0;
					int capacity = 0;
					
					/* reading buffer */
					char * content = (char *)malloc(READBUFSIZE);
					/* sys path buffer */
					size_t battEntryPathLen = battDirPathSize+NAME_MAX;
					char * battEntryPath = (char *)malloc(battEntryPathLen);
					/* The file being opened */
					FILE * battEntryFile = NULL;
					int op = 0;
					
					while((battEntry = readdir(battDir)) != NULL) {
						if(strcmp(battEntry->d_name,power_supply_type_entry)==0) {
							/* Is this a battery? */
							op = 1;
						} else if(strcmp(battEntry->d_name,battery_present_entry)==0) {
							/* Is the device present? */
							op = 2;
						} else if(strcmp(battEntry->d_name,capacity_entry)==0) {
							/* The charging level */
							op = 3;
						} else {
							op = 0;
						}
						
						if(op > 0) {
							/* Get the path to the property */
							snprintf(battEntryPath,battEntryPathLen,"%s/%s",battDirPath,battEntry->d_name);
							
							if((battEntryFile = fopen(battEntryPath,"r")) != NULL) {
								switch(op) {
									case 1:
										/* Is this a battery? */
										if(fgets(content,READBUFSIZE,battEntryFile)!=NULL) {
											size_t lcontent = strlen(content);
											if(content[lcontent-1]=='\n') {
												content[lcontent-1] = '\0';
											}
											isBattery = strcmp(battery_type,content) == 0;
										}
										break;
									case 2:
										/* Is the device present? */
										/*
										fprintf(stderr,"%s => (%d)\n",battEntryPath,isPresent,fscanf(battEntryFile,"%d",&isPresent));
										*/
										fscanf(battEntryFile,"%d",&isPresent);
										break;
									case 3:
										/* The charging level */
										/*
										fprintf(stderr,"%s => (%d)\n",battEntryPath,capacity,fscanf(battEntryFile,"%d",&capacity));
										*/
										fscanf(battEntryFile,"%d",&capacity);
										break;
								}
								fclose(battEntryFile);
							}
						}
					}
					closedir(battDir);
					
					if(content!=NULL) {
						free((void *)content);
					}
					if(battEntryPath!=NULL) {
						free((void *)battEntryPath);
					}
					
					/* We found a present battery, so record it */
					if(isBattery > 0 && isPresent > 0) {
						battStats = (struct BattStats *)realloc((void *)battStats,sizeof(struct BattStats *)*(numBatts+2));
						
						battStats[numBatts].capacity = capacity;
						strcpy(battStats[numBatts].name,battName);
						numBatts++;
						battStats[numBatts].name[0] = '\0';
					}
				}
				free((void *)battName);
				
				/* Restoring the position where we left the directory reading */
				if((psDir = opendir(power_supply_basedir)) != NULL) {
					seekdir(psDir,nextPos);
				} else {
					break;
				}
			}
		}
		closedir(psDir);
	}
	
	return battStats;
}
