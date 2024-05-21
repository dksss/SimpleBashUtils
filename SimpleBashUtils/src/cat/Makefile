CAT = my_cat
CC = gcc
FLAGS = -Wall -Werror -Wextra -std=c11

$(CAT) : my_cat.o cat_functions.o
	$(CC) $(FLAGS) my_cat.o cat_functions.o -o $(CAT)

my_cat.o : main.c cat_functions.h
	$(CC) $(FLAGS) -c main.c -o my_cat.o

cat_functions.o : cat_functions.c cat_functions.h
	$(CC) $(FLAGS) -c cat_functions.c -o cat_functions.o

clean :
	rm $(CAT) *.o

rebuild: clean $(CAT)

test: cat_tests.sh
	bash cat_tests.sh

style_test:
	clang-format -style=Google -n *.c