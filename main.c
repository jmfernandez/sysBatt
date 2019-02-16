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