#ifndef AST_H
#define AST_H

#include "lexical_scanner.h"

enum ASTType
{
    IDT,
    STR,
    INT,
    TAU,
    LAMBDA,
    WHERE,
    LET,
    AUG,
    COND,
    AMP,
    OR,
    NOT,
    GR,
    GE,
    LS,
    LE,
    EQ,
    NE,
    PLUS,
    MINUS,
    NEG,
    MULTI,
    DIV,
    EXPO,
    AT,
    GAMMA,
    TRUE,
    FALSE,
    NIL,
    DUMMY,
    WITHIN,
    AND,
    REC,
    EQUAL,
    FUN_FORM,
    PARAN,
    COMMA,
    YSTAR,
    DELTA_THEN,
    DELTA_ELSE,
    BETA,
    ENV
};
struct Node
{
    string value;
    ASTType type;
    Node *left;
    Node *right;
    Node(string v, ASTType t, Node *l = nullptr, Node *r = nullptr) : value(v), type(t), left(l), right(r) {}
};

Node *getAST(string filename);
void printAST(Node *node);

#endif
