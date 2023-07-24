#ifndef CSE_H
#define CSE_H

#include "st.h"

struct CSENode
{
    string value;
    int nextStruct =-1;
    int currentEnv =-1;
    vector<string> var;
    CSENode(string v, int n, int c, vector<string> v2) : value(v), nextStruct(n), currentEnv(c), var(v2) {}
};

vector<vector<CSENode>> getControlStruct(Node *root);
void execute(vector<vector<CSENode>> contStruc);

#endif