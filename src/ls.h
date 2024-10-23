#pragma once

#include "vector.h"


#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <errno.h>

typedef struct ListArgs {
    bool all;            
    bool almost_all;     
    bool ignore_backups; 
    bool directory;       
    bool dereference;      
    bool human_readable; 
    bool si;             
    bool size;
    bool reverse;         
    bool long_format;

    enum { NONE, SIZE, TIME } sort_by; // Соответствует --sort. По умолчанию: NONE (без сортировки)
} ListArgs;

typedef enum ListErrorCode {
    LIST_SUCCESS,
    LIST_DIRECTORY_ACCESS_FAULT,
    LIST_STAT_FUNC_ERROR,
    // <-- YOUR CODE HERE -->
} ListErrorCode;

ListArgs* DefaultListArgs(void);
void FreeListArgs(ListArgs* args);
/*
*  See the docs for the following functions and types:
*  - malloc, calloc, free
*  - opendir, readdir, closedir
*  - stat, lstat
*  - struct stat: S_ISDIR, S_ISCHR, S_ISBLK, S_ISLNK, S_ISFIFO, S_ISSOCK, getpwuid, getgrgid
*  - time_t: localtime, strftime
*  - snprintf
*  - qsort
*/
ListErrorCode ListPaths(GenericVector* paths, const ListArgs* args, FILE* out);
void MyShellGlobbing(char* pattern, GenericVector* paths, bool dereference);