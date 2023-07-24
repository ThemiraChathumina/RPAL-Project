#ifndef CSE_H
#define CSE_H

#include "st.h"

vector<vector<Node>> getControlStruct(Node *root);
void execute(vector<vector<Node>> contStruc);

#endif