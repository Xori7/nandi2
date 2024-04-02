#define SOURCE_FILES \
    SOURCE_X(nstring) \
    SOURCE_X(nmemory) \
    SOURCE_X(nthreading_windows) \
    SOURCE_X(nlog) \
    SOURCE_X(ntest) \
    SOURCE_X(nwindow_windows) \
    SOURCE_X(ninput_windows) \

// Path separator
#ifdef _WIN32
#define PS "\\"
#else
#define PS "/"
#endif // _WIN32

#define BUILD_DIR "."PS"build"PS

#define SOURCE_X(s) "src/"#s".c "
const char *SOURCE = SOURCE_FILES;
#undef SOURCE_X

#define SOURCE_X(s) BUILD_DIR"bin/"#s".o "
const char *OBJECT = SOURCE_FILES;
#undef SOURCE_X

const char *TEST_C = 
"#include \"include/nandi.h\"\n"

"#define TEST_INCLUDE\n"
#define SOURCE_X(s) "#include \"../src/"#s".c\"\n"
SOURCE_FILES
#undef SOURCE_X
"#undef TEST_INCLUDE\n"

"int main(int argc, char **argv) {\n"
"n_log_register_console_handler();\n"
"#define TEST_IMPL\n"
#define SOURCE_X(s) "#include \"../src/"#s".c\"\n"
SOURCE_FILES
#undef SOURCE_X
"#undef TEST_IMPL\n"

"return 0;\n"
"}";

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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>

#ifdef _WIN32
#include <windows.h>
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

void make_directory(const char *path);
void recompile(char *executableName);
int get_file_edit_time(const char *file);
void file_rename(const char *old, const char *new);
void file_copy(const char *from, const char *to);
char *string_format(const char *format, ...);

void cmd_execute(const char *command) {
    printf("[cmd] %s\n", command);
    system(command);
}

#ifdef WITH_CONFIG
#include "build/config.h"

int main(int argc, char **argv) {
    printf("\n---[ BUILD ]---\n");
    printf("Configuration:\n");
    printf("Platform: %s\n", PLATFORM_NAMES[PLATFORM]);
    printf("Optimization: %s\n", OPT_LEVEL);
#ifdef TEST 
    printf("Test: Yes\n");
#else
    printf("Test: No\n");
#endif // TEST

    make_directory(BUILD_DIR"bin");
    make_directory(BUILD_DIR"include");

#define SOURCE_X(s) if (get_file_edit_time("src/"#s".c") > get_file_edit_time("build/bin/"#s".o") || get_file_edit_time("src/nandi.h") > get_file_edit_time("build/bin/"#s".o")) { \
cmd_execute(string_format(COMPILER" -Wall src/"#s".c "OPT_LEVEL" -c -o ./build/bin/"#s".o")); \
cmd_execute(string_format(COMPILER" -Wall src/"#s".c "OPT_LEVEL" -S -o ./build/bin/"#s".nasm")); \
}

SOURCE_FILES
#undef SOURCE_X
    cmd_execute(string_format(COMPILER" %s -lcomctl32 -shared -o %s", OBJECT, BUILD_DIR"bin"PS"nandi.dll"));
    file_copy("."PS"src"PS"nandi.h", BUILD_DIR"include"PS"nandi.h");

#ifdef TEST
    FILE* testFile = fopen(BUILD_DIR"test.c", "w");
    fprintf(testFile, "%s", TEST_C);
    fclose(testFile);
    cmd_execute(string_format(COMPILER" -Wall %s %s -DTEST_BUILD -L"BUILD_DIR"bin -lnandi -o %s", BUILD_DIR"test.c", OPT_LEVEL, BUILD_DIR"bin"PS"test"));
    cmd_execute(BUILD_DIR"bin"PS"test");
#endif // TEST
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
        fprintf(configFile, "#define TEST\n");
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
        char c = fgetc(srcFile); 
        while (c != EOF) 
        { 
            fputc(c, dstFile); 
            c = fgetc(srcFile); 
        } 
    }

    fclose(srcFile);
    fclose(dstFile);
}

char *string_format(const char *format, ...) {
    va_list args;
    size_t length;
    char *space;

    va_start(args, format);
    length = vsnprintf(NULL, 0, format, args);
    if ((space = malloc(length + 1)) != NULL) {
        vsnprintf(space, length + 1, format, args);
        return space;
    }
    va_end(args);
    return NULL;
}

