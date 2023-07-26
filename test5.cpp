#include "st.h"
#include "lexical_scanner.h"
#include "ast.h"
#include <iostream>
#include "cse.h"
using namespace std;
int main(int argc, char **argv)
{

    string filename = argv[argc - 1];
    Node *root = getST(filename);
    control_node temp;
    generateControlStruct(root, &temp);
    initializeCSE(&temp);
    execute();
    return 0;
}
