#ifndef GREP_H
#define GREP_H

#include <getopt.h>  // getopt_long()
#include <regex.h>
#include <stdio.h>   // fgets()
#include <stdlib.h>  // calloc(), realloc()
#include <string.h>  // strlen(), strcpy()

#define INIT_MEMORY 5
#define ADD_MEMORY 5
#define BUF_SIZE 5000

#define FL cnfgs->flags

typedef struct Option_flags {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} Option_flags;

typedef struct Patterns {
  int pat_count;
  char **data;
  int memory_count;
} Patterns;

typedef struct Grep_cnfgs {
  Option_flags *flags;
  Patterns *pat;
  int count_files;
} Grep_cnfgs;

void grep_start(int argc, char **argv);
void free_memory(Grep_cnfgs *cnfgs);
Grep_cnfgs *init_grep_cnfgs();
int parser(int argc, char **argv, Grep_cnfgs *cnfgs, int *file);
void get_pattern(char *optarg, Patterns *e_opt);
void add_memory(Patterns *pat);
int get_patterns_from_file(char *filename, Patterns *pat);
void grep_main(char *filename, Grep_cnfgs *cnfgs);
int compile_regexp(regex_t *reg, Grep_cnfgs *cnfgs);
void print_result(regex_t *regexp, Grep_cnfgs *cnfgs, FILE *fp, char *filename);
void opt_o_handling(regmatch_t *pmatch, regex_t *regexp, char *temp);
void opt_l_handling(char *filename, int match_lines);
void opt_c_handling(Grep_cnfgs *cnfgs, char *filename, int match_lines);

#endif