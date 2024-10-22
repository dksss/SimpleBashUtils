GCC := gcc
CCFLAGS := -Wall -Werror -Wextra -std=c11

.PHONY: all clang_check clean rebuild

all: clang_check s21_cat

s21_cat:
	$(CC) $(CCFLAGS) -o $@ ./*.c

test: s21_cat
	bash run_cat_test.sh

clang_check:
	clang-format -style=Google -n ./*.h ./*.c

clean:
	rm s21_cat

rebuild: clean all