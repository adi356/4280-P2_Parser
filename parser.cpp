#include "parser.h"

/* BNF Grammer
<program> -> <vars> xopen <stats> xclose
<vars>    -> empty | xdata <varList>
<varList> -> identifier : integer <Y>
<Y>       -> ; | <varList>
<exp>     -> <M> <X>
<X>       -> / <exp> | * <exp> | empty
<M>       -> <N> <Z>
<Z>       -> + <M> | empty
<N>       -> <R> <NN>
<NN>      -> - <N> | ~ <N> | empty
<R>       -> ( <exp> ) | identifier | integer
<stats>   -> <stat> <mStat>
<mStat>   -> empty | <stat> <mStat>
<stat>    -> <in> | <out> | <block> | <if> | <loop> | <assign>
<block>   -> {<vars> <stats>}
<in>      -> xin >> identifier;
<out>     -> xout << <exp>;
<if>      -> xcond [<exp> <RO> <exp>] <stat>
<loop>    -> xloop [<exp> <RO> <exp>] <stat>
<assign>  -> xlet identifier <exp>;
<RO>      -> <<(onetoken) | >> (one token) | < | >| = | %
*/

Parser::Parser(string fileName) {
    //check file is open and make sure associated with stream object
    string outputFileName = "filter-parser-temp.dat";
    scanner.filterScan(fileName, outputFileName);

    file.open(outputFileName.c_str());
    if (!file.is_open()) {
        cout << "[ERROR]: cannot open file" << endl;
        exit(EXIT_FAILURE);
    }

    EMPTY_tk.value = "EMPTY";
    scannerStatusCode = 0;
    getline(file, fileInput);
}

/*-----------------------------------
    non-terminal methods (change)
------------------------------------*/

// <program> -> <vars> xopen <stats> xclose
NodeT* Parser::program() {
    NodeT* node = createNode("<program>");

    node->c1 = vars(); // Parse <vars>

    // Check for xopen keyword
    if (tk.id == KW_tk && keyword_map[tk.value] == "openTk") {
        nextScan(); // Move to the next token after xopen
        node->c2 = stats(); // Parse <stats>

        // Check for xclose keyword
        if (tk.id == KW_tk && keyword_map[tk.value] == "closeTk") {
            nextScan(); // Move to the next token after xclose
            return node; // Return the parsed <program> node
        } else {
            expectedToken.assign("xclose");
            parserError();
        }
    } else {
        expectedToken.assign("xopen");
        parserError();
    }
    return node;
}

// <vars> -> empty | xdata <varList>
NodeT* Parser::vars() {
    NodeT* node = createNode("<vars>");

    if ((tk.id == KW_tk) && (keyword_map[tk.value] == "dataTk")) {
        nextScan();

        //check if IDtk
        if (tk.id == IDENT_tk) {
            node->c1 = varList();
            return node;
        } else {
            expectedToken.assign("IDENTIFIER");
            parserError();
        }
    } else {
        node->tokens.push_back(EMPTY_tk);
        return node;
    }
    exit(EXIT_FAILURE);
}

// <varList> -> identifier : integer <Y>
NodeT* Parser::varList() {
    NodeT* node = createNode("<varList>");

    // Check for identifier token
    if (tk.id == IDENT_tk) {
        node->tokens.push_back(tk);
        nextScan();

        // Check for colon token
        if ((tk.id == OP_tk) && (operator_map[tk.value] == "colonTk")) {
            nextScan();

            // Check for integer token
            if (tk.id == NUM_tk) {
                node->tokens.push_back(tk);
                nextScan();

                //proceed to Y
                node->c1 = Y();
                return node;
            } else {
                expectedToken.assign("INTEGER");
                parserError();
            }
        } else {
            expectedToken.assign(":");
            parserError();
        }
    } else {
        expectedToken.assign("IDENTIFIER");
        parserError();
    }

    exit(EXIT_FAILURE);
}

// <Y> -> ; | <varList>
NodeT* Parser::Y() {
    NodeT* node = createNode("<Y>");

    //check for semicolon 
    if ((tk.id == OP_tk) && (operator_map[tk.value] == "semiColonTk")) {
        nextScan();
        return node;
    } else {
        //recursive call to handle <varList>
        node->c1 = varList();
        return node;
    }
    exit(EXIT_FAILURE);

}

// <exp> -> <M> <X>
NodeT* Parser::exp() {
    NodeT* node = createNode("<exp>");

    node->c1 = M();
    node->c2 = X();

    return node;
}

