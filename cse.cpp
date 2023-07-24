#include "cse.h"
#include <stack>

vector<vector<ControlNode *>>
getControlStruct(Node *root)
{
    ControlNode *n1 = new ControlNode(GAMMA, "gamma");
    ControlNode *n2 = new ControlNode(GAMMA, "gamma");
    ControlNode *n3 = new ControlNode(OP, "*");
    ControlNode *n4 = new ControlNode(GAMMA, "gamma");
    ControlNode *n5 = new ControlNode(LAMBDA, "lambda", 1, {"x"});
    ControlNode *n6 = new ControlNode(INT, "4");
    ControlNode *n7 = new ControlNode(INT, "2");

    vector<ControlNode *> v1 = {n1, n2, n3, n4, n5, n6, n7};

    ControlNode *m1 = new ControlNode(GAMMA, "gamma");
    ControlNode *m2 = new ControlNode(GAMMA, "gamma");
    ControlNode *m3 = new ControlNode(OP, "-");
    ControlNode *m4 = new ControlNode(ID, "x");
    ControlNode *m5 = new ControlNode(INT, "1");

    vector<ControlNode *> v2 = {m1, m2, m3, m4, m5};

    vector<vector<ControlNode *>> contStruc = {v1, v2};

    return contStruc;
}

void execute(vector<vector<ControlNode *>> contStruc)
{
    stack<ControlNode *> controlStack;
    stack<StackNode *> stateStack;
    Environment *env = new Environment(0, {}, {});
    ControlNode *en = new ControlNode(ENV, "0");
    StackNode *e0 = new StackNode(ENV, "0");
    controlStack.push(en);
    stateStack.push(e0);
    for (int j = 0; j < contStruc[0].size(); j++)
    {
        controlStack.push(contStruc[0][j]);
    }
}