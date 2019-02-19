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
#include "sysBatt.h"

#include <time.h>
#include <string.h>

const static char NO_BATTERY[] = "No battery";


char date_label[] = "%d-%b-%y (%a); %I:%M %p";

int main(int argc,char** argv) {
	
    char buf[150];
    time_t now = time ((time_t *) 0);
    struct tm *tm = localtime (&now);
    size_t off_date = 0;
    memset (buf, 0, sizeof(buf));
    
      struct BattStats * bStats = getBattStats();
      
      if ( bStats != NULL ) {
        if(bStats[0].name[0] != '\0') {
          snprintf(buf,sizeof(buf),"%s %d%% ",bStats[0].name,bStats[0].capacity);
        }
        free((void *)bStats);
      }
      
      if (buf[0] == '\0') {
        strcpy(buf,NO_BATTERY);
        off_date = sizeof(NO_BATTERY);
        buf[off_date] = ' ';
      } else {
        off_date = strlen(buf);
      }
    
    strftime (buf+off_date, sizeof(buf)-off_date-1, date_label, tm);
	
	puts(buf);

	return 0;
}