// <X> -> / <exp> | * <exp> | empty
NodeT* Parser::X() {
    NodeT* node = createNode("<X>");

    //check for division or multiplication operators
    if ((tk.id == OP_tk) && (operator_map[tk.value] == "divideTk" || operator_map[tk.value] == "multiplyTk")) {
        node->tokens.push_back(tk); //store operator
        nextScan();
        node->c1 = exp(); //process recursively
    } else {
        node->tokens.push_back(EMPTY_tk); //if no operator have empty X
    }
    return node;
}

// <M> -> <N> <Z>
NodeT* Parser::M() {
    NodeT* node = createNode("<M>");

    node->c1 = N();
    node->c2 = Z();

    return node;
}

// <Z> -> + <M> | empty
NodeT* Parser::Z() {
    NodeT* node = createNode("<Z>");

    // check for addition operator
    if ((tk.id == OP_tk) && (operator_map[tk.value] == "plusTk")) {
        node->tokens.push_back(tk); 
        nextScan();

        node->c1 = M();
    } else {
        node->tokens.push_back(EMPTY_tk);
    }
    return node;
}


// <N> -> <R> <NN>
NodeT* Parser::N() {
    NodeT* node = createNode("<N>");

    node->c1 = R();
    node->c2 = NN();

    return node;
}

// <NN> -> - <N> | ~ <N> | empty
NodeT* Parser::NN() {
    NodeT* node = createNode("<NN>");

    if ((tk.id == OP_tk) && (operator_map[tk.value] == "minusTk")) {
        node->tokens.push_back(tk);
        nextScan();
        node->c1 = N();
    } else if ((tk.id == OP_tk) && (operator_map[tk.value] == "negateTk")) {
        node->tokens.push_back(tk);
        nextScan();
        node->c1 = N();
    } else {
        node->tokens.push_back(EMPTY_tk);
    }
    return node;
}

// <R> -> ( <exp> ) | identifier | integer
NodeT* Parser::R() {
    NodeT* node = createNode("<R>");

    if ((tk.id == OP_tk) && (operator_map[tk.value] == "leftParenTk")) {
        nextScan();
        node->c1 = exp();

        if ((tk.id == OP_tk) && (operator_map[tk.value] == "rightParenTk")) {
            nextScan();
            return node;
        } else {
            expectedToken.assign(")");
            parserError();
        }
    //check if IDtk
    } else if (tk.id == IDENT_tk) {
        node->tokens.push_back(tk);
        nextScan();
        return node;
    } else if (tk.id == NUM_tk) {
        node->tokens.push_back(tk);
        nextScan();
        return node;
    } else {
        expectedToken.assign("( or Identifier or Integer");
        parserError();
    }
    exit(EXIT_FAILURE);
}

// <stats> -> <stat> <mStat>
NodeT* Parser::stats() {
    NodeT* node = createNode("<stats>");

    node->c1 = stat();
    node->c2 = mStat();

    return node;
}

// <mStat> -> empty | <stat> <mStat>
NodeT* Parser::mStat() {
    NodeT* node = createNode("<mStat>");
    // // COMMITED CODE
    // Check if the current token indicates the end of statements
    if (tk.id != KW_tk || (keyword_map[tk.value] != "closeTk")) {
        node->c1 = stat(); // Process the current statement
        node->c2 = mStat(); // Continue processing additional statements
    } else {
        // If encountering closeTk, indicates an empty <mStat> (end of statements)
        node->tokens.push_back(EMPTY_tk);
        return node; // Return an empty <mStat>
    }

    // Error handling for unexpected token at the end of statements
    expectedToken.assign("Statement or end of block (closeTk)");
    parserError();
    exit(EXIT_FAILURE);

}

// <stat> -> <in> | <out> | <block> | <if> | <loop> | <assign>
NodeT* Parser::stat() {
    NodeT* node = createNode("<stat>");

    //COMMITED CODE
    // // Determine which type of statement it is
    // if (tk.id == KW_tk) {
    //     if (keyword_map[tk.value] == "inTk") {
    //         node->c1 = in();
    //     } else if (keyword_map[tk.value] == "outTk") {
    //         node->c1 = out();
    //     } else if (keyword_map[tk.value] == "condTk") {
    //         node->c1 = ifxcond();
    //     } else if (keyword_map[tk.value] == "loopTk") {
    //         node->c1 = loop();
    //     } else if (keyword_map[tk.value] == "letTk") {
    //         node->c1 = assign();
    //     } else if (keyword_map[tk.value] == "openCurlyTk") {
    //         node->c1 = block();
    //     } else {
    //         expectedToken.assign("Statement");
    //         parserError();
    //         exit(EXIT_FAILURE);
    //     }
    // } else {
    //     expectedToken.assign("Statement");
    //     parserError();
    //     exit(EXIT_FAILURE);
    // }
    
    // return node;
    // TODO: still issue with stat
    if (tk.id == KW_tk) {
            if (tk.data == "xin") {
                node->c1 = in();
            } else if (tk.data == "xout") {
                node->c1 = out();
            } else if (tk.data == "xcond") {
                node->c1 = ifxcond();
            } else if (tk.data == "xlet") {
                node->c1 = assign();
            } else if (tk.data == "xloop") {
                node->c1 = loop();
            } else {
                expectedToken.assign("statement");
                parserError();
                exit(EXIT_FAILURE);
            }
    } else if ((tk.id == OP_tk) && (operator_map[tk.value] == "leftCurlyTk")) {
        node->c1 = block();
        return node;
    } else {
            expectedToken.assign("Staaatement");
            parserError();
            exit(EXIT_FAILURE);
    }
        return node;
}

