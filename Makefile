# Makefile to help make building everything simpler 

default:
	bison -d parser.ypp
	flex -o scanner.c scanner.l 
	clang++ -g -o compiler parser.tab.cpp scanner.c driver.cpp Node.cpp

clean: 
	rm -f parser.tab.*
	rm -f scanner.c	

clean-all:
	rm -f parser.tab.*
	rm -f scanner.c
	rm -f compiler
