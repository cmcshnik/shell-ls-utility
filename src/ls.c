#include "ls.h"
#include "file_object.h"

ListArgs* DefaultListArgs() {
    ListArgs* args = malloc(sizeof(ListArgs));
    if (!args) {
        return NULL;
    }

    args->all = false;
    args->almost_all = true;
    args->ignore_backups = false;
    args->directory = false;
    args->dereference = false;
    args->human_readable = false;
    args->si = false;
    args->size = false;
    args->reverse = false;
    args->long_format = false;
    args->sort_by = NONE;
    return args;
}

void FreeListArgs(ListArgs* args) {
    free(args);
}

ListErrorCode ListPaths(const GenericVector* paths, const ListArgs* args, FILE* out) 
    struct dirent *entry;
    DIR* dir;
    GenericVector* result_output;
    FileObject* file_obj;
    bool is_accepted;
    struct stat* cur_stat;
    char* cur_path;
    int status;
    //bool do_continue;
    //char* buf_path;

    // // Глоббинг
    //do_continue = true;
    // for (int k = 0; k < paths->len_; ++k) {
    //     buf_path = strdup(cur_path);

    //     if (buf_path[0] == '\'' && buf_path[strlen(buf_path) - 1] == '\'') {
    //         do_continue = false;
    //         DeleteElement(paths, i, &free);

    //         buf_path[strlen(buf_path) - 1] = '\0';
    //         buf_path = buf_path + 1;
            

    //         for (int j = 0; j < paths->len_; ++j) {
    //             if (my_glob(buf_path, (char*)GetElement(paths, j))) {
                    
    //             }
    //         }
    //     } 

    //     free(buf_path);
    // }

    for (int i = 0; i < paths->len_; ++i) {        
        cur_path = (char*)GetElement(paths, i);

        // Получение информации об объекте, лежащем по адресу cur_path, с обработкой флага -L
        cur_stat = malloc(sizeof(struct stat));
        if (args->dereference) {
            status = stat(cur_path, cur_stat);
        } else {
            status = lstat(cur_path, cur_stat);
        }

        if (status == -1) {
            free(cur_stat);
            return LIST_STAT_FUNC_ERROR;
        }

        // Если это НЕ папка, обрабатываем как одиночный файл и идём дальше
        if (!S_ISDIR(cur_stat->st_mode)) {
            file_obj = NewFileObject(cur_path, "", args->dereference);

            
            PrintFile(file_obj, args, out);

            free(cur_stat);
            FreeFileObject(file_obj);
            fputc('\n', out);
            continue;
        }

        // Если это всё же папка...
        free(cur_stat);
        dir = opendir(cur_path);
        if (!dir) {
            return LIST_DIRECTORY_ACCESS_FAULT;
        }
        result_output = NewGenericVector(1);

        // Вывод названий директорий, если таковых несколько
        if ((!args->directory) && (paths->len_ > 1)) {
            printf("\033[1m%s\033[0m:\n", cur_path);
        }

        // Чтение и обработка файлов из директории
        while ((entry = readdir(dir)) != NULL) {
            is_accepted = true;

            // Получение информации о файле и обработка флага -L
            file_obj = NewFileObject(entry->d_name, cur_path, args->dereference);

            // Обработка флага -a
            if (!args->all) {
                if (basename(file_obj->name_)[0] == '.') {
                    is_accepted = false;
                }
            }

            // Обработка флага -А
            if (args->almost_all) {
                if (strcmp(basename(file_obj->name_), ".") == 0 || strcmp(basename(file_obj->name_), "..") == 0) {
                    is_accepted = false;
                }
            }

            // Обработка флага -В
            if (args->ignore_backups) {
                if (file_obj->name_[strlen(file_obj->name_) - 1] == '~') {
                    is_accepted = false;
                }
            }

            // Обработка флага -d
            if (args->directory) {
                if (strcmp(basename(file_obj->name_), ".") == 0) {
                    ChangeFileObjectName(file_obj, cur_path);
                    is_accepted = true;
                } else {
                    is_accepted = false;
                }
            }

            if (is_accepted) {
                Append(result_output, file_obj);
            } else {
                FreeFileObject(file_obj);
            }
        }

        // Сортировка
        if (args->sort_by == SIZE) {
            qsort(result_output->arr_, result_output->len_, sizeof(FileObject*), &SizeOrder);
            ReverseGenericVector(result_output);
        } else if (args->sort_by == TIME) {
            qsort(result_output->arr_, result_output->len_, sizeof(FileObject*), &TimeOrder);
            ReverseGenericVector(result_output);
        } else {
            qsort(result_output->arr_, result_output->len_, sizeof(FileObject*), &LexicographicallOrder);
        }

        // Обработка флага -r
        if (args->reverse) {
            ReverseGenericVector(result_output);
        }
        

        // Печать файлов
        for (int j = 0; j < result_output->len_; ++j) {
            file_obj = GetElement(result_output, j);
            
            PrintFile(file_obj, args, out);
        }

        closedir(dir);  
        if (!args->directory) {
            fputs("\n\n", out);
        };

        // Освобождение памяти
        FreeGenericVector(result_output, &FreeFileObject);
    }

    //FreeFileObject(file_obj);

    if (args->directory) {
        fputc('\n', out);
    }

    // for (int i = 0; i < paths->len_; ++i) {
    //     printf("%s ", cur_path);
    // }
    // fputs("\n", out);

    // printf("%d ", args->all);
    // printf("%d ", args->almost_all);
    // printf("%d ", args->ignore_backups);
    // printf("%d ", args->directory);
    // printf("%d ", args->dereference);
    // printf("%d ", args->human_readable);
    // printf("%d ", args->si);
    // printf("%d ", args->size);
    // printf("%d ", args->reverse);
    // printf("%d ", args->long_format);

    return LIST_SUCCESS;