// <block> -> {<vars> <stats>}
NodeT* Parser::block() {
    NodeT* node = createNode("<block>");

    // Check for left curly brace
    if ((tk.id == OP_tk) && (operator_map[tk.value] == "leftCurlyTk")) {
        nextScan(); // Move to the next token
        node->c1 = vars(); // Parse variables inside the block
        node->c2 = stats(); // Parse statements inside the block

        // Check for right curly brace
        if ((tk.id == OP_tk) && (operator_map[tk.value] == "rightCurlyTk")) {
            nextScan(); // Move to the next token after the block
            return node;
        } else {
            expectedToken.assign("}");
            parserError();
            exit(EXIT_FAILURE);
        }
    } else {
        expectedToken.assign("{");
        parserError();
        exit(EXIT_FAILURE);
    }
}
// <in> -> xin >> identifier;
NodeT* Parser::in() {
    NodeT* node = createNode("<in>");

    //check for xin keyword
    if ((tk.id == KW_tk) && (keyword_map[tk.value] == "inTk")) {
        nextScan();

        //check for >>
        if ((tk.id == OP_tk) && (operator_map[tk.value] == "nsGreaterTk")) {
            nextScan();

            //check for identifier
            if (tk.id == IDENT_tk) {
                node->tokens.push_back(tk);
                nextScan();
                //check for ;
                if ((tk.id == OP_tk) && (operator_map[tk.value] == "semiColonTk")) {
                    nextScan();
                    return node; //return <in> node
                } else {
                    expectedToken.assign(";");
                    parserError();
                }
            } else {
                expectedToken.assign("IDENTIFIER");
                parserError();
            }
        } else {
            expectedToken.assign(">>");
            parserError();
        }
    } else {
        expectedToken.assign("xin");
        parserError();
    }
    exit(EXIT_FAILURE);
}

// <out> -> xout << <exp>;
NodeT* Parser::out() {
    NodeT* node = createNode("<out>");

    // Check for xout keyword
    if ((tk.id == KW_tk) && (keyword_map[tk.value] == "outTk")) {
        nextScan();

        // Check for <<
        if ((tk.id == OP_tk) && (operator_map[tk.value] == "nsLessTk")) {
            nextScan();

            // Parse expression
            node->c1 = exp();

            // Check for ;
            if ((tk.id == OP_tk) && (operator_map[tk.value] == "semiColonTk")) {
                nextScan();
                return node; // Return <out> node
            } else {
                expectedToken.assign(";");
                parserError();
            }
        } else {
            expectedToken.assign("<<");
            parserError();
        }
    } else {
        expectedToken.assign("xout");
        parserError();
    }
    exit(EXIT_FAILURE); 
}

// <if> -> xcond [<exp> <RO> <exp>] <stat>
NodeT* Parser::ifxcond() {
   NodeT* node = createNode("<if>");

   if ((tk.id == KW_tk) && (keyword_map[tk.value] == "condTk")) {
        nextScan(); // Move to the next token after 'xcond'

        //if (tk.value == "[") {
        if ((tk.id == OP_tk) && (operator_map[tk.value] == "leftBracketTk")) {
            nextScan(); // Move past '['
            node->c1 = exp(); // Parse the first expression

            node->c2 = R0(); // Parse the relational operator

            node->c3 = exp(); // Parse the second expression

            if ((tk.id == OP_tk) && (operator_map[tk.value] == "rightBracketTk")) {
                nextScan(); // Move past ']'

                node->c4 = stat(); // Parse the statement inside if

                return node;
            } else {
                expectedToken.assign("]");
                parserError();
            }
        } else {
            expectedToken.assign("[");
            parserError();
        }
    } else {
        expectedToken.assign("xcond");
        parserError();
    }

    exit(EXIT_FAILURE); 
}
        

