SHELL := /bin/bash
CC = c++

FLAGS = -Wall --std=c++17 -O3 -g

# To check for memory/standard library usage errors, use below flags instead
#FLAGS = -Wall --std=c++17 -O3 -g -fno-omit-frame-pointer -fsanitize=address -D_GLIBCXX_DEBUG

.DEFAULT_GOAL := segclobber
.PHONY: clean rmdb ftest test db

COMMON_OBJS := database3.o utils.o state.o solver.o bound.o zobrist.o miscTypes.o


# Main solver executable
segclobber: main.o $(COMMON_OBJS)
	$(CC) $(FLAGS) $^ -o $@

# Executable for building database files
DBManage4: dbmanage4.o $(COMMON_OBJS)
	$(CC) $(FLAGS) $^ -o $@


%.o: %.cpp %.h options.h
	$(CC) $(FLAGS) $< -o $@ -c

%.o: %.cpp options.h
	$(CC) $(FLAGS) $< -o $@ -c

clean:
	-rm *.o segclobber DBManage4

rmdb:
	-rm database3.bin database3_alt.bin

test: segclobber
	time python3 runtests.py

ftest: segclobber
	time python3 runtests.py --persist

db: rmdb DBManage4
	touch database3.bin database3_alt.bin
	-time ./DBManage4

