#include "./s21_cat.h"

int handling_keys(S21_Options_Flags* const options, int argc, char** argv) {
  int keys_status = kNo_Error;

  const char* opts = "+benstvET";
  const struct option opts_l[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                  {"number", no_argument, NULL, 'n'},
                                  {"squeeze-blank", no_argument, NULL, 's'},
                                  {0, 0, 0, 0}};

  int val;
  while (keys_status == kNo_Error &&
         (-1 != (val = getopt_long(argc, argv, opts, opts_l, NULL)))) {
    keys_status = parsing_keys(options, val);
  }

  return keys_status;
}

int parsing_keys(S21_Options_Flags* const options, int value) {
  int key_status = kNo_Error;

  opterr = 0;
  switch (value) {
    case 'b':
      options->flag_b = 1;
      break;
    case 'n':
      options->flag_n = 1;
      break;
    case 's':
      options->flag_s = 1;
      break;
    case 't':
      options->flag_t = 1;
      options->flag_v = 1;
      break;
    case 'e':
      options->flag_e = 1;
      options->flag_v = 1;
      break;
    case 'v':
      options->flag_v = 1;
      break;
    case 'E':
      options->flag_e = 1;
      break;
    case 'T':
      options->flag_t = 1;
      break;
    case '?':
    default:
      key_status = kInvalid_Key;
      fprintf(stderr, "s21_cat: invalid option -- '%c'\n", optopt);
      break;
  }

  return key_status;
}

void handling_data(int argc, char** argv, S21_Options_Flags* const opt) {
  int file_index = -1;
  int files_status = get_file_index(argc, argv, &file_index);

  if (files_status == kNo_Error) {
    print_data_from_file(argc, argv, opt, file_index);
  } else {
    print_data_with_keys(stdin, opt);
  }
}

int get_file_index(int argc, char** argv, int* index) {
  int status = kInvalid_Files;

  for (int i = 1; i < argc && status == kInvalid_Files; ++i) {
    if (argv[i][0] != '-') {
      status = kNo_Error;
      *index = i;
    }
  }

  return status;
}

void print_data_from_file(int argc, char** argv, S21_Options_Flags* const opt,
                          int index) {
  for (; index < argc; ++index) {
    FILE* fp = fopen(argv[index], "r");

    if (fp == NULL) {
      fprintf(stderr, "s21_cat: %s: No such file ordirectory\n", argv[index]);
    } else {
      print_data_with_keys(fp, opt);
      fclose(fp);
    }
  }
}

void print_data_with_keys(FILE* const filename, S21_Options_Flags* const opt) {
  int cur_ch = S21_NL_CHAR, past_ch = S21_NL_CHAR;
  int line_status = S21_NEW_LINE;
  static int lines_counter = 1;
  int squeeze_counter = 0;

  if (opt->flag_b) {
    opt->flag_n = 0;
  }

  while ((cur_ch = fgetc(filename)) != EOF) {
    if (opt->flag_s) {
      if (check_squeeze_lines(cur_ch, past_ch, &squeeze_counter)) {
        continue;
      }
    }

    if (opt->flag_b || opt->flag_n) {
      print_line_count(cur_ch, &lines_counter, line_status, opt);
    }

    if (opt->flag_t) {
      processing_opt_t(&cur_ch);
    }

    if (opt->flag_e) {
      processing_opt_e(cur_ch);
    }

    if (opt->flag_v) {
      processing_opt_v(&cur_ch);
    }

    printf("%c", cur_ch);
    past_ch = cur_ch;
    line_status = (cur_ch == S21_NL_CHAR ? S21_NEW_LINE : S21_SAME_LINE);
  }
}

int check_squeeze_lines(int cur, int prev, int* counter) {
  if (cur == S21_NL_CHAR && prev == S21_NL_CHAR) {
    *counter += 1;
  } else {
    *counter = 0;
  }

  return (*counter > 1 ? 1 : 0);
}

void print_line_count(int cur, int* counter, int new_line,
                      const S21_Options_Flags* const opt) {
  if (opt->flag_b) {
    if (cur != S21_NL_CHAR && new_line) {
      printf("%6d\t", (*counter)++);
    }
  } else {
    if (new_line) {
      printf("%6d\t", (*counter)++);
    }
  }
}

void processing_opt_t(int* cur) {
  if (*cur == S21_TAB) {
    printf("^");
    *cur = 'I';
  }
}

void processing_opt_e(int cur) {
  if (cur == S21_NL_CHAR) {
    printf("$");
  }
}

void processing_opt_v(int* cur) {
  if ((*cur >= 0 && *cur < 9) || (*cur > 10 && *cur < 32)) {
    *cur += 64;
    printf("^");
  } else if (*cur == 127) {
    *cur = 63;
    printf("^");
  }
}