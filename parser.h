#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <fstream>
#include <string>

#include "node.h"
#include "scanner.h"

using namespace std;

/*-----------------------------
         Parser Class          
------------------------------*/

class Parser: public Scanner {
    private: 
        ifstream file;          //file pointer
        string fileInput;       //current line of input file

        Token tk;               //token holder
        Token EMPTY_tk;         //empty token holder
        string expectedToken;   //keep track of expected token when there is error

        Scanner scanner;        //scanner object
        int scannerStatusCode;  //keep track scanner status -> grab new line or use old line

        //BNF
        NodeT *program();
        NodeT *vars();
        NodeT *varList();
        NodeT *Y();
        NodeT *exp();
        NodeT *X();
        NodeT *M();
        NodeT *Z();
        NodeT *N();
        NodeT *NN();
        NodeT *R();
        NodeT *stats();
        NodeT *mStat();
        NodeT *stat();
        NodeT *block();
        NodeT *in();
        NodeT *out();
        //figure out name for if()
        NodeT *ifxcond();
        NodeT *loop();
        NodeT *assign();
        NodeT *R0();

        //Parser function
        void nextScan();
        NodeT *createNode(string productionName);
        void parserError();

    public:
        Parser(string fileName);
        NodeT *parse();
};
#endif