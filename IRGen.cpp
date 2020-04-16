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

// we'll use this to keep track of 
// the known variables during generation
// we'll handle it essentially the same as the symbol table used
// during the semantic analysis
static std::map<int, std::map<std::string, llvm::AllocaInst *>> vars; 	

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

void printValue(llvm::Value* v){
	llvm::raw_ostream* ir_out = new llvm::raw_os_ostream(std::cout);
	v->print(*ir_out, false);
	delete ir_out;
}

// Perform initial setup required to generate IR using llvm
void IRGen::generateIR(Node* root){
	llvm::Module* module = TheModule.get();

	// insert the input() and output() functions
	std::vector<llvm::Type*> argList(0);
	llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(TheContext), argList, false);
	module->getOrInsertFunction("input", FT);

	argList.push_back(llvm::Type::getInt32Ty(TheContext));
	FT = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), argList, false);
	module->getOrInsertFunction("output", FT);
	
	llvm::Value* val = (llvm::Value*)codegen(root);

	setIRString();
	std::cout << IR_string << std::endl;

	return;
}

void clearScope(int scope){
	if(not vars[scope].empty()){
		vars[scope].clear();
	}
}

static llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction, 
											const std::string &VarName,
											llvm::Type* t){
	llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
	return TmpB.CreateAlloca(t, 0, VarName.c_str());
}

