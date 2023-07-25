#include "st.h"
#include "lexical_scanner.h"
#include "ast.h"
#include <iostream>
#include "cse.h"

int main()
{

    Node *root = getST("1.txt");
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
