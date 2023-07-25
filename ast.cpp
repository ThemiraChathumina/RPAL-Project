#include "lexical_scanner.h"
#include "ast.h"
#include <iostream>
#include <unordered_set>
#include <stack>
#include <string>
#include <vector>
using namespace std;

Token next_token;
vector<Token> tokens;
stack<Node *> nodeStack;
int token_index = 0;

bool isStringInList(const vector<string> &stringList, const string &searchString)
{
    unordered_set<string> stringSet(stringList.begin(), stringList.end());
    return stringSet.find(searchString) != stringSet.end();
}
bool isKeyWord(const string &s)
{
    vector<string> keyWords = {"let", "where", "in", "fn", "and", "rec", "within", "aug", "not", "or", "gr", "ge", "ls", "le", "eq", "ne", "true", "false", "nil", "dummy"};
    return isStringInList(keyWords, s);
}

void Read(string expected)
{
    if (next_token.value != expected)
    {
        cout << "Error: unexpected token " << endl;
        exit(1);
    }
    if (token_index >= (int)tokens.size())
    {
        next_token.value = "";
        next_token.type = NAN;
        return;
    }
    next_token = tokens[token_index++];
}

void Build_tree(string x, int n, ASTType type)
{
    Node *p = nullptr;
    for (int i = 1; i <= n; i++)
    {
        Node *c = nodeStack.top();
        nodeStack.pop();
        c->right = p;
        p = c;
    }
    nodeStack.push(new Node(x, type, p, nullptr));
}

void D();
void Da();
void Db();
void Dr();
void Vb();
void Vl();
void E();
void Ew();
void T();
void Ta();
void Tc();
void B();
void Bt();
void Bs();
void Bp();
void A();
void At();
void Af();
void Ap();
void R();
void Rn();

void E()
{

    if (next_token.value == "let")
    {
        Read("let");
        D();
        Read("in");
        E();
        Build_tree("let", 2, LET);
    }
    else if (next_token.value == "fn")
    {
        Read("fn");
        int n = 0;
        while ((next_token.value == "(") or (next_token.type == IDENTIFIER))
        {
            Vb();
            n++;
        }
        Read(".");
        E();
        Build_tree("lambda", n + 1, LAMBDA);
    }
    else
    {
        Ew();
    }
}

void Ew()
{
    T();
    if (next_token.value == "where")
    {
        Read("where");
        Dr();
        Build_tree("where", 2, WHERE);
    }
}

void T()
{
    Ta();
    int n = 0;
    while (next_token.value == ",")
    {
        n++;
        Read(",");
        Ta();
    }
    if (n > 0)
    {
        Build_tree("tau", n + 1, TAU);
    }
}

void Ta()
{
    Tc();
    while (next_token.value == "aug")
    {
        Read("aug");
        Tc();
        Build_tree("aug", 2, AUG);
    }
}

void Tc()
{
    B();
    if (next_token.value == "->")
    {
        Read("->");
        Tc();
        Read("|");
        Tc();
        Build_tree("->", 3, COND);
    }
}

// B -> B ’or’ Bt => ’or’
// -> Bt ;
void B()
{
    Bt();
    while (next_token.value == "or")
    {
        Read("or");
        Bt();
        Build_tree("or", 2, OR);
    }
}

// Bt -> Bt ’&’ Bs => ’&’
// -> Bs ;
void Bt()
{
    Bs();
    while (next_token.value == "&")
    {
        Read("&");
        Bs();
        Build_tree("&", 2, AMP);
    }
}

// Bs -> ’not’ Bp => ’not’
// -> Bp ;
void Bs()
{
    if (next_token.value == "not")
    {
        Read("not");
        Bp();
        Build_tree("not", 1, NOT);
    }
    else
    {
        Bp();
    }
}

// Bp -> A (’gr’ | ’>’ ) A => ’gr’
// -> A (’ge’ | ’>=’) A => ’ge’
// -> A (’ls’ | ’<’ ) A => ’ls’
// -> A (’le’ | ’<=’) A => ’le’
// -> A ’eq’ A => ’eq’
// -> A ’ne’ A => ’ne’
// -> A ;
void Bp()
{
    A();
    if ((next_token.value == "gr") || next_token.value == ">")
    {
        Read(next_token.value);
        A();
        Build_tree("gr", 2, GR);
    }
    else if ((next_token.value == "ge") || next_token.value == ">=")
    {
        Read(next_token.value);
        A();
        Build_tree("ge", 2, GE);
    }
    else if ((next_token.value == "ls") || next_token.value == "<")
    {
        Read(next_token.value);
        A();
        Build_tree("ls", 2, LS);
    }
    else if ((next_token.value == "le") || next_token.value == "<=")
    {
        Read(next_token.value);
        A();
        Build_tree("le", 2, LE);
    }
    else if (next_token.value == "eq")
    {
        Read(next_token.value);
        A();
        Build_tree("eq", 2, EQ);
    }
    else if (next_token.value == "ne")
    {
        Read(next_token.value);
        A();
        Build_tree("ne", 2, NE);
    }
}

