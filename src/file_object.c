#include "file_object.h"

#define STYLE_DIR "\033[1;34m"
#define STYLE_LINK "\033[1;36m"
#define STYLE_BACKUP "\033[30m"
#define STYLE_END "\033[0m"

// Для человекочитаемого вывода
const char* units_h[] = {"", "K", "M", "G"};
const char* units_si[] = {"", "k", "m", "g"};


FileObject* NewFileObject(char* file_name, char* dir_name, bool read_symlinks) {
    FileObject* obj = malloc(sizeof(FileObject));
    if (!obj) {
        return NULL;
    }

    // Выделяем память и смотрим, оканчивается ли пусть на /
    if ((dir_name[strlen(dir_name) - 1] != '/') && (strlen(dir_name) != 0)) {
        obj->name_ = malloc((strlen(file_name) + 1 + strlen(dir_name) + 1) * sizeof(char));
    } else {
        obj->name_ = malloc((strlen(file_name) + 0 + strlen(dir_name) + 1) * sizeof(char));
    }

    if (!obj->name_) {
        return NULL;
    }

    // Склеиваем вместе название папки файла и имя файла, если нужно доклеиваем /
    strcpy(obj->name_, dir_name);
    if ((dir_name[strlen(dir_name) - 1] != '/') && (strlen(dir_name) != 0)) {
        strcat(obj->name_, "/");
    }
    strcat(obj->name_, file_name);

    // Получем информацию о файле
    obj->info_ = malloc(sizeof(struct stat));
    if (!obj->info_) {
        free(obj);
        return NULL;
    }

    if (read_symlinks) {
        stat(obj->name_, obj->info_);
    } else {
        lstat(obj->name_, obj->info_);
    }

    // Cтили
    if (S_ISDIR(obj->info_->st_mode)) {
        obj->style_ = STYLE_DIR;
    } else if (S_ISLNK(obj->info_->st_mode)) {
        obj->style_ = STYLE_LINK;
    } else if (obj->name_[strlen(obj->name_) - 1] == '~') {
        obj->style_ = STYLE_BACKUP;
    } else {
        obj->style_ = STYLE_END;
    }

    return obj;
}

char* GetFileName(FileObject* obj) {
    return obj->name_;
}

struct stat* GetFileInfo(FileObject* obj) {
    return obj->info_;
}


FileObject* ChangeFileObjectName(FileObject* obj, char* new_name) {
    free(obj->name_);

    obj->name_ = strdup(new_name);
    if (!obj->name_) {
        return NULL;
    }

    return obj;
}

void FreeFileObject(void* obj) {
    FileObject* file_obj = (FileObject*)obj;

    free(file_obj->info_);
    free(file_obj->name_);
    free(file_obj);
}

// Функция для печати в феловекочитаемом виде
void PrintHumanReadableSize(off_t size, int si) {
    const char** units = si ? units_si : units_h;
    int base = si ? 1000 : 1024;
    
    double human_size = size;
    int unit_index = 0;

    while (human_size >= base && unit_index < 4) {
        human_size /= base;
        unit_index++;
    }

    printf(" %.1f%s", human_size, units[unit_index]);
}

// Функция для отображения прав доступа в формате drwxrwxrwx
static void PrintPermissions(mode_t mode, FILE* out) {
    if (S_ISDIR(mode)) {
        fputc('d', out);
    } else if (S_ISLNK(mode)) {
        fputc('l', out);
    } else {
        fputc('-', out);
    }
    fputc( (mode & S_IRUSR) ? 'r' : '-', out);
    fputc( (mode & S_IWUSR) ? 'w' : '-', out);

    if (mode & S_ISUID) {
        fputc( (mode & S_IXUSR) ? 's' : 'S', out);
    } else {
        fputc( (mode & S_IXUSR) ? 'x' : '-', out);
    }

    fputc( (mode & S_IRGRP) ? 'r' : '-', out);
    fputc( (mode & S_IWGRP) ? 'w' : '-', out);

    if (mode & S_ISGID) {
        fputc( (mode & S_IXGRP) ? 's' : 'S', out);
    } else {
        fputc( (mode & S_IXGRP) ? 'x' : '-', out);
    }

    fputc( (mode & S_IROTH) ? 'r' : '-', out);
    fputc( (mode & S_IWOTH) ? 'w' : '-', out);
    if (mode & S_ISVTX) {
        fputc( (mode & S_IXOTH) ? 't' : 'T', out);
    } else {
        fputc( (mode & S_IXOTH) ? 'x' : '-', out);
    }
}

