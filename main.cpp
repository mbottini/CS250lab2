#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stack>
#include <map>

// Some typedefs to make things less verbose.
typedef std::string string;

typedef std::vector<string> stringVector;
typedef stringVector::const_iterator stringIter;
typedef std::map<string, bool> boolMap;

// Parses the input into tokens. "(A v B) ^ C <-> D" becomes
// a vector with the following elements: "(", "A", "v", "B", ")", "^", "C",
// "<->", and "D".
// |
// v
stringVector getTokens(const string& inputString);

// Boolean functions to make it easier to understand how the Shunting-Yard
// algorithm works.
// |
// v
bool isOperator(const char& ch);
bool isParentheses(const char& ch);
bool isVariable(const string& s);
bool isOperator(const string& s);
bool isLeftParen(const string& s);
bool isRightParen(const string& s);
int getPrecedence(const string& s);

// Obtains the variables from the vector of tokens. A variable is any
// alphabet character that isn't "v", "c", or "t". Those are reserved for
// the OR operator, logical falsity, and logical trith, respectively.
// |
// v
stringVector getVariables(const stringVector& tokenVector);

// Uses Djikstra's Shunting-Yard Algorithm to turn infix notation into
// Reverse Polish Notation.
// |
// v
stringVector convertRPN(const stringVector& tokenVector);

// Given a vector in RPN and a map of Boolean values, evaluate the result.
// |
// v
bool evalRPN(const stringVector& tokenVector, const boolMap& bmap);

// Used inside evalRPN to evaluate individual pairs of arguments.
// |
// v
string evalExpression(const string& arg1, const string& arg2,
                      const string& op, const boolMap& bmap);

// Given an integer value, take the bits of that integer and use them to
// set the truth-false values in bmap.
// For example, providing 5 (101) as the generator integer and the elements of
// "p", "q", and "r" in the vector of variables will make bmap set the following
// values: bmap["p"] = true, bmap["q"] = false, bmap["r"] = true.
// |
// v
void setValues(boolMap& bmap, const stringVector& variableVector, int comboGen);

// Combines two vectors, eliminating duplicates,
// and also sorts them. This will then get plugged into setValues to generate 
// truth-false tables for all variables.
// |
// v
stringVector combineVectors(const stringVector& vector1, 
                            const stringVector& vector2);

void printBool(bool b);
void printSpaces(int n);

int main() {
    stringVector tokenVector1, tokenVector2;
    stringVector variableVector1, variableVector2;
    stringVector variableVectorUnion;
    stringVector RPNVector1, RPNVector2;
    string inputString1, inputString2;
    boolMap bmap;
    bool result1, result2;

    std::cout << "Premise: ";
    getline(std::cin, inputString1);
    std::cout << "Conclusion: ";
    getline(std::cin, inputString2);

    tokenVector1 = getTokens(inputString1);
    tokenVector2 = getTokens(inputString2);

    variableVector1 = getVariables(tokenVector1);
    variableVector2 = getVariables(tokenVector2);
    variableVectorUnion = combineVectors(variableVector1, variableVector2);

    RPNVector1 = convertRPN(tokenVector1);
    RPNVector2 = convertRPN(tokenVector2);
    
    bool validConclusion = true;

    // Set initial values of bmap. "c" and "t" never change. ComboGen
    // is initially set to make every variable "true."
    bmap["c"] = false;
    bmap["t"] = true;

    int comboGen = 1;

    for(int i = 0; i < variableVectorUnion.size(); i++) {
        comboGen *= 2;
    }

    comboGen--;

    // Print out the top part of the output table.

    int colWidth1 = inputString1.length();
    int colWidth2 = inputString2.length();

    for(stringIter i = variableVectorUnion.begin(); i !=
            variableVectorUnion.end(); ++i) {
        std::cout << *i << " ";
    }
    std::cout << "| ";

    std::cout << inputString1 << " | " << inputString2 << "\n";

    // Print the truth table values.
    while(comboGen >= 0) {
        setValues(bmap, variableVectorUnion, comboGen);

        result1 = evalRPN(RPNVector1, bmap);
        result2 = evalRPN(RPNVector2, bmap);

        for(stringIter i = variableVectorUnion.begin();
                i != variableVectorUnion.end(); ++i) {
            printBool(bmap.at(*i)); 
            std::cout << " ";
        }

        std::cout << "|";
        printSpaces(colWidth1 / 2);

        if(inputString1.length() % 2 == 1) {
            std::cout << " ";
        }

        printBool(result1);
        printSpaces(colWidth1 / 2);

        std::cout << " |";
        printSpaces(colWidth2 / 2); 

        if(inputString2.length() % 2 == 1) {
            std::cout << " ";
        }

        printBool(result2);
        printSpaces(colWidth2 / 2);

        if(result1 != result2) {
            std::cout << " Invalid!";
            validConclusion = false;
        }

        std::cout << "\n";
        comboGen--;
    }

    if(validConclusion) {
        std::cout << "Valid statement. All truth table values line up.\n";
    }

    else {
        std::cout << "Invalid statement. Invalid truth table values are "
                  << "labelled.\n";
    }

    return 0;
}

