#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <stack>
#include <map>

typedef std::string string;
typedef std::vector<std::string> stringVector;
typedef std::vector<std::string>::const_iterator stringIter;
typedef std::map<std::string, bool> boolMap;


std::vector<std::string> getTokens(const std::string& inputString);
bool isOperator(const char& ch);
bool isParentheses(const char& ch);
bool isVariable(const std::string& s);
bool isOperator(const std::string& s);
bool isLeftParen(const std::string& s);
bool isRightParen(const std::string& s);
int getPrecedence(const std::string& s);
std::vector<std::string> getVariables(
                             const std::vector<std::string>& tokenVector);
std::vector<std::string> convertRPN(
                             const std::vector<std::string>& tokenVector);
bool evalRPN(const std::vector<std::string>& tokenVector, 
        const std::map<std::string, bool>& bmap);
std::string evalExpression(const std::string& arg1, const std::string& arg2,
                           const std::string& op, 
                           const std::map<std::string,bool>& bmap);
void setValues(std::map<std::string, bool>& bmap,
               const std::vector<std::string>& variableVector, int comboGen);
std::vector<std::string> combineVectors(const std::vector<std::string>& vector1,
                                    const std::vector<std::string>& vector2);

int main() {
    std::vector<std::string> tokenVector1, tokenVector2;
    std::vector<std::string> variableVector1, variableVector2;
    std::vector<std::string> variableVectorUnion;
    std::vector<std::string> RPNVector1, RPNVector2;
    std::string inputString1 = "((P v Q) ^ (Q -> R) XOR (P ^ R)) <-> (R ^ Q)";
    std::string inputString2 = "(P v R)";
    std::map<std::string,bool> bmap;
    bool result1, result2;

    tokenVector1 = getTokens(inputString1);
    tokenVector2 = getTokens(inputString2);

    variableVector1 = getVariables(tokenVector1);
    variableVector2 = getVariables(tokenVector2);
    variableVectorUnion = combineVectors(variableVector1, variableVector2);

    RPNVector1 = convertRPN(tokenVector1);
    RPNVector2 = convertRPN(tokenVector2);
    
    bool validConclusion = true;

    bmap["c"] = false;
    bmap["t"] = true;

    int comboGen = 1;

    for(int i = 0; i < variableVectorUnion.size(); i++) {
        comboGen *= 2;
    }

    comboGen--;

    while(comboGen >= 0) {
        setValues(bmap, variableVectorUnion, comboGen);

        result1 = evalRPN(RPNVector1, bmap);
        result2 = evalRPN(RPNVector2, bmap);

        for(std::vector<std::string>::const_iterator i = variableVectorUnion.begin();
                i != variableVectorUnion.end(); ++i) {
            if(bmap.at(*i)) {
                std::cout << "T ";
            }

            else {
                std::cout << "F ";
            }
        }

        if(result1) {
            std::cout << "T ";
        }

        else {
            std::cout << "F ";
        }

        if(result2) {
            std::cout << "T ";
        }

        else {
            std::cout << "F ";
        }

        if(result1 != result2) {
            std::cout << "Invalid!";
            validConclusion = false;
        }

        std::cout << "\n";
        comboGen--;
    }

    if(validConclusion) {
        std::cout << "Valid conclusion. All truth table values line up.\n";
    }

    else {
        std::cout << "Invalid conclusion. Invalid truth table values are "
                  << "labelled.\n";
    }

    return 0;
}

std::vector<std::string> getTokens(const std::string& inputString) {
    std::vector<std::string> tokenVector;
    std::string currentString = "";

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

bool isVariable(const std::string& s) {
    return s.length() == 1 && isalpha(s[0]) && s[0] != 'v';
}

bool isOperator(const std::string& s) {
    std::string validOperators[] = {"!", "v", "^", "->", "<->", "XOR"};

    for(int i = 0; i < 6; i++) {
        if(s == validOperators[i]) {
            return true;
        }
    }

    return false;
}

bool isLeftParen(const std::string& s) {
    return s.length() == 1 && s[0] == '(';
}

bool isRightParen(const std::string& s) {
    return s.length() == 1 && s[0] == ')';
}

int getPrecedence(const std::string& s) {
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

std::vector<std::string> getVariables(
                                const std::vector<std::string>& tokenVector) {
    std::vector<std::string> variableVector;
    for(std::vector<std::string>::const_iterator i = tokenVector.begin();
                            i != tokenVector.end(); ++i) {
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

std::vector<std::string> convertRPN(
                               const std::vector<std::string>& tokenVector) {
    std::vector<std::string> RPNVector;
    std::stack<std::string> operatorStack;
    int precedence;

    for(std::vector<std::string>::const_iterator i = tokenVector.begin();
                                        i != tokenVector.end(); ++i) {
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

bool evalRPN(const std::vector<std::string>& tokenVector, 
             const std::map<std::string, bool>& bmap) {
    std::stack<std::string> variableStack;
    std::string arg1, arg2;

    for(std::vector<std::string>::const_iterator i = tokenVector.begin();
                                        i != tokenVector.end(); ++i) {
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
                    variableStack.push(evalExpression(arg1, arg2, *i, bmap));
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

std::string evalExpression(const std::string& arg1, const std::string& arg2,
                           const std::string& op,
                           const std::map<std::string,bool>& bmap) {
    bool bArg1, bArg2;

    if(op == "!") {
        bArg1 = bmap.at(arg1);
        if(bArg1) {
            return "c";
        }

        else {
            return "t";
        }
    }

    else {
        bArg1 = bmap.at(arg1);
        bArg2 = bmap.at(arg2);

        // Logical AND.
        if(op == "v") {
            if(bArg1 && bArg2) {
                return "t";
            }

            else {
                return "c";
            }
        }

        // Logical OR.
        if(op == "^") {
            if(bArg1 || bArg2) {
                return "t";
            }

            else {
                return "c";
            }
        }

        // Logical XOR.
        if(op == "XOR") {
            if((bArg1 || bArg2) && !(bArg1 && bArg2)) {
                return "t";
            }

            else {
                return "c";
            }
        }

        // Implies operator.
        if(op == "->") {
            if(!bArg1 || bArg2) {
                return "t";
            }

            else {
                return "c";
            }
        }

        // If and only if operator.
        if(op == "<->") {
            if((bArg1 && bArg2) || (!bArg1 && bArg2)) {
                return "t";
            }

            else {
                return "c";
            }
        }

        else {
            std::cout << "Invalid operator!\n";
            return "c";
        }
    }
}

void setValues(std::map<std::string, bool>& bmap,
               const std::vector<std::string>& variableVector, int comboGen) {
    int j = 0;
    for(std::vector<std::string>::const_iterator i = variableVector.begin();
            i != variableVector.end(); i++) {
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

std::vector<std::string> combineVectors(const std::vector<std::string>& vector1,
                                    const std::vector<std::string>& vector2) {
    std::vector<std::string> newVector = vector1;

    for(std::vector<std::string>::const_iterator i = vector2.begin();
            i != vector2.end(); ++i) {
        if(std::find(newVector.begin(), 
                     newVector.end(), *i) == newVector.end()) {
            newVector.push_back(*i);
        }
    }

    std::sort(newVector.begin(), newVector.end());

    return newVector;
}
