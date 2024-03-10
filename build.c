#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#define WINDOWS 0
#define LINUX 1

//config
#define PLATFORM WINDOWS

#ifndef STAGE_RECOMPILE

int get_file_edit_time(const char *file) {
    struct stat fileStat = {0};

    if (stat(file, &fileStat) < 0) {
        printf("Failed to get file \'%s\' stats", file);
    }
    return fileStat.st_mtime;

}

void file_rename(const char *old, const char *new) {
#ifdef _WIN32
    if (!MoveFileEx(old, new, MOVEFILE_REPLACE_EXISTING)) {
        printf("Failed to rename file '%s'\n", old);
    }
#else 
    if (rename(old, new) < 0) {
        printf("Failed to rename file '%s'\n", old);
    }
#endif // _WIN32
}

void rebuild() {
    if (get_file_edit_time("build.c") > get_file_edit_time("build.exe")) {
        file_rename("build.exe", "build/build.exe.old");
        system("cc build.c -o build");
        system("build");
        exit(0);
    }
}

int main(int argc, char **argv) {
    rebuild();
    printf("\nBaaaaar");
}

#else

int main(int argc, char **argv) {
    printf("STAGE_1");
}

#endif //STAGE_RECOMPILE

