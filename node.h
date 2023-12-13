#ifndef NODE_H
#define NODE_H
#include <string>
#include <vector>
#include "token.h"
using namespace std;

typedef struct node_t {
    int level;                        //level of node
    string label;                     //label corresponds to nonterminal (EX: node.labe = <loop>)
    vector<Token> tokens;             //the token of this node which stores identifiers, operators, etc.
    struct node_t *c1, *c2, *c3, *c4; //can be dynamic (EX: vector<struct node_t> child)
}NodeT;

#endif