// Функция для вывода информации о файле
void PrintFile(FileObject* file_obj, const ListArgs* args, FILE* out) {
    if (!args->long_format) {
        if (args->size) {
            printf("%ld ", file_obj->info_->st_blocks);
        }

        printf("%s%s%s\t", file_obj->style_, basename(file_obj->name_), STYLE_END);
        return;
    }

    struct stat* info = file_obj->info_;

    // Размер файла в блоках, если нужно
    if (args->size) {
        printf("%ld ", file_obj->info_->st_blocks);
    }

    // Права
    PrintPermissions(info->st_mode, out);

    // Количество жестких ссылок 
    printf(" %lu", info->st_nlink);

    // Имя владельца
    struct passwd *pwd = getpwuid(info->st_uid);
    if (pwd != NULL) {
        printf(" %s", pwd->pw_name);
    } else {
        printf(" %d", info->st_uid);
    }

    // Название группы
    struct group *grp = getgrgid(info->st_gid);
    if (grp != NULL) {
        printf(" %s", grp->gr_name);
    } else {
        printf(" %d", info->st_gid);
    }

    // Размер файла
    if (args->human_readable) {
        PrintHumanReadableSize((long long)info->st_size, args->si);
    } else {
        printf(" %5lld", (long long)info->st_size);
    }

    // Выводим время последней модификации
    char timebuf[30];
    struct tm *timeinfo = localtime(&info->st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", timeinfo);
    printf(" %s", timebuf);

    // Выводим имя файла
    printf(" %s%s%s", file_obj->style_, basename(file_obj->name_), STYLE_END);
}

// Функции сравнения файлов для сортировки
int LexicographicallOrder(const void* void_file_obj1, const void* void_file_obj2) { 
    FileObject* file_obj1 = *(FileObject**)void_file_obj1;
    FileObject* file_obj2 = *(FileObject**)void_file_obj2;

    char c1, c2;
    int i;
    for (i = 0; file_obj1->name_[i] != '\0' || file_obj2->name_[i] != '\0'; i++) { 
        c1 = file_obj1->name_[i];
        c2 = file_obj2->name_[i];
        c1 = isalpha(c1) ? tolower(c1) : c1;
        c2 = isalpha(c2) ? tolower(c2) : c2;

        if (c1 > c2) { 
            return 1;  
        } else if (c2 > c1) { 
            return -1; 
        } 
    } 
    
    if (file_obj1->name_[i] != '\0') { 
        return 1; 
    } else if (file_obj2->name_[i] != '\0') { 
        return -1; 
    } else { 
        return 0;
    } 
} 

int SizeOrder(const void* void_file_obj1, const void* void_file_obj2) {
    FileObject* file_obj1 = *(FileObject**)void_file_obj1;
    FileObject* file_obj2 = *(FileObject**)void_file_obj2;

    if (file_obj1->info_->st_size < file_obj2->info_->st_size) {
        return -1;
    } else if (file_obj1->info_->st_size > file_obj2->info_->st_size) {
        return 1;
    } else {
        return 0;
    }
}

int TimeOrder(const void* void_file_obj1, const void* void_file_obj2) {
    FileObject* file_obj1 = *(FileObject**)void_file_obj1;
    FileObject* file_obj2 = *(FileObject**)void_file_obj2;

    if (file_obj1->info_->st_mtime < file_obj2->info_->st_mtime) {
        return -1;
    } else if (file_obj1->info_->st_mtime > file_obj2->info_->st_mtime) {
        return 1;
    } else {
        return 0;
    }
}