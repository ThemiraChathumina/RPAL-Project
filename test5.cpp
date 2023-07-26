#include "st.h"
#include "lexical_scanner.h"
#include "ast.h"
#include <iostream>
#include "cse.h"
using namespace std;
int main(int argc, char **argv)
{
    string filename = argv[1];
    Node *root = getST(filename);
    // Node *root = get_sample();
    // printAST(root);
    control_node temp;
    generateControlStruct(root, &temp);
    // control_node *temp2 = temp.next;
    // print_control_node(temp2);
    initializeCSE(&temp);
    execute();
    return 0;
}
