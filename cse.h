#ifndef CSE_H
#define CSE_H

#include "st.h"

enum Type
{
    INT,
    STRING,
    BOOL,
    OP,
    TAU,
    LAMBDA,
    GAMMA,
    DELTA,
    BETA,
    ENV,
    ID,
    LIST
};

struct ControlNode
{
    Type type;
    string value;
    int next;
    vector<string> var;
    ControlNode(Type t, string v, int c = -1, vector<string> var = {}) : type(t), value(v), next(c), var(var) {}
};

struct StackNode
{
    Type type;
    string value;
    int current;
    int next;
    vector<string> var;
    vector<StackNode> children;
    StackNode(Type t, string v, int c = -1, int n = -1, vector<string> var = {}) : type(t), value(v), current(c), next(n), var(var) {}
};

struct Environment
{
    int env;
    vector<string> var;
    vector<string> val;
    vector<Type> types;
    Environment *parent;
    Environment(int e, vector<string> v, vector<string> va, vector<Type> t, Environment *p = nullptr) : env(e), var(v), val(va), types(t), parent(p) {}
};

vector<vector<ControlNode *>>
getControlStruct(Node *root);
void execute(vector<vector<ControlNode *>> contStruc);

#endif