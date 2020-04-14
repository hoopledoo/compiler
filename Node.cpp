/********************************************************
 * Tyler Wolf 
 * 
 * Simple Node class cpp file for use in compiler project
 *
 ********************************************************/

#include "Node.h"

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

void Node::setLoc(int l, int c){
	line = l;
	column = c;
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
    if (s.len) std::cout << "[" << s.len << "]";
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

int checkDeclared(std::string id, int scope, bool is_array){
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
        	 	symbol s;
        	 	s.type = left_child->getName();  
        		s.name = left_child->getRightSib()->getID();  
        		s.len = 0;  
        		s.description = "func";
        		s.is_array = false;
        		if(not addSymbol(s, scope)) success = false;
			}
			else if (attributes["name"] == "varDec"){
        	 	symbol s;
        	 	s.type = left_child->getName();  
        		s.name = left_child->getRightSib()->getID();  
        		s.len = 0;  
        		s.description = "var";
        		s.is_array = false;
        		s.assigned = false;
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
        		if(not addSymbol(s, scope)) success = false;
			}
			else if (attributes["name"] == "param" ){
        	 	symbol s;
        	 	s.type = left_child->getName();  
        		s.name = left_child->right_sib->getID();  
        		s.len = 0;  
        		s.description = "param";
        		s.is_array = false;
        		s.assigned = true;
        		if(not addSymbol(s, scope+1)) success = false;
			}
			else if (attributes["name"] == "arrayParam" ){
        	 	symbol s;
        	 	s.type = left_child->getName();  
        		s.name = left_child->right_sib->getID();  
        		s.len = 0;  
        		s.description = "paramPtr";
        		s.is_array = true;
        		s.assigned = true;
        		if(not addSymbol(s, scope+1)) success = false;
			}			
			else if (attributes["name"] == "compoundStmt" ){
				scope_change = true;
				scope++;
			}

			/* HANDLE ASSIGNMENTS */

			else if (attributes["name"] == "ASSIGN" ){
				if(left_child->getName() == "varIndex"){
					if(not checkDeclared(left_child->left_child->getID(), scope, true)) success = false;
				}
				else{
					if(not checkDeclared(left_child->getID(), scope, false)) success = false;
				}
			}	

			/* HANDLE FUNCTION CALL */
			else if (attributes["name"] == "call" ){
				if(not checkDeclared(left_child->getID(), scope, false)) success = false;
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

void Node::walkTreeGenerateIR(){

}
