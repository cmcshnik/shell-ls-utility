#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <dirent.h>
#include <libgen.h>
#include <ctype.h> 

#include "ls.h"


typedef struct FileObject {
    char* name_;
    char* style_;
    struct stat* info_;
} FileObject;

// Освобождение объекта типа FileObj (приведение типов внутри)
void FreeFileObject(void* obj);

// Получение имени
char* GetFileName(FileObject* obj);

// Получение информации
struct stat* GetFileInfo(FileObject* obj);

// Создание 
FileObject* NewFileObject(char* file_name, char* dir_name, bool read_symlinks);

// Смена названия с освобождением памяти 
FileObject* ChangeFileObjectName(FileObject* obj, char* new_name);

// Вывод подробной информации о файле
void PrintFile(FileObject* file_obj, const ListArgs* args, FILE* out);

// Лексикографическая сортировка файлов 
int LexicographicallOrder(const void* file_obj1, const void* file_obj2);

// Сортировка по размеру
int SizeOrder(const void* void_file_obj1, const void* void_file_obj2);

// Сортировка по времени
int TimeOrder(const void* void_file_obj1, const void* void_file_obj2);

// Функция для печати в феловекочитаемом виде
void PrintHumanReadableSize(off_t size, int si)