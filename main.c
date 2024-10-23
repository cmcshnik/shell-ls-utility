#include "src/vector.h"
#include "src/ls.h"
#include "src/file_object.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <string.h>
#include <linux/limits.h>


// valgrind --leak-check=full --track-origins=yes
// ls -l /usr/bin/crontab
// ls -l /usr/bin/sudo
// ls -ld /tmp

// Все возможные флаги и их индексы в массиве
const int kFlagsAmount = 22;
const char* kFlags[kFlagsAmount] = {
    "--all", "-a", // 0 1
    "--almost-all", "-A", // 2 3
    "--ignore-backups", "-B", // 4 5
    "--directory", "-d", // 6 7
    "--dereference", "-L", // 8 9
    "--human-readable", "-h", // 10 11
    "--si", // 12
    "--size", "-s", // 13 14
    "--sort", // 15
    "--reverse", "-r", // 16 17
    "-S", // 18
    "-t", // 19
    "-U", // 20
    "-l" // 21
};

// TODO: тесты, сортировка папок, вывод total, обработка проблем с системными вызовами, ичпользовать геттеры

// Проверка на существование второго аргумента для параметров -i, -o, -v
static void CheckingSecondFlagArgument(int cur, int argc, char** argv) {
    if (cur == argc) {
        perror("Second argument is missing!");
    }
    
    for (int i = 0; i < kFlagsAmount; ++i) {
        if (strcmp(argv[cur], kFlags[i]) == 0) {
            perror("Second argument is missing!");
        }
    }
}


int main(int argc, char** argv) {
    int i = 1;
    bool flag_exists;
    char* current_dir = malloc(2 * sizeof(char));
    current_dir[0] = '.';
    current_dir[1] = '\0';
    ListErrorCode status;
    errno = 0;

    ListArgs* args = DefaultListArgs();
    if (!args) {
        errno = ENOMEM;
        perror("Memory allocation error occured!");
    }
    
    GenericVector* vector = NewGenericVector(1);

    while (i < argc) {
        flag_exists = false;
        for (int j = 0; j < kFlagsAmount; ++j) {
            if (strcmp(argv[i], kFlags[j]) == 0) {
                flag_exists = true;
                switch (j) {
                    case 0:
                    case 1: { args->all = true; args->almost_all = false; break; }   
                    case 2:
                    case 3: { args->almost_all = true; break; } 
                    case 4:
                    case 5: { args->ignore_backups = true; break; } 
                    case 6:
                    case 7: { args->directory = true; break; } 
                    case 8:
                    case 9: { args->dereference = true; break; } 
                    case 10:
                    case 11: { args->human_readable = true; break; } 
                    case 12: { args->si = true; break; } 
                    case 13:
                    case 14: { args->size = true; break; } 
                    case 15: { 
                        i++;
                        CheckingSecondFlagArgument(i, argc, argv);

                        if (strcmp(argv[i], "none") == 0) {
                            args->sort_by = NONE;
                        } else if (strcmp(argv[i], "time") == 0) {
                            args->sort_by = TIME;
                        } else if (strcmp(argv[i], "size") == 0) {
                            args->sort_by = SIZE;
                        } else {
                            errno = EINVAL;
                            perror("Second argument");
                        }

                        break; 
                    } 
                    case 16:
                    case 17: { args->reverse = true; break; }
                    case 18: { args->sort_by = SIZE; break; } 
                    case 19: { args->sort_by = TIME; break; } 
                    case 20: { args->sort_by = NONE; break; } 
                    case 21: { args->long_format = true; break; } 
                default:
                    break;
                }
            }
        }
        if (flag_exists) {
            i++;
            continue;
        }

        if (argv[i][0] == '-') {
            errno = EINVAL;
            perror("Flags error");
        }
            
        Append(vector, strdup(argv[i]));
        i++;
    }

    // Если пути не указаны, смотреть актуальную директорию
    if (vector->len_ == 0) {
        Append(vector, strdup(current_dir));
    }

    // Вызов функции "ls"
    if (errno == 0) {
        status = ListPaths(vector, args, stdout);
        switch (status)
        {
        case LIST_SUCCESS:
            break;
        case LIST_DIRECTORY_ACCESS_FAULT: {
            perror("Error getting to directory");
            break;
        } 
        case LIST_STAT_FUNC_ERROR: {
            perror("Can't get information about file/directory");
            break;
        }
        default:
            break;
        }
    }

    // Освобождение всей использованной памяти
    FreeListArgs(args);
    FreeGenericVector(vector, &free);
    free(current_dir);

    return 0;
}
