CC=g++
CFLAGS=-I.
LIBS=-ldl

cec-tester.out: clean
	@$(CC) cec-tester.cpp $(LIBS) -o cec-tester.out

run: cec-tester.out
	@./cec-tester.out

clean:
	@rm -rf *.o *.out

ubuntu-install:
	sudo apt install libcec-dev cec-utils
