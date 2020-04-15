/********************************************************
 * Tyler Wolf 
 * 
 * Simple Node class header file for use in compiler project
 *
 ********************************************************/

#ifndef CP_NODE_
#define CP_NODE_

#include <string>
#include <map>
#include <iostream>

typedef struct symbol {
    std::string 	name;
    std::string 	description;
    std::string 	type;
    int         	len; /* used for array variables */
    int 			num_params; /* used for functions */
    bool			assigned;
    bool			is_array;
} symbol;

extern std::map<int, std::map<std::string, symbol>> symbolTable;

class Node {
public:
	Node();
	Node(std::string s);
	Node(int a);

	void init();

	virtual ~Node();

	Node* getLeftChild();
	Node* getRightChild();
	void setLeftChild(Node* n);
	void setRightChild(Node* n);

	void setRightSib(Node* n);
	Node* getRightSib() { return right_sib; }

	void setName(std::string s);
	void setLabel(std::string s);
	void setJumpTo(std::string s);
	void setID(std::string s);
	void setJumpOn(bool b);
	void setJumpAlways();

	std::string getName();
	std::string getLabel();
	std::string getJumpTo();
	std::string getID();
	bool getJumpOn();
	bool getJumpAlways();

	void walkTreePrint(int depth=0);

	// For Semantic Analysis
	int walkTreeCheckSymbols(int scope=0);

	// For Generating IR
	void walkTreeGenerateIR(std::string s="default_name");
	void codegen();

private:

	Node* left_child;
	Node* right_child;

	// We'll allow for up to 2 siblings (left and right)
	Node* left_sib;
	Node* right_sib;

	// Maintain various strings via a map
	//	-  name
	// 	-  jump_to (target label)
	// 	-  label   
	//  -  ID
	std::map<std::string, std::string> attributes;
	bool jump_on;
	bool jump_always;

	int val;
	bool raw_val;
	bool has_id;

};

#endif //CP_NODE_