#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

// Path separator
#ifdef _WIN32
#define PS "\\"
#else
#define PS "/"
#endif // _WIN32

//Compiler
#ifndef COMPILER
#   ifdef _WIN32
#       if defined(__GNUC__)
#           define COMPILER "gcc"
#       elif defined(__clang__)
#           define COMPILER "clang"
#       elif defined(_MSC_VER)
#           define COMPILER "cl.exe"
#       else
#           define COMPILER "cc"
#       endif
#   else
#       if defined(__GNUC__)
#           define COMPILER "gcc"
#       elif defined(__clang__)
#           define COMPILER "clang"
#       else
#           define COMPILER "cc"
#       endif
#   endif // _WIN32
#endif // COMPILER

//Config
#define WINDOWS 0
#define LINUX 1

#ifdef _WIN32
#define DEFAULT_PLATFORM "WINDOWS"
#endif // _WIN32
#ifdef __linux__
#define DEFAULT_PLATFORM "LINUX"
#endif // _WIN32

void make_build_directory();
void rebuild(char *executableName);
int get_file_edit_time(const char *file);
void file_rename(const char *old, const char *new);

#ifdef WITH_CONFIG
#include "build/config.h"

int main(int argc, char **argv) {
    printf("\n---[ STAGE 2 ]---\n");
    printf("PLATFORM: %d", PLATFORM);
}

#else

int main(int argc, char **argv) {
    make_build_directory();
    if (argc > 0) {
        rebuild(argv[0]);
    }

    printf("\n---[ STAGE 1 ]---\n");
    // Create default configuration file
    const char* configPath = "."PS"build"PS"config.h";
    FILE* configFile = fopen(configPath, "r");
    if (!configFile) {
        configFile = fopen(configPath, "w");
        fprintf(configFile, "#define PLATFORM "DEFAULT_PLATFORM);
    }
    fclose(configFile);

    system(COMPILER" build.c -o ."PS"build"PS"build_with_config -DWITH_CONFIG");
    system("."PS"build"PS"build_with_config");
}

#endif // WITH_CONFIG

void make_build_directory() {
    const char* buildPath = "."PS"build";
    DIR* dir = opendir(buildPath);
    if (dir) {
        closedir(dir);
    }
    else {
    #ifdef _WIN32
        CreateDirectory(buildPath, NULL);
    #else 
        mkdir(buildPath, 0700); 
    #endif // _WIN32
    }
}

void rebuild(char *executableName) {
    if (get_file_edit_time("build.c") > get_file_edit_time(executableName)) {
        file_rename(executableName, "."PS"build"PS"build.old");
        system(COMPILER" build.c -o build");
        system("."PS"build");
        exit(0);
    }
}

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