stringVector getTokens(const string& inputString) {
    stringVector tokenVector;
    string currentString = "";

    int i = 0;

    while(i < inputString.length()) {

        currentString = "";

        // Test for "implies" operator.
        if(inputString[i] == '-' && inputString.length() - i > 1) {
            if(inputString.substr(i, 2) == "->") {
                tokenVector.push_back("->");
                i += 2;
            }

            else {
                i++;
            }
        }

        // Test for "XOR" operator.
        else if(inputString[i] == 'X' && inputString.length() - i > 2) {
            if(inputString.substr(i, 3) == "XOR") {
                tokenVector.push_back("XOR");
                i += 3;
            }

            else {
                i++;
            }
        }

        // Test for "iff" operator.

        else if(inputString[i] == '<' && inputString.length() - i > 2) {
            if(inputString.substr(i, 3) == "<->") {
                tokenVector.push_back("<->");
                i += 3;
            }

            else {
                i++;
            }
        }

        // Otherwise, it's hopefully an operator ('v', '^', '!', '(', or ')')
        // or a character.      
        else if(isOperator(inputString[i]) || isParentheses(inputString[i]) 
                                           || isalpha(inputString[i])) {
            currentString += inputString[i];
            tokenVector.push_back(currentString);
            i++;
        }

        // If it's not, then it's just ignored.
        else {
            i++;
        }
    }

    return tokenVector;
}

bool isOperator(const char& ch) {
    char validOperators[] = {'!', 'v', '^'};

    for(int i = 0; i < 3; i++) {
        if(ch == validOperators[i]) {
            return true;
        }
    }

    return false;
}

bool isParentheses(const char& ch) {
    return ch == '(' || ch == ')';
}

bool isVariable(const string& s) {
    return s.length() == 1 && isalpha(s[0]) && s[0] != 'v';
}

bool isOperator(const string& s) {
    string validOperators[] = {"!", "v", "^", "->", "<->", "XOR"};

    for(int i = 0; i < 6; i++) {
        if(s == validOperators[i]) {
            return true;
        }
    }

    return false;
}

bool isLeftParen(const string& s) {
    return s.length() == 1 && s[0] == '(';
}

bool isRightParen(const string& s) {
    return s.length() == 1 && s[0] == ')';
}

int getPrecedence(const string& s) {
    if(s == "<->") {
        return 0;
    }

    if(s == "->") {
        return 1;
    }

    if(s == "XOR") {
        return 2;
    }

    if(s == "v") {
        return 3;
    }

    if(s == "^") {
        return 4;
    }

    if(s == "!") {
        return 5;
    }

    else {
        return -1;
    }
}

stringVector getVariables(const stringVector& tokenVector) {
    stringVector variableVector;
    for(stringIter i = tokenVector.begin(); i != tokenVector.end(); ++i) {

        if(i->length() == 1 && isalpha(i->at(0)) && i->at(0) != 'v' &&
                                 i->at(0) != 'c' && i->at(0) != 't') {

            if(!(std::find(variableVector.begin(), variableVector.end(), *i) !=
                    variableVector.end())) {
                variableVector.push_back(*i);
            }
        }
    }

    return variableVector;
}

stringVector convertRPN(const stringVector& tokenVector) {
    stringVector RPNVector;
    std::stack<string> operatorStack;
    int precedence;

    for(stringIter i = tokenVector.begin(); i != tokenVector.end(); ++i) {
        if(isVariable(*i)) {
            RPNVector.push_back(*i);
        }

        else if(isOperator(*i)) {
            precedence = getPrecedence(*i);

            if(precedence == -1) {
                std::cout << "Invalid operator plugged into "
                << "precedence function!\n";
                RPNVector.clear();
                return RPNVector;
            }

            while(!(operatorStack.empty()) && 
                    precedence <= getPrecedence(operatorStack.top())) {
                RPNVector.push_back(operatorStack.top());
                operatorStack.pop();
            }

            operatorStack.push(*i);
        }

        else if(isLeftParen(*i)) {
            operatorStack.push(*i);
        }

        else if(isRightParen(*i)) {
            if(operatorStack.empty()) {
                std::cout << "Parentheses mismatch!\n";
                RPNVector.clear();
                return RPNVector;
            }

            while(operatorStack.top() != "(") {
                if(operatorStack.empty()) {
                    std::cout << "Parentheses mismatch!\n";
                    RPNVector.clear();
                    return RPNVector;
                }

                RPNVector.push_back(operatorStack.top());
                operatorStack.pop();
            }

            operatorStack.pop();
        }
    }

    while(!(operatorStack.empty())) {
        RPNVector.push_back(operatorStack.top());
        operatorStack.pop();
    }

    return RPNVector;
}

