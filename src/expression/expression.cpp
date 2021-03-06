/*
	*** Mathematical Expression
	*** src/expression/expression.cpp
	Copyright T. Youngs 2015-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "expression/expression.h"
#include "expression/variable.h"
#include "expression/function.h"
#include "expression/expression_grammar.hh"
#include "expression/value.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <stdarg.h>
#include <string.h>

// Static members
Expression* Expression::target_ = NULL;

// Constructors
Expression::Expression()
{
	// Private variables
	generateMissingVariables_ = false;

	// Initialise
	clear();
}

Expression::Expression(const char* commands)
{
	// Initialise
	clear();

	// Generate expression
	generate(commands);
}

// Destructor
Expression::~Expression()
{
	clear();
}

/*
 * Creation
 */

// Reset values in parser, ready for next source
void Expression::resetParser()
{
	stringPos_ = -1;
	tokenStart_ = 0;
	functionStart_ = -1;
	stringSource_.clear();
	stringLength_ = 0;
	useAdditionalConstants_ = false;
	target_ = NULL;
}

// Clear contents of expression
void Expression::clear()
{
	nodes_.clear();
	statements_.clear();

	// Clear variables and constants, except those that are persistent
	variables_.removeIfData(false);
	constants_.removeIfData(false);

	isValid_ = false;
}

// Set flag to specify that missing variables should be generated
void Expression::setGenerateMissingVariables(bool generate)
{
	generateMissingVariables_ = generate;
}

// Return whether missing variables will be generated
bool Expression::generateMissingVariables()
{
	return generateMissingVariables_;
}

// Get next character from current input stream
char Expression::getChar()
{
	char c = 0;

	// Are we at the end of the current string?
	if (stringPos_ == stringLength_) return 0;

	// Return current char
	c = stringSource_[stringPos_];
	stringPos_++;
	return c;
}

// Peek next character from current input stream
char Expression::peekChar()
{
	return (stringPos_ == stringLength_ ? 0 : stringSource_[stringPos_]);
}

// 'Replace' last character read from current input stream
void Expression::unGetChar()
{
	--stringPos_;
}

// Generate an expression
bool Expression::generate(const char* expressionText)
{
	resetParser();
	target_ = this;

	stringSource_ = expressionText;
	stringSource_ += ';';
	stringPos_ = 0;
	stringLength_ = stringSource_.length();
	Messenger::printVerbose("Parser source string is '%s', length is %i\n", stringSource_.get(), stringLength_);

	// Perform the parsing
	isValid_ = ExpressionParser_parse() == 0;

	target_ = NULL;

	return isValid_;
}

// Return whether current expression is valid
bool Expression::isValid()
{
	return isValid_;
}

// Return current expression target
Expression *Expression::target()
{
	return target_;
}

/*
 * Execution
 */

// Print expression
void Expression::print()
{
	printf("Leaf Structure (%i statements):\n", statements_.nItems());
	int n=1;
	for (RefListItem<ExpressionNode,int> *ri = statements_.first(); ri != NULL; ri = ri->next)
	{
		printf("-------------------------------------------------------------\n");
		printf("Statement %i:\n", n);
		printf("item pointer is %p\n", ri->item);
		ri->item->nodePrint(1);
		n ++;
	}
	printf("-------------------------------------------------------------\n");
}

/*
 * Statement / Command Addition
 */

// Add a node representing a whole statement to the execution list
bool Expression::addStatement(ExpressionNode* leaf)
{
	if (leaf == NULL)
	{
		printf("Internal Error: NULL Node passed to Expression::addStatement().\n");
		return false;
	}
	Messenger::printVerbose("Added statement node %p\n", leaf);
	leaf->setParent(this);
	statements_.add(leaf);

	return true;
}

// Add an operator to the Expression
ExpressionNode* Expression::addOperator(ExpressionFunctions::Function func, ExpressionNode* arg1, ExpressionNode* arg2)
{
	if (arg1 && (!arg1->returnsNumber())) return NULL;
	if (arg2 && (!arg2->returnsNumber())) return NULL;

	// Create new command node
	ExpressionFunction* leaf = new ExpressionFunction(func);
	nodes_.own(leaf);
	Messenger::printVerbose("Added operator '%s' (%p)...\n", ExpressionFunctions::data[func].keyword, leaf);
	// Add arguments and set parent
	leaf->addArguments(1,arg1);
	leaf->setParent(this);
	if (arg2 != NULL) leaf->addArguments(1,arg2);
	leaf->setReturnsNumber(ExpressionFunctions::data[func].returnsNumber);

	return leaf;
}

// Add function-based leaf node to topmost branch on stack
ExpressionNode* Expression::addFunctionNodeWithArglist(ExpressionFunctions::Function func, ExpressionNode* arglist)
{
	// Create new command node
	ExpressionFunction* leaf = new ExpressionFunction(func);
	nodes_.own(leaf);
	Messenger::printVerbose("Added function '%s' (%p)...\n", ExpressionFunctions::data[func].keyword, leaf);

	// Add argument list to node and set parent
	leaf->addJoinedArguments(arglist);
	leaf->setParent(this);

	// Store the function's return type
	leaf->setReturnsNumber(ExpressionFunctions::data[func].returnsNumber);

	// Check that the correct arguments were given to the command and run any prep functions
	if (!leaf->checkArguments(ExpressionFunctions::data[func].arguments, ExpressionFunctions::data[func].keyword))
	{
		Messenger::printVerbose("Error: Function syntax is '%s(%s)'.\n", ExpressionFunctions::data[func].keyword, ExpressionFunctions::data[func].argText);
		leaf = NULL;
	}

	return leaf;
}

