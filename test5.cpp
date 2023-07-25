#include "st.h"
#include "lexical_scanner.h"
#include "ast.h"
#include <iostream>

// Node *get_sample()
// {
//     Node *root = new Node("gamma", NAN);
//     root->left = new Node("Print", IDENTIFIER);
//     root->left->right = new Node("*", NAN);
//     root->left->right->left = new Node("gamma", NAN);
//     root->left->right->left->right = new Node("2", INTEGER);
//     root->left->right->left->left = new Node("lambda", NAN);
//     root->left->right->left->left->right = new Node("4", INTEGER);
//     root->left->right->left->left->left = new Node("x", IDENTIFIER);
//     root->left->right->left->left->left->right = new Node("-", NAN);
//     root->left->right->left->left->left->right->left = new Node("x", IDENTIFIER);
//     root->left->right->left->left->left->right->left->right = new Node("1", INTEGER);
//     return root;
// }

int main()
{

    Node *root = getST("1.txt");
    // Node *root = get_sample();
    printAST(root);
    return 0;
}
