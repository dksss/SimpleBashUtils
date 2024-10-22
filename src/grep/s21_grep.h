#ifndef SIMPLE_BASH_UTILS_GREP_S21_GREP_H
#define SIMPLE_BASH_UTILS_GREP_S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define S21_MEM_ERR_MSG "Bad alloc"
#define S21_FILE_OPEN_ERROR_MSG "No such file or directory"
#define S21_USAGE_MSG "Usage: s21_grep [OPTION]... PATTERNS [FILE]..."
#define S21_INVALID_KEY_ERR_MSG "s21_grep: invalid option -- "

#define S21_INIT_PATTERN_MEMORY 10
#define S21_ADD_PATTERN_MEMORY 5

#define S21_BUF_SIZE 1024

enum S21_Error_Flag {
  kNo_Error,
  kInvalid_Keys,
  kInvalid_Files,
  kMemory_Error,
  kRegexp_Compile_Error
};

typedef struct {
  int flag_e;
  int flag_i;
  int flag_v;
  int flag_c;
  int flag_l;
  int flag_n;
  int flag_h;
  int flag_s;
  int flag_f;
  int flag_o;
} S21_Options_Flags;

typedef struct {
  int paterns_count;
  int pat_memory_cap;
  char** data;
} S21_Patterns;

typedef struct {
  S21_Options_Flags* opt;
  S21_Patterns* pat;
  int count_files;
} S21_Util_Config;

int handling_arguments(S21_Util_Config* conf, int argc, char** argv);
void handling_data(S21_Util_Config* conf, int argc, char** argv);

int init_config(S21_Util_Config* const conf);
int init_patterns(S21_Patterns* const patterns);
int add_patterns_memory(S21_Patterns* const patterns);
void free_configs(S21_Util_Config* const conf);
void free_options(S21_Options_Flags* options);
void free_patterns(S21_Patterns* patterns);
void free_regexps(regex_t* regex, int size);

int setup_config(S21_Util_Config* const conf, int argc, char** argv);
int parsing_keys(S21_Util_Config* const conf, int argc, char** argv);
int set_key(S21_Util_Config* const conf, int value);
int get_pattern(S21_Patterns* const pattern, const char* str);
int get_patterns_from_file(S21_Patterns* const conf, const char* filename);
void get_data_from_file(char* str, FILE* const fp);

int compile_regexps(regex_t* regexp, int* regexp_count, S21_Util_Config* conf);
void print_regexp_error(int error_code, const regex_t* reg);

void print_data(const S21_Util_Config* const conf, const regex_t* const regexp,
                int argc, char** argv);
void print_data_from_files(const S21_Util_Config* const conf,
                           const regex_t* const regexp, int argc, char** argv);
void print_data_with_configs(const S21_Util_Config* const conf,
                             const regex_t* const regexp, FILE* fp,
                             const char* filename);
void print_filenames_with_matches(const S21_Util_Config* const conf,
                                  const regex_t* const regexp, FILE* fp,
                                  const char* filename);
void print_counted_matches(const S21_Util_Config* const conf,
                           const regex_t* const regexp, FILE* fp,
                           const char* filename);
void print_matching_data(const S21_Util_Config* const conf,
                         const regex_t* const regexp, FILE* fp,
                         const char* filename);
void print_matching_data_lines(const S21_Util_Config* const conf,
                               const regex_t* const regexp, FILE* fp,
                               const char* filename);
int is_match(const char* str, const S21_Util_Config* const conf,
             const regex_t* const regexp, regmatch_t* const match_strstr);

#endif  // SIMPLE_BASH_UTILS_GREP_S21_GREP_H