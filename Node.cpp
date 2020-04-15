/********************************************************
 * Tyler Wolf 
 * 
 * Simple Node class cpp file for use in compiler project
 *
 ********************************************************/

#include "Node.h"
#include "llvm-include.h"

static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;

Node::Node(){
	// Initialize all Children to 0
	left_child = 0;
	right_child = 0;

	// Initialize all Siblings to 0
	right_sib = 0;

	raw_val = false;
	jump_always = false;
	has_id = false;
}

Node::Node(std::string s) : Node::Node() {
	attributes["name"] = s;
}

Node::Node(int n) : Node::Node() {
	raw_val = true;
	val = n;
}

Node::~Node(){

	// Clean up children
	if(left_child) { 
		delete left_child; 
		left_child = 0; 
	}

	// Clean up siblings
	if (right_sib) { 
		delete right_sib; 
		right_sib = 0; 
	}
	
	/* Not sure if this is necessary
	 * 
	 * // Clean up the strings associated with the map
	 * std::map<std::string, std::string>::iterator it = attributes.begin();
	 * while (it != attributes.end()) { (it->second).clear(); it++;	}
	*/
}

void Node::setName(std::string s){
	attributes["name"] = s;
}

void Node::setLabel(std::string s){
	attributes["label"] = s;
}

void Node::setJumpTo(std::string s){
	attributes["jump_to"] = s;
}

void Node::setID(std::string s){
	has_id = true;
	attributes["id"] = s;
}

std::string Node::getName() {
	if (attributes.count("name")) 	return attributes["name"];
	else 	return 0;
}

std::string Node::getLabel(){
	if (attributes.count("label")) 	return attributes["label"];
	else 	return 0;
}

std::string Node::getJumpTo() {
	if (attributes.count("jump_to")) return attributes["jump_to"];
	else 	return 0;
}

std::string Node::getID() {
	if (attributes.count("id"))		return attributes["id"];
	else	return 0;
}

void Node::setJumpAlways(){
	jump_always = true;
}

void Node::setJumpOn(bool b){
	jump_always = false;
	jump_on = b;
}

bool Node::getJumpAlways(){
	return jump_always;
}

bool Node::getJumpOn(){
	return jump_on;
}

void Node::setLeftChild(Node* n){
	left_child = n;
}

void Node::setRightChild(Node* n){
	right_child = n;
}

Node* Node::getLeftChild(){
	return left_child;
}

Node* Node::getRightChild(){
	return right_child;
}

void Node::setRightSib(Node* n){
	right_sib = n;
}

void Node::walkTreePrint(int depth){
	for(int i=0; i<depth; i++){
		std::cout << "  ";	
	}

	if(not attributes.empty()){
		if (attributes.count("name")) std::cout << attributes["name"];
		if (attributes.count("id")) std::cout << ": " << attributes["id"];
		if (attributes.count("label")) std::cout << ", label=" << attributes["label"];
		if (attributes.count("jump_to")) {
			std::cout << ", jump_to=" << attributes["jump_to"];
			if (jump_always) std::cout << " -- UNCONDITIONAL";
			else {
				if(jump_on) std::cout << " -- JUMP ON TRUE";
				else  std::cout << " -- JUMP ON FALSE";
			}
		}
		std::cout << std::endl;	
	}
	else if (raw_val) std::cout << this->val << std::endl;

	// Print sub-tree (recursively)
	int children_depth = depth + 1;

	Node* looper = 0;
	if(left_child){ 
		looper = left_child; 
		while(looper){
			looper->walkTreePrint(children_depth);
			looper = looper->getRightSib();
		}
	}
	
}

void printSymbol(symbol s, int scope){
	for(int i=0; i<scope; i++){
		std::cout << "    ";
	}
	std::cout << s.description << " " << s.type << " " << s.name;
    if (s.len>0) std::cout << "[" << s.len << "]";
    std::cout << std::endl;
}

int addSymbol(symbol s, int scope){
	// Case where the current scope's symbol table is empty
    if (symbolTable[scope].empty()){
    	// std::cout << "Starting new table for scope " << scope << std::endl;
        symbolTable[scope][s.name] = s;
        printSymbol(s, scope);
        return 1;
    }
    else{
        if(symbolTable[scope].count(s.name)) {
        	std::cerr << "!~~ Semantic error: \n\t\e[1m";
        	std::cerr << s.name << "\e[0m";
        	std::cerr << " already declared in local scope (symbol table level_" << scope << ")" << std::endl;
        	std::cerr << "*********************!" << std::endl;
        	return 0;
        }
        else{
        	symbolTable[scope][s.name] = s;
        	printSymbol(s,scope);
        	return 1;
        }
     }
}

