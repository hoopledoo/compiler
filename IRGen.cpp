/********************************************************
 * Tyler Wolf 
 * 
 * IRGen cpp file for use in compiler project
 * 04/15/2020
 *
 ********************************************************/
#include "IRGen.h"

static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;
static std::string IR_string;
static std::map<std::string, TYPE> vars; // we'll use this to keep track of 
										 // the known variables during generation

IRGen::IRGen(std::string s){
	TheModule = llvm::make_unique<llvm::Module> (s + "_module", TheContext);
	TheModule->setSourceFileName(s);
}

void setIRString(){
	// Handle setting the IR_string
	if(not IR_string.empty()) IR_string.clear();
	llvm::raw_string_ostream* ir_out = new llvm::raw_string_ostream(IR_string);
	TheModule.get()->print(*ir_out, nullptr);
	delete ir_out;
}

// Perform initial setup required to generate IR using llvm
void IRGen::generateIR(Node* root){
	llvm::Module* module = TheModule.get();

	codegen(root);

	setIRString();
	std::cout << IR_string << std::endl;

	return;
}

// Recursive method to generate code for each node
// This behavior will be defined based on the type
// of node we are dealing with
llvm::Value* IRGen::codegen(Node*n){
	llvm::Value* L = 0;

	// Handle the different named nodes
	if(not n->attributes.empty() and n->attributes.count("name")){	
				/*		handle add operator						*/

		if(n->attributes["name"] == "ADD") {
			std::cout << "generating '+'" << std::endl;
		}	
				/*		handle subtraction operator				*/

		else if(n->attributes["name"] == "SUB") {
			std::cout << "generating '-'" << std::endl;
		}	
				/*		handle multiply operator				*/

		else if(n->attributes["name"] == "MULT") {
			std::cout << "generating '*'" << std::endl;
		}
				/*		handle division operator				*/

		else if(n->attributes["name"] == "DIV") {
			std::cout << "generating '/'" << std::endl;
		}
				/*		handle equality operator				*/

		else if(n->attributes["name"] == "EQ") {
			std::cout << "generating '=='" << std::endl;
		}
				/*		handle inequality operator				*/

		else if(n->attributes["name"] == "NEQ") {
			std::cout << "generating '!='" << std::endl;
		}
				/*		handle less than operator				*/

		else if(n->attributes["name"] == "LT") {
			std::cout << "generating '<'" << std::endl;
		}
				/*		handle less than or equal operator		*/

		else if(n->attributes["name"] == "LTE") {
			std::cout << "generating '<='" << std::endl;
		}
				/*		handle greater than operator			*/

		else if(n->attributes["name"] == "GT") {
			std::cout << "generating '>'" << std::endl;
		}
				/*		handle greater than or equal operator	*/

		else if(n->attributes["name"] == "GTE") {
			std::cout << "generating '>='" << std::endl;
		}
				/*		handle if statement 					*/

		else if(n->attributes["name"] == "if-scope") {
			std::cout << "generating if stmt" << std::endl;
		}
				/*		handle function call					*/

		else if(n->attributes["name"] == "call") {
			std::cout << "generating call" << std::endl;
		}
				/*		handle return 							*/

		else if(n->attributes["name"] == "RETURN") {
			std::cout << "generating return" << std::endl;
		}
				/*		handle variable declaration				*/

		else if(n->attributes["name"] == "varDec") {
			std::cout << "generating variable declaration" << std::endl;
		}		
				/*		handle array variable declaration		*/

		else if(n->attributes["name"] == "arrayVarDec") {
			std::cout << "generating array declaration" << std::endl;
		}	
				/*		handle function  declaration			*/

		else if(n->attributes["name"] == "funcDec") {
			std::cout << "generating function declaration" << std::endl;
		}	
				/*		handle assignment operator				*/

		else if(n->attributes["name"] == "ASSIGN") {
			std::cout << "generating '='" << std::endl;
		}
				/*		handle while loop 						*/

		else if(n->attributes["name"] == "while-scope") {
			std::cout << "generating '='" << std::endl;
		}

	}
	else if (n->raw_val) {
		std::cout << "handling constant value: " << n->val << std::endl;
	}

	Node* looper = 0;
	if(n and n->left_child){ 
		looper = n->left_child; 
		while(looper){
			codegen(looper);
			looper = looper->getRightSib();
		}
	}

	return L;

}