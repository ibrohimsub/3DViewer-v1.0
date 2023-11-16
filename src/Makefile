# Compiler and compiler flags
CC = gcc
CFLAGS =-Wall -Wextra -Werror -std=c11
LDFLAGS = -lm -lcheck
GCOV_FLAGS?=-fprofile-arcs -ftest-coverage
LINUX_FLAGS=-lsubunit -lrt -lpthread -lm

# Directories and file patterns
SRC_DIR = ./backend
TEST_DIR = ./tests
OBJ_DIR = ./build
TEST_OBJ_DIR = ./tests
FRONTEND_DIR = ./frontend
DIST_DIR = ./3DViewer_v1.0
GCOV_DIR = ./gcov_report
DOC_DIR = ./documentation

# Operating System-specific variables
OS := $(shell uname -s)
OPEN_CMD = xdg-open  # Default for Linux
ifeq ($(OS), Darwin)
	OPEN_CMD = open  # Use open on macOS
endif

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))
TEST_OBJ_FILES = $(patsubst $(TEST_DIR)/%.c, $(TEST_OBJ_DIR)/%.o, $(TEST_FILES))

# Ensure that the build directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Define the target for the backend object file
$(OBJ_DIR)/s21_back.o: $(SRC_DIR)/s21_back.c | $(OBJ_DIR)
	$(CC) -g -c $(CFLAGS) $< -o $@

# Targets
.PHONY: all install uninstall open_exe dvi dist tests gcov_report valgrind clean clang

all: clean tests install

install:
	mkdir -p $(OBJ_DIR)
	cd $(FRONTEND_DIR) && qmake && make && make clean && rm Makefile && cd ../ && mv $(FRONTEND_DIR)/3DViewer.app $(OBJ_DIR)

uninstall: clean

open_exe:
	$(OPEN_CMD) $(OBJ_DIR)/3DViewer.app

dvi:
	$(OPEN_CMD) README.html

dist: clean $(OBJ_DIR)
	mkdir -p $(DIST_DIR)
	cp -r $(SRC_DIR) $(DIST_DIR)
	cp -r $(OBJ_DIR) $(DIST_DIR)
	cp -r $(FRONTEND_DIR) $(DIST_DIR)
	tar czvf 3DViewer_v1.0.tgz $(DIST_DIR)
	rm -rf $(DIST_DIR)

tests: clean
	gcc -Wall -Wextra -Werror -std=c11 ./tests/test.c ./backend/s21_back.c  -o $(TEST_OBJ_DIR)/test -lm -lcheck
	$(TEST_OBJ_DIR)/test

gcov_report: clean
ifeq ($(OS), Darwin)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) ${TEST_FILES} ${SRC_FILES} ${SRC} -o test $(LDFLAGS)
else
	$(CC) $(CFLAGS) $(GCOV_FLAGS) ${TEST_FILES} ${SRC_FILES} ${SRC} -o test $(LDFLAGS) $(LINUX_FLAGS)
endif
	./test
	lcov -t "stest" -o s21_test.info -c -d .
	genhtml -o report s21_test.info
	$(OPEN_CMD) ./report/index.html

valgrind: $(TEST_OBJ_FILES) $(OBJ_FILES) $(OBJ_DIR)/s21_back.o
	$(CC) -g $(TEST_OBJ_FILES) $(OBJ_FILES) $(OBJ_DIR)/s21_back.o $(LDFLAGS) -o $(TEST_OBJ_DIR)/test
	CK_FORK=no valgrind --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(TEST_OBJ_DIR)/test

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(GCOV_DIR)
	rm -rf $(TEST_OBJ_DIR)/*.o
	rm -f $(TEST_OBJ_DIR)/*.gcda
	rm -f $(TEST_OBJ_DIR)/*.gcno *.txt
	rm -f $(TEST_OBJ_DIR)/*.gcov
	rm -f $(TEST_OBJ_DIR)/*.info
	rm -f $(TEST_OBJ_DIR)/*.css
	rm -f $(TEST_OBJ_DIR)/*.html
	rm -f $(TEST_OBJ_DIR)/*.dSYM
	rm -f $(TEST_OBJ_DIR)/test
	rm -f s21_back.* s21_test.info test test.*
	rm -rf *.tgz
	rm -rf report

clang:
	clang-format -i $(SRC_DIR)/*.c $(SRC_DIR)/*.h $(FRONTEND_DIR)/*.cpp $(FRONTEND_DIR)/*.h $(TEST_DIR)/*.c
