# Makefile to help make building everything simpler 

default:
	bison -d parser.ypp
	flex -o scanner.c scanner.l 
	clang++ -g parser.tab.cpp scanner.c driver.cpp Node.cpp \
				$(shell llvm-config --cxxflags --ldflags --libs core) \
				-ltinfo \
				-lpthread \
				-o compiler

io:
	clang -O3 -c -emit-llvm -S input.c -o input.ll
	clang -O3 -c -emit-llvm -S output.c -o output.ll

clean: 
	rm -f parser.tab.*
	rm -f scanner.c	

clean-all:
	rm -f parser.tab.*
	rm -f scanner.c
	rm -f compiler
