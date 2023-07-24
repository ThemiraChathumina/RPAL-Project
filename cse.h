#ifndef CSE_H
#define CSE_H

#include "st.h"

struct CSENode
{
    string value;
    int next;
    int current;
    vector<string> var;
    CSENode(string v, int n, int c, vector<string> v2) : value(v), next(n), current(c), var(v2) {}
};

vector<vector<Node>> getControlStruct(Node *root);
void execute(vector<vector<Node>> contStruc);

#endif