#ifndef CAT_H
#define CAT_H

#include <getopt.h>
#include <stdio.h>

#define NOFILE 0

typedef struct Option_flags {
  int opt_b;
  int opt_e;
  int opt_n;
  int opt_s;
  int opt_t;
  int opt_v;
} Option_flags;

void work_with_arguments(int argc, char **argv);
int get_flags(int argc, char **argv, Option_flags *opt_flags);
int get_file_index(int argc, char **argv);
void print_file(Option_flags flags, int file_index, int argc, char **argv);
void print_file_with_flags(FILE *file, Option_flags *flags);

#endif