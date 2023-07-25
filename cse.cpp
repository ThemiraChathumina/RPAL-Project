#include "cse.h"
#include <stack>
#include <iostream>
#include <tuple>

using namespace std;

using Value = tuple<string, Type>;

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

Value check_environment(Environment *current, string variable)
{
    while (current != nullptr)
    {
        for (int i = 0; i < current->var.size(); i++)
        {
            if (current->var[i] == variable)
            {
                Value myTuple = make_tuple(current->val[i], current->types[i]);
                return myTuple;
            }
        }
        current = current->parent;
    }
    Value error = make_tuple("error", ID);
    return error;
}

void execute(vector<vector<ControlNode *>> contStruc)
{
    stack<ControlNode *> controlStack;
    stack<StackNode *> stackStack;
    Environment *env = new Environment(0, {}, {}, {});
    ControlNode *control_e0 = new ControlNode(ENV, "0");
    controlStack.push(control_e0);
    StackNode *stack_e0 = new StackNode(ENV, "0");
    stackStack.push(stack_e0);
    int current_environment_id = 0;
    Environment *current_environment = env;
    for (int j = 0; j < contStruc[0].size(); j++)
    {
        controlStack.push(contStruc[0][j]);
    }
    while (true)
    {
        ControlNode *c = controlStack.top();
        StackNode *s = stackStack.top();
        if (c->type == ID)
        {
            controlStack.pop();
            Value value = check_environment(current_environment, c->value);
            string val = get<0>(value);
            Type type = get<1>(value);
            if (val == "error" && type == ID)
            {
                cout << "Error: Variable " << c->value << " not found in the environment." << endl;
                return;
            }
            else
            {
                StackNode *newNode = new StackNode(type, val, current_environment_id);
                stackStack.push(newNode);
            }
        }
    }
}