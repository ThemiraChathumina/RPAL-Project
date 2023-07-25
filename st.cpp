#include "st.h"

// the binary tree is a first-child, next-sibling tree. So change nodes according to it.

// add functions for each node required by the cse machine.
Node *standerdize_let(Node *node)
{
    if (node == nullptr)
        return node;
    node->value = "gamma";
    node->type = GAMMA;
    if (node->left != nullptr)
    {
        node->left->value = "lambda";
        node->left->type = LAMBDA;
    }
    else
    {
        return node;
    }
    Node *temp = (node->left)->right;
    if (temp == nullptr)
    {
        return node;
    }
    if (node->left->left->right != nullptr)
    {
        node->left->right = node->left->left->right;
        node->left->left->right = temp;
    }
    else
    {
        return node;
    }
    return node;
}

Node *standerdize_where(Node *node)
{
    if (node == nullptr)
    {
        return node;
    }
    node->value = "gamma";
    node->type = GAMMA;

    Node *P, *X, *E;
    if (node->left != nullptr)
    {
        P = node->left;
    }
    else
    {
        return node;
    }
    if (P->right != nullptr)
    {
        if (P->right->type == EQUAL)
        {
            P->right->value = "lambda";
            P->right->type = LAMBDA;
        }
        else
        {
            return node;
        }
    }
    else
    {
        return node;
    }
    X = P->right->left;
    if (X == nullptr)
    {
        return node;
    }
    E = X->right;
    if (E == nullptr)
    {
        return node;
    }
    node->left = P->right;
    P->right = nullptr;
    node->left->right = E;
    X->right = P;
    return node;
}

Node *standerdize_within(Node *node)
{
    if (node == nullptr)
    {
        return node;
    }
    Node *X1 = nullptr, *X2 = nullptr, *E1 = nullptr, *E2 = nullptr;
    if (node->left != nullptr)
    {
        X1 = node->left->left;
    }
    else
    {
        return node;
    }
    if (node->left->right != nullptr)
    {
        X2 = node->left->right->left;
    }
    else
    {
        return node;
    }
    if (X2 != nullptr)
    {
        E2 = X2->right;
    }
    else
    {
        return node;
    }
    if (X1 != nullptr)
    {
        E1 = X1->right;
    }
    else
    {
        return node;
    }
    node->value = "=";
    node->type = EQUAL;
    node->left = X2;
    Node *temp = new Node("gamma", GAMMA);
    X2->right = temp;
    temp = nullptr;
    temp = new Node("lambda", LAMBDA);
    X2->right->left = temp;
    temp->right = E1;
    temp->left = X1;
    X1->right = E2;
    return node;
}

Node *standardize_funform(Node *node)
{
    if (node == nullptr)
    {
        return node;
    }
    node->value = "=";
    node->type = EQUAL;

    Node *P = nullptr, *V = nullptr, *temp_lambda = nullptr;

    P = node->left;
    if (P == nullptr)
    {
        return node;
    }

    V = P->right;
    if (V == nullptr)
    {
        return node;
    }

    while (V->right != nullptr)
    {
        temp_lambda = new Node("lambda", LAMBDA);

        temp_lambda->left = V;

        P->right = temp_lambda;

        P = V;
        V = V->right;
    }

    P->right = V;

    return node;
}

Node *standardize_at(Node *node)
{
    if (node == nullptr)
    {
        return node;
    }

    Node *E1 = nullptr, *N = nullptr, *E2 = nullptr;

    E1 = node->left;
    if (E1 == nullptr)
    {
        return node;
    }

    N = E1->right;
    if (N == nullptr)
    {
        return node;
    }

    E2 = N->right;
    if (E2 == nullptr)
    {
        return node;
    }

    node->value = "gamma";
    node->type = GAMMA;

    Node *temp_gamma = new Node("gamma", GAMMA);

    node->left = temp_gamma;
    temp_gamma->right = E2;
    temp_gamma->left = N;
    N->right = E1;
    E1->right = nullptr;

    return node;
}