// A -> A ’+’ At => ’+’
// -> A ’-’ At => ’-’
// -> ’+’ At
// -> ’-’ At => ’neg’
// -> At ;
void A()
{

    if (next_token.value == "+")
    {
        Read("+");
        At();
    }
    else if (next_token.value == "-")
    {
        Read("-");
        At();
        Build_tree("neg", 1, NEG);
    }
    else
    {
        At();
    }
    while (next_token.value == "-" || next_token.value == "+")
    {
        string op = next_token.value;
        Read(op);
        At();
        if (op == "-")
            Build_tree(op, 2, MINUS);
        else
            Build_tree(op, 2, PLUS);
    }
}

// At -> At ’*’ Af => ’*’
// -> At ’/’ Af => ’/’
// -> Af ;
void At()
{
    Af();
    while (next_token.value == "*" || next_token.value == "/")
    {
        string op = next_token.value;
        Read(op);
        Af();
        if (op == "*")
            Build_tree(op, 2, MULTI);
        else
            Build_tree(op, 2, DIV);
    }
}

// Af -> Ap ’**’ Af => ’**’
// -> Ap ;
void Af()
{
    Ap();
    if (next_token.value == "**")
    {
        Read("**");
        Af();
        Build_tree("**", 2, EXPO);
    }
}

// Ap -> Ap ’@’ ’<IDENTIFIER>’ R => ’@’
// -> R ;
void Ap()
{
    R();
    while (next_token.value == "@")
    {
        Read("@");
        ASTType type;
        if (next_token.type == IDENTIFIER && !isKeyWord(next_token.value))
        {
            type = IDT;
        }
        else if (next_token.type == INTEGER)
        {
            type = INT;
        }
        else if (next_token.type == STRING)
        {
            type = STR;
        }
        else
        {
            cout << "Error: unexpected token " << endl;
            exit(1);
        }
        nodeStack.push(new Node(next_token.value, type));
        Read(next_token.value);
        R();
        Build_tree("@", 3, AT);
    }
}

// R -> R Rn => ’gamma’
// -> Rn ;
void R()
{
    Rn();
    while ((next_token.type == IDENTIFIER && !isKeyWord(next_token.value)) || next_token.type == INTEGER || next_token.type == STRING ||
           (next_token.value == "true") || (next_token.value == "false") || (next_token.value == "nil") ||
           next_token.value == "(" || next_token.value == "dummy")
    {
        Rn();
        Build_tree("gamma", 2, GAMMA);
    }
}

// Rn -> ’<IDENTIFIER>’
// -> ’<INTEGER>’
// -> ’<STRING>’
// -> ’true’ => ’true’
// -> ’false’ => ’false’
// -> ’nil’ => ’nil’
// -> ’(’ E ’)’
// -> ’dummy’ => ’dummy’ ;
void Rn()
{
    if ((next_token.type == IDENTIFIER && !isKeyWord(next_token.value)) || next_token.type == INTEGER || next_token.type == STRING)
    {
        ASTType type;
        if (next_token.type == IDENTIFIER && !isKeyWord(next_token.value))
        {
            type = IDT;
        }
        else if (next_token.type == INTEGER)
        {
            type = INT;
        }
        else if (next_token.type == STRING)
        {
            type = STR;
        }
        else
        {
            cout << "Error: unexpected token " << endl;
            exit(1);
        }
        nodeStack.push(new Node(next_token.value, type));
        Read(next_token.value);
    }
    else if (next_token.value == "(")
    {
        Read("(");
        E();
        Read(")");
    }
    else if (next_token.value == "true")
    {
        Read("true");
        Build_tree("true", 0, TRUE);
    }
    else if (next_token.value == "false")
    {
        Read("false");
        Build_tree("false", 0, FALSE);
    }
    else if (next_token.value == "nil")
    {
        Read("nil");
        Build_tree("nil", 0, NIL);
    }
    else
    {
        Read("dummy");
        Build_tree("dummy", 0, DUMMY);
    }
}

// D -> Da ’within’ D => ’within’
// -> Da ;
void D()
{
    Da();
    if (next_token.value == "within")
    {
        Read("within");
        D();
        Build_tree("within", 2, WITHIN);
    }
}

// Da -> Dr ( ’and’ Dr )+ => ’and’
// -> Dr ;
void Da()
{
    Dr();
    int n = 0;
    while (next_token.value == "and")
    {
        n++;
        Read("and");
        Dr();
    }
    if (n > 0)
    {
        Build_tree("and", n + 1, AND);
    }
}

// Dr -> ’rec’ Db => ’rec’
// -> Db ;
void Dr()
{
    if (next_token.value == "rec")
    {
        Read("rec");
        Db();
        Build_tree("rec", 1, REC);
    }
    else
    {
        Db();
    }
}

