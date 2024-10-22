GCC := gcc
CCFLAGS := -Wall -Werror -Wextra -std=c11

.PHONY: all clang_check clean rebuild

all: clang_check s21_grep

s21_grep:
	$(CC) $(CCFLAGS) -o $@ ./*.c

test: s21_grep
	bash run_grep_test.sh

clang_check:
	clang-format -style=Google -n ./*.h ./*.c

clean:
	rm s21_grep

rebuild: clean s21_grep