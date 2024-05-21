GREP = my_grep
CC = gcc
FLAGS = -Wall -Werror -Wextra -std=c11

$(GREP): main.o grep_functions.o
	$(CC) $(FLAGS) main.o grep_functions.o -o $(GREP)

main.o: main.c grep_functions.h
	$(CC) $(FLAGS) -c main.c -o main.o

grep_functions.o: grep_functions.c grep_functions.h
	$(CC) $(FLAGS) -c grep_functions.c -o grep_functions.o

clean:
	rm $(GREP) *.o

rebuild: clean $(GREP)

test: grep_test.sh
	bash grep_test.sh

style_test:
	clang-format -style=Google -n *.c *.h