// Add a function node to the list (overloaded to accept simple arguments instead of a list)
ExpressionNode* Expression::addFunctionNode(ExpressionFunctions::Function func, ExpressionNode* a1, ExpressionNode* a2, ExpressionNode* a3, ExpressionNode* a4)
{
	// Create new command node
	ExpressionFunction* leaf = new ExpressionFunction(func);
	nodes_.own(leaf);
	Messenger::printVerbose("Added function '%s' (%p)...\n", ExpressionFunctions::data[func].keyword, leaf);

	if (a1 != NULL) leaf->addArgument(a1);
	if (a2 != NULL) leaf->addArgument(a2);
	if (a3 != NULL) leaf->addArgument(a3);
	if (a4 != NULL) leaf->addArgument(a4);
	leaf->setParent(this);

	// Store the function's return type
	leaf->setReturnsNumber(ExpressionFunctions::data[func].returnsNumber);

	// Check that the correct arguments were given to the command and run any prep functions
	if (!leaf->checkArguments(ExpressionFunctions::data[func].arguments, ExpressionFunctions::data[func].keyword))
	{
		Messenger::printVerbose("Error: Function syntax is '%s(%s)'.\n", ExpressionFunctions::data[func].keyword, ExpressionFunctions::data[func].argText);
		leaf = NULL;
	}

	return leaf;
}

// Add value node targetting specified variable
ExpressionNode* Expression::addValueNode(ExpressionVariable* var)
{
	ExpressionValue* vnode = new ExpressionValue(var);
	nodes_.own(vnode);
	vnode->setParent(this);

	return vnode;
}

// Link two arguments together with their member pointers
ExpressionNode* Expression::joinArguments(ExpressionNode* arg1, ExpressionNode* arg2)
{
	arg1->prevArgument = arg2;
	arg2->nextArgument = arg1;
	Messenger::printVerbose("Joining arguments %p and %p\n", arg1, arg2);

	return arg1;
}

// Join two commands together
ExpressionNode* Expression::joinCommands(ExpressionNode* node1, ExpressionNode* node2)
{
	ExpressionFunction* leaf = new ExpressionFunction(ExpressionFunctions::Joiner);
	nodes_.own(leaf);
	leaf->setParent(this);
	if (node1 != NULL) leaf->addArgument(node1);
	if (node2 != NULL) leaf->addArgument(node2);

	Messenger::printVerbose("Joined command nodes %p and %p (joiner node is %p)\n", node1, node2, leaf);

	return leaf;
}

/*
 * Variables / Constants
 */

// Add double constant
ExpressionVariable* Expression::createConstant(double d, bool persistent)
{
	ExpressionVariable* var = new ExpressionVariable();
	var->set(d);
	var->setReadOnly();

	// If persistent, add to the persistent nodes list
	if (persistent) persistentNodes_.own(var);
	else nodes_.own(var);

	constants_.add(var, persistent);

	return var;
}

// Add variable to topmost scope
ExpressionVariable* Expression::createVariable(const char* name, bool persistent, ExpressionNode* initialValue)
{
	Messenger::printVerbose("A new variable '%s' is being created.\n", name);

	ExpressionVariable* var = new ExpressionVariable;
	var->setName(name);
	if (!var->setInitialValue(initialValue))
	{
		delete var;
		Messenger::print("Failed to set initial value for variable.\n");
		return NULL;
	}

	// If persistent, add to the persistent nodes list
	if (persistent) persistentNodes_.own(var);
	else nodes_.own(var);

	variables_.add(var, persistent);

	Messenger::printVerbose("Created variable '%s'.\n", name);

	return var;
}

// Add variable, with double as initial value source
ExpressionVariable* Expression::createVariableWithValue(const char* name, double initialValue, bool persistent)
{
	ExpressionVariable* var = createVariable(name, persistent);

	Messenger::printVerbose("Setting initial value of variable '%s' to '%e'.\n", name, initialValue);
	var->set(initialValue);

	return var;
}

// Search for variable in current scope
ExpressionVariable* Expression::variable(const char* name)
{
	// Search global scope first
	ExpressionVariable* result = NULL;

	for (RefListItem<ExpressionVariable,bool>* ri = variables_.first(); ri != NULL; ri = ri->next)
	{
		if (DissolveSys::sameString(ri->item->name(), name))
		{
			result = ri->item;
			break;
		}
	}

	if (result == NULL) Messenger::printVerbose("...variable '%s' not found.\n", name);
	else Messenger::printVerbose("...variable '%s' found.\n", name);

	return result;
}

// Return variables
RefList<ExpressionVariable,bool>& Expression::variables()
{
	return variables_;
}

// Return constants
RefList<ExpressionVariable,bool>& Expression::constants()
{
	return constants_;
}

/*
 * Execution
 */

// Execute expression
double Expression::execute(bool& success)
{
	double expressionResult = 0.0;

	for (RefListItem<ExpressionNode,int> *ri = statements_.first(); ri != NULL; ri = ri->next)
	{
// 		ri->item->nodePrint(1);
		success = ri->item->execute(expressionResult);
		if (!success) break;
	}

	// Print some final verbose output
// 	Messenger::print("Final result from expression = %f\n", expressionResult);
	if (!success) Messenger::warn("Execution FAILED.\n");

	return expressionResult;
}
