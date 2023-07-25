#include "cse.h"
#include <stack>
#include <iostream>
#include <tuple>

using namespace std;

void generateControlStruct(Node *node, control_node *ctrl_node)
{
    if (node == NULL)
    {
        return;
    }

    // if node is lambda. this case won't have any left  child only right sibling
    if (node->type == LAMBDA)
    {
        control_node *temp_ctrl_node = new control_node(node);
        Node *save_right = node->left->right; // because if there isn't comma next few lines will make it NULL
        if (node->left->type != COMMA)
        {
            temp_ctrl_node->bound_variable = node->left;  // if this is comma it would be left child of left child
            temp_ctrl_node->bound_variable->right = NULL; // there could only be one variable
        }
        else
            temp_ctrl_node->bound_variable = node->left->left; // if this is comman it would be left child of left child

        ctrl_node->next = temp_ctrl_node;
        ctrl_node = ctrl_node->next; // move forward

        control_node *new_cs = new control_node;   // yeah design sucks. this will be a memory leak - NOPE resolved it :)
        generateControlStruct(save_right, new_cs); // this  will return the control struct of right child
        // link the new cs
        ctrl_node->lambda_closure = new_cs->next;

        delete new_cs;

        if (node->right != NULL)
            generateControlStruct(node->right, ctrl_node);
    }

    // else if node is tau. this case won't have any left child only right sibling
    else if (node->type == TAU)
    {
        // create new tau node
        control_node *temp_ctrl_node = new control_node;
        temp_ctrl_node->node = node;

        ctrl_node->next = temp_ctrl_node;
        ctrl_node = ctrl_node->next;

        Node *temp_ast_node = node->left;
        int num_children = 0;

        generateControlStruct(temp_ast_node, ctrl_node);

        while (temp_ast_node != NULL)
        {
            num_children++;
            temp_ast_node = temp_ast_node->right;
        }

        // set the num of children in tau
        ctrl_node->child_count = num_children;

        while (ctrl_node->next != NULL)
            ctrl_node = ctrl_node->next; // because generateControlStruct above can generate more than one number of control structs

        if (node->right != NULL)
            generateControlStruct(node->right, ctrl_node);
    }

    // else if node is conditional. this case won't have any left  child only right sibling
    /* if node is CONDITIONAL it breaks the B, T & E part from the right sibling list
       otherwise due to this procedure being a recurisive it will automatically generate the
       control structure for B, T & E */
    else if (node->type == COND)
    {
        // Break the B, T & E and save each of them
        Node *temp_B = NULL, *temp_T = NULL, *temp_E = NULL;

        temp_B = node->left; // shouldn't be NULL
        if (temp_B == NULL)
        {
            cout << "left child (B) is NULL! " << endl;
            return;
        }
        temp_T = temp_B->right; // shoudln't be NULL
        if (temp_T == NULL)
        {
            cout << "middle child (T) is NULL! " << endl;
            return;
        }

        temp_E = temp_T->right; // shouldn't be NULL
        if (temp_E == NULL)
        {
            cout << "right child (E) is NULL! " << endl;
            return;
        }
        // break all right siblings
        temp_B->right = NULL;
        temp_T->right = NULL;
        // temp_E should already be NULL

        // need to generate lambda_closure_then control structure
        // create new AST node of DELTA_THEN type
        Node *temp_lambda_closure_then = new Node("lambda_closure_then", DELTA_THEN);
        // create new control struct
        control_node *temp_ctrl_node = new control_node;
        temp_ctrl_node->node = temp_lambda_closure_then;
        ctrl_node->next = temp_ctrl_node; // link the newly created control struct in the list
        ctrl_node = ctrl_node->next;
        // generate control struct for temp_T;
        control_node *temp_cntst = new control_node;
        generateControlStruct(temp_T, temp_cntst); // temp_cntst's next have control struct
        ctrl_node->lambda_closure = temp_cntst->next;
        delete temp_cntst;

        // need to generate lambda_closure_else control structure
        // create new AST node of DELTA_ELSE type
        Node *temp_lambda_closure_else = new Node("lambda_closure_else", DELTA_ELSE);
        control_node *temp_ctrl_node_else = new control_node;
        temp_ctrl_node_else->node = temp_lambda_closure_else;
        ctrl_node->next = temp_ctrl_node_else; // link the newly created control struct in the list
        ctrl_node = ctrl_node->next;
        // generate control struct for temp_T;
        control_node *temp_ctrl_nodetruct = new control_node;
        generateControlStruct(temp_E, temp_ctrl_nodetruct); // temp_cntst's next have control struct
        ctrl_node->lambda_closure = temp_ctrl_nodetruct->next;
        delete temp_ctrl_nodetruct;

        // create BETA control structure
        Node *temp_beta = new Node("beta", BETA);
        control_node *temp_beta_cs = new control_node;
        temp_beta_cs->node = temp_beta;
        ctrl_node->next = temp_beta_cs;
        ctrl_node = ctrl_node->next;

        // generate control struct for B
        // Node *temp_lambda_closure_b= new Node;
        // temp_lambda_closure_b->set_node_type(DELTA_ELSE);
        // temp_lambda_closure_then->set_node_value("lambda_closure");
        // create new control struct
        // temp_ctrl_node = new control_node;
        // temp_ctrl_node->node = temp_B;
        // ctrl_node->next = temp_ctrl_node; //link the newly created control struct in the list
        // generate control struct for temp_T;
        control_node *temp_ctrl_nodetruct_B = new control_node;
        generateControlStruct(temp_B, temp_ctrl_nodetruct_B); // temp_cntst's next have control struct
        ctrl_node->next = temp_ctrl_nodetruct_B->next;
        delete temp_ctrl_nodetruct_B;
        while (ctrl_node->next != NULL)
            ctrl_node = ctrl_node->next;

        if (node->right != NULL)
            generateControlStruct(node->right, ctrl_node);
    }

    // else - for rest of the nodes. this case will have both left child and right sibling
    else
    {
        // create new CS node
        control_node *temp_ctrl_node = new control_node;

        // set its values
        temp_ctrl_node->node = node; // save the current standardized tree node

        // move forward the CS pointer
        ctrl_node->next = temp_ctrl_node;
        ctrl_node = ctrl_node->next;
        // recursively do the same for both left and right
        if (node->left != NULL)                           // will generate only one CS node
            generateControlStruct(node->left, ctrl_node); // this will automatically change the value of ctrl_node
        while (ctrl_node->next != NULL)                   // introducing inefficiency. couldn't resolve the design issue :(
            ctrl_node = ctrl_node->next;
        if (node->right != NULL)
            generateControlStruct(node->right, ctrl_node);
    }
}

