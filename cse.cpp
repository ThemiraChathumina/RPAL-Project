#include "cse.h"
#include <stack>
#include <iostream>
#include <sstream>
#include <tuple>
#include <string>

using namespace std;

bool compareFirstCharactersEQ(const string &str1, const string &str2)
{
    return str1[0] >= str2[0];
}

bool compareFirstCharactersNEQ(const string &str1, const string &str2)
{
    return str1[0] > str2[0];
}

void generateControlStruct(Node *node, control_node *ctrl_node)
{
    if (node == NULL)
    {
        return;
    }

    if (node->type == LAMBDA)
    {
        control_node *temp_ctrl_node = new control_node(node);
        Node *save_right = node->left->right; // because if there isn't comma next few lines will make it NULL
        if (node->left->type != COMMA)
        {
            temp_ctrl_node->bound_variable = node->left;  // if this is comma it would be left child of left child
            temp_ctrl_node->bound_variable->right = NULL; 
        }
        else
            temp_ctrl_node->bound_variable = node->left->left; // if this is comma it would be left child of left child

        ctrl_node->next = temp_ctrl_node;
        ctrl_node = ctrl_node->next; // move forward

        control_node *new_cs = new control_node;   
        generateControlStruct(save_right, new_cs); 
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
    else if (node->type == COND)
    {
        // Break the B, T & E and save each of them
        Node *temp_B = NULL, *temp_T = NULL, *temp_E = NULL;

        temp_B = node->left; // shouldn't be NULL
        if (temp_B == NULL)
        {
            return;
        }
        temp_T = temp_B->right; // shoudln't be NULL
        if (temp_T == NULL)
        {
            return;
        }

        temp_E = temp_T->right; // shouldn't be NULL
        if (temp_E == NULL)
        {
            return;
        }
        // break all right siblings
        temp_B->right = NULL;
        temp_T->right = NULL;

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
        while (ctrl_node->next != NULL)                   
            ctrl_node = ctrl_node->next;
        if (node->right != NULL)
            generateControlStruct(node->right, ctrl_node);
    }
}

void print_node(Node *in_node)
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
        cerr << "Some other type of node." << in_node->type << endl;
        exit(0);
    }
    cout << ":" << in_node->value << ">" << endl;
}

void print_exec_node(exec_node *in_element);

void print_tuple(exec_node *tuple)
{
    queue<exec_node *> temp_tupples;
    cout << "(";
    while ((tuple->queue_val).size() != 0)
    {
        // pop an element from queue
        exec_node *temp_tuple = (tuple->queue_val).front();

        // call print_execution_element
        if (temp_tuple != NULL)
        {
            print_exec_node(temp_tuple);
            if ((tuple->queue_val).size() != 1)
                cout << ", ";
        }

        // save it in another queue
        temp_tupples.push(temp_tuple);
        (tuple->queue_val).pop();
    }

    // print )
    cout << ")";
    // push back all the elements
    while (temp_tupples.size() != 0)
    {
        (tuple->queue_val).push(temp_tupples.front());
        temp_tupples.pop();
    }
}

void print_exec_node(exec_node *in_element)
{
    if (in_element == NULL)
    {
        cout << "CSE_machine::print_execution - in_element in NULL " << endl;
        exit(0);
    }

    switch (in_element->type)
    {
    case EXEC_INT:
        cout << in_element->int_val;
        break;

    case EXEC_STR: 
        for (int i = 0; i < (int)((in_element->str_val).size()); i++)
        {
            if ((in_element->str_val)[i] == '\\')
            {
                i++;
                if ((in_element->str_val)[i] == 'n')
                    cout << endl;
                else
                    cout << "\t";
            }

            else
                cout << (in_element->str_val)[i];
        }
        
        break;
    case EXEC_ENV:
        cout << "<ENVIRONMENT>" << endl;
        break;

    case EXEC_TUPPLE: 
        print_tuple(in_element);
        break;
    case EXEC_CTRL_STRUCT:
        cout << "[lambda closure: ";
        cout << in_element->ctrl_node_val->bound_variable->value << " ]";
        break;
    case EXEC_TRUE:
        cout << "true";
        break;

    case EXEC_FALSE:
        cout << "false";
        break;

    case EXEC_DUMMY:
        cout << "dummy";
        break;
    case EXEC_YSTAR:
        cout << "<YSTAR>" << endl;
        break;

    case EXEC_NILL:
        cout << "nil";
        break;
    case EXEC_ITA:
        cout << "<ITA>" << endl;
        cout << "[lambda closure: ";
        cout << in_element->ctrl_node_val->bound_variable->value << " ]" << endl;
        break;

    default:
        cout << "print_execution_element - Some other type." << endl;
    }
}

int i;
stack<control_node *> control_stack;
stack<exec_node *> exec_stack;
environment *current_environment;

void initializeCSE(control_node *ctrl_node)
{
    current_environment = new environment;
    i = 0;
    // We need to put environment first of all in both stacks
    // create new exec_node of type environment
    exec_node *temp_exe_ele = new exec_node; // this is for EXECUTION ELEMENT
    temp_exe_ele->type = EXEC_ENV;
    temp_exe_ele->env_val = current_environment;
    exec_stack.push(temp_exe_ele);

    // create new control struct for CONTROL STACK
    Node *node = new Node("env", ENV);
    control_node *new_control_node = new control_node;
    new_control_node->node = node;               // this node will tell the type of control struct in execution
    new_control_node->env = current_environment; // this stores the environment
    control_stack.push(new_control_node);

    // push all control stack elements in control stack
    while (ctrl_node != NULL)
    {
        control_stack.push(ctrl_node);
        ctrl_node = ctrl_node->next;
    }
}


void load(control_node *lambda_closure) //loads control structure to control stack
{
    while (lambda_closure != NULL)
    {
        control_stack.push(lambda_closure);
        lambda_closure = lambda_closure->next;
    }
}


