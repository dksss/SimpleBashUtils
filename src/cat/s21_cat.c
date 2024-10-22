#include "./s21_cat.h"

int main(int argc, char** argv) {
  S21_Options_Flags options = {0};

  int status_flag = handling_keys(&options, argc, argv);

  if (status_flag == kNo_Error) {
    handling_data(argc, argv, &options);
  }

  return 0;
}