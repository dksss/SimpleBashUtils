#include "grep_functions.h"



Grep_cnfgs *init_grep_cnfgs() {
  Grep_cnfgs *cnfgs = (Grep_cnfgs *)calloc(1, sizeof(Grep_cnfgs));

  Option_flags *flags = (Option_flags *)calloc(1, sizeof(Option_flags));

  Patterns *pat = (Patterns *)calloc(1, sizeof(Patterns));
  pat->pat_count = 0;
  pat->data = (char **)calloc(
      INIT_MEMORY,
      sizeof(char *) * INIT_MEMORY);  // Выделяем память под 5 элементов
  if (pat->data != NULL)
    pat->memory_count = INIT_MEMORY;
  else
    fprintf(stderr, "Memory allocation error\n");

  cnfgs->flags = flags;
  cnfgs->pat = pat;
  cnfgs->count_files = 0;

  return cnfgs;
}

void free_memory(Grep_cnfgs *cnfgs) {
  for (int i = 0; i < cnfgs->pat->pat_count; i++) free(cnfgs->pat->data[i]);

  free(cnfgs->pat->data);
  free(cnfgs->pat);
  free(cnfgs->flags);
  free(cnfgs);
}

int parser(int argc, char **argv, Grep_cnfgs *cnfgs, int *file_err) {
  int err_check = 0;
  char ch;

  while ((ch = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1) {
    switch (ch) {
      case 'e':
        cnfgs->flags->e = 1;
        get_pattern(optarg, cnfgs->pat);
        break;
      case 'i':
        cnfgs->flags->i = 1;
        break;
      case 'v':
        cnfgs->flags->v = 1;
        break;
      case 'c':
        cnfgs->flags->c = 1;
        break;
      case 'l':
        cnfgs->flags->l = 1;
        break;
      case 'n':
        cnfgs->flags->n = 1;
        break;
      case 'h':
        cnfgs->flags->h = 1;
        break;
      case 's':
        cnfgs->flags->s = 1;
        break;
      case 'f':
        cnfgs->flags->f = 1;
        *file_err = get_patterns_from_file(optarg, cnfgs->pat);
        break;
      case 'o':
        cnfgs->flags->o = 1;
        break;
      default:
        err_check = 1;
        break;
    }
  }
  if (!cnfgs->flags->e && !cnfgs->flags->f) {
    char pattern[BUF_SIZE] = {0};
    strcpy(pattern, argv[optind]);
    get_pattern(pattern, cnfgs->pat);
    optind++;
  }
  if (err_check != 1) cnfgs->count_files = argc - optind;
  if (cnfgs->count_files == 0) err_check = 1;

  return err_check;
}

void get_pattern(char *optarg, Patterns *pat) {
  int len = strlen(optarg);
  char *temp_arr = (char *)malloc(sizeof(char) * len + 1);
  strcpy(temp_arr, optarg);

  if (pat->pat_count == pat->memory_count) add_memory(pat);

  pat->data[pat->pat_count] = temp_arr;
  pat->pat_count++;
}

void add_memory(Patterns *pat) {
  char **temp_arr = (char **)realloc(
      pat->data, sizeof(char *) * (pat->memory_count + ADD_MEMORY));

  if (temp_arr != NULL) {
    for (int i = pat->memory_count; i < pat->memory_count + ADD_MEMORY; i++)
      temp_arr[i] = NULL;

    pat->data = temp_arr;
    pat->memory_count += ADD_MEMORY;
  } else
    fprintf(stderr, "Memory allocation error\n");
}

int get_patterns_from_file(char *filename, Patterns *pat) {
  int file_open_err = 0;
  FILE *fp = fopen(filename, "r");
  char buffer[BUF_SIZE] = {0};

  if (fp != NULL) {
    while (fgets(buffer, BUF_SIZE, fp) != NULL) get_pattern(buffer, pat);

    fclose(fp);
  } else {
    file_open_err = 1;
    fprintf(stderr, "./s21_grep: %s: No such file or directory\n", filename);
  }
  return file_open_err;
}

int compile_regexp(regex_t *reg, Grep_cnfgs *cnfgs) {
  int compile_err = 0;
  int compile_flag = 0;
  if (cnfgs->flags->e) compile_flag |= REG_EXTENDED;
  if (cnfgs->flags->i) compile_flag |= REG_ICASE;

  for (int i = 0; i < cnfgs->pat->pat_count; i++) {
    compile_err += regcomp(&reg[i], cnfgs->pat->data[i], compile_flag);
  }

  return compile_err;
}

void grep_start(int argc, char **argv) {
  Grep_cnfgs *cnfgs = init_grep_cnfgs();
  int file_err = 0;
  int err_check = parser(argc, argv, cnfgs, &file_err);

  if (err_check == 0 && file_err == 0) {
    for (int i = optind; i < argc; i++) grep_main(argv[i], cnfgs);
  } else if (err_check)
    fprintf(
        stderr,
        "usage: ./s21_grep [-ivclnhso] [-e pattern] [-f file] [file] ...]\n");

  free_memory(cnfgs);
}

void grep_main(char *filename, Grep_cnfgs *cnfgs) {
  FILE *fp = fopen(filename, "r");
  regex_t *regexp = malloc(sizeof(regex_t) * cnfgs->pat->pat_count);

  int compile_err = compile_regexp(regexp, cnfgs);

  if (fp == NULL) {
    if (!cnfgs->flags->s)
      fprintf(stderr, "./s21_grep: %s: No such file or directory\n", filename);
  } else if (compile_err != 0)
    fprintf(stderr, "./s21_grep: Regexp compile error\n");
  else {
    print_result(regexp, cnfgs, fp, filename);
    fclose(fp);
  }

  for (int i = 0; i < cnfgs->pat->pat_count; i++) regfree(&regexp[i]);
  free(regexp);
}

void print_result(regex_t *regexp, Grep_cnfgs *cnfgs, FILE *fp,
                  char *filename) {
  char temp[BUF_SIZE] = {0};
  int lines_count = 1;
  int match_lines = 0;
  regmatch_t pmatch;

  while (fgets(temp, BUF_SIZE, fp) != NULL) {
    for (int i = 0; i < cnfgs->pat->pat_count; i++) {
      int status = regexec(&regexp[i], temp, 1, &pmatch, 0);

      if ((status == 0 && !FL->v) || (status == REG_NOMATCH && FL->v)) {
        if (cnfgs->count_files > 1 && !FL->h && !FL->c && !FL->l)
          printf("%s:", filename);

        if (FL->l || FL->c) match_lines++;

        if (FL->n && !FL->c && !FL->l) printf("%d:", lines_count);

        if (FL->o && !FL->v)
          opt_o_handling(&pmatch, &regexp[i], temp);
        else if (FL->o && FL->v)
          printf("%s", temp);

        if (feof(fp) && !FL->c && !FL->l && !FL->o)
          printf("%s\n", temp);
        else if (!FL->c && !FL->l && !FL->o)
          printf("%s", temp);
      }
      lines_count++;
    }
  }
  if (FL->c) opt_c_handling(cnfgs, filename, match_lines);
  if (FL->l) opt_l_handling(filename, match_lines);
}

void opt_c_handling(Grep_cnfgs *cnfgs, char *filename, int match_lines) {
  if (FL->l && match_lines > 0) {
    if (!FL->h)
      printf("%s:1\n", filename);
    else
      printf("1\n");
  } else {
    if (cnfgs->count_files > 1 && !FL->h) {
      printf("%s:%d\n", filename, match_lines);
    } else {
      printf("%d\n", match_lines);
    }
  }
}

void opt_l_handling(char *filename, int match_lines) {
  if (match_lines > 0) printf("%s\n", filename);
}

void opt_o_handling(regmatch_t *pmatch, regex_t *regexp, char *temp) {
  int check_matches = 0;
  while (temp != NULL) {
    printf("%.*s\n", (int)(pmatch->rm_eo - pmatch->rm_so),
           temp + pmatch->rm_so);
    temp += pmatch->rm_eo;
    check_matches = regexec(regexp, temp, 1, pmatch, 0);
    if (check_matches) temp = NULL;
  }
}