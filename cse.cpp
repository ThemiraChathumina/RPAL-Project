#include "cse.h"
using namespace std;
int structNum = 0;
vector<CSENode> traverse(Node *node, vector<vector<CSENode>> &contStruct, int curr){
    if (node->value == "lambda"){
        contStruct[curr].push_back(CSENode(node->value, ++structNum, -1, vector<string>(node->)));
    }
    else{
        contStruct[curr].push_back(CSENode(node->value, -1, -1, vector<string>()));
        if (node->left != NULL){
            traverse(node->left, contStruct, curr);
        }
        if (node->right != NULL){
            traverse(node->right, contStruct, curr);
        }
    }
}
vector<vector<CSENode>> getControlStruct(Node *root){
    vector<vector<CSENode>> contStruc;
    vector<CSENode> temp;

}
void execute(vector<vector<CSENode>> contStruc);