// Recursive method to generate code for each node
// This behavior will be defined based on the type
// of node we are dealing with
void* IRGen::codegen(Node*n, int scope){
	llvm::Value* L = 0;

	// Handle the different named nodes
	if(not n->attributes.empty() and n->attributes.count("name")){	
				/*		handle add operator						*/

		if(n->attributes["name"] == "ADD") {
			std::cout << "generating '+'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateAdd(L,R);
		}	
				/*		handle subtraction operator				*/

		else if(n->attributes["name"] == "SUB") {
			std::cout << "generating '-'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateSub(L,R);
		}	
				/*		handle multiply operator				*/

		else if(n->attributes["name"] == "MULT") {
			std::cout << "generating '*'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateMul(L,R);

		}
				/*		handle division operator				*/

		else if(n->attributes["name"] == "DIV") {
			std::cout << "generating '/'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateSDiv(L,R);
		}
				/*		handle equality operator				*/

		else if(n->attributes["name"] == "EQ") {
			std::cout << "generating '=='" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpEQ(L,R);
		}
				/*		handle inequality operator				*/

		else if(n->attributes["name"] == "NEQ") {
			std::cout << "generating '!='" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpNE(L,R);
		}
				/*		handle less than operator				*/

		else if(n->attributes["name"] == "LT") {
			std::cout << "generating '<'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpSLT(L,R);
		}
				/*		handle less than or equal operator		*/

		else if(n->attributes["name"] == "LTE") {
			std::cout << "generating '<='" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpSLE(L,R);
		}
				/*		handle greater than operator			*/

		else if(n->attributes["name"] == "GT") {
			std::cout << "generating '>'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpSGT(L,R);
		}
				/*		handle greater than or equal operator	*/

		else if(n->attributes["name"] == "GTE") {
			std::cout << "generating '>='" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpSGE(L,R);
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
			// TODO, figure out how to create return value (if there is one 
			// likely solution = using the vars to lookup the value
			// return Builder.CreateRet();
		}
				/*		handle variable declaration				*/

		else if(n->attributes["name"] == "varDec") {
			std::cout << "generating variable declaration" << std::endl;
			std::string id = n->right_child->getID();
			llvm::Function* CurrFunction = Builder.GetInsertBlock()->getParent();
			llvm::AllocaInst* Alloca = CreateEntryBlockAlloca(CurrFunction, id, llvm::Type::getInt32Ty(TheContext));
			vars[scope][id] = Alloca;
		}		
				/*		handle array variable declaration		*/

		else if(n->attributes["name"] == "arrayVarDec") {
			std::cout << "generating array declaration" << std::endl;
			std::string id = n->right_child->getID();
			llvm::Function* CurrFunction = Builder.GetInsertBlock()->getParent();
			llvm::AllocaInst* Alloca = CreateEntryBlockAlloca(CurrFunction, id, llvm::Type::getInt32PtrTy(TheContext));
			vars[scope][id] = Alloca;
		}	
				/*		handle function  declaration			*/

		else if(n->attributes["name"] == "funcDec") {
			std::cout << "generating function declaration" << std::endl;

			// Start by creating the argument list
		  	std::vector<llvm::Type*> argList(0);

			std::string rtype = n->left_child->getName();
			std::string name = n->left_child->right_sib->getID();
			int num_params = 0;
			
			// Build out the array of parameters.
			if(n->left_child->right_sib->right_sib->getName() == "paramList"){
				Node* param_list = n->left_child->right_sib->right_sib;
				Node* param = param_list->left_child;

				// Loop through all params
				while(param){
					if(param->getName() == "arrayParam"){
						argList.push_back(llvm::Type::getInt32PtrTy(TheContext));
					}
					else if(param->getName() == "param"){
						argList.push_back(llvm::Type::getInt32Ty(TheContext));
					}
					
					param = param->right_sib;
				}
			}
			
			llvm::FunctionType *FT = 0;
			if(rtype == "INT"){
				FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(TheContext), argList, false);
			}
			else if(rtype == "VOID"){
				FT = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), argList, false);
			}
			else{
				std::cerr << "FATAL ERROR: return type for function neither INT nor VOID - should never see this msg." << std::endl;
			}

			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, TheModule.get());

			// Create a new basic block to start insertion into.
			llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", F);
			Builder.SetInsertPoint(BB);

			// OBTAIN THE FUNCTION CONTENT:
			llvm::Value* funcVal = (llvm::Value*)codegen(n->right_child, scope);

			if (funcVal) {
  				// Validate the generated code, checking for consistency.
  				llvm::verifyFunction(*F);
			}

			return funcVal;
		}	
		else if(n->attributes["name"] == "ID"){
			return vars[scope][n->getID()];
		}
				/*		handle assignment operator				*/

		else if(n->attributes["name"] == "ASSIGN") {
			std::cout << "generating '='" << std::endl;
			int scope_found = -1;
			std::string id = n->left_child->getID();

			for(int i=scope; i>=0; i--){
				if(not vars[i].empty() and vars[i].count(id)){
					scope_found = i;
					break;
				}
			}

			// Get the memory reference
			llvm::AllocaInst* Alloca = (llvm::AllocaInst*)codegen(n->left_child, scope);
			//llvm::Value* CurVar = Builder.CreateLoad(Alloca);

			// Recursively evaluate what the right-hand side of the equals sign should be
			llvm::Value* NextVar = (llvm::Value*)codegen(n->right_child, scope);

			// Store the results back into the var
			Builder.CreateStore(NextVar, Alloca);

		}
				/*		handle while loop 						*/

		else if(n->attributes["name"] == "while-scope") {
			std::cout << "generating '='" << std::endl;
		}
				/*		handle scope change on compound stmt 	*/
		else if (n->attributes["name"] == "compoundStmt" ){
			scope++;

			Node* looper = n->left_child;
			while(looper){
				codegen(looper, scope);
				looper = looper->right_sib;
			}

			clearScope(scope--);
		}

		else if(n->attributes["name"] == "while-scope"){

		}

		else if(n->attributes["name"] == "localDeclarations" or \
				n->attributes["name"] == "declarationList" or \
				n->attributes["name"] == "stmtList"){

			Node* looper = n->left_child;
			while(looper){
				codegen(looper, scope);
				looper = looper->right_sib;
			}
		}

	}
	else if (n->raw_val) {
		std::cout << "handling constant value: " << n->val << std::endl;
		return llvm::ConstantInt::get(TheContext, llvm::APInt(32, n->val));
	}

	/*
	Node* looper = 0;
	if(n and n->left_child){ 
		looper = n->left_child; 
		while(looper){
			codegen(looper, scope);
			looper = looper->getRightSib();
		}
	}
	*/

	return L;
}