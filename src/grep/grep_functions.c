#include "./s21_grep.h"

int handling_arguments(S21_Util_Config* conf, int argc, char** argv) {
  int error_status = kNo_Error;

  error_status = init_config(conf);

  if (error_status == kNo_Error) {
    error_status = setup_config(conf, argc, argv);
  } else if (error_status == kMemory_Error) {
    fprintf(stderr, "%s\n", S21_MEM_ERR_MSG);
  }

  return error_status;
}

void handling_data(S21_Util_Config* conf, int argc, char** argv) {
  regex_t* regexp = NULL;
  int compiled_regexprs = 0;

  regexp = (regex_t*)calloc(conf->pat->paterns_count, sizeof(regex_t));
  int error_status = compile_regexps(regexp, &compiled_regexprs, conf);

  if (error_status == kNo_Error) {
    print_data(conf, regexp, argc, argv);
  }

  free_regexps(regexp, compiled_regexprs);
}

// ===================================================
// =                Work with memory                 =
// ===================================================

int init_config(S21_Util_Config* const conf) {
  int error_status = kNo_Error;

  conf->opt = (S21_Options_Flags*)calloc(1, sizeof(S21_Options_Flags));
  conf->pat = (S21_Patterns*)calloc(1, sizeof(S21_Patterns));

  if (conf->opt != NULL && conf->pat != NULL) {
    error_status = init_patterns(conf->pat);
  } else {
    error_status = kMemory_Error;
  }

  return error_status;
}

int init_patterns(S21_Patterns* const patterns) {
  patterns->pat_memory_cap = S21_INIT_PATTERN_MEMORY;
  patterns->data = (char**)calloc(patterns->pat_memory_cap, sizeof(char*));

  return ((patterns->data != NULL) ? kNo_Error : kMemory_Error);
}

int add_patterns_memory(S21_Patterns* const patterns) {
  patterns->pat_memory_cap += S21_ADD_PATTERN_MEMORY;
  patterns->data = (char**)realloc(patterns->data, patterns->pat_memory_cap);

  return ((patterns->data != NULL) ? kNo_Error : kMemory_Error);
}

void free_configs(S21_Util_Config* const conf) {
  free_options(conf->opt);
  free_patterns(conf->pat);
}

void free_options(S21_Options_Flags* options) {
  if (options != NULL) {
    free(options);
    options = NULL;
  }
}

void free_patterns(S21_Patterns* patterns) {
  if (patterns != NULL) {
    for (int i = 0; i < patterns->paterns_count; ++i) {
      free(patterns->data[i]);
    }

    free(patterns->data);
    free(patterns);
    patterns = NULL;
  }
}

void free_regexps(regex_t* regexp, int size) {
  if (regexp != NULL) {
    for (int i = 0; i < size; ++i) {
      regfree(&regexp[i]);
    }

    free(regexp);
  }
}

// ===================================================
// =           Parsing and setup configs             =
// ===================================================

int setup_config(S21_Util_Config* const conf, int argc, char** argv) {
  int error_status = kNo_Error;

  error_status = parsing_keys(conf, argc, argv);

  if (error_status == kNo_Error) {
    if (!conf->opt->flag_e && !conf->opt->flag_f) {
      error_status = get_pattern(conf->pat, argv[optind++]);
    }

    conf->count_files = argc - optind;
  }

  return error_status;
}

int parsing_keys(S21_Util_Config* const conf, int argc, char** argv) {
  int error_status = kNo_Error;

  const char* short_flags = "+ivclnhsoe:f:";

  int value = 0;
  while (error_status == kNo_Error &&
         (-1 != (value = getopt_long(argc, argv, short_flags, NULL, NULL)))) {
    error_status = set_key(conf, value);
  }

  return error_status;
}

