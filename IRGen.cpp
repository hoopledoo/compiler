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

	setIRString();

	std::cout << IR_string << std::endl;

	/*
	TODO: remove this stuff, copied from spreadsheet project
	unique_ptr<llvm::Module> module = create_uniq<llvm::Module>());

	// Get the namedValues hash table from the cell
	map<string, llvm::Value *> namedValues = cell->getNamedValues();

	// Construct the expression string which is what we'll use to name the function
	string name = cell->getID() + "_exp";

	// From the Function Prototype Section of Kaleidoscope Demo
  	vector<llvm::Type*> argList(cell->controllersMap.size(), llvm::Type::getInt32Ty(TheContext));
	llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(TheContext), argList, false);
	llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, module);

	// Set name for all arguments, obtain an iterator for the cell controllersMap
	map<string, SS_Cell*>::iterator mapPair = cell->controllersMap.begin();
	for (auto& Arg: F->args()){
		Arg.setName(mapPair->first);
		++mapPair;
	}

	// Create a new basic block to start insertion into.
	llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", F);
	Builder.SetInsertPoint(BB);

	// Record the function arguments in the namedValues map.
	namedValues.clear();

	for (auto &Arg : F->args()) {
		namedValues[Arg.getName()] = &Arg;
	}

	llvm::Value * val = walkTreeGenerateIR(namedValues);
	if (val) {
  		// Finish off the function.
  		Builder.CreateRet(val);

  		// Validate the generated code, checking for consistency.
  		llvm::verifyFunction(*F);

  		cell->setIR();
  		cell->setDataLayout();
  		//cell->runCode();
  	}
	*/

	return;
}