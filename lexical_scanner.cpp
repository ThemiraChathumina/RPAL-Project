#include <iostream>
#include <string>
#include <cctype>
#include <fstream>
#include <vector>
#include "lexical_scanner.h"
using namespace std;

// Helper function to check if a character is an operator symbol
bool isOperatorSymbol(char c)
{
    string operatorSymbols = "+-<*>&.@/:=~|$!#%^_[]{}\"'?";
    return operatorSymbols.find(c) != string::npos;
}

// Function to get the next token from the input string
Token getNextToken(string &input)
{
    Token token;
    int i = 0;

    // Skip over any initial white spaces
    while (isspace(input[i]))
        i++;

    // Check if the next token is an identifier
    if (isalpha(input[i]))
    {
        token.type = IDENTIFIER;
        while (isalnum(input[i]) || input[i] == '_')
        {
            token.value += input[i];
            i++;
        }
    }
    // Check if the next token is a comment
    else if (input.substr(i, 2) == "//")
    {
        token.type = DELETE;
        while (input[i] != '\n')
        {
            i++;
        }
    }
    // Check if the next token is an integer
    else if (isdigit(input[i]))
    {
        token.type = INTEGER;
        while (isdigit(input[i]))
        {
            token.value += input[i];
            i++;
        }
    }
    // Check if the next token is a string
    else if (input[i] == '\'')
    {
        token.type = STRING;
        i++;
        while (input[i] != '\'')
        {
            if (input[i] == '\\')
            {
                i++;
                if (input[i] == 't')
                    token.value += '\t';
                else if (input[i] == 'n')
                    token.value += '\n';
                else if (input[i] == '\\')
                    token.value += '\\';
                else if (input[i] == '\'')
                    token.value += '\'';
            }
            else
            {
                token.value += input[i];
            }
            i++;
        }
        i++;
    }
    // Check if the next token is an operator
    else if (isOperatorSymbol(input[i]))
    {
        token.type = OPERATOR;
        while (isOperatorSymbol(input[i]))
        {
            token.value += input[i];
            i++;
        }
    }
    // Check if the next token is punctuation
    else if (input[i] == '(' || input[i] == ')' || input[i] == ';' || input[i] == ',')
    {
        token.type = PUNCTION;
        token.value = input[i];
        i++;
    }

    // Remove the processed characters from the input string
    input.erase(0, i);

    // Return the next token
    return token;
}

// Function to tokenize the contents of a file and return a vector of tokens
vector<Token> tokenizeFile(string fileName)
{
    vector<Token> tokens;

    // Open the input file and read its contents into a string
    ifstream inputFile(fileName);
    string input((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();
    // Append a new line character at the end of the input string
    input += '\n';
    // Strip initial white spaces, new lines, end of lines, and horizontal tabs
    int i = 0;
    while (isspace(input[i]))
        i++;
    input.erase(0, i);

    // Tokenize the input string using the getNextToken function
    while (!input.empty())
    {
        Token token = getNextToken(input);
        if (input.empty())
            break;
        if (token.type != DELETE)
            tokens.push_back(token);
    }
    return tokens;
}
