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
    ControlNode(Type t, string v, int c = NULL, vector<string> var = {}) : type(t), value(v), next(c), var(var) {}
};

struct StackNode
{
    Type type;
    string value;
    int current;
    int next;
    vector<string> var;
    vector<StackNode> children;
};

struct Environment
{
    int env;
    vector<string> var;
    vector<string> val;
    Environment *parent;
    Environment(int e, vector<string> v, vector<string> val, Environment *p = nullptr) : env(e), var(v), val(val), parent(p) {}
};

vector<vector<ControlNode>>
getControlStruct(Node *root);
void execute(vector<vector<ControlNode>> contStruc);

#endif