// <loop> -> xloop [<exp> <RO> <exp>] <stat>
NodeT* Parser::loop() {
   NodeT* node = createNode("<loop>");

    if ((tk.id == KW_tk) && (keyword_map[tk.value] == "loopTk")) {
        nextScan(); // Move to the next token after 'xloop'

        if ((tk.id = OP_tk) && (operator_map[tk.value] == "leftBracketTk")) {
            nextScan(); // Move past '['
            node->c1 = exp(); // Parse the first expression

            node->c2 = R0(); // Parse the relational operator

            node->c3 = exp(); // Parse the second expression

            if ((tk.id = OP_tk) && (operator_map[tk.value] == "rightBracketTk")) {
                nextScan(); // Move past ']'

                node->c4 = stat(); // Parse the statement inside loop

                return node;
            } else {
                expectedToken.assign("]");
                parserError();
            }
        } else {
            expectedToken.assign("[");
            parserError();
        }
    } else {
        expectedToken.assign("xloop");
        parserError();
    }

    exit(EXIT_FAILURE);
}

// <assign> -> xlet identifier <exp>;
NodeT* Parser::assign() {
    NodeT* node = createNode("<assign>");

    //COMMITED CODE
    // //check for xlet
    // if ((tk.id == KW_tk) && (keyword_map[tk.value] == "letTk")) {
    //     nextScan();
    //     //check for identifier
    //     if (tk.id == IDENT_tk) {
    //         node->tokens.push_back(tk); // Store the identifier token
    //         nextScan();

    //         if ((tk.id == OP_tk) && (operator_map[tk.value] == "equalTk")) {
    //             nextScan();
    //             node->c1 = exp(); // Parse the expression
    //             return node;
    //         } else {
    //             expectedToken.assign("=");
    //             parserError();
    //         }
    //     } else {
    //         expectedToken.assign("IDENTIFIER");
    //         parserError();
    //     }
    // } else {
    //     expectedToken.assign("xlet");
    //     parserError();
    // }
    // exit(EXIT_FAILURE);

    // Check for xlet keyword
    if ((tk.id == KW_tk) && (keyword_map[tk.value] == "letTk")) {
        nextScan();

        // Check for identifier
        if (tk.id == IDENT_tk) {
            node->tokens.push_back(tk); // Store the identifier token
            nextScan();

            node->c1 = exp();
            
            //check for ;
            if ((tk.id == OP_tk) && (operator_map[tk.value] == "semiColonTk")) {
                nextScan();
                return node;
            } else {
                expectedToken.assign(";");
                parserError();
            }
        } else {
            expectedToken.assign("IDENTIFIER");
            parserError();
        }
    } else {
        expectedToken.assign("xlet");
        parserError();
    }
    exit(EXIT_FAILURE);

}

// <R0> -> <<(onetoken) | >> (one token) | < | >| = | %
NodeT* Parser::R0() {
    NodeT* node = createNode("<RO>");

    if ((tk.id == OP_tk) && ((operator_map[tk.value] == "nsLessTk") ||
        (operator_map[tk.value] == "nsGreaterTk") || (operator_map[tk.value] == "lessThanTk") ||
        (operator_map[tk.value] == "greaterThanTk") || (operator_map[tk.value] == "equalTk") ||
        (operator_map[tk.value] == "modulusTk"))) {
        node->tokens.push_back(tk); // Store the relational operator
        nextScan();
        return node;
    } else {
        expectedToken.assign("Relational Operator");
        parserError();
    }
    exit(EXIT_FAILURE);
}

void Parser::nextScan() {
    do {
        if (scannerStatusCode == -1) {
            if (!getline(file, fileInput)) {
                if (file.eof()) {
                    scanner.isCommentMode();
                    scanner.invokeEOF(tk);
                    return;
                }
            }
        }
        scannerStatusCode = scanner.scan(fileInput, tk);
        /* For Debugging
        if(scannerStatusCode != -1) {
            scanner.tokenToString(tk);
        }*/
    } while (scannerStatusCode == -1);
}

NodeT* Parser::createNode(string production_name) {
    NodeT* node = new NodeT;
    node->label = production_name;
    node->c1 = NULL;
    node->c2 = NULL;
    node->c3 = NULL;
    node->c4 = NULL;
    return node;
}

//construct parse tree
NodeT* Parser::parse() {
    NodeT* root = NULL;
    nextScan();
    root = program();

    //if last token is not EOF_tk error has occured, and exit
    if (tk.id != EOF_tk) {
        expectedToken.assign("EOF_tk");
        parserError();
        exit(EXIT_FAILURE);
    } else {
        cout << "Parsing completed with no errors. Generating tree..." << endl;
        file.close();
        return root;
    }
}

//print error message with line number
void Parser::parserError() {
    cout << "[ERROR] parsing error at " << scanner.getScannerPosition() << "-> expected ["
    << expectedToken << "] but received [" << tk.value << "]" << endl;
    file.close();
    exit(EXIT_FAILURE);
}