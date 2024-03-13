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
#       if defined(__clang__)
#           define COMPILER "clang"
#       elif defined(__GNUC__)
#           define COMPILER "gcc"
#       elif defined(_MSC_VER)
#           define COMPILER "cl.exe"
#       else
#           define COMPILER "cc"
#       endif
#   else
#       if defined(__clang__)
#           define COMPILER "clang"
#       elif defined(__GNUC__)
#           define COMPILER "gcc"
#       else
#           define COMPILER "cc"
#       endif
#   endif // _WIN32
#endif // COMPILER

#define WINDOWS 0
#define LINUX 1

const char* PLATFORM_NAMES[2] = {
    "Windows", 
    "Linux"
};

#ifdef _WIN32
#define DEFAULT_PLATFORM "WINDOWS"
#endif // _WIN32
#ifdef __linux__
#define DEFAULT_PLATFORM "LINUX"
#endif // _WIN32
#define DEFAULT_OPT_LEVEL "-O0"

void make_directory(const char *path);
void recompile(char *executableName);
int get_file_edit_time(const char *file);
void file_rename(const char *old, const char *new);
void file_copy(const char *from, const char *to);

void cmd_execute(const char *command) {
    printf("[cmd] %s\n", command);
    system(command);
}

#define BUILD_DIR "."PS"build"PS

#ifdef WITH_CONFIG
#include "build/config.h"

#define SOURCE \
    "src/nstring.c " \
    "src/nmemory.c "

int main(int argc, char **argv) {
    printf("\n---[ BUILD ]---\n");
    printf("Configuration:\n");
    printf("Platform: %s\n", PLATFORM_NAMES[PLATFORM]);
    printf("Optimization: %s\n", OPT_LEVEL);

    make_directory(BUILD_DIR"bin");
    make_directory(BUILD_DIR"include");

    cmd_execute(COMPILER" -shared -Wall "SOURCE" "OPT_LEVEL" -o "BUILD_DIR"bin"PS"nandi.dll");
    file_copy("."PS"src"PS"nandi.h", BUILD_DIR"include"PS"nandi.h");
}

#else

int main(int argc, char **argv) {
    make_directory(BUILD_DIR);
    if (argc > 0) {
        recompile(argv[0]);
    }

    // Create default configuration file
    const char* configPath = BUILD_DIR"config.h";
    FILE* configFile = fopen(configPath, "r");
    if (!configFile) {
        printf("Generating default '%s' file...\n", configPath);
        configFile = fopen(configPath, "w");
        fprintf(configFile, "#define PLATFORM "DEFAULT_PLATFORM"\n");
        fprintf(configFile, "#define OPT_LEVEL \""DEFAULT_OPT_LEVEL"\"\n");
    }
    fclose(configFile);
    printf("All setup done\n");

    cmd_execute(COMPILER" -Wall build.c -o "BUILD_DIR"build_with_config -DWITH_CONFIG");
    cmd_execute(BUILD_DIR"build_with_config");
}

#endif // WITH_CONFIG

void make_directory(const char *path) {
    DIR* dir = opendir(path);
    if (dir) {
        closedir(dir);
    }
    else {
        printf("Creating '%s' directory...\n", path);
    #ifdef _WIN32
        CreateDirectory(path, NULL);
    #else 
        mkdir(path, 0700); 
    #endif // _WIN32
    }
}

void recompile(char *executableName) {
    if (get_file_edit_time("build.c") > get_file_edit_time(executableName)) {
        printf("Recompiling 'build.c'...\n");
        file_rename(executableName, BUILD_DIR"build.old");
        cmd_execute(COMPILER" -Wall build.c -o build");
        cmd_execute("."PS"build");
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

void file_copy(const char *src, const char *dst) {
    FILE* srcFile = fopen(src, "r");
    FILE* dstFile = fopen(dst, "w");

    if (srcFile) {
        fseek(srcFile, 0, SEEK_END);
        long length = ftell(srcFile);
        fseek(srcFile, 0, SEEK_SET);
        char *buffer = malloc(length);
        if (buffer) {
            fread(buffer, 1, length, srcFile);
        }
        fwrite(buffer, 1, length, dstFile);
        free(buffer);
    }

    fclose(srcFile);
    fclose(dstFile);
}

