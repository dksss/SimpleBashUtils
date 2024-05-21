#include <stdio.h>

#include "grep_functions.h"

int main(int argc, char **argv) {
  if (argc > 2)
    grep_start(argc, argv);
  else
    fprintf(stderr, "ERROR\n");

  return 0;
}