void print_ast_node(Node *in_node)
{
    cout << "<";
    switch (in_node->type)
    {
    case IDT:
        cout << "IDENTIFIER";
        break;

    case STR:
        cout << "STRING";
        break;

    case INT:
        cout << "INTEGER";
        break;

    case TAU:
        cout << "TAU";
        break;

    case LAMBDA:
        cout << "LAMBDA";
        break;

    case OR:
        cout << "OR";
        break;

    case NOT:
        cout << "NOT";
        break;

    case GR:
        cout << "GR";
        break;

    case GE:
        cout << "GE";
        break;

    case LS:
        cout << "LS";
        break;

    case LE:
        cout << "LE";
        break;

    case EQ:
        cout << "EQ"; // this should be binary operator not EQUAL
        break;

    case NE:
        cout << "NE";
        break;

    case PLUS:
        cout << "PLUS";
        break;

    case MINUS:
        cout << "MINUS";
        break;

    case NEG:
        cout << "NEG";
        break;

    case AMP:
        cout << "AMP";
        break;

    case AUG:
        cout << "AUG";
        break;

    case MULTI:
        cout << "MULTI";
        break;

    case DIV:
        cout << "DIV";
        break;

    case EXPO:
        cout << "EXPO";
        break;

    case GAMMA:
        cout << "GAMMA";
        break;

    case TRUE:
        cout << "TRUE";
        break;

    case FALSE:
        cout << "FALSE";
        break;

    case NIL:
        cout << "NIL";
        break;

    case DUMMY:
        cout << "DUMMY";
        break;

        /*case EQUAL: cout<<"EQUAL";
              break; */

    case YSTAR:
        cout << "YSTAR";
        break;

    case DELTA_THEN:
        cout << "DELTA_THEN";
        break;

    case DELTA_ELSE:
        cout << "DELTA_ELSE";
        break;

    case BETA:
        cout << "BETA";
        break;

    case ENV:
        cout << "ENVIRONMENT";
        break;
    default:
        cerr << "Some other type of node !! - " << in_node->type << endl;
        exit(0);
    }
    cout << ":" << in_node->value << ">" << endl;
}

void print_control_node(control_node *ctrl_node)
{
    // control_node *temp = ctrl_node->next;
    cout << "===========================================================" << endl;
    while (ctrl_node != NULL)
    {
        // cout<<"[ "<<ctrl_node->node->type<<": "<<ctrl_node->node->value<<" ]"<<endl;
        print_ast_node(ctrl_node->node);
        if (ctrl_node->node->type == TAU)
            cout << "Number of children: " << ctrl_node->child_count << endl;

        Node *temp_bv = ctrl_node->bound_variable;
        while (temp_bv != NULL)
        {
            cout << "\t";
            cout << " (" << temp_bv->value << ")";
            temp_bv = temp_bv->right;
        }
        cout << endl;
        // cout<<"\n----------------------------------------------------"<<endl;

        if (ctrl_node->lambda_closure != NULL)
            print_control_node(ctrl_node->lambda_closure);
        ctrl_node = ctrl_node->next;
    }
    cout << "============================================================" << endl;
}