exec_node *checkEnvironment(string key, environment *env)	//traverses the enviroment tree and finds the value of a variable
{
    if (env->bound_variable[key] != NULL)
    {
        return env->bound_variable[key];
    }
    else if (env->parent != NULL)
    {
        return checkEnvironment(key, env->parent);
    }
    else
        return NULL;
}

void insert(string key, exec_node *value, environment *env)
{
    env->bound_variable[key] = value;
}

void execute()
{
    // while control is not empty
    while (control_stack.size() > 2)
    {
        // pop an element from control stack
        control_node *popped_ctrl_node = control_stack.top();
        control_stack.pop();
        // print_node(popped_ctrl_node->node);
        // apply rules
        if (popped_ctrl_node->node->type == INT)
        { // if node is INTEGER
            // create new exec_node
            exec_node *new_exe_ele = new exec_node();
            new_exe_ele->type = EXEC_INT; // set type

            // convert string value to int
            int temp_int = atoi((popped_ctrl_node->node->value).c_str());
            new_exe_ele->int_val = temp_int;
            // push the new exec_node on exeuction stack
            exec_stack.push(new_exe_ele);
        }

        else if (popped_ctrl_node->node->type == NIL)
        {

            exec_node *new_nil = new exec_node;
            new_nil->type = EXEC_NILL;
            exec_stack.push(new_nil);
        }

        else if (popped_ctrl_node->node->type == DUMMY)
        {

            exec_node *new_dummy = new exec_node;
            new_dummy->type = EXEC_DUMMY;
            exec_stack.push(new_dummy);
        }

        else if (popped_ctrl_node->node->type == TRUE)
        {
            exec_node *new_true = new exec_node;
            new_true->type = EXEC_TRUE;
            exec_stack.push(new_true);
        }

        else if (popped_ctrl_node->node->type == FALSE)
        {
            exec_node *new_false = new exec_node;
            new_false->type = EXEC_FALSE;
            exec_stack.push(new_false);
        }
        else if (popped_ctrl_node->node->type == STR)
        { // if node is STRING

            // create new exec_node
            exec_node *new_exe_ele = new exec_node();
            new_exe_ele->type = EXEC_STR; // set type


            new_exe_ele->str_val = popped_ctrl_node->node->value;
            // push the new exec_node on exeuction stack
            exec_stack.push(new_exe_ele);
        }

        // if node is IDENTIFIER
        else if (popped_ctrl_node->node->type == IDT)
        {

            // lookup in current environment
            exec_node *idt_exe_ele = checkEnvironment(popped_ctrl_node->node->value, current_environment);

            // if lookup is unsuccessfull it might be primitive function
            if (idt_exe_ele == NULL)
            {
                // check if its primitive function
                if ((popped_ctrl_node->node->value).compare("Print") == 0 || (popped_ctrl_node->node->value).compare("print") == 0)
                {
                    // do appropriate action
                    // create new execution element
                    exec_node *new_exe_ele = new exec_node;
                    new_exe_ele->type = EXEC_PRIMITIVE_FUNC;
                    new_exe_ele->primitive_func_val = PRINT;
                    exec_stack.push(new_exe_ele); // push
                    // delete popped_ctrl_node;
                }
                else if ((popped_ctrl_node->node->value).compare("ItoS") == 0 || (popped_ctrl_node->node->value).compare("itos") == 0)
                {
                    // do appropriate action
                    // create new execution element
                    exec_node *new_exe_ele = new exec_node;
                    new_exe_ele->type = EXEC_PRIMITIVE_FUNC;
                    new_exe_ele->primitive_func_val = ITOS;
                    exec_stack.push(new_exe_ele); // push
                    // delete popped_ctrl_node;
                }
                else if ((popped_ctrl_node->node->value).compare("Istruthvalue") == 0 || (popped_ctrl_node->node->value).compare("istruthvalue") == 0)
                {
                    // do appropriate action
                    // create new execution element
                    exec_node *new_exe_ele = new exec_node;
                    new_exe_ele->type = EXEC_PRIMITIVE_FUNC;
                    new_exe_ele->primitive_func_val = ISTRUTHVALUE;
                    exec_stack.push(new_exe_ele); // push
                    // delete popped_ctrl_node;
                }

                else if ((popped_ctrl_node->node->value).compare("Isstring") == 0 || (popped_ctrl_node->node->value).compare("isstring") == 0)
                {
                    // do appropriate action
                    // create new execution element
                    exec_node *new_exe_ele = new exec_node;
                    new_exe_ele->type = EXEC_PRIMITIVE_FUNC;
                    new_exe_ele->primitive_func_val = ISSTRING;
                    exec_stack.push(new_exe_ele); // push
                    // delete popped_ctrl_node;
                }

                else if ((popped_ctrl_node->node->value).compare("Isinteger") == 0 || (popped_ctrl_node->node->value).compare("isinteger") == 0)
                {
                    // do appropriate action
                    // create new execution element
                    exec_node *new_exe_ele = new exec_node;
                    new_exe_ele->type = EXEC_PRIMITIVE_FUNC;
                    new_exe_ele->primitive_func_val = ISINTEGER;
                    exec_stack.push(new_exe_ele); // push
                    // delete popped_ctrl_node;
                }

                else if ((popped_ctrl_node->node->value).compare("Istuple") == 0 || (popped_ctrl_node->node->value).compare("istuple") == 0)
                {
                    // do appropriate action
                    // create new execution element
                    exec_node *new_exe_ele = new exec_node;
                    new_exe_ele->type = EXEC_PRIMITIVE_FUNC;
                    new_exe_ele->primitive_func_val = ISTUPLE;
                    exec_stack.push(new_exe_ele); // push
                    // delete popped_ctrl_node;
                }

                else if ((popped_ctrl_node->node->value).compare("Order") == 0 || (popped_ctrl_node->node->value).compare("order") == 0)
                {
                    // do appropriate action
                    // create new execution element
                    exec_node *new_exe_ele = new exec_node;
                    new_exe_ele->type = EXEC_PRIMITIVE_FUNC;
                    new_exe_ele->primitive_func_val = ORDER;
                    exec_stack.push(new_exe_ele); // push
                    // delete popped_ctrl_node;
                }

                else if (((popped_ctrl_node->node->value).compare("Stern") == 0) || ((popped_ctrl_node->node->value).compare("stern") == 0) || ((popped_ctrl_node->node->value).compare("Stem") == 0) || ((popped_ctrl_node->node->value).compare("stem") == 0) || ((popped_ctrl_node->node->value).compare("Conc") == 0) || ((popped_ctrl_node->node->value).compare("conc") == 0))
                {
                    // do appropriate action
                    // create new execution element
                    exec_node *new_exe_ele = new exec_node;
                    new_exe_ele->type = EXEC_PRIMITIVE_FUNC;
                    if ((popped_ctrl_node->node->value).compare("Stern") == 0 || (popped_ctrl_node->node->value).compare("stern") == 0)
                        new_exe_ele->primitive_func_val = STERN;
                    else if ((popped_ctrl_node->node->value).compare("Stem") == 0 || (popped_ctrl_node->node->value).compare("stem") == 0)
                        new_exe_ele->primitive_func_val = STEM;
                    else
                    {
                        new_exe_ele->primitive_func_val = CONC;
                        new_exe_ele->conc_flag = false;
                    }
                    exec_stack.push(new_exe_ele); // push
                    // delete popped_ctrl_node;
                }

                else
                { 
                    cout << "\nIDENTIFIER : <" << (popped_ctrl_node->node->value) << "> is not declared !" << endl;
                    exit(0);
                }
            }

            else
            { // identifier's value found in some environment //we have an execution element in idt_exe_ele
                exec_stack.push(idt_exe_ele);
            }
        }

        else if (popped_ctrl_node->node->type == LAMBDA)
        { // if node is LAMBDA

            // anoint it with current environment
            popped_ctrl_node->env = current_environment;

            // create new exec_node
            exec_node *new_exe_ele = new exec_node();
            new_exe_ele->type = EXEC_CTRL_STRUCT; // set type
            new_exe_ele->ctrl_node_val = popped_ctrl_node;

            // push it on execution stack
            exec_stack.push(new_exe_ele);
        }

        else if (popped_ctrl_node->node->type == TAU)
        { // if node is TAU
            // create an execution element
            exec_node *new_exec_ele = new exec_node; // it will have queue
            new_exec_ele->type = EXEC_TUPPLE;

            // pop n element from execution stack
            int n = popped_ctrl_node->child_count;
            for (int i = 0; i < n; i++)
            {
                exec_node *temp_ele = exec_stack.top();
                if (temp_ele == NULL)
                {
                    cout << "\nERROR: Processing TAU node. number of nodes in the stack is less than expected" << endl;
                    exit(0);
                }
                (new_exec_ele->queue_val).push(temp_ele);
                exec_stack.pop();
            }

            // push back the queue into execution stack
            exec_stack.push(new_exec_ele);
        }

        else if (popped_ctrl_node->node->type == ENV)
        { // if it is ENVIRONMENT

            // pop from execution stack and save it
            exec_node *saved_exe_ele = exec_stack.top();
            if (saved_exe_ele != NULL)
            {
                exec_stack.pop(); // pop another element
                exec_node *another_exe_ele = exec_stack.top();

                // it should be same environment as control stack's one
                if (!((another_exe_ele->type == EXEC_ENV) && ((another_exe_ele->env_val) == (popped_ctrl_node->env))))
                { // confirm if it is environment and same as the popped one
                    cout << "ERROR - Expecting an ENVIRONMENT element on execution stack" << endl;
                    exit(0);
                }
                exec_stack.pop(); // ENV was matched so pop it
                exec_stack.push(saved_exe_ele);
            }
            else
            { // go NULL from stack
                cout << "ERROR - Was expecting some element on execution stack but got NULL" << endl;
                exit(0);
            }

            // UPDATE current environment
            stack<exec_node *> temp_stack;
            while (exec_stack.size() != 0 && ((exec_stack.top())->type != EXEC_ENV))
            {
                temp_stack.push(exec_stack.top());
                exec_stack.pop();
            }

            if (exec_stack.size() != 0)
            {
                current_environment = (exec_stack.top())->env_val;
            }
            // put back the elements
            while (temp_stack.size() != 0)
            {
                exec_stack.push(temp_stack.top());
                temp_stack.pop();
            }
        }

        // Handle <YSTAR> node
        else if (popped_ctrl_node->node->type == YSTAR)
        {

            // create a new execution element of type EXEC_YSTAR and push it in execution stack
            exec_node *new_y_star = new exec_node;
            new_y_star->type = EXEC_YSTAR;
            exec_stack.push(new_y_star);
        }

        else if (popped_ctrl_node->node->type == GAMMA)
        { // if node is GAMMA
            // check the element on execution stack
            if ((exec_stack.top())->type == EXEC_CTRL_STRUCT)
            { 
                // pop the element from execution stack
                exec_node *temp_exe_ele = exec_stack.top(); // temp_exe_ele contains control structure
                exec_stack.pop();

                // create new environment
                environment *new_env = new environment();

                // link the new environment to environment in control struct
                new_env->parent = temp_exe_ele->ctrl_node_val->env;

                // bound_variable shouldn't be NULL
                if (temp_exe_ele->ctrl_node_val->bound_variable == NULL)
                {
                    cout << "Bound variable of this popped control structure is NULL!" << endl;
                    exit(0);
                }
                // bound the new variable - could have two cases
                // assuming single binding
                // pop an element from exection stack
                exec_node *bind_exe_ele = exec_stack.top();
                exec_stack.pop();
                // if bind_exe_ele is tuple  we must have linked list in popped control struct's bound_variable
                if (bind_exe_ele->type == EXEC_TUPPLE)
                {
                    // so we have Linked list in temp_exe_ele->ctrl_node_val->bound_variable AND tuple in bind_exe_ele
                    Node *temp_bound_var = temp_exe_ele->ctrl_node_val->bound_variable;

                    // if bound variable is not linked list bound whole tuple to the variable
                    if (temp_bound_var->right == NULL)
                    {
                        insert(temp_bound_var->value, bind_exe_ele, new_env);
                    }

                    else
                    {
                        queue<exec_node *> temp_for_tuple;

                        while (temp_bound_var != NULL)
                        {
                            // get the Nodes value to use as key
                            string temp_key = temp_bound_var->value;
                            // get the value from queue
                            exec_node *popped_from_queue = (bind_exe_ele->queue_val).front();
                            if (popped_from_queue == NULL)
                            {
                                cout << "ERROR - Binding variables - tuple on execution has less number of elements" << endl;
                                exit(0);
                            }
                            temp_for_tuple.push(popped_from_queue); // preserve the tuple
                            (bind_exe_ele->queue_val).pop();        // pop the element;
                            // insert key and value
                            insert(temp_key, popped_from_queue, new_env);
                            temp_bound_var = temp_bound_var->right;
                        }

                        while (temp_for_tuple.size() != 0)
                        {
                            (bind_exe_ele->queue_val).push(temp_for_tuple.front());
                            temp_for_tuple.pop();
                        }
                    }
                }

                else
                {
                    // add this element with key from temp_exe_ele's bound var in new environment
                    insert(temp_exe_ele->ctrl_node_val->bound_variable->value, bind_exe_ele, new_env);
                }

                // change the current environment to new environment
                current_environment = new_env;
                exec_node *new_exe_ele = new exec_node();
                new_exe_ele->type = EXEC_ENV;
                new_exe_ele->env_val = new_env;

                // push the new environment in both control stack and execution stack
                // push it in exec_stack
                exec_stack.push(new_exe_ele);

                // create new control struct and push it in control stack
                Node *new_ast_node = new Node("env", ENV);
                control_node *new_control_st = new control_node;
                new_control_st->node = new_ast_node;       // this node will tell the type of control struct in execution
                new_control_st->env = current_environment; // this stores the environment
                control_stack.push(new_control_st);

                // load control stack with popped control stack's lambda_closure
                if (temp_exe_ele->ctrl_node_val->lambda_closure == NULL)
                {
                    cout << "this lambda doesn't have lambda_closure !! " << endl;
                    exit(0);
                }
                load(temp_exe_ele->ctrl_node_val->lambda_closure);
            }

            // Handle <Ystar> node
            else if ((exec_stack.top())->type == EXEC_YSTAR)
            {
                // pop <YSTAR> node and discard it
                exec_stack.pop();
                // pop <CS> node and change its type from <CS> to <ITA>
                exec_node *temp_cs = exec_stack.top(); // shouldn't be NULL
                if (temp_cs == NULL || temp_cs->type != EXEC_CTRL_STRUCT)
                {
                    cout << "ERROR -Processing <YSTAR> node. Was expecting <LAMBDA> on execution stack but it is either NULL or some other type !" << endl;
                    exit(0);
                }
                temp_cs->type = EXEC_ITA; // change its type
            }

            // Handle <EXEC_ITA>
            else if ((exec_stack.top())->type == EXEC_ITA)
            {
                i++;

                // create new execution element of type EXEC_CTRL_STRUCT
                exec_node *new_ctrl_struct = new exec_node;
                new_ctrl_struct->type = EXEC_CTRL_STRUCT;
                new_ctrl_struct->ctrl_node_val = (exec_stack.top())->ctrl_node_val;

                // push the new element
                exec_stack.push(new_ctrl_struct);

                // create new GAMMA ast node and new control stru and push it in CS
                Node *another_gamma_to_push = new Node("gamma", GAMMA);
                control_node *another_gamma_cs = new control_node;
                another_gamma_cs->node = another_gamma_to_push;

                control_stack.push(another_gamma_cs);

                // push two gamma CS in CS Stack
                // control_stack.push(popped_ctrl_node); //pop back the same gamma

                // create new GAMMA ast node and new control stru and push it in CS
                Node *new_gamma_to_push = new Node("gamma", GAMMA);
                control_node *new_gamma_cs = new control_node;
                new_gamma_cs->node = new_gamma_to_push;
                control_stack.push(new_gamma_cs);
            }
            // must be some primitive function
            else if ((exec_stack.top())->type == EXEC_PRIMITIVE_FUNC)
            {
                // pop it
                exec_node *popped_primitive_func = exec_stack.top();
                exec_stack.pop();

                if (popped_primitive_func->primitive_func_val == PRINT)
                {

                    // pop the node and print it
                    exec_node *another_ele = exec_stack.top(); // shouldn't be null
                    if (another_ele == NULL)
                    {
                        cout << "ERROR - Was expecting an element to Print but found NULL" << endl;
                        exit(0);
                    }
                    // pop the element
                    exec_stack.pop();
                    print_exec_node(another_ele); // print it
                    // delete another_ele;

                    // store the DUMMY in execution stack
                    exec_node *new_exec_ele = new exec_node;
                    new_exec_ele->type = EXEC_DUMMY;
                    exec_stack.push(new_exec_ele);

                    // break;
                }

                else if (popped_primitive_func->primitive_func_val == ITOS)
                {
                    // pop the node and print it
                    exec_node *another_ele = exec_stack.top(); // shouldn't be null
                    if (another_ele == NULL)
                    {
                        cout << "ERROR - Was expecting an element to Print but found NULL please check PRINT again" << endl;
                        exit(0);
                    }
                    // pop the element
                    exec_stack.pop();
                    if (another_ele->type != EXEC_INT)
                    {
                        cout << "ERROR - ITOS operands invalied" << endl;
                        exit(0);
                    }
                    stringstream ss;
                    ss << (another_ele->int_val);
                    string temp_string = ss.str();
                    exec_node *itos_ele = new exec_node;
                    itos_ele->type = EXEC_STR;
                    itos_ele->str_val = temp_string;
                    exec_stack.push(itos_ele);
                }

                else if (popped_primitive_func->primitive_func_val == ISINTEGER)
                {
                    exec_node *check_integer = exec_stack.top();
                    if (check_integer == NULL)
                    {
                        cout << "ERROR - Please check Istuple again.." << endl;
                        exit(0);
                    }

                    exec_stack.pop();
                    exec_node *new_check_int = new exec_node;
                    if (check_integer->type == EXEC_INT)
                        new_check_int->type = EXEC_TRUE;
                    else
                        new_check_int->type = EXEC_FALSE;

                    exec_stack.push(new_check_int);
                }

                else if (popped_primitive_func->primitive_func_val == ISTRUTHVALUE)
                {
                    exec_node *check_tuple = exec_stack.top();
                    if (check_tuple == NULL)
                    {
                        cout << "ERROR - Please check Istruthvalue" << endl;
                        exit(0);
                    }

                    exec_stack.pop();
                    exec_node *new_check_tuple = new exec_node;
                    if (check_tuple->type == EXEC_FALSE || check_tuple->type == EXEC_TRUE)
                        new_check_tuple->type = EXEC_TRUE;
                    else
                        new_check_tuple->type = EXEC_FALSE;

                    exec_stack.push(new_check_tuple);
                }

                else if (popped_primitive_func->primitive_func_val == ISSTRING)
                {
                    exec_node *check_tuple = exec_stack.top();
                    if (check_tuple == NULL)
                    {
                        cout << "ERROR - Please check Isstring" << endl;
                        exit(0);
                    }

                    exec_stack.pop();
                    exec_node *new_check_tuple = new exec_node;
                    if (check_tuple->type == EXEC_STR)
                        new_check_tuple->type = EXEC_TRUE;
                    else
                        new_check_tuple->type = EXEC_FALSE;

                    exec_stack.push(new_check_tuple);
                }

                else if (popped_primitive_func->primitive_func_val == ISTUPLE)
                {
                    exec_node *check_tuple = exec_stack.top();
                    if (check_tuple == NULL)
                    {
                        cout << "ERROR - Please check Istuple" << endl;
                        exit(0);
                    }

                    exec_stack.pop();
                    exec_node *new_check_tuple = new exec_node;
                    if (check_tuple->type == EXEC_TUPPLE || check_tuple->type == EXEC_NILL)
                        new_check_tuple->type = EXEC_TRUE;
                    else
                        new_check_tuple->type = EXEC_FALSE;

                    exec_stack.push(new_check_tuple);
                }

                // Handle ORDER
                else if (popped_primitive_func->primitive_func_val == ORDER)
                {
                    // pop an element
                    exec_node *temp_tupple = exec_stack.top();

                    // it should be <tuple>
                    if (temp_tupple == NULL || (temp_tupple->type != EXEC_TUPPLE && temp_tupple->type != EXEC_NILL))
                    {
                        cout << "ERROR - Was expecting a tuple next to ORDER." << endl;
                        exit(0);
                    }
                    exec_stack.pop();

                    // count number of elements in queue.
                    int num_elements;
                    if (temp_tupple->type == EXEC_NILL)
                        num_elements = 0;
                    else
                        num_elements = (temp_tupple->queue_val).size();
                    // change the type and value of the execution element
                    exec_node *new_int_element = new exec_node;
                    new_int_element->type = EXEC_INT;
                    new_int_element->int_val = num_elements;
                    exec_stack.push(new_int_element);
                }

                else if (popped_primitive_func->primitive_func_val == STEM)
                {
                    exec_node *stem_element = exec_stack.top();
                    if (stem_element == NULL || stem_element->type != EXEC_STR)
                    {
                        cout << "ERROR - Stem's operand is invalid" << endl;
                        exit(0);
                    }

                    exec_stack.pop();
                    string stem_string = stem_element->str_val;
                    stem_string = stem_string.substr(0, 1);
                    exec_node *new_stem_ele = new exec_node;
                    new_stem_ele->type = EXEC_STR;
                    new_stem_ele->str_val = stem_string;
                    exec_stack.push(new_stem_ele);
                }

                // CONC needs two gamma
                else if (popped_primitive_func->primitive_func_val == CONC)
                {
                    // pop two strings
                    exec_node *first_string = exec_stack.top();
                    if (first_string == NULL || first_string->type != EXEC_STR)
                    {
                        cout << "ERROR - CONC's first operand is invalid" << endl;
                        exit(0);
                    }
                    exec_stack.pop();

                    // if conc_flag is false means conc hasn't been applied yet so just store the string
                    if (popped_primitive_func->conc_flag == false)
                    {
                        popped_primitive_func->str_val = first_string->str_val;
                        popped_primitive_func->conc_flag = true;
                        exec_stack.push(popped_primitive_func);
                    }

                    else
                    { 
                        exec_node *conc_string = new exec_node;
                        conc_string->type = EXEC_STR;
                        conc_string->str_val = popped_primitive_func->str_val + first_string->str_val;
                        exec_stack.push(conc_string);
                    }
                }
                else if (popped_primitive_func->primitive_func_val == STERN)
                {
                    // case STERN:
                    // pop the node
                    exec_node *stern_element = exec_stack.top(); // shouldn't be null
                    if (stern_element == NULL || stern_element->type != EXEC_STR)
                    { // should be string
                        cout << "ERROR - Was expecting an element to Stern but found NULL or not string please check STERN again" << endl;
                        exit(0);
                    }
                    // pop the element
                    exec_stack.pop();
                    // delete stern_element;

                    // apply stern
                    string stern_string = stern_element->str_val;

                    string resultant_string = stern_string.substr(1, stern_string.size()); // sterning
                    exec_node *new_stern_exe = new exec_node;
                    new_stern_exe->type = EXEC_STR;
                    new_stern_exe->str_val = resultant_string;
                    exec_stack.push(new_stern_exe);
                }
            }

            else if ((exec_stack.top())->type == EXEC_TUPPLE)
            { // if there is tuple on Execution Stack
                // pop the tuple
                exec_node *tupple_exe_ele = exec_stack.top(); // this contains queue
                exec_stack.pop();

                // pop another element. It should be INTEGER
                exec_node *second_exe_ele = exec_stack.top();
                if (second_exe_ele == NULL || second_exe_ele->type != EXEC_INT)
                { // should be INTEGER only
                    cout << "ERROR - Processing <GAMMA>, <tuple> on Execution Stack" << endl;
                    exit(0);
                }
                exec_stack.pop();

                queue<exec_node *> temp_queue; // temp queue
                for (int i = 1; i < (second_exe_ele->int_val); i++)
                {
                    if ((tupple_exe_ele->queue_val).size() == 0)
                    {
                        cout << "ERROR - Processing <GAMMA>, <tuple> on Execution Stack" << endl;
                        exit(0);
                    }
                    temp_queue.push((tupple_exe_ele->queue_val).front()); // pop n-1 elements
                    (tupple_exe_ele->queue_val).pop();
                }

                exec_node *resultant_ele = new exec_node;
                exec_node *temp_ex_el = (tupple_exe_ele->queue_val).front();
                while ((tupple_exe_ele->queue_val).size() != 0)
                {
                    temp_queue.push((tupple_exe_ele->queue_val).front());
                    (tupple_exe_ele->queue_val).pop();
                }

                resultant_ele->type = temp_ex_el->type; // same type
                resultant_ele->int_val = temp_ex_el->int_val;
                resultant_ele->str_val = temp_ex_el->str_val;
                resultant_ele->queue_val = temp_ex_el->queue_val;
                resultant_ele->ctrl_node_val = temp_ex_el->ctrl_node_val;
                resultant_ele->env_val = temp_ex_el->env_val;
                resultant_ele->primitive_func_val = temp_ex_el->primitive_func_val;

                if (resultant_ele == NULL)
                {
                    cout << "ERROR - Processing <GAMMA>, <tuple> on Execution Stack..Processing last element" << endl;
                    exit(0);
                }

                // now put back the popped elements
                // this will preserve the queue
                while (temp_queue.size() != 0)
                {
                    (tupple_exe_ele->queue_val).push(temp_queue.front());
                    temp_queue.pop();
                }
                // push the resultant element in execution stack
                exec_stack.push(resultant_ele);
            }
            else
            {
                cout << "WHILE PROCESSING GAMMA - SOME OTHER TYPE" << endl;
                print_exec_node(exec_stack.top());
                exit(0);
            }
        }

        else if ((popped_ctrl_node->node->type == BETA))
        { // BETA Node
            // pop an element from exeuction stack. It should be either TRUE or FALSE
            exec_node *beta_exe_element = exec_stack.top();
            if (beta_exe_element == NULL)
            {
                cout << "ERROR - While processing BETA node - Execution Stack is empty" << endl;
                exit(0);
            }

            exec_stack.pop();
            if (!(beta_exe_element->type == EXEC_TRUE || beta_exe_element->type == EXEC_FALSE))
            {
                cout << "ERROR - While processing BETA node" << endl;
                exit(0);
            }

            control_node *load_delta = NULL;
            // we have truth value in beta_exe_element
            if (beta_exe_element->type == EXEC_TRUE)
            {
                // pop an element from CONTROL STACK and discard and load next element
                control_node *temp_delta = control_stack.top(); // it should be lambda_closure else
                if (temp_delta == NULL || temp_delta->node->type != DELTA_ELSE)
                {
                    cout << "ERROR - Processing <BETA> was expecting DELTA_ELSE" << endl;
                    exit(0);
                }
                control_stack.pop();

                temp_delta = control_stack.top(); // access delta_then
                if (temp_delta == NULL || temp_delta->node->type != DELTA_THEN)
                {
                    cout << "ERROR - Processing <BETA> was expecting DELTA_THEN" << endl;
                    exit(0);
                }
                control_stack.pop();

                load_delta = temp_delta;
            }

            else
            {
                control_node *temp_delta = control_stack.top();
                if (temp_delta == NULL || temp_delta->node->type != DELTA_ELSE)
                {
                    cout << "ERROR - Processing <BETA> was expecting DELTA_ELSE" << endl;
                    exit(0);
                }
                control_stack.pop();

                control_node *temp_delta_discard = control_stack.top(); // access delta_then
                if (temp_delta_discard == NULL || temp_delta_discard->node->type != DELTA_THEN)
                {
                    cout << "ERROR - Processing <BETA> was expecting DELTA_THEN" << endl;
                    exit(0);
                }
                control_stack.pop();

                load_delta = temp_delta;
            }

            // load control struct
            load(load_delta->lambda_closure);
        }
        else if (popped_ctrl_node->node->type == EQ || popped_ctrl_node->node->type == NE)
        {
            // pop two element
            exec_node *temp_first = exec_stack.top(); // shouldn't be NULL
            if (temp_first == NULL)
            {
                cout << "ERROR - Processing EQ operator. But there are less than two elements on Execution Stack " << endl;
                exit(0);
            }
            exec_stack.pop(); // actually pop the element
            exec_node *temp_second = exec_stack.top();
            if (temp_second == NULL)
            {
                cout << "ERROR - Processing EQ operator. But there are less than two elements on Execution Stack" << endl;
                exit(0);
            }
            exec_stack.pop(); // actually pop the element

            bool temp_result;
            // both should be of either STRING, INTEGER or BOOL
            if (temp_first->type == EXEC_STR && temp_second->type == EXEC_STR)
            {
                if (popped_ctrl_node->node->type == EQ)
                    temp_result = !((temp_first->str_val).compare(temp_second->str_val));
                else
                    temp_result = ((temp_first->str_val).compare(temp_second->str_val));
            }

            else if (temp_first->type == EXEC_INT && temp_second->type == EXEC_INT)
            { // both are INTEGERS

                if (popped_ctrl_node->node->type == EQ)
                    temp_result = (temp_first->int_val == temp_second->int_val);
                else
                    temp_result = (temp_first->int_val != temp_second->int_val);
            }

            else if ((temp_first->type == EXEC_TRUE || temp_first->type == EXEC_FALSE) && (temp_second->type == EXEC_FALSE || temp_second->type == EXEC_TRUE))
            {
                if (popped_ctrl_node->node->type == EQ)
                {

                    if (temp_first->type == temp_second->type)
                        temp_result = true;
                    else
                        temp_result = false;
                }
                else
                {
                    if (temp_first->type != temp_second->type)
                        temp_result = true;
                    else
                        temp_result = false;
                }
            }

            else
            {
                cout << "ERROR - EQ operator's operands should have been of same type" << endl;
                exit(0);
            }
            // create new execution element
            exec_node *new_eq_exe_ele = new exec_node;
            if (temp_result)
                new_eq_exe_ele->type = EXEC_TRUE;
            else
                new_eq_exe_ele->type = EXEC_FALSE;

            // push into execution stack
            exec_stack.push(new_eq_exe_ele);
        }

        // AUG Operator
        else if (popped_ctrl_node->node->type == AUG)
        {
            // pop first element
            exec_node *temp_first_tupple = exec_stack.top();
            if (temp_first_tupple == NULL)
            {
                cout << "ERROR - Processing AUG function. Was expecting <tuple> or <NIL>" << endl;
                exit(0);
            }

            exec_stack.pop();

            // pop second element
            exec_node *temp_second_tupple = exec_stack.top();
            if (temp_second_tupple == NULL)
            {
                cout << "ERROR - Processing AUG function. Was expecting <tuple> or <NIL>" << endl;
                exit(0);
            }

            exec_stack.pop();

            // first element has to be either tuple or NIL
            if (!(temp_first_tupple->type == EXEC_TUPPLE || temp_first_tupple->type == EXEC_NILL))
            {
                cout << "ERROR - CSE_machine - was expecting first operand of AUG to be either tuple or NIL" << endl;
                exit(0);
            }

            // second element should either be INT, STR, TRUE, FALSE, NIL, DUMMY or tuple
            if (!(temp_second_tupple->type == EXEC_TUPPLE || temp_second_tupple->type == EXEC_NILL || temp_second_tupple->type == EXEC_INT || temp_second_tupple->type == EXEC_STR || temp_second_tupple->type == EXEC_TRUE || temp_second_tupple->type == EXEC_FALSE || temp_second_tupple->type == EXEC_DUMMY))
            {
                cout << "ERROR - CSE_machine - was expecting second operand of AUG to be either tuple, DUMMY, TRUE, FALSE, INTEGER, STRING or NIL" << endl;
                exit(0);
            }

            exec_node *new_aug_tuple = new exec_node;
            new_aug_tuple->type = EXEC_TUPPLE;

            queue<exec_node *> preserve_tuple;

            // if first element is tuple
            if ((temp_first_tupple->type != EXEC_NILL))
            {
                while ((temp_first_tupple->queue_val).size() != 0)
                {
                    (new_aug_tuple->queue_val).push((temp_first_tupple->queue_val).front());
                    preserve_tuple.push((temp_first_tupple->queue_val).front()); // to preserve tuple
                    (temp_first_tupple->queue_val).pop();                       
                }
            }
            // preserve tuple
            while (preserve_tuple.size() != 0)
            {
                (temp_first_tupple->queue_val).push(preserve_tuple.front());
                preserve_tuple.pop();
            }
            (new_aug_tuple->queue_val).push(temp_second_tupple);
            exec_stack.push(new_aug_tuple); // push back the first tuple
        }

        else if ((popped_ctrl_node->node->type == AMP) || (popped_ctrl_node->node->type == OR))
        {
            // pop two elements from execution stack
            exec_node *first_op = exec_stack.top();
            if (first_op == NULL)
            {
                cout << "ERROR -first op is not found" << endl;
                exit(0);
            }

            exec_stack.pop();

            exec_node *second_op = exec_stack.top();
            if (second_op == NULL)
            {
                cout << "ERROR -second op is not found" << endl;
                exit(0);
            }

            exec_stack.pop();

            if (!(first_op->type == EXEC_TRUE || first_op->type == EXEC_FALSE) || !(second_op->type == EXEC_TRUE || second_op->type == EXEC_FALSE))
            {
                cout << "ERROR - & must have both operands as boolean" << endl;
                exit(0);
            }

            exec_node *new_amp = new exec_node;

            if (popped_ctrl_node->node->type == OR)
            {
                if (first_op->type == EXEC_FALSE && second_op->type == EXEC_FALSE)
                    new_amp->type = EXEC_FALSE;
                else
                    new_amp->type = EXEC_TRUE;
            }
            else if (popped_ctrl_node->node->type == AMP)
            {
                if (first_op->type == EXEC_TRUE && second_op->type == EXEC_TRUE)
                    new_amp->type = EXEC_TRUE;
                else
                    new_amp->type = EXEC_FALSE;
            }
            exec_stack.push(new_amp);
        }

        // NEG operator
        else if (popped_ctrl_node->node->type == NEG)
        {
            // pop an element from execution stack
            exec_node *not_ele = exec_stack.top();
            if (not_ele == NULL)
            {
                cout << "ERROR - <NEG>'s operand is NULL !" << endl;
                exit(0);
            }
            exec_stack.pop();
            if (not_ele->type != EXEC_INT)
            {
                cout << "\tERROR - <NEG> operator was expecting <INTEGER> as operand." << endl;
                exit(0);
            }

            exec_node *neg_ele = new exec_node;
            neg_ele->type = EXEC_INT;
            neg_ele->int_val = -(not_ele->int_val);
            exec_stack.push(neg_ele);
        }

        else if (popped_ctrl_node->node->type == NOT)
        {
            // pop an element from execution stack
            exec_node *not_ele = exec_stack.top();
            if (not_ele == NULL)
            {
                cout << "ERROR - <NOT>'s operand is NULL" << endl;
                exit(0);
            }
            exec_stack.pop();

            exec_node *not_new_ele = new exec_node;

            if (not_ele->type == EXEC_TRUE)
                not_new_ele->type = EXEC_FALSE;
            else if (not_ele->type == EXEC_FALSE)
                not_new_ele->type = EXEC_TRUE;
            else
            {
                cout << "ERROR - <NOT>'s operand should be truthvalue" << endl;
                exit(0);
            }

            exec_stack.push(not_new_ele);
        }

        // PLUS, DIV operator
        else if ((popped_ctrl_node->node->type == DIV) ||
                 popped_ctrl_node->node->type == PLUS ||
                 (popped_ctrl_node->node->type == MINUS) ||
                 (popped_ctrl_node->node->type == GR) ||
                 (popped_ctrl_node->node->type == GE) ||
                 (popped_ctrl_node->node->type == LE) ||
                 (popped_ctrl_node->node->type == MULTI) || (popped_ctrl_node->node->type == LS))
        {
            exec_node *temp_pop_1 = exec_stack.top();
            // this should not be NULL and should be integer
            if (temp_pop_1 == NULL || (temp_pop_1->type != EXEC_INT && temp_pop_1->type != EXEC_STR))
            {
                cout << "ERROR -First Element on execution stack is either not INTEGER , STR or there is no element" << endl;
                exit(0);
            }
            exec_stack.pop();

            exec_node *temp_pop_2 = exec_stack.top(); // pop second element

            // both should not be NULL and should be integers or strings
            if (temp_pop_2 == NULL || (temp_pop_2->type != temp_pop_1->type))

            {
                cout << "ERROR -First Element on execution stack is either not INTEGER , STR or there is no element" << endl;
                exit(0);
            }
            exec_stack.pop();
            // perform calculation
            int temp_result;

            if (popped_ctrl_node->node->type == GR)
            {
                temp_result = (temp_pop_1->int_val) > (temp_pop_2->int_val);
                if (temp_pop_1->type == EXEC_STR && temp_pop_2->type == EXEC_STR)
                {
                    temp_result = compareFirstCharactersNEQ(temp_pop_1->str_val, temp_pop_2->str_val);
                }
            }

            else if (popped_ctrl_node->node->type == GE)
            {
                temp_result = (temp_pop_1->int_val) >= (temp_pop_2->int_val);
                if (temp_pop_1->type == EXEC_STR && temp_pop_2->type == EXEC_STR)
                {
                    temp_result = compareFirstCharactersEQ(temp_pop_1->str_val, temp_pop_2->str_val);
                }
            }

            else if (popped_ctrl_node->node->type == LE)
            {
                temp_result = (temp_pop_1->int_val) <= (temp_pop_2->int_val);
                if (temp_pop_1->type == EXEC_STR && temp_pop_2->type == EXEC_STR)
                {
                    temp_result = compareFirstCharactersEQ(temp_pop_2->str_val, temp_pop_1->str_val);
                }
            }

            else if (popped_ctrl_node->node->type == LS)
            {
                temp_result = (temp_pop_1->int_val) < (temp_pop_2->int_val);
                if (temp_pop_1->type == EXEC_STR && temp_pop_2->type == EXEC_STR)
                {
                    temp_result = compareFirstCharactersNEQ(temp_pop_2->str_val, temp_pop_1->str_val);
                }
            }

            else
            {

                if (temp_pop_1->type != EXEC_INT)
                {
                    cout << "ERROR -First Element on execution stack is either not INTEGER , STR or there is no element" << endl;
                    exit(0);
                }

                else if (popped_ctrl_node->node->type == DIV)
                    temp_result = (temp_pop_1->int_val) / (temp_pop_2->int_val);

                else if (popped_ctrl_node->node->type == PLUS)
                    temp_result = (temp_pop_1->int_val) + (temp_pop_2->int_val);

                else if (popped_ctrl_node->node->type == MINUS)
                    temp_result = (temp_pop_1->int_val) - (temp_pop_2->int_val);

                else if (popped_ctrl_node->node->type == MULTI)
                    temp_result = (temp_pop_1->int_val) * (temp_pop_2->int_val);
            }

            // create new exec_node. btw we don't need the popped ones anymore
            exec_node *new_exe_ele = new exec_node;

            if (popped_ctrl_node->node->type == GR || popped_ctrl_node->node->type == GE || (popped_ctrl_node->node->type == LE) || (popped_ctrl_node->node->type == LS))
            {
                if (temp_result)
                    new_exe_ele->type = EXEC_TRUE;
                else
                    new_exe_ele->type = EXEC_FALSE;
            }
            else
            {
                new_exe_ele->type = EXEC_INT;
                new_exe_ele->int_val = temp_result;
            }

            exec_stack.push(new_exe_ele);
        }

        else
        {
            cout << "CSE_machine execution - Some Unexpected type of node in Control Stack !" << endl;
            print_node(popped_ctrl_node->node);

            exit(0);
        }
    }

}
