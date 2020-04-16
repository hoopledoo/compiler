/********************************************************
 * Tyler Wolf 
 * 
 * IRGen header file for use in compiler project
 * 04/15/2020
 *
 ********************************************************/

#ifndef IR_GEN_
#define IR_GEN_

#include "Node.h"
#include "llvm-include.h"

class IRGen {
public:
	IRGen(std::string s);
	virtual ~IRGen(){};

	void generateIR(Node* n);
	llvm::Value* codegen(Node* n, int scope=0);
};

#endif //IR_GEN_