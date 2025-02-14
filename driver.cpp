
#include "driver.h"
#include "parser.tab.hpp"

extern FILE *yyin;

Node* root;

std::map<int, std::map<std::string, symbol>> symbolTable;

int main(int argc, char** argv){
	// Check for correct number of arguments
	if ( argc != 2) {
		std::cout << "Please specify a file." << std::endl; 
		return -1;
	}

	// Open a file handle to a particular file:
	FILE *myfile = fopen(argv[1], "r");
	// Make sure it is valid:
	if (!myfile) {
		std::cout << "I can't open" << argv[1] << std::endl;
		return -1;
	}

	// Set Flex to read from it instead of defaulting to STDIN:
	yyin = myfile;

	yyparse();

	std::cout << "Symbol Table:" << std::endl;
	int successful = root->walkTreeCheckSymbols();
	if (not successful) {
		std::cout << "There was at least one conflict during semantic scope check. " << std::endl;
		exit(-1);
	}

	std:: cout << "\nAST:" << std::endl;
	root->walkTreePrint();

	std::cout << "\nGenerating IR..." << std::endl;
	IRGen* generator = new IRGen(argv[1]);
	generator->generateIR(root);
	std::cout << generator->getIRString();

	std::ofstream outfile;
	outfile.open(std::string(argv[1]) + ".ll", std::ios::out);
	if (!outfile){
		std::cout << "Failed to open output file" << std::string(argv[1]) + ".ll" << std::endl;
	}
	else{
		outfile << generator->getIRString();
		outfile.close();
	}

	delete generator;
	delete root;
	fclose(myfile);

	return 0;
}