int countParams(Node* params){
	int num_params = 0;
	if(params){
		Node* param = params->getLeftChild();
		while(param){
			num_params++;
			param = param->getRightSib();
		}
	}
	return num_params;
}

int countArgs(Node* args){
	int num_args = 0;
	if(args){
		Node* arg = args->getLeftChild();
		while(arg){
			num_args++;
			arg = arg->getRightSib();
		}
	}
	return num_args;
}

int checkDeclaredFunc(std::string id, int scope, Node* args){
	int scope_found = 0;
	bool declared = false;
	// Count number of arguments
	int num_args = countArgs(args);

	/* Special case check for functions:

		int input(void){}
		void output(int val){}
		
		which are both defined as a part of the C- language.
	*/
	if(id=="input" and num_args == 0){
		return 1;
	}else if (id=="output" and num_args == 1){
		return 1;
	}
	
	// Starting with the most local scope working outwards
	// see if the id has been declared yet
	for(int i=scope; i>=0; i--){
		if(not symbolTable[i].empty() and symbolTable[i].count(id)){
			scope_found = i;
			declared = true;
			break;
		}
	}

	if (not declared){
		std::cerr << "!~~ Semantic error: \n\tfunction \e[1m";
    	std::cerr << id << "\e[0m";
    	std::cerr << " not declared in scope " << std::endl;
    	std::cerr << "*********************!" << std::endl;
		return 0;
	}

	return 1;

}

int checkDeclaredVar(std::string id, int scope, bool is_array){
	bool declared = false;
	int scope_found;

	// Starting with the most local scope working outwards
	// see if the id has been declared yet
	for(int i=scope; i>=0; i--){
		if(not symbolTable[i].empty() and symbolTable[i].count(id)){
			if(symbolTable[i][id].is_array){
				if (is_array){
					scope_found = i;
					declared = true;
					break;
				}
				else{
					std::cerr << "!~~ Semantic error: \n\t\e[1m";
					std::cerr << id << "\e[0m";
					std::cerr << " declared in scope as array, but is assigned as non-array variable" << std::endl;
					std::cerr << "*********************!" << std::endl;
					return 0;
				}
			}
			else {
				if (is_array){
					std::cerr << "!~~ Semantic error: \n\t\e[1m";
					std::cerr << id << "\e[0m";
					std::cerr << " declared in scope as non-array, but is indexed as array during assign" << std::endl;
					std::cerr << "*********************!" << std::endl;
					return 0;						
				}
				else{
					scope_found = i;
					declared = true;
					break;
				}
			}
		}
	}

     // ERROR TO WRITE
     if(declared) {
     	// Update the symbol as being declared in this scope
     	symbolTable[scope_found][id].assigned = true;
     	return 1;
     }
     else {
        std::cerr << "!~~ Semantic error: \n\t\e[1m";
    	std::cerr << id << "\e[0m";
    	std::cerr << " not declared in scope " << std::endl;
    	std::cerr << "*********************!" << std::endl;
    	return 0;
    }
}

int checkAssigned(std::string id, int scope){
	bool assigned = false;
	int scope_found;

	// Starting with the most local scope working outwards
	// see if the id has been declared yet
	for(int i=scope; i>=0; i--){
		if(not symbolTable[i].empty() and symbolTable[i].count(id)){
			if(symbolTable[i][id].assigned){
				assigned = true;
				scope_found = i;
				break;
			}
		}
	}

     // ERROR TO WRITE
     if(not assigned){
        std::cerr << "!~~ Semantic error: \n\t\e[1m";
    	std::cerr << id << "\e[0m";
    	std::cerr << " referenced before assignment " << std::endl;
    	std::cerr << "*********************!" << std::endl;
    	return 0;
	}
	else{
		return 1;
	}
}

void cleanupTable(int scope){
	if(not symbolTable[scope].empty()){
		symbolTable[scope].clear();
	}
}

/* We will use an integer to keep track of the current scope. When checking
 * for variable assignments, we first check the current scope, and work our way back
 * down to 0 (which is the global scope). Every time we enter a new compound statement
 * we will increment the scope by one. When we finish with that compound statement,
 * we decrement the scope by 1.
 */
