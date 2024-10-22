#include "./s21_grep.h"

int main(int argc, char** argv) {
  if (argc > 1) {
    S21_Util_Config grep_config = {0};

    int error_status = handling_arguments(&grep_config, argc, argv);
    if (error_status == kNo_Error) {
      handling_data(&grep_config, argc, argv);
    }

    free_configs(&grep_config);
  } else {
    fprintf(stderr, "%s\n", S21_USAGE_MSG);
  }

  return 0;
}