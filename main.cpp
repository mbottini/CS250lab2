#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

std::vector<std::string> getTokens(std::string inputString);
bool isOperator(char ch);
bool isParentheses(char ch);
std::vector<std::string> getVariables(std::vector<std::string> tokenVector);

int main() {
    std::vector<std::string> tokenVector;
    std::vector<std::string> variableVector;
    std::string inputString = "((P v Q) ^ (Q -> R) XOR (P ^ R)) <-> (R ^ Q)";
    tokenVector = getTokens(inputString);
    variableVector = getVariables(tokenVector);

    std::cout << "Tokens:\n";
    for (std::vector<std::string>::const_iterator i = tokenVector.begin();
            i != tokenVector.end(); ++i) {
        std::cout << *i << " ";
    }
    std::cout << "\nVariables:\n";

    for (std::vector<std::string>::const_iterator i = variableVector.begin();
            i != variableVector.end(); ++i) {
        std::cout << *i << " ";
    }

    std::cout << "\n";

    return 0;
}

std::vector<std::string> getTokens(std::string inputString) {
    std::vector<std::string> tokenVector;
    std::string currentString = "";

    int i = 0;

    while(i < inputString.length()) {

        currentString = "";

        // Test for "implies" operator.
        if(inputString[i] == '-' && currentString.length() - i > 1) {
            if(inputString.substr(i, 2) == "->") {
                currentString = "->";
                tokenVector.push_back(currentString);
                i += 2;
            }

            else {
                i++;
            }
        }

        // Test for "XOR" operator.
        else if(inputString[i] == 'X' && currentString.length() - i > 2) {
            if(inputString.substr(i, 3) == "XOR") {
                currentString = "XOR";
                tokenVector.push_back(currentString);
                i += 3;
            }

            else {
                i++;
            }
        }

        // Test for "iff" operator.

        else if(inputString[i] == '<' && currentString.length() - i > 2) {
            if(inputString.substr(i, 3) == "<=>") {
                currentString = "<=>";
                tokenVector.push_back(currentString);
                i += 3;
            }

            else {
                i++;
            }
        }

        // Otherwise, it's hopefully an operator ('v', '^', '!', '(', or ')')
        // or a character.      
        else if(isOperator(inputString[i]) || isalpha(inputString[i])) {
            currentString += inputString[i];
            tokenVector.push_back(currentString);
            i++;
        }

        // If it's neither, then it's just ignored.
        else {
            i++;
        }
    }

    return tokenVector;
}

bool isOperator(char ch) {
    char validOperators[] = {'!', 'v', '^', '(', ')'};

    for(int i = 0; i < 5; i++) {
        if(ch == validOperators[i]) {
            return true;
        }
    }

    return false;
}

std::vector<std::string> getVariables(std::vector<std::string> tokenVector) {
    std::vector<std::string> variableVector;
    for(std::vector<std::string>::const_iterator i = tokenVector.begin();
                            i != tokenVector.end(); ++i) {
        if(i->length() == 1 && isalpha(i->at(0)) && i->at(0) != 'v') {
            if(!(std::find(variableVector.begin(), variableVector.end(), *i) !=
                    variableVector.end())) {
                variableVector.push_back(*i);
            }
        }
    }

    return variableVector;
}
