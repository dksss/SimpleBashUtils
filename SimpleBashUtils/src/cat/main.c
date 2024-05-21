#include <stdio.h>

#include "cat_functions.h"

int main(int argc, char **argv) {
  if (argc > 1)
    work_with_arguments(argc, argv);
  else
    fprintf(stderr, "ERROR\n");

  return 0;
}