Node *standardize_lambda(Node *node)
{
    if (node == nullptr)
    {
        return node;
    }

    Node *V = nullptr, *V_next = nullptr;
    bool standardize = false;

    V = node->left;
    if (V == nullptr)
    {
        return node;
    }

    V_next = V->right;
    if (V_next == nullptr)
    {
        return node;
    }

    while (V_next->right != nullptr)
    {
        standardize = true;

        Node *temp_lambda = new Node("lambda", LAMBDA);

        V->right = temp_lambda;
        temp_lambda->left = V_next;

        V = V_next;
        V_next = V_next->right;
    }

    if (standardize)
        V->right = V_next;

    return node;
}

Node *standardize_and(Node *node)
{
    if (node == nullptr)
    {
        return node;
    }
    Node *eq = nullptr;
    eq = node->left;
    if (eq == nullptr)
    {
        return node;
    }
    Node *new_comma = nullptr, *new_tau = nullptr, *temp = nullptr;

    new_comma = new Node(",", COMMA);
    new_tau = new Node("tau", TAU);

    node->left = new_comma;
    new_comma->right = new_tau;

    new_comma->left = eq->left;
    new_tau->left = new_comma->left->right;
    if (new_comma == nullptr || new_tau == nullptr)
    {
        return node;
    }
    new_comma = new_comma->left;
    new_tau = new_tau->left;

    temp = eq;
    eq = eq->right;
    delete temp;

    while (eq != nullptr)
    {
        new_comma->right = eq->left;
        new_tau->right = new_comma->right->right;
        if (new_comma == nullptr || new_tau == nullptr)
        {
            return node;
        }
        new_comma = new_comma->right;
        new_tau = new_tau->right;

        temp = eq;
        eq = eq->right;
        delete temp;
    }

    new_comma->right = nullptr;
    return node;
}

Node *standardize_rec(Node *node)
{
    if (node == nullptr)
    {
        return node;
    }
    node->value = "=";
    node->type = EQUAL;
    Node *eq = nullptr, *X = nullptr, *E = nullptr;
    eq = node->left;
    if (eq == nullptr)
    {
        return node;
    }
    X = eq->left;
    if (X == nullptr)
    {
        return node;
    }
    E = X->right;
    if (E == nullptr)
    {
        return node;
    }
    node->left = X;

    Node *temp_gamma = new Node("gamma", GAMMA);
    X->right = temp_gamma;

    Node *ystar = new Node("Y*", YSTAR);
    temp_gamma->left = ystar;

    Node *temp_lambda = new Node("lambda", LAMBDA);
    ystar->right = temp_lambda;

    Node *copy_of_x = new Node(X->value, X->type);
    copy_of_x->left = X->left;

    temp_lambda->left = copy_of_x;
    copy_of_x->right = E;
    return node;
}

// Function to perform post-order traversal
void postOrderTraversal(Node *node)
{
    if (node == nullptr)
        return;

    // Traverse left subtree
    postOrderTraversal(node->left);

    // Traverse right subtree
    postOrderTraversal(node->right);

    // Process current node
    if (node->type == LET)
    {
        node = standerdize_let(node);
    }
    else if (node->type == WHERE)
    {
        node = standerdize_where(node);
    }
    else if (node->type == WITHIN)
    {
        node = standerdize_within(node);
    }
    else if (node->type == FUN_FORM)
    {
        node = standardize_funform(node);
    }
    else if (node->type == AT)
    {
        node = standardize_at(node);
    }
    else if (node->type == LAMBDA)
    {
        node = standardize_lambda(node);
    }
    else if (node->type == AND)
    {
        node = standardize_and(node);
    }
    else if (node->type == REC)
    {
        node = standardize_rec(node);
    }
    // add else conditions for each function created above.
}

Node *standerdize(Node *node)
{
    postOrderTraversal(node);
    return node;
}

Node *getST(string filename)
{
    Node *root = getAST(filename);
    root = standerdize(root);
    return root;
}

void printST(Node *node)
{
    printAST(node);
}