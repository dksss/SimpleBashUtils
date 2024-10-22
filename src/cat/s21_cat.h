#ifndef SIMPLE_BASH_UTILS_CAT_S21_CAT_H_
#define SIMPLE_BASH_UTILS_CAT_S21_CAT_H_

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define S21_NEW_LINE 1
#define S21_SAME_LINE 0

#define S21_NL_CHAR 10
#define S21_TAB 9

enum S21_Error_Flag { kNo_Error, kInvalid_Key, kInvalid_Files };

typedef struct {
  int flag_b;
  int flag_e;
  int flag_n;
  int flag_s;
  int flag_t;
  int flag_v;
} S21_Options_Flags;

int handling_keys(S21_Options_Flags* const options, int argc, char** argv);
int parsing_keys(S21_Options_Flags* const options, int value);
void handling_data(int argc, char** argv, S21_Options_Flags* const opt);
int get_file_index(int argc, char** argv, int* index);
void print_data_from_file(int argc, char** argv, S21_Options_Flags* const opt,
                          int index);
void print_data_with_keys(FILE* const filename, S21_Options_Flags* const opt);

// keys processing
int check_squeeze_lines(int cur, int prev, int* counter);
void print_line_count(int cur, int* counter, int new_line,
                      const S21_Options_Flags* const opt);
void processing_opt_t(int* cur);
void processing_opt_e(int cur);
void processing_opt_v(int* cur);

#endif  // SIMPLE_BASH_UTILS_CAT_S21_CAT_H_