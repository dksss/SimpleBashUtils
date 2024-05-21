#include "cat_functions.h"

void work_with_arguments(int argc, char **argv) {
  Option_flags flags = {0};
  int check;

  check = get_flags(argc, argv, &flags);

  if (check) {
    int file_index = get_file_index(argc, argv);
    if (file_index) {
      print_file(flags, file_index, argc, argv);
    } else
      fprintf(stderr, "ERROR: No files\n");
  } else
    fprintf(stderr, "usage: ./s21_cat [-benstvTE] [file ...]\n");
}

int get_flags(int argc, char **argv, Option_flags *opt_flags) {
  struct option long_options[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                  {"number", no_argument, NULL, 'n'},
                                  {"squeeze-blank", no_argument, NULL, 's'},
                                  {0, 0, 0, 0}};

  int check_flags = 1;
  char flag;

  while ((flag = getopt_long(argc, argv, "+benstvET", long_options, NULL)) !=
         -1) {
    switch (flag) {
      case 'b':
        opt_flags->opt_b = 1;
        break;
      case 'e':
        opt_flags->opt_e = 1;
        opt_flags->opt_v = 1;
        break;
      case 'n':
        opt_flags->opt_n = 1;
        break;
      case 's':
        opt_flags->opt_s = 1;
        break;
      case 't':
        opt_flags->opt_t = 1;
        opt_flags->opt_v = 1;
        break;
      case 'v':
        opt_flags->opt_v = 1;
        break;
      case 'E':
        opt_flags->opt_e = 1;
        opt_flags->opt_v = 0;
        break;
      case 'T':
        opt_flags->opt_t = 1;
        opt_flags->opt_v = 0;
        break;
      default:
        check_flags = 0;
        break;
    }
  }
  return check_flags;
}

int get_file_index(int argc, char **argv) {
  int index = NOFILE;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      index = i;
      break;
    }
  }

  return index;
}

void print_file(Option_flags flags, int file_index, int argc, char **argv) {
  FILE *fp = NULL;

  while (file_index < argc) {
    if ((fp = fopen(argv[file_index], "r")) != NULL) {
      print_file_with_flags(fp, &flags);
      fclose(fp);
    } else
      fprintf(stderr, "./s21_cat: %s: No such file or directory\n",
              argv[file_index]);

    file_index++;
  }
}

void print_file_with_flags(FILE *file, Option_flags *flags) {
  int cur_ch = '\n';
  int past_ch = '\n';

  int lines_counter = 1;
  int line_status = 1;
  int squeeze_counter = 0;

  while ((cur_ch = fgetc(file)) != EOF) {
    if (flags->opt_s && past_ch == '\n' && cur_ch == '\n')
      squeeze_counter++;
    else
      squeeze_counter = 0;

    if (squeeze_counter < 2) {
      if ((flags->opt_n && flags->opt_b == 0 && line_status == 1) ||
          (flags->opt_b && line_status == 1 && cur_ch != '\n')) {
        printf("%6d\t", lines_counter++);
        line_status = 0;
      }
      if (flags->opt_t && cur_ch == '\t') {
        printf("^");
        cur_ch = 'I';
      }
      if (flags->opt_e && cur_ch == '\n') printf("$");
      if (flags->opt_v && ((cur_ch < 32 && cur_ch != '\n' && cur_ch != '\t') ||
                           cur_ch == 127)) {
        printf("^");
      }
      if ((cur_ch < 32 || (cur_ch > 126 && cur_ch < 160)) && cur_ch != '\n' &&
          cur_ch != '\t') {
        cur_ch = (cur_ch > 126 ? cur_ch - 64 : cur_ch + 64);
      }
      printf("%c", cur_ch);
      past_ch = cur_ch;
      line_status = (cur_ch == '\n' ? 1 : 0);
    }
  }
}