int set_key(S21_Util_Config* const conf, int value) {
  int error_status = kNo_Error;

  opterr = 0;
  switch (value) {
    case 'i':
      conf->opt->flag_i = 1;
      break;
    case 'v':
      conf->opt->flag_v = 1;
      break;
    case 'c':
      conf->opt->flag_c = 1;
      break;
    case 'l':
      conf->opt->flag_l = 1;
      break;
    case 'n':
      conf->opt->flag_n = 1;
      break;
    case 'h':
      conf->opt->flag_h = 1;
      break;
    case 's':
      conf->opt->flag_s = 1;
      break;
    case 'o':
      conf->opt->flag_o = 1;
      break;
    case 'e':
      conf->opt->flag_e = 1;
      error_status = get_pattern(conf->pat, optarg);
      break;
    case 'f':
      conf->opt->flag_f = 1;
      error_status = get_patterns_from_file(conf->pat, optarg);
      break;
    case '?':
    default:
      error_status = kInvalid_Keys;
      if (!conf->opt->flag_s) {
        fprintf(stderr, "%s%c\n", S21_INVALID_KEY_ERR_MSG, optopt);
      }
  }

  return error_status;
}

int get_pattern(S21_Patterns* const pattern, const char* str) {
  int error_status = kNo_Error;

  if (pattern->paterns_count == pattern->pat_memory_cap) {
    error_status = add_patterns_memory(pattern);
  }

  if (!error_status) {
    pattern->data[pattern->paterns_count] =
        (char*)calloc(strlen(str) + 1, sizeof(char));
    strcpy(pattern->data[pattern->paterns_count++], str);
  } else if (error_status == kMemory_Error) {
    fprintf(stderr, "%s\n", S21_MEM_ERR_MSG);
  }

  return error_status;
}

int get_patterns_from_file(S21_Patterns* const conf, const char* filename) {
  int error_status = kNo_Error;

  FILE* fp = fopen(filename, "r");
  if (fp != NULL) {
    char* tmp = calloc(S21_BUF_SIZE, sizeof(char));
    get_data_from_file(tmp, fp);
    error_status = get_pattern(conf, tmp);
    free(tmp);
    fclose(fp);
  } else {
    error_status = kInvalid_Files;
    fprintf(stderr, "%s: %s: %s\n", "s21_grep", filename,
            S21_FILE_OPEN_ERROR_MSG);
  }

  return error_status;
}

void get_data_from_file(char* str, FILE* const fp) {
  int size = 0;
  int capacity = S21_BUF_SIZE;

  char ch = 0;
  while ((ch = fgetc(fp)) != EOF) {
    str[size++] = ch;

    if (size == capacity) {
      capacity += S21_BUF_SIZE;
      str = (char*)realloc(str, capacity);
    }
  }
  str[size] = '\0';  //* Для профилактики из-за реаллока
}

// ===================================================
// =               Work with regexps                 =
// ===================================================

int compile_regexps(regex_t* regexp, int* regexp_count, S21_Util_Config* conf) {
  int error_status = kNo_Error, error_flag = kNo_Error;
  int compile_flag = 0;

  if (conf->opt->flag_e) {
    compile_flag |= REG_EXTENDED;
  }
  if (conf->opt->flag_i) {
    compile_flag |= REG_ICASE;
  }

  for (int i = 0; i < conf->pat->paterns_count && error_status == kNo_Error;
       ++i, ++(*regexp_count)) {
    error_flag = regcomp(&regexp[i], conf->pat->data[i], compile_flag);

    if (error_flag != 0) {
      error_status = kRegexp_Compile_Error;
      print_regexp_error(error_flag, &regexp[i]);
    }
  }

  return error_status;
}

void print_regexp_error(int error_code, const regex_t* reg) {
  char error_msg[S21_BUF_SIZE] = {0};
  int error_len = regerror(error_code, reg, error_msg, sizeof(error_msg));

  fprintf(stderr, "s21_grep: regexp error: %.*s\n", error_len, error_msg);
}

// ===================================================
// =                  Print data                     =
// ===================================================

void print_data(const S21_Util_Config* const conf, const regex_t* const regexp,
                int argc, char** argv) {
  if (conf->count_files != 0) {
    print_data_from_files(conf, regexp, argc, argv);
  } else {
    print_data_with_configs(conf, regexp, stdin, "(standard input)");
  }
}