bool evalRPN(const stringVector& RPNVector, const boolMap& bmap) {
    std::stack<string> variableStack;
    string arg1, arg2;

    if(RPNVector.empty()) {
        return false;
    }

    // If it's just one variable in the statement, return the variable.
    if(RPNVector.size() == 1) {
        if(evalExpression(RPNVector[0], "", "", bmap) == "t") {
            return true;
        }

        else {
            return false;
        }
    }

    for(stringIter i = RPNVector.begin(); i != RPNVector.end(); ++i) {
        if(isVariable(*i)) {
            variableStack.push(*i);
        }

        // Otherwise, it's an operator.
        else {
            // Unary operator takes one argument.
            if(*i == "!") {
                if(variableStack.size() > 0) {
                    arg1 = variableStack.top();
                    variableStack.pop();
                    arg2 = "";
                    // Evaluate the arguments and push the result onto the
                    // stack.
                    variableStack.push(evalExpression(arg1, arg2, *i, bmap));
                }

                else {
                    std::cout << "Invalid expression! Not enough arguments.\n";
                    return false;
                }
            }

            // Otherwise, it's a binary operator and takes two.
            else {
                if(variableStack.size() > 1) {
                    arg1 = variableStack.top();
                    variableStack.pop();
                    arg2 = variableStack.top();
                    variableStack.pop();
                    variableStack.push(evalExpression(arg2, arg1, *i, bmap));
                }

                else {
                    std::cout << "Invalid expression! Not enough arguments.\n";
                    return false;
                }
            }
        }
    }

    // We should have a single expression at this point - either c or t.

    if(variableStack.size() != 1) {
        std::cout << "Invalid expression! Too many arguments.\n";
        return false;
    }

    if(variableStack.top() == "t") {
        return true;
    }

    else {
        if(variableStack.top() != "c") {
            std::cout << "Invalid expression! It didn't simplify to t or c.\n";
        }
        return false;
    }
}

string evalExpression(const string& arg1, const string& arg2,
                           const string& op, const boolMap& bmap) {
    bool bArg1, bArg2;
    std::string result;

    if(op == "") {
        bArg1 = bmap.at(arg1);
        if(bArg1) {
            return "t";
        }

        else {
            return "c";
        }
    }

    else if(op == "!") {
        bArg1 = bmap.at(arg1);
        if(bArg1) {
            result = "c";
        }

        else {
            result = "t";
        }
    }

    else {
        bArg1 = bmap.at(arg1);
        bArg2 = bmap.at(arg2);

        // Logical AND.
        if(op == "^") {
            if(bArg1 && bArg2) {
                result = "t";
            }

            else {
                result = "c";
            }
        }

        // Logical OR.
        else if(op == "v") {
            if(bArg1 || bArg2) {
                result = "t";
            }

            else {
                result = "c";
            }
        }

        // Logical XOR.
        else if(op == "XOR") {
            if((bArg1 || bArg2) && !(bArg1 && bArg2)) {
                result = "t";
            }

            else {
                result = "c";
            }
        }

        // Implies operator.
        else if(op == "->") {
            if(!bArg1 || bArg2) {
                result = "t";
            }

            else {
                result = "c";
            }
        }

        // If and only if operator.
        else if(op == "<->") {
            if((bArg1 && bArg2) || (!bArg1 && !bArg2)) {
                result = "t";
            }

            else {
                result = "c";
            }
        }

        else {
            std::cout << "Invalid operator!\n";
            result = "c";
        }
    }

    // std::cout << result << "\n";
    return result;
}

void setValues(boolMap& bmap, const stringVector& variableVector, 
                                                  int comboGen) {
    int j = 0;
    for(stringIter i = variableVector.begin(); i != variableVector.end(); i++) {
        if((comboGen >> j) & 0x01) {
            bmap[*i] = true;
        }

        else {
            bmap[*i] = false;
        }
        j++;
    }

    return;
}

stringVector combineVectors(const stringVector& vector1, 
                            const stringVector& vector2) {
    stringVector newVector = vector1;

    for(stringIter i = vector2.begin(); i != vector2.end(); ++i) {
        if(std::find(newVector.begin(), 
                     newVector.end(), *i) == newVector.end()) {
            newVector.push_back(*i);
        }
    }

    std::sort(newVector.begin(), newVector.end());

    return newVector;
}

void printBool(bool b) {
    if(b) {
        std::cout << "T";
    }

    else {
        std::cout << "F";
    }

    return;
}

void printSpaces(int n) {
    for(int i = 0; i < n; i++) {
        std::cout << " ";
    }
    return;
}
