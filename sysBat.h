#ifndef _SYSBAT_H
#	define _SYSBAT_H

#include <dirent.h>

struct BatStats {
	char name[NAME_MAX+1];
	int capacity;
};

struct BatStats * getBatStats();

#endif	/*_SYSBAT_H*/