// Db -> Vl ’=’ E => ’=’
// -> ’<IDENTIFIER>’ Vb+ ’=’ E => ’fcn_form’
// -> ’(’ D ’)’ ;
void Db()
{
    if (next_token.value == "(")
    {
        Read("(");
        D();
        Read(")");
    }
    else if (next_token.type == IDENTIFIER && !isKeyWord(next_token.value))
    {
        ASTType type;
        if (next_token.type == IDENTIFIER && !isKeyWord(next_token.value))
        {
            type = IDT;
        }
        else if (next_token.type == INTEGER)
        {
            type = INT;
        }
        else if (next_token.type == STRING)
        {
            type = STR;
        }
        else
        {
            cout << "Error: unexpected token " << endl;
            exit(1);
        }
        nodeStack.push(new Node(next_token.value, type));
        Read(next_token.value);
        if (next_token.value == "," || next_token.value == "=")
        {
            Vl();
            Read("=");
            E();
            Build_tree("=", 2, EQUAL);
        }
        else
        {
            int n = 0;
            while (next_token.value == "(" || (next_token.type == IDENTIFIER && !isKeyWord(next_token.value)))
            {
                n++;
                Vb();
            }
            Read("=");
            E();
            Build_tree("fcn_form", n + 2, FUN_FORM);
        }
    }
}

// Vl -> ’<IDENTIFIER>’ list ’,’ => ’,’?;
void Vl()
{
    int n = 1;
    while (next_token.value == ",")
    {
        n++;
        Read(",");
        ASTType type;
        if (next_token.type == IDENTIFIER && !isKeyWord(next_token.value))
        {
            type = IDT;
        }
        else if (next_token.type == INTEGER)
        {
            type = INT;
        }
        else if (next_token.type == STRING)
        {
            type = STR;
        }
        else
        {
            cout << "Error: unexpected token " << endl;
            exit(1);
        }
        nodeStack.push(new Node(next_token.value, type));
        Read(next_token.value);
    }

    if (n > 1)
    {
        Build_tree(",", n, COMMA);
    }
}

// Vb -> ’<IDENTIFIER>’
// -> ’(’ Vl ’)’
// -> ’(’ ’)’ => ’()’;
void Vb()
{
    if ((next_token.type == IDENTIFIER && !isKeyWord(next_token.value)))
    {
        ASTType type;
        if (next_token.type == IDENTIFIER && !isKeyWord(next_token.value))
        {
            type = IDT;
        }
        else if (next_token.type == INTEGER)
        {
            type = INT;
        }
        else if (next_token.type == STRING)
        {
            type = STR;
        }
        else
        {
            cout << "Error: unexpected token " << endl;
            exit(1);
        }
        nodeStack.push(new Node(next_token.value, type));
        Read(next_token.value);
    }
    else if (next_token.value == "(")
    {
        Read("(");
        if (next_token.value == ")")
        {
            Read(")");
            Build_tree("()", 2, PARAN);
        }
        else
        {
            ASTType type;
            if (next_token.type == IDENTIFIER && !isKeyWord(next_token.value))
            {
                type = IDT;
            }
            else if (next_token.type == INTEGER)
            {
                type = INT;
            }
            else if (next_token.type == STRING)
            {
                type = STR;
            }
            else
            {
                cout << "Error: unexpected token " << endl;
                exit(1);
            }
            nodeStack.push(new Node(next_token.value, type));
            Read(next_token.value);
            Vl();
            Read(")");
        }
    }
}

void print_tree(Node *node, int indent = 0)
{
    if (!node)
        return;
    for (int i = 0; i < indent; i++)
        cout << ".";
    if (node->type != IDT && node->type != INT && node->type != STR && node->type != DUMMY && node->type != TRUE && node->type != FALSE && node->type != NIL && node->type != YSTAR)
    {
        cout << node->value << endl;
    }
    else
    {
        if (node->type == IDT)
            cout << "<ID:" << node->value << ">" << endl;
        else if (node->type == INT)
            cout << "<INT:" << node->value << ">" << endl;
        else if (node->type == STR)
            cout << "<STR:'" << node->value << "'>" << endl;
        else if (node->type == DUMMY)
            cout << "<DUMMY>" << endl;
        else if (node->type == TRUE)
            cout << "<TRUE>" << endl;
        else if (node->type == FALSE)
            cout << "<FALSE>" << endl;
        else if (node->type == NIL)
            cout << "<NIL>" << endl;
        else if (node->type == YSTAR)
            cout << "<Y*>" << endl;
    }
    print_tree(node->left, indent + 1);
    print_tree(node->right, indent);
}

void printAST(Node *node)
{
    print_tree(node);
}

Node *getAST(string filename)
{
    tokens = tokenizeFile(filename);

    Token token = tokens[token_index++];
    next_token = token;

    E();

    Node *root = nodeStack.top();

    return root;
}