void print_data_from_files(const S21_Util_Config* const conf,
                           const regex_t* const regexp, int argc, char** argv) {
  int error_status = kNo_Error;

  FILE* fp = NULL;
  for (int i = optind; i < argc && !error_status; ++i) {
    fp = fopen(argv[i], "r");

    if (fp != NULL) {
      print_data_with_configs(conf, regexp, fp, argv[i]);
      fclose(fp);
    } else {
      if (!conf->opt->flag_s) {
        fprintf(stderr, "s21_grep: %s: %s\n", argv[i], S21_FILE_OPEN_ERROR_MSG);
      }
    }
  }
}

void print_data_with_configs(const S21_Util_Config* const conf,
                             const regex_t* const regexp, FILE* fp,
                             const char* filename) {
  if (conf->opt->flag_l) {
    print_filenames_with_matches(conf, regexp, fp, filename);
  } else if (conf->opt->flag_c) {
    print_counted_matches(conf, regexp, fp, filename);
  } else if (conf->opt->flag_o) {
    print_matching_data(conf, regexp, fp, filename);
  } else {
    print_matching_data_lines(conf, regexp, fp, filename);
  }
}

void print_filenames_with_matches(const S21_Util_Config* const conf,
                                  const regex_t* const regexp, FILE* fp,
                                  const char* filename) {
  char* tmp = (char*)calloc(S21_BUF_SIZE * 4, sizeof(char));

  while (fgets(tmp, S21_BUF_SIZE * 4, fp) != NULL) {
    if (is_match(tmp, conf, regexp, NULL)) {
      printf("%s\n", filename);
      break;
    }
  }

  free(tmp);
}

void print_counted_matches(const S21_Util_Config* const conf,
                           const regex_t* const regexp, FILE* fp,
                           const char* filename) {
  char* tmp = (char*)calloc(S21_BUF_SIZE * 4, sizeof(char));
  int counted_lines = 0;

  while (fgets(tmp, S21_BUF_SIZE * 4, fp) != NULL) {
    if (is_match(tmp, conf, regexp, NULL)) {
      ++counted_lines;
    }
  }

  if (conf->count_files > 1 && !conf->opt->flag_h) {
    printf("%s:", filename);
  }

  printf("%d\n", counted_lines);
  free(tmp);
}

void print_matching_data_lines(const S21_Util_Config* const conf,
                               const regex_t* const regexp, FILE* fp,
                               const char* filename) {
  char* tmp = (char*)calloc(S21_BUF_SIZE * 4, sizeof(char));
  int lines_count = 0;

  while (fgets(tmp, S21_BUF_SIZE * 4, fp) != NULL) {
    ++lines_count;

    if (is_match(tmp, conf, regexp, NULL)) {
      if (conf->count_files > 1 && !conf->opt->flag_h) {
        printf("%s:", filename);
      }

      if (conf->opt->flag_n) {
        printf("%d:", lines_count);
      }

      if (feof(fp)) {
        printf("%s\n", tmp);
      } else {
        printf("%s", tmp);
      }
    }
  }

  free(tmp);
}

void print_matching_data(const S21_Util_Config* const conf,
                         const regex_t* const regexp, FILE* fp,
                         const char* filename) {
  char* tmp = (char*)calloc(S21_BUF_SIZE * 4, sizeof(char));
  int lines_count = 0;
  regmatch_t match = {0};

  while (fgets(tmp, S21_BUF_SIZE * 4, fp) != NULL) {
    ++lines_count;
    char* current_line = tmp;

    while (is_match(current_line, conf, regexp, &match)) {
      if (conf->count_files > 1 && !conf->opt->flag_h) {
        printf("%s:", filename);
      }

      if (conf->opt->flag_n) {
        printf("%d:", lines_count);
      }

      printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
             current_line + match.rm_so);
      current_line += match.rm_eo;
    }
  }

  free(tmp);
}

int is_match(const char* str, const S21_Util_Config* const conf,
             const regex_t* const regexp, regmatch_t* const match_strstr) {
  int match_status = 0;
  int nmatch = (match_strstr == NULL ? 0 : 1);

  for (int i = 0; i < conf->pat->paterns_count; ++i) {
    if (regexec(&regexp[i], str, nmatch, match_strstr, 0) == 0) {
      match_status = 1;
    }
  }

  if (conf->opt->flag_v) {
    match_status = !match_status;
  }

  if (conf->opt->flag_o && conf->opt->flag_v) {
    match_status = 0;
  }

  return match_status;
}