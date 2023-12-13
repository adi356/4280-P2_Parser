#ifndef TESTTREE_H
#define TESTTREE_H

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include "node.h"
using namespace std;

void traversePreorder(NodeT *node, int depth = 0, string indent = "", bool last = true);

#endif