int Node::walkTreeCheckSymbols(int scope){
	bool scope_change = false;
	bool success = true;

	if(not attributes.empty()){
		if(attributes.count("name")){

			/* HANDLE DIFFERENT TYPES OF DECLARATIONS */

			if (attributes["name"] == "funcDec"){
				int num_params = countParams(left_child->getRightSib()->getRightSib());
				std::string type = left_child->getName();
				std::string id = left_child->getRightSib()->getID();
				
				// Disallow redefining the C- `input()` method
				if(id=="input" and num_params == 0 and type == "INT"){
					std::cerr << "!~~ Semantic error: \n\t\e[1m";
			    	std::cerr <<"int input(void) \e[0m";
			    	std::cerr << " is language defined " << std::endl;
			    	std::cerr << "*********************!" << std::endl;
					success = false;
				}
				// Disallow redefining the C- `output()` method
				else if(id=="output" and num_params == 1 and type == "VOID"){
					std::cerr << "!~~ Semantic error: \n\t\e[1m";
			    	std::cerr <<"void input(int) \e[0m";
			    	std::cerr << " is language defined " << std::endl;
			    	std::cerr << "*********************!" << std::endl;
					success = false;
				}
				else{	
	        	 	symbol s;
	        	 	s.type = type;  
	        		s.name = id;  
	        		s.len = -1;  
	        		s.description = "func";
	        		s.is_array = false;
	        		s.num_params = num_params;
	        		if(not addSymbol(s, scope)) success = false;
				}
			}
			else if (attributes["name"] == "varDec"){
        	 	symbol s;
        	 	s.type = left_child->getName();  
        		s.name = left_child->getRightSib()->getID();  
        		s.len = -1;  
        		s.description = "var";
        		s.is_array = false;
        		s.assigned = false;
        		s.num_params = -1;
        		if(not addSymbol(s, scope)) success = false;
			}
			else if (attributes["name"] == "arrayVarDec"){
        	 	symbol s;
        	 	s.type = left_child->getName();  
        		s.name = left_child->right_sib->getID();  
        		s.len = left_child->right_sib->right_sib->val;  
        		s.description = "arrayVar";
        		s.is_array = true;
        		s.assigned = false;
        		s.num_params = -1;
        		if(not addSymbol(s, scope)) success = false;
			}
			else if (attributes["name"] == "param" ){
        	 	symbol s;
        	 	s.type = left_child->getName();  
        		s.name = left_child->right_sib->getID();  
        		s.len = -1;  
        		s.description = "param";
        		s.is_array = false;
        		s.assigned = true;
        		s.num_params = -1;
        		if(not addSymbol(s, scope+1)) success = false;
			}
			else if (attributes["name"] == "arrayParam" ){
        	 	symbol s;
        	 	s.type = left_child->getName();  
        		s.name = left_child->right_sib->getID();  
        		s.len = -1;  
        		s.description = "paramPtr";
        		s.is_array = true;
        		s.assigned = true;
        		s.num_params = -1;
        		if(not addSymbol(s, scope+1)) success = false;
			}			
			else if (attributes["name"] == "compoundStmt" ){
				scope_change = true;
				scope++;
			}

			/* HANDLE ASSIGNMENTS */

			else if (attributes["name"] == "ASSIGN" ){
				if(left_child->getName() == "varIndex"){
					if(not checkDeclaredVar(left_child->left_child->getID(), scope, true)) success = false;
				}
				else{
					if(not checkDeclaredVar(left_child->getID(), scope, false)) success = false;
				}
			}	

			/* HANDLE FUNCTION CALL */
			else if (attributes["name"] == "call" ){
				if(not checkDeclaredFunc(left_child->getID(), scope, right_child)) success = false;
			}	

			/* HANDLE REFERENCES */
			// LTE, LT, GT, GTE, EQ, NEQ, ADD, SUB, MULT, DIV
			else if(attributes["name"] == "LTE" or 
					attributes["name"] == "LT" or 
					attributes["name"] == "GT" or 
					attributes["name"] == "GTE" or 
					attributes["name"] == "EQ" or 
					attributes["name"] == "NEQ" or 
					attributes["name"] == "ADD" or 
					attributes["name"] == "SUB" or 
					attributes["name"] == "MULT" or 
					attributes["name"] == "DIV")
			{
				if(right_child->has_id) {
					if(not checkAssigned(right_child->getID(), scope)) success = false;
				}
				if(left_child->has_id){
					if(not checkAssigned(left_child->getID(),scope)) success = false;
				}
			}

			else if(attributes["name"] == "RETURN"){
				// TODO: implement semantic checking of RETURNs
			}

		} 
	}


	Node* looper = 0;
	if(left_child){ 
		looper = left_child; 
		while(looper){
			if(not looper->walkTreeCheckSymbols(scope)) success = false;
			looper = looper->getRightSib();
		}
	}

	if(scope_change) cleanupTable(scope--);
	return success;
}

// Perform initial setup required to generate IR using llvm
void Node::walkTreeGenerateIR(std::string s){
	llvm::Module* module = TheModule.get();
	std::string mod_name = s + "_module";

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

// Recursive method to generate code for each node
// This behavior will be defined based on the type
// of node we are dealing with
void Node::codegen(){

}
