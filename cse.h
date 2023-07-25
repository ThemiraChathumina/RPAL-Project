#ifndef CSE_H
#define CSE_H

#include "st.h"
#include <unordered_map>
#include <queue>


enum exec_type
{
    EXEC_ENV,
    EXEC_INT, 
    EXEC_STR, 
    EXEC_TUPPLE, 
    EXEC_CTRL_STRUCT, 
    EXEC_PRIMITIVE_FUNC, 
    EXEC_TRUE, 
    EXEC_FALSE, 
    EXEC_NILL, 
    EXEC_DUMMY, 
    EXEC_YSTAR, 
    EXEC_ITA
};

enum primitive_function 
{
    PRINT, 
    CONC, 
    ORDER, 
    STERN, 
    ISTUPLE, 
    ISINTEGER, 
    STEM, 
    ISTRUTHVALUE, 
    ISSTRING, 
    ITOS 
};
struct exec_node;
struct control_node;
struct environment
{
    unordered_map<string, exec_node*> bound_variable;
    environment *parent;
    environment(environment *p = nullptr) : parent(p) {}
};    

struct exec_node
{
    bool conc_flag;
    exec_type type;
    int int_val;
    string str_val;
    environment *env_val;
    control_node *ctrl_node_val;
    queue<exec_node*> queue_val;
    primitive_function primitive_func_val;
};

struct control_node
{
    Node *node;
    control_node *next;
    control_node *lambda_closure;
    Node *bound_variable;
    environment *env;
    int child_count;
    control_node(Node *n=nullptr, control_node *l = nullptr, Node *b = nullptr, environment *e = nullptr, int c = 0) : node(n), next(nullptr), lambda_closure(l), bound_variable(b), env(e), child_count(c) {}
};

void generateControlStruct(Node *node, control_node *ctrl_node);
void print_control_node(control_node *ctrl_node);
exec_node* checkEnvironment(string key, environment *env);
void initializeCSE(control_node *ctrl_node);
void load(control_node *ctrl_node);
void insert(string key, exec_node *value, environment *env);
void execute();
void print_tuple(exec_node *tuple);
void print_exec_node(exec_node *exec_node);

#endif