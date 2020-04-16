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
static std::map<std::string, llvm::Function*> funcs;

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
	if(v){
		llvm::raw_ostream* ir_out = new llvm::raw_os_ostream(std::cout);
		v->print(*ir_out, false);
		delete ir_out;
	}
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
	
	// Generate the code for the entire module
	llvm::Value* val = (llvm::Value*)codegen(root);

	setIRString();
	std::cout << IR_string << std::endl;

	return;
}

static llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction, 
											const std::string &VarName,
											llvm::Type* t,
											llvm::Value* size){
	llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
	return TmpB.CreateAlloca(t, size, VarName.c_str());
}

// Recursive method to generate code for each node
// This behavior will be defined based on the type
// of node we are dealing with
void* IRGen::codegen(Node*n, int scope){
	llvm::Value* L = 0;

	// Handle the different named nodes
	if(n and not n->attributes.empty() and n->attributes.count("name")){	
				/****************************************************
				 *			handle addition operator				*
				 ****************************************************/
		if(n->attributes["name"] == "ADD") {
			std::cout << "generating '+'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateAdd(L,R);
		}	
				/****************************************************
				 *			handle subtraction operator				*
				 ****************************************************/
		else if(n->attributes["name"] == "SUB") {
			std::cout << "generating '-'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateSub(L,R);
		}	
				/****************************************************
				 *			handle multiplication operator				*
				 ****************************************************/
		else if(n->attributes["name"] == "MULT") {
			std::cout << "generating '*'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateMul(L,R);

		}
				/****************************************************
				 *			handle division operator				*
				 ****************************************************/
		else if(n->attributes["name"] == "DIV") {
			std::cout << "generating '/'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateSDiv(L,R);
		}
				/****************************************************
				 *			handle equality operator				*
				 ****************************************************/
		else if(n->attributes["name"] == "EQ") {
			std::cout << "generating '=='" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpEQ(L,R);
		}
				/****************************************************
				 *			handle inequality operator				*
				 ****************************************************/
		else if(n->attributes["name"] == "NEQ") {
			std::cout << "generating '!='" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpNE(L,R);
		}
				/****************************************************
				 *			handle less than operator				*
				 ****************************************************/
		else if(n->attributes["name"] == "LT") {
			std::cout << "generating '<'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpSLT(L,R);
		}
				/****************************************************
				 *			handle less than or equal operator		*
				 ****************************************************/
		else if(n->attributes["name"] == "LTE") {
			std::cout << "generating '<='" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpSLE(L,R);
		}
				/****************************************************
				 *			handle greater than operator			*
				 ****************************************************/
		else if(n->attributes["name"] == "GT") {
			std::cout << "generating '>'" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpSGT(L,R);
		}
				/****************************************************
				 *			handle greater than or equal operator	*
				 ****************************************************/
		else if(n->attributes["name"] == "GTE") {
			std::cout << "generating '>='" << std::endl;
			L = (llvm::Value*)codegen(n->left_child, scope);
			llvm::Value* R = (llvm::Value*)codegen(n->right_child, scope);
			return Builder.CreateICmpSGE(L,R);
		}
				/****************************************************
				 *			handle variable declaration				*
				 ****************************************************/
		else if(n->attributes["name"] == "varDec") {
			std::string id = n->right_child->getID();
			std::cout << "variable declaration: " << id << " in scope "<< scope<< std::endl;

			llvm::Function* CurrFunction = Builder.GetInsertBlock()->getParent();
			llvm::AllocaInst* Alloca = CreateEntryBlockAlloca(CurrFunction, id, llvm::Type::getInt32Ty(TheContext), nullptr);
			vars[scope][id] = Alloca;
		}		
				/****************************************************
				 *			handle array variable declaration 		*
				 ****************************************************/
		else if(n->attributes["name"] == "arrayVarDec") {
			std::string id = n->left_child->right_sib->getID();
			int size = n->right_child->val;
			std::cout << "array declaration" << id << "[" << size << "] in scope "<< scope<< std::endl;

			llvm::Function* CurrFunction = Builder.GetInsertBlock()->getParent();
			llvm::Constant* array_size = llvm::ConstantInt::get(TheContext, llvm::APInt(32, size));
			llvm::AllocaInst* Alloca = CreateEntryBlockAlloca(CurrFunction, id, llvm::Type::getInt32PtrTy(TheContext), array_size);
			vars[scope][id] = Alloca;
		}	
				/****************************************************
				 *			handle function declaration				*
				 ****************************************************/
		else if(n->attributes["name"] == "funcDec") {
			// Start by creating the argument list
		  	std::vector<llvm::Type*> argList(0);
		  	std::vector<std::string> argNames(0);

			std::string rtype = n->left_child->getName();
			std::string id = n->left_child->right_sib->getID();
			std::cout << "\n\nfunction declaration for " << id << " in scope " << scope << std::endl;
			int num_params = 0;
			
			Node* param = n->left_child->right_sib->right_sib;
			if(param->getName() == "paramList"){
				param = param->left_child;
			}

			// Loop through all params
			while(param){
				if(param->getName() == "arrayParam"){
					argList.push_back(llvm::Type::getInt32PtrTy(TheContext));
					argNames.push_back(param->right_child->getID());
				}
				else if(param->getName() == "param"){
					argList.push_back(llvm::Type::getInt32Ty(TheContext));
					argNames.push_back(param->right_child->getID());
				}
				
				param = param->right_sib;
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

			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, id, TheModule.get());

			// Create a new basic block to start insertion into.
			llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", F);
			Builder.SetInsertPoint(BB);

			// Allocate our arguments, and insert them into our vars symbol table
			// Start by clearing the symbol table, in case there's anything in it.
			std::cout << "clearing scope in funcDec " << scope+1 << std::endl;
			vars[scope+1].clear();

			auto name = argNames.begin();
			for(auto &Arg : F->args()){
				std::cout << "function arg variable declaration: " << *name << " in scope "<< scope+1 << std::endl;
				llvm::AllocaInst* Alloca = CreateEntryBlockAlloca(F, *name, Arg.getType(), nullptr);
				Builder.CreateStore(&Arg, Alloca);
				vars[scope+1][*name] = Alloca;
				name++;
			}

			// OBTAIN THE FUNCTION CONTENT:
			llvm::Value* funcVal = (llvm::Value*)codegen(n->right_child, scope);

			if (funcVal) {
  				// Validate the generated code, checking for consistency.
  				llvm::verifyFunction(*F);
			}

			std::cout << "Adding function " << id << " to funcs map" << std::endl;
			funcs[id] = F;

			return funcVal;
		}	
		else if(n->attributes["name"] == "ID"){
			for(int i=scope; i>=0; i--){
				std::cout << "Checking scope " << i << " for variable " << n->getID() << std::endl;
				if (vars[i].count(n->getID())) {
					std::cout << "Found " << n->getID() << " in scope " << i << std::endl;
					return vars[i][n->getID()];
				}
			}
			return 0;
		}
				/****************************************************
				 *			handle assignment operator				*
				 ****************************************************/
		else if(n->attributes["name"] == "ASSIGN") {
			std::cout << "generating '='" << std::endl;

			/* Handle assignment directly to variable */
			llvm::AllocaInst* Alloca = 0;
			std::string id;
			if(n->left_child->getName() == "ID"){
				int scope_found = -1;
				id = n->left_child->getID();

				for(int i=scope; i>=0; i--){
					if(not vars[i].empty() and vars[i].count(id)){
						scope_found = i;
						break;
					}
				}

				// Get the memory reference
				Alloca = (llvm::AllocaInst*)codegen(n->left_child, scope_found);
			}	
			/* Handle assignment to an array location */
			else if(n->left_child->getName() == "varIndex"){
				llvm::AllocaInst* ptr = 0;
				id = n->left_child->left_child->getID();
				int indx = n->left_child->right_child->val;
				std::cout << "handling indexed array assignment " << id << "[" << indx << "]\n";

				for(int i=scope; i>=0; i--){
					std::cout << "Checking scope " << i << " for variable " << id << std::endl;
					if (vars[i].count(id)) {
						std::cout << "ptr found in scope " << i << std::endl;
						ptr = vars[i][id];
						break;
					}
				}
				Alloca = (llvm::AllocaInst* )Builder.CreateConstGEP1_32(ptr, indx);

			}

			llvm::Value* NextVar = 0;
			// Recursively evaluate what the right-hand side of the equals sign should be
			if(not n->right_child->attributes.empty() and n->right_child->getName()=="ID"){
				llvm::AllocaInst* tmp_a = (llvm::AllocaInst*)codegen(n->right_child, scope);
				NextVar = Builder.CreateLoad(tmp_a);
			}
			else{
				NextVar = (llvm::Value*)codegen(n->right_child, scope);
			}

			// Store the results back into the var
			return Builder.CreateStore(NextVar, Alloca);
		}
				/****************************************************
				 *			handle var Index reference				*
				 ****************************************************/
		else if(n->attributes["name"] == "varIndex"){
			llvm::AllocaInst* ptr = 0;
			int indx = n->right_child->val;
			std::string id = n->left_child->getID();
			std::cout << "handling array index " << id << "[" << indx << "]" << std::endl;
			for(int i=scope; i>=0; i--){
				std::cout << "Checking scope " << i << " for variable " << id << std::endl;
				if (vars[i].count(id)) {
					std::cout << "ptr found in scope " << i << std::endl;
					ptr = vars[i][id];
					break;
				}
			}
			return Builder.CreateConstGEP1_32(ptr, indx);
		}
				/****************************************************
				 *			handle function call 					*
				 ****************************************************/
		else if(n->attributes["name"] == "call") {
			std::vector<llvm::Value *> passingArgs;
			std::string id = n->left_child->getID();
			llvm::Function* callee = funcs[id];
			std::cout << "generating call to " << id << "(...)" << std::endl;

			Node* looper = n->right_child->left_child;
			while(looper){
				llvm::Value* result;

				// Handle IDs
				if(not looper->attributes.empty() and looper->attributes["name"] == "ID"){
					llvm::AllocaInst* ptr = (llvm::AllocaInst*)codegen(looper, scope);
					result = Builder.CreateLoad(ptr);
				}
				// Handle Constants
				else{
					result = llvm::ConstantInt::get(TheContext, llvm::APInt(32, looper->val));
				}
				
				printValue(result);
				passingArgs.push_back(result);
				looper = looper->right_sib;
			}
			return Builder.CreateCall(callee, passingArgs, "calltmp");
		}
				/****************************************************
				 *			handle while loop control				*
				 ****************************************************/
		else if(n->attributes["name"] == "while-scope") {
			std::cout << "generating 'while loop'  (TODO)" << std::endl;
		}
				/****************************************************
				 *			handle if statement						*
				 ****************************************************/
		else if(n->attributes["name"] == "if-scope") {
			std::cout << "generating if stmt  (TODO)" << std::endl;
		}
				/****************************************************
				 *			handle return statement					*
				 ****************************************************/
		else if(n->attributes["name"] == "RETURN") {
			std::cout << "generating return  (TODO)" << std::endl;
			// TODO, figure out how to create return value (if there is one 
			// likely solution = using the vars to lookup the value
			// return Builder.CreateRet();
		}
				/****************************************************
				 *		handle scope change on compound stmt 		*
				 ****************************************************/					 	
		else if (n->attributes["name"] == "compoundStmt" ){
			scope++;

			Node* looper = n->left_child;
			while(looper){
				codegen(looper, scope);
				looper = looper->right_sib;
			}
			std::cout << "leaving compoundstmt, clearing scope " << scope << std::endl;
			vars[scope--].clear();
		}
				/****************************************************
				 *			handle nodes that recurse 				*
				 ****************************************************/
		else if(n->attributes["name"] == "localDeclarations" or \
				n->attributes["name"] == "declarationList" or \
				n->attributes["name"] == "stmtList" or \
				n->attributes["name"] == "argList"){

			Node* looper = n->left_child;
			while(looper){
				codegen(looper, scope);
				looper = looper->right_sib;
			}
		}

	}
				/****************************************************
				 *			handle constnat values					*
				 ****************************************************/	
	else if (n and n->raw_val) {
		std::cout << "handling constant value: " << n->val << std::endl;
		return llvm::ConstantInt::get(TheContext, llvm::APInt(32, n->val));
	}

	return L;
}