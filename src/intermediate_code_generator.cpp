/* INTERMEDIATE CODE GENERATOR V5.0  */
/*            12/12/2025             */

/* Standard C++ Libraries for User I/O, File I/O */
#include <iostream>
#include <fstream>

/* Global declarations */
std::ifstream inptf;
std::ofstream optf;
/******************************************************/
/* Symbol Table Entry Struct */
enum class IntrinsicType {INT, FLOAT, KEYWORD, NONE};
union intorfloat {int i; float f;};
struct SymbolTableEntry {
    char name[25];
    bool is_identifier;
    IntrinsicType type;
    void *lvalue_ptr;

    SymbolTableEntry() {
        this->name[0] = '\0';
        this->is_identifier = false;
        this->type = IntrinsicType::NONE;
        this->lvalue_ptr = nullptr;
    }

    SymbolTableEntry(const char *name, bool is_ident, IntrinsicType intrinsic_type) {
        int i = 0;
        while(name[i] != '\0' && i < 24) {
            this->name[i] = name[i];
            i++;
        }
        this->name[i] = '\0';
        this->is_identifier = is_ident;
        this->type = intrinsic_type;
        this->lvalue_ptr = nullptr;
    }

    SymbolTableEntry(const char *name, bool is_ident, IntrinsicType intrinsic_type, intorfloat val) {
        int i = 0;
        while(name[i] != '\0' && i < 24) {
            this->name[i] = name[i];
            i++;
        }
        this->name[i] = '\0';
        this->is_identifier = is_ident;
        this->type = intrinsic_type;
        switch(this->type) {
            case(IntrinsicType::INT):
                lvalue_ptr = new int(val.i); break;
            case(IntrinsicType::FLOAT):
                lvalue_ptr = new float(val.f); break;
            default:
                this->lvalue_ptr = nullptr;
        }
    }

    void printEntry() {
        optf << "\nEntry name: " << this->name << 
                     "\nIs Identifier: " << (this->is_identifier ? "True" : "False") <<
                     "\nType: ";
        switch(this->type) {
            case(IntrinsicType::INT):
                optf << "INT"; break;
            case(IntrinsicType::FLOAT):
                optf << "FLOAT"; break;
            case(IntrinsicType::KEYWORD):
                optf << "KEYWORD"; break;
            default:
                optf << "NONE";
        }
        if(lvalue_ptr == nullptr) {
            optf << "\nlvalue: NULL\nrvalue: NULL" << std::endl;
        } else {
            optf << "\nlvalue: " << this->lvalue_ptr <<
                         "\nrvalue: ";
            if(this->type == IntrinsicType::INT)
                optf << *static_cast<int*>(this->lvalue_ptr) << std::endl;
            else
                optf << *static_cast<float*>(this->lvalue_ptr) << std::endl;
        }
    }

    void modifyValue(intorfloat val) {
        switch(this->type) {
            case(IntrinsicType::INT):
                *static_cast<int*>(lvalue_ptr) = val.i; break;
            case(IntrinsicType::FLOAT):
                *static_cast<float*>(lvalue_ptr) = val.f; break;
        }
    }

    void modifyType(IntrinsicType type) {
        if(this->lvalue_ptr != nullptr) {
            if(this->type == IntrinsicType::INT)
                delete static_cast<int*>(this->lvalue_ptr);
            else if(this->type == IntrinsicType::FLOAT)
                delete static_cast<float*>(this->lvalue_ptr);
            this->lvalue_ptr = nullptr;
        }
        this->type = type;
        switch(type) {
            case(IntrinsicType::INT):
                this->lvalue_ptr = new int; break;
            case(IntrinsicType::FLOAT):
                this->lvalue_ptr = new float; break;
        }
    }

    bool hasName(const char* name) {
        int i;
        for(i = 0; i < 24 && this->name[i] != '\0' && name[i] != '\0'; i++)
            if(this->name[i] != name[i])
                return false;
        return this->name[i] == name[i];
    }

    // Deconstructor
    ~SymbolTableEntry() {
        if(this->lvalue_ptr != nullptr) {
            if(this->type == IntrinsicType::INT)
                delete static_cast<int*>(this->lvalue_ptr);
            else if(this->type == IntrinsicType::FLOAT)
                delete static_cast<float*>(this->lvalue_ptr);
            this->lvalue_ptr = nullptr;
        }
    }

    // Copy Constructor
    SymbolTableEntry(const SymbolTableEntry& copy) {
        int i = 0;
        for(; i < 24 && copy.name[i] != '\0'; i++)
            this->name[i] = copy.name[i];
        this->name[i] = '\0';
        this->is_identifier = copy.is_identifier;
        this->type = copy.type;
        if(copy.lvalue_ptr != nullptr) {
            switch(this->type) {
                case(IntrinsicType::INT):
                    this->lvalue_ptr = new int(*static_cast<int*>(copy.lvalue_ptr)); break;
                case(IntrinsicType::FLOAT):
                    this->lvalue_ptr = new float(*static_cast<float*>(copy.lvalue_ptr)); break;
                default:
                    this->lvalue_ptr = nullptr;
            }
        } else
            this->lvalue_ptr = nullptr;
    }

    // Copy Assignment Constructor
    SymbolTableEntry& operator=(const SymbolTableEntry& other) {
        if(this == &other)
            return *this;
        if(this->lvalue_ptr != nullptr) {
            if(this->type == IntrinsicType::INT)
                delete static_cast<int*>(this->lvalue_ptr);
            else if(this->type == IntrinsicType::FLOAT)
                delete static_cast<float*>(this->lvalue_ptr);
            this->lvalue_ptr = nullptr;
        }
        int i = 0;
        for(; i < 24 && other.name[i] != '\0'; i++)
            this->name[i] = other.name[i];
        this->name[i] = '\0';
        this->is_identifier = other.is_identifier;
        this->type = other.type;
        if(other.lvalue_ptr != nullptr) {
            switch(this->type) {
                case(IntrinsicType::INT):
                    this->lvalue_ptr = new int(*static_cast<int*>(other.lvalue_ptr)); break;
                case(IntrinsicType::FLOAT):
                    this->lvalue_ptr = new float(*static_cast<float*>(other.lvalue_ptr)); break;
                default:
                    this->lvalue_ptr = nullptr;
            }
        } else
            this->lvalue_ptr = nullptr;
        return *this;
    }
};
/* End of Symbol Table Entry Struct */
/******************************************************/
/* Symbol Table List Struct */
struct SymbolTableList {
    /* Symbol Table Node Struct */
    struct SymbolTableNode {
        SymbolTableEntry entry;
        SymbolTableNode *next;

        // Default Constructor
        SymbolTableNode() {
            this->entry = SymbolTableEntry();
            this->next = nullptr;
        }

        // Constructor with parameter of SymbolTableEntry
        SymbolTableNode(const SymbolTableEntry &x) {
            this->entry = x;
            this->next = nullptr;
        }
    };
    /* End of SymbolTableNode struct */
    int size;
    SymbolTableNode *head, *tail;

    // Default Constructor
    SymbolTableList() {
        this->head = nullptr;
        this->tail = nullptr;
        size = 0;
    }

    // Appends to the end of the list
    void append(const SymbolTableEntry& entry) {
        if(size++ == 0) {
            this->head = new SymbolTableNode(entry);
            this->tail = this->head;
        } else {
            this->tail->next = new SymbolTableNode(entry);
            this->tail = this->tail->next;
        }
    }

    // Checks if our list contains an entry with the name s1
    bool contains(const char* s1) {
        SymbolTableNode *curr_node = this->head;
        while(curr_node != nullptr) {
            if(curr_node->entry.hasName(s1))
                return true;
            curr_node = curr_node->next;
        }
        return false;
    }

    // Searches and returns a pointer to the node that contains an entry with the name we're looking for
    SymbolTableNode* search(const char* name) {
        SymbolTableNode *curr_node = this->head;
        while(curr_node != nullptr) {
            if(curr_node->entry.hasName(name))
                return curr_node;
            curr_node = curr_node->next;
        }
        return nullptr;
    }

    // Modifies the node with the entry name with a new val
    void modifyValue(const char* name, intorfloat val) {
        SymbolTableNode *curr_node = search(name);
        if(curr_node != nullptr) {
            switch(curr_node->entry.type) {
                case(IntrinsicType::INT):
                    *static_cast<int*>(curr_node->entry.lvalue_ptr) = val.i; break;
                case(IntrinsicType::FLOAT):
                    *static_cast<float*>(curr_node->entry.lvalue_ptr) = val.f; break;
            }
        }
    }

    // Prints SymbolTableList
    void print() {
        SymbolTableNode *curr_node = this->head;
        optf << "\nSymbol Table has " << this->size << " entries.\n";
        while(curr_node != nullptr) {
            curr_node->entry.printEntry();
            curr_node = curr_node->next;
        }
        optf << std::endl;
    }

    // Deconstructor
    ~SymbolTableList() {
        while(this->head != nullptr) {
            SymbolTableNode *curr_node = this->head->next;
            delete head;
            this->head = curr_node;
        }
        this->tail = nullptr;
        this->size = 0;
    }
};
/* Runtime stack value */
struct StackValue {
    bool isAddr;         // true => this represents an address (use .addr)
    IntrinsicType type;  // INT or FLOAT (meaningful when isAddr==false)
    int i;
    float f;
    void* addr;

    static StackValue makeAddr(void* a) {
        StackValue s;
        s.isAddr = true;
        s.addr = a; 
        s.type = IntrinsicType::NONE; 
        s.i = 0; s.f = 0.0f; 
        return s;
    }
    static StackValue makeInt(int v) {
        StackValue s;
        s.isAddr = false;
        s.type = IntrinsicType::INT; 
        s.i = v; 
        s.f = (float)v; 
        s.addr = nullptr; 
        return s;
    }
    static StackValue makeFloat(float v) {
        StackValue s;
        s.isAddr = false;
        s.type = IntrinsicType::FLOAT;
        s.f = v; s.i = (int)v;
        s.addr = nullptr;
        return s;
    }
};
/******************************************************/
/* Variables */
int charClass;
char nextChar;
int lexLen;
int scannedToken = 0;
char scannedLexeme[100];
int nextToken = 0;
char lexeme [100];
int futureToken = 0;
char futureLexeme[100];
int futureFutureToken = 0;
char futureFutureLexeme[100];
bool successful_parse;
SymbolTableList symbol_table;
/* Character classes */
#define LETTER 0
#define DIGIT 1
#define EOL 98
#define UNKNOWN 99
/* Token codes */
enum Token {
    INT_CONST     = 10,
    FLOAT_CONST   = 11,
    IDENT         = 12,
    ASSIGN_OP     = 20,
    ADD_OP        = 21,
    SUB_OP        = 22,
    MULT_OP       = 23,
    DIV_OP        = 24,
    LEFT_PAREN    = 25,
    RIGHT_PAREN   = 26,
    SEP_SEMICOMMA = 41,
    SEP_COMMA     = 42,
    KEYWORD_INT   = 51,
    KEYWORD_FLOAT = 52
};
enum Type {INT = 1, FLOAT = 2};
/******************************************************/
/* SNode Class */
enum class Label{OP, INT_CONST, FLOAT_CONST, IDENT};
union SNodeVal {
    int integer;
    float floating;
    const char *token;
    Token op;
};
struct SNode {
    Label label;
    SNodeVal content;
    int string_size;
    SNode *left, *right;
    Type computed_type, expected_type, actual_type;
    bool addr_indicator;
    bool evaluated;
    SNode() {
        this->label = Label::OP;
        this->content.token = "NULL";
        this->left = nullptr;
        this->right = nullptr;
        this->string_size = 0;
        this->addr_indicator = false;
        this->evaluated = false;
    }
    SNode(Label label, SNodeVal content, SNode *left, SNode *right) {
        this->label = label;
        if(this->label == Label::IDENT)
            this->content.token = symbol_table.search(content.token)->entry.name;
        else
            this->content = content;
        this->left = left;
        this->right = right;
        this->addr_indicator = false;
    }
    void printNode() {
        std::cout << "Node Contents:";
        switch(this->label) {
            case(Label::OP):
                std::cout << "\nLabel: OP";
                switch(this->content.op) {
                    case(ADD_OP):
                        std::cout << "\nContent: +"; break;
                    case(SUB_OP):
                        std::cout << "\nContent: -"; break;
                    case(MULT_OP):
                        std::cout << "\nContent: *"; break;
                    case(DIV_OP):
                        std::cout << "\nContent: /"; break;
                    case(ASSIGN_OP):
                        std::cout << "\nContent: ="; break;
                }
                break;
            case(Label::IDENT):
                std::cout << "\nLabel: IDENT" <<
                             "\nContent: " << this->content.token;
                break;
            case(Label::INT_CONST):
                std::cout << "\nLabel: INT_CONST" <<
                             "\nContent: " << this->content.integer;
                break;
            case(Label::FLOAT_CONST):
                std::cout << "\nLabel: FLOAT_CONST" <<
                             "\nContent: " << this->content.floating;
        }
        std::cout << "\nThis: " << this;
        std::cout << "\nLeft: ";
        if(this->left != nullptr)
            std::cout << this->left;
        else
            std::cout << "NULL";
        std::cout << "\nRight: ";
        if(this->right != nullptr)
            std::cout << this->right;
        else
            std::cout << "NULL";
        std::cout << "\nActual Type: " << this->actual_type << 
                     "\nExpected Type: " << this->expected_type << 
                     "\nComputed Type: " << this->computed_type <<
                     "\nAddress Indicator " << this->addr_indicator << std::endl;
    }
    ~SNode() {
        if(this->left != nullptr) {
            delete this->left;
            this->left = nullptr;
        }
        if(this->right != nullptr) {
            delete this->right;
            this->right = nullptr;
        }
    }
};
/******************************************************/
/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
void lexScan();
void lex();
SNode* declare_list();
SNode* assign_list();
SNode* assign();
SNode* expr();
SNode* term();
SNode* factor();
bool initializeSymbolTable();
void primeLexAnalyzer();
void setActualType(SNode* parse_tree);
void setExpectedType(SNode* parse_tree);
void setComputedType(SNode* parse_tree, bool is_lhs_of_assign);
void semanticAnalyzer(SNode* parse_tree);
void printIntermediateCode(SNode* type_checked_parse_tree);
void traversePrint(SNode* root);
int lexemeToInt();
float lexemeToFloat();
StackValue runStackMachine(SNode* root);
/******************************************************/
/* Look up a symbol-table entry by its lvalue pointer */
// Find the symbol-table node that stores at this address (linear search)
SymbolTableList::SymbolTableNode* searchByAddress(void* addr) {
    SymbolTableList::SymbolTableNode* cur = symbol_table.head;
    while (cur != nullptr) {
        if (cur->entry.lvalue_ptr == addr) return cur;
        cur = cur->next;
    }
    return nullptr;
}
/******************************************************/
/* main driver */
int main() {
    /* Open the input data file and process its contents */
    inptf.open("input.txt");
    if(inptf.is_open()) {
        optf.open("output.txt");
        if(optf.is_open()) {
            if(!initializeSymbolTable())
                return 1;
            optf << "================================================\n";
            primeLexAnalyzer();
            while (true) {
                /* ----------------------------
                1. End-of-file?
                ----------------------------- */
                if (nextToken == EOF)
                    break;

                /* ----------------------------
                2. Blank line? Skip EOL
                ----------------------------- */
                if (nextToken == EOL)
                {
                    lex();      // consume EOL
                    continue;
                }

                /* ----------------------------
                3. Parse One Statement
                declare_list() will decide:
                    - <declare_list>
                    - <assign_list>
                ----------------------------- */
                successful_parse = true;
                declare_list();    // OR assign_list() depending on first token

                if (!successful_parse)
                {
                    std::cout << "Stopping due to syntax or semantic error.\n";
                    break;
                }

                /* ----------------------------
                4. Must consume exactly one EOL
                    after a complete statement
                ----------------------------- */
                if (nextToken == EOL)
                    lex();      // consume the EOL

                // If nextToken == EOF, loop will exit on next evaluation
            }
            symbol_table.print();
            optf << "================================================";
        } else
            optf << "ERROR: Failed to open \"output.txt\".\n";
    } else
        optf << "ERROR: Failed to open \"input.txt\".\n";
    return 0;
}
/*****************************************************/
/* lookup - a function to lookup operators and parentheses
and return the token */
int lookup(char ch) {
    switch (ch) {
        case '(':
            addChar();
            scannedToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            scannedToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            scannedToken = ADD_OP;
            break;
        case '-':
            addChar();
            scannedToken = SUB_OP;
            break;
        case '*':
            addChar();
            scannedToken = MULT_OP;
            break;
        case '/':
            addChar();
            scannedToken = DIV_OP;
            break;
        case '=':
            addChar();
            scannedToken = ASSIGN_OP;
            break;
        case ',':
            addChar();
            scannedToken = SEP_COMMA;
            break;
        case ';':
            addChar();
            scannedToken = SEP_SEMICOMMA;
            break;
        case '\n':
            scannedToken = EOL;
            break;
        default:
            addChar();
            scannedToken = EOF;
            break;
    }
    return scannedToken;
}
/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar() {
    if (lexLen <= 98) {
        scannedLexeme[lexLen++] = nextChar;
        scannedLexeme[lexLen] = 0;
    }
    else {
        successful_parse = false;
        std::cout << "ERROR addChar(): Lexeme is too long." << std::endl;
    }
}
/*****************************************************/
/* getChar - a function to get the next character of
input and determine its character class */
void getChar() {
    if((nextChar = inptf.get()) != EOF) {
        if(isalpha(nextChar))
            charClass = LETTER;
        else if(isdigit(nextChar))
            charClass = DIGIT;
        else 
            charClass = UNKNOWN;
    } else
        charClass = EOF;
}
/*****************************************************/
/* getNonBlank - a function to call getChar until it 
returns a non-whitespace or newline character */
void getNonBlank() {
    while (nextChar == ' ' || nextChar == '\t' || nextChar == '\r' || nextChar == '\f' || nextChar == '\v') {
        getChar();
    }
}
/*****************************************************/
SNode* declare_list() {
    IntrinsicType type;
    if (nextToken == KEYWORD_INT)
        type = IntrinsicType::INT;
    else if (nextToken == KEYWORD_FLOAT)
        type = IntrinsicType::FLOAT;
    else {
        SNode* assign_list_snode_ptr = assign_list();
        if(!successful_parse || assign_list_snode_ptr == nullptr)
            return nullptr;
        semanticAnalyzer(assign_list_snode_ptr);
        return assign_list_snode_ptr;
    }
    lex();  // move past 'int' or 'float'
    do {
        // Expect identifier
        if (nextToken != IDENT) {
            successful_parse = false;
            std::cout << "ERROR <declare_list>: Expected Identifier.\n";
            return nullptr;
        }
        // Check redeclaration
        if (symbol_table.contains(lexeme)) {
            successful_parse = false;
            std::cout << "ERROR <declare_list>: " << lexeme << " already declared.\n";
            return nullptr;
        }
        // Add to symbol table
        symbol_table.append(SymbolTableEntry(lexeme, true, type));
        // Allocate storage for this variable
        SymbolTableList::SymbolTableNode *entry = symbol_table.search(lexeme);
        if (type == IntrinsicType::INT)
            entry->entry.lvalue_ptr = new int(0);       // default int value
        else
            entry->entry.lvalue_ptr = new float(0.0f);  // default float value
        // Build IDENT node
        SNodeVal val;
        val.token = entry->entry.name;
        SNode* ident_node = new SNode(Label::IDENT, val, nullptr, nullptr);
        lex();  // consume identifier
        // Handle optional assignment
        if (nextToken == ASSIGN_OP) {
            val.op = ASSIGN_OP;
            lex(); // consume '='
            SNode* expr_node = expr();
            if (!successful_parse || expr_node == nullptr)
                return nullptr;
            SNode* assign_node = new SNode(Label::OP, val, ident_node, expr_node);
            semanticAnalyzer(assign_node);
            delete assign_node;
        } else {
            // Bare declaration: int x;
            //std::cout << "declare " << val.token << " (" << symbol_table.search(val.token)->entry.lvalue_ptr << ")" << std::endl;
        }
        // Done processing this declaration
        if (nextToken == SEP_COMMA)
            lex();    // skip comma and loop again
        else
            break;    // declaration list ends
    } while (true);
    return nullptr; // not used, but OK since each decl is processed independently
}
/*****************************************************/
/* assign_list - Parses strings in the language generated by the rule:  */
/* <assign_list> -> {<ident> = } <assign>                               */
SNode *assign_list() {
    SNode *ident_snodeptr[10];
    SNodeVal val;
    int count = 0;
    if(nextToken != IDENT) {
        successful_parse = false;
        std::cout << "Error <assign_list>: Expected Identifier." << std::endl;
        return nullptr;
    }
    if(!symbol_table.contains(lexeme)) {
        successful_parse = false;
        std::cout << "ERROR <assign_list>: " << lexeme << " was not declared." << std::endl;
        return nullptr;
    }   
    val.token = symbol_table.search(lexeme)->entry.name;
    ident_snodeptr[count] = new SNode(Label::IDENT, val, NULL, NULL);
    lex();
    while(nextToken == ASSIGN_OP && futureToken == IDENT && futureFutureToken == ASSIGN_OP) {
        lex();
        if(!symbol_table.contains(lexeme)) {
            successful_parse = false;
            std::cout << "ERROR <assign_list>: " << lexeme << " was not declared." << std::endl;
            return nullptr;
        }
        val.token = symbol_table.search(lexeme)->entry.name;
        if(count < 10)
            ident_snodeptr[++count] = new SNode(Label::IDENT, val, nullptr, nullptr);
        else {
            successful_parse = false;
            std::cout << "ERROR <assign_list>: Too many identifiers in <assign_list>." << std::endl;
            return nullptr;
        }
        lex();
    }
    if(nextToken != ASSIGN_OP) {
        successful_parse = false;
        std::cout << "ERROR <assign_list>: Expected '=' after identifier." << std::endl;
        return nullptr;
    }
    val.op = ASSIGN_OP;
    lex();
    SNode *expr_snodeptr = expr();
    if(!successful_parse)
        return nullptr;
    SNode *assign_list = new SNode(Label::OP, val, ident_snodeptr[count--], expr_snodeptr);
    while(count >= 0)
        assign_list = new SNode(Label::OP, val, ident_snodeptr[count--], assign_list);
    return assign_list;
} /* End of function assign_list() */
/*****************************************************/
/* assign - Parses strings in the language generated by the rule: */
/* <assign> -> <ident> = <expr>                                   */
SNode* assign() {
    SNode *assign_snodeptr = nullptr;
    SNodeVal val;
    if(nextToken == IDENT) {
        val.token = symbol_table.search(lexeme)->entry.name;
        // Error-check for non-existing identifiers, ie, nullptr returns from symbol_table.search()
        SNode *ident_snodeptr = new SNode(Label::IDENT, val, nullptr, nullptr);
        lex();
        if(nextToken == ASSIGN_OP) {
            val.op = ASSIGN_OP;
            lex();
            assign_snodeptr = new SNode(Label::OP, val, ident_snodeptr, expr());
        } else return nullptr; // Throw an error b/c missing "="
    } else return nullptr; // Throw an error b/c not an IDENT
    return assign_snodeptr;
} /* End of function assign() */
/* expr - Parses strings in the language generated by the rule:   */
/* <expr> -> <term> {(+ | -) <term>}                              */
SNode* expr() {
    SNode *expr_snodeptr = term();
    // Check for successful parse of term()
    while(nextToken == ADD_OP || nextToken == SUB_OP) {
        SNodeVal add_or_sub_op;
        add_or_sub_op.op = (nextToken == ADD_OP) ? ADD_OP : SUB_OP;
        lex();
        expr_snodeptr = new SNode(Label::OP, add_or_sub_op, expr_snodeptr, term());
        // Check for successful parse of term()
    }
    return expr_snodeptr;
} /* End of function expr() */
/*****************************************************/
/* term - Parses strings in the language generated by the rule: */
/* <term> -> <factor> {(* | /) <factor>}                        */
SNode* term() {
    SNode *term_snodeptr = factor();
    // Check for successful parse of factor()
    while(nextToken == MULT_OP || nextToken == DIV_OP) {
        SNodeVal mult_or_div_op;
        mult_or_div_op.op = (nextToken == MULT_OP) ? MULT_OP : DIV_OP;
        lex();
        term_snodeptr = new SNode(Label::OP, mult_or_div_op, term_snodeptr, factor());
        // Check for successful parse of factor()
    }
    return term_snodeptr;
} /* End of function term() */
/*****************************************************/
/* factor - Parses strings in the language generated by the rule: */
/* <factor> -> ident | int_constant | float_const | (<expr>)      */
SNode* factor() {
    SNode *F_snode_ptr = nullptr;
    SNodeVal val;
    /* Determine which RHS */
    if (nextToken == IDENT) {
        val.token = symbol_table.search(lexeme)->entry.name;
        // Error-check for non-existing identifiers, ie, nullptr returns from symbol_table.search()
        F_snode_ptr = new SNode(Label::IDENT, val, nullptr, nullptr);
        /* Get the next token */
        lex();
    } else if(nextToken == INT_CONST) {
        val.integer = lexemeToInt();
        F_snode_ptr = new SNode(Label::INT_CONST, val, nullptr, nullptr);
        /* Get the next token */
        lex();
     } else if(nextToken == FLOAT_CONST) {
        val.floating = lexemeToFloat();
        F_snode_ptr = new SNode(Label::FLOAT_CONST, val, nullptr, nullptr);
        /* Get the next token */
        lex();
     }
    /* If the RHS is (<expr>), call lex to pass over the right parenthesis, */
    /* call expr, and check for the right parathesis.                       */
    else {
        if (nextToken == LEFT_PAREN) {
            lex();
            F_snode_ptr = expr();
            if(!successful_parse)
                return nullptr;
            if (nextToken == RIGHT_PAREN)
                lex();
            else 
                optf << "Error: Expected Right Parathesis.\n";
        } /* End of if (nextToken == ... */
        /* It was not an id, an integer literal, or a left paraenthesis */
        else
            optf << "Error: Expected ident, int_constant, float_constant, or (<expr>).\n";
    } /* End of else */
    return F_snode_ptr;
} /* End of function factor */
/*****************************************************/
// lexemeToInt - Convert the current lexeme to an int
int lexemeToInt() {
    int tmp = 0;
    for(int i = 0; lexeme[i] != '\0'; i++) {
        tmp *= 10;
        tmp += lexeme[i] - '0';
    }
    return tmp;
} /* End of lexemeToInt */
/*****************************************************/
// lexemeToFloat - Convert the current lexeme to a float
float lexemeToFloat() {
    float before_decimal = 0.0f, after_decimal = 0.f;
    int i = 0;
    while(lexeme[i] != '\0' && lexeme[i] != '.' && lexeme[i] != 'e' && lexeme[i] != 'E') {
        before_decimal *= 10;
        before_decimal += lexeme[i] - '0';
        i++;
    }
    if(lexeme[i] == 'e' || lexeme[i] == 'E') {
        i++;
        if(lexeme[i] == '+' || lexeme[i] != '-') {
            i += (lexeme[i] == '+') ? 1 : 0;
            int exponent{0};
            while(lexeme[i] != '\0' && i < lexLen) {
                exponent *= 10;
                exponent += lexeme[i] - '0';
                i++;
            }
            for(int j = 0; j < exponent; j++)
                before_decimal *= 10;
            return before_decimal;
        } else if(lexeme[i] == '-') {
            int exponent{0};
            i++;
            while(lexeme[i] != '\0' && i < lexLen) {
                exponent *= 10;
                exponent += lexeme[i] - '0';
                i++;
            }
            for(int j = 0; j < exponent; j++)
                before_decimal /= 10;
            return before_decimal;
        }
    } else if(lexeme[i] == '.') {
        int dividend {1};
        i++;
        while(lexeme[i] != 'e' && lexeme[i] != 'E' && lexeme[i] != '\0') {
            if(lexeme[i] != '0') {
                int digit = lexeme[i] - '0';
                float temp = digit;
                for(int j = 0; j < dividend; j++)
                    temp /= 10;
                after_decimal += temp;
            }
            i++;
            dividend++;
        }
        after_decimal += before_decimal;
        if(lexeme[i] == 'e' || lexeme[i] == 'E') {
            i++;
            if(lexeme[i] == '+' || lexeme[i] != '-') {
                i += (lexeme[i] == '+') ? 1 : 0;
                int exponent {0};
                while(lexeme[i] != '\0' && i < lexLen) {
                    exponent *= 10;
                    exponent += lexeme[i] - '0';
                    i++;
                }
                for(int j = 0; j < exponent; j++)
                    after_decimal *= 10;
            } else if(lexeme[i] == '-') {
                i++;
                int exponent{0};
                while(lexeme[i] != '\0' && i < lexLen) {
                    exponent *= 10;
                    exponent += lexeme[i] - '0';
                    i++;
                }
                for(int j = 0; j < exponent; j++)
                    after_decimal /= 10;
            }
        }
        return after_decimal;
    } else
        return before_decimal;
    return 0.0f;
} /* End of lexemeToFloat */
/*****************************************************/
/* Checks for successful initialization of our SymbolTableList 
   with the specified keywords.                                */
bool initializeSymbolTable() {
    symbol_table.append(SymbolTableEntry("int", false, IntrinsicType::KEYWORD));
    symbol_table.append(SymbolTableEntry("float", false, IntrinsicType::KEYWORD));
    return (symbol_table.size == 2);
}
/*****************************************************/
void setActualType(SNode *node) {
    /* 
        Check if the node is a leaf node.
        If a leaf, then their actual type is intrinsically given.
        So set this leaf node's actual type to their intrinsic type and return.
    */
    if(node->left == nullptr && node->right == nullptr) {
        if(node->label == Label::INT_CONST)
            node->actual_type = INT;
        else if(node->label == Label::FLOAT_CONST)
            node->actual_type = FLOAT;
        else
            node->actual_type = (symbol_table.search(node->content.token)->entry.type == IntrinsicType::INT) ? INT : FLOAT;
        return;
    }
    /* 
        Otherwise, the node must not be a leaf.
        Check if it's an operator node.
    */
    if(node->left != nullptr)
        setActualType(node->left);
    Type left_node_type = node->left->actual_type;
    if(node->right != nullptr)
        setActualType(node->right);
    Type right_node_type = node->right->actual_type;
    if(left_node_type == INT && right_node_type == INT)
        node->actual_type = INT;
    else
        node->actual_type = FLOAT;
} /* End of setActualType */
/*****************************************************/
void setExpectedType(SNode *node) {
    if (node == nullptr) return;
    // Leaf node
    if (node->left == nullptr && node->right == nullptr)
        return;
    if (node->label == Label::OP && node->content.op == ASSIGN_OP) {
        // IMPORTANT: the assign node itself should expect the LHS (storage) type.
        // This makes the assign node's computed_type reflect the LHS storage
        // (so we will emit a cast if RHS actual != LHS storage).
        node->expected_type = node->left->actual_type;
        // LHS keeps its actual type (storage type) and gets propagated down
        node->left->expected_type = node->left->actual_type;
        setExpectedType(node->left);
        // RIGHT: do NOT force the entire RHS to LHS type; allow RHS subtree to compute naturally.
        if (node->right) {
            node->right->expected_type = node->right->actual_type;
            setExpectedType(node->right);
        }
        return;
    }
    // Arithmetic operator (+, -, *, /)
    if (node->label == Label::OP) {
        // propagate parent's expected_type to children
        node->left->expected_type = node->expected_type;
        node->right->expected_type = node->expected_type;

        setExpectedType(node->left);
        setExpectedType(node->right);
        return;
    }
    // default propagation (shouldn't usually reach here for fully internal nodes)
    if (node->left != nullptr) {
        node->left->expected_type = node->expected_type;
        setExpectedType(node->left);
    }
    if (node->right != nullptr) {
        node->right->expected_type = node->expected_type;
        setExpectedType(node->right);
    }
} /* End of setExpectedType */
/*****************************************************/
void setComputedType(SNode *node, bool is_lhs_of_assign) {
    /* 1. Compute this node's own computed type (preorder) */
    if (node->actual_type == node->expected_type)
        node->computed_type = node->actual_type;
    else
        node->computed_type = node->expected_type;
    /* 2. If this node is an IDENT and is on assignment LHS → mark address */
    if (node->label == Label::IDENT && is_lhs_of_assign)
        node->addr_indicator = true;
    /* 3. Leaf → stop */
    if (node->left == nullptr && node->right == nullptr)
        return;
    /* 4. Determine how to propagate context to children */
    if (node->label == Label::OP && node->content.op == ASSIGN_OP) {
        /* LEFT child of ASSIGN must push address */
        if (node->left)
            setComputedType(node->left, true);

        /* RIGHT child must NOT push address */
        if (node->right)
            setComputedType(node->right, false);

        return;
    }
    /* 5. Non-assignment operator → both children inherit same context (false) */
    if (node->left != nullptr)
        setComputedType(node->left, false);
    if (node->right != nullptr)
        setComputedType(node->right, false);
} /* End of setComputedType */
/*****************************************************/
void semanticAnalyzer(SNode *parse_tree) {
    setActualType(parse_tree);
    parse_tree->expected_type = parse_tree->left->actual_type;
    setExpectedType(parse_tree);
    setComputedType(parse_tree, false);
    printIntermediateCode(parse_tree);
} /* End of semanticAnalyzer */
/*****************************************************/
void printILCode(SNode* node) {
    if (node == nullptr) return;
    // Postorder traversal: left → right → self
    printILCode(node->left);
    printILCode(node->right);
    // ========== IDENTIFIER ==========
    if (node->label == Label::IDENT) {
        SymbolTableList::SymbolTableNode* entry = symbol_table.search(node->content.token);
        if (node->addr_indicator) {
            optf << "push addr(" << node->content.token << ")("
                      << (entry ? entry->entry.lvalue_ptr : 0) << ")\n";
        }
        else {
            if (entry != nullptr) {
                if (entry->entry.type == IntrinsicType::INT) {
                    int v = *(int*)entry->entry.lvalue_ptr;
                    optf << "push " << node->content.token
                              << "(" << v << ")\n";
                }
                else {
                    float v = *(float*)entry->entry.lvalue_ptr;
                    optf << "push " << node->content.token
                              << "(" << v << ")\n";
                }
            }
            else {
                optf << "push " << node->content.token << "(?)\n";
            }
        }
        return;
    }
    // ========== INT CONST ==========
    if (node->label == Label::INT_CONST) {
        optf << "push " << node->content.integer << "\n";
        return;
    }
    // ========== FLOAT CONST ==========
    if (node->label == Label::FLOAT_CONST) {
        optf << "push " << node->content.floating << "\n";
        return;
    }
    // ========== OPERATORS ==========
    if (node->label == Label::OP) {
        // ---- ASSIGNMENT ----
        if (node->content.op == ASSIGN_OP) {
            // Determine cast needed before assignment
            Type lhs = node->left->actual_type;
            Type rhs = node->right->computed_type;
            if (lhs == INT && rhs == FLOAT)
                optf << "ftoi\n";
            else if (lhs == FLOAT && rhs == INT)
                optf << "itof\n";

            optf << "assign\n";
            return;
        }
        // ---- ARITHMETIC ----
        switch (node->content.op) {
            case ADD_OP:
                if (node->computed_type == FLOAT)
                    optf << "fadd\n";
                else
                    optf << "iadd\n";
                break;
            case SUB_OP:
                if (node->computed_type == FLOAT)
                    optf << "fsub\n";
                else
                    optf << "isub\n";
                break;
            case MULT_OP:
                if (node->computed_type == FLOAT)
                    optf << "fmult\n";
                else
                    optf << "imult\n";
                break;
            case DIV_OP:
                if (node->computed_type == FLOAT)
                    optf << "fdiv\n";
                else
                    optf << "idiv\n";
                break;
        }
    }
} /* End of printILCode */
/*****************************************************/
void printIntermediateCode(SNode* root) {
    if (root == NULL) return;
    // First print IL
    printILCode(root);
    // Then run the stack machine to actually evaluate the statement
    StackValue result = runStackMachine(root);
    optf << std::endl; // separate statements
} /* End of printIntermediateCode() */
/*****************************************************/
/* copyLexeme() - Copies c-string @param2 into @param1 */
/* copy -> original                                    */
void copyLexeme(char* dest, const char* src) {
    int i = 0;
    while(i < 99 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
} /* End of copyLexeme */
/*****************************************************/
void traversePrint(SNode* root) {
    if(root == nullptr)
        return;
    if(root->left != nullptr)
        traversePrint(root->left);
    root->printNode();
    std::cout << std::endl;
    if(root->right != nullptr)
        traversePrint(root->right);
} /* End of traversePrint */
/*****************************************************/
/* lexScan() */
void lexScan() {
    if(scannedToken == EOF)
        return;
    lexLen = 0;
    getNonBlank();
    if (nextChar == '\n') {
        scannedToken = EOL;
        scannedLexeme[0] = '\'';
        scannedLexeme[1] = '\\';
        scannedLexeme[2] = 'n';
        scannedLexeme[3] = '\'';
        scannedLexeme[4] = '\0';
        getChar();   // consume newline
        return;
    }
    switch (charClass) {
    /* Parse identifiers */
        case LETTER:
        addChar();
        getChar();
        while (charClass == LETTER || charClass == DIGIT) {
            addChar();
            getChar();
        }
        /* Check Symbol Table to see if this identifier is already a word used in the symbol table.
           Ex. Previously declared variable or a keyword.
        */
        if(symbol_table.contains(scannedLexeme)) {
            if(symbol_table.search(scannedLexeme) == symbol_table.search("int"))
                scannedToken = KEYWORD_INT;
            else if(symbol_table.search(scannedLexeme) == symbol_table.search("float"))
                scannedToken = KEYWORD_FLOAT;
            else
                scannedToken = IDENT;
        } else
            scannedToken = IDENT;
        break;
    /* Parse Integer constants and Float constants */
    case DIGIT: {
        addChar();
        getChar();
        while(charClass == DIGIT) {
            addChar();
            getChar();
        }
        bool is_float = false;
        if(nextChar == '.') {
            is_float = true;
            addChar();
            getChar();
            while(charClass == DIGIT) {
                addChar();
                getChar();
            }
        }
        if(nextChar == 'e' || nextChar == 'E') {
            is_float = true;
            addChar();
            getChar();
            if(nextChar == '+' || nextChar == '-') {
                addChar();
                getChar();
            }
            while(charClass == DIGIT) {
                addChar();
                getChar();
            }
        }
        scannedToken = is_float ? FLOAT_CONST : INT_CONST;
        break;
    }
    /* Parentheses and operators */
    case UNKNOWN:
        lookup(nextChar);
        getChar();
        break;
    /* EOF */
    case EOF:
        scannedToken = EOF;
        scannedLexeme[0] = 'E';
        scannedLexeme[1] = 'O';
        scannedLexeme[2] = 'F';
        scannedLexeme[3] = '\0';
        break;
    } /* End of switch */
} /* End of function lexScan */
/***************************************************************/
/* lex - a simple lexical analyzer for arithmetic expressions */
void lex() {
    nextToken = futureToken;
    copyLexeme(lexeme, futureLexeme);
    futureToken = futureFutureToken;
    copyLexeme(futureLexeme, futureFutureLexeme);
    lexScan();
    futureFutureToken = scannedToken;
    copyLexeme(futureFutureLexeme, scannedLexeme);
} /* End of function lex */
/*****************************************************/
void primeLexAnalyzer() {
    getChar();
    lexScan();
    nextToken = scannedToken;
    copyLexeme(lexeme, scannedLexeme);
    lexScan();
    futureToken = scannedToken;
    copyLexeme(futureLexeme, scannedLexeme);
    lexScan();
    futureFutureToken = scannedToken;
    copyLexeme(futureFutureLexeme, scannedLexeme);
} /* End of primeLexAnalyzer */
/*****************************************************/
// ============================================================================
// Runtime stack machine: executes AST exactly like IL code does.
// ============================================================================
StackValue runStackMachine(SNode *root)
{
    const int STACK_MAX = 64;
    StackValue stack[STACK_MAX];
    int sp = 0;

    // --------------------
    // push helper
    // --------------------
    void (*push)(StackValue*, int&, const StackValue&) =
        [](StackValue *s, int &p, const StackValue &v)
        {
            if (p < 64) s[p++] = v;
            else std::cerr << "Runtime stack overflow\n";
        };

    // --------------------
    // pop helper
    // --------------------
    StackValue (*pop)(StackValue*, int&) =
        [](StackValue *s, int &p) -> StackValue
        {
            if (p > 0) return s[--p];
            std::cerr << "Runtime stack underflow\n";
            return StackValue::makeInt(0);
        };

    // =====================================================================
    // Recursive executor (postorder)
    // =====================================================================
    struct Exec {
        static void execNode(
            SNode *node,
            StackValue stack[],
            int &sp,
            void (*pushFn)(StackValue*, int&, const StackValue&),
            StackValue (*popFn)(StackValue*, int&)
        )
        {
            if (!node) return;

            // Postorder traversal
            if (node->left)  execNode(node->left,  stack, sp, pushFn, popFn);
            if (node->right) execNode(node->right, stack, sp, pushFn, popFn);

            // ---------------------------------------------------------
            // IDENT leaf
            // ---------------------------------------------------------
            if (node->label == Label::IDENT)
            {
                SymbolTableList::SymbolTableNode *entry =
                    symbol_table.search(node->content.token);

                if (node->addr_indicator)
                {
                    void *addr = entry ? entry->entry.lvalue_ptr : NULL;
                    pushFn(stack, sp, StackValue::makeAddr(addr));
                }
                else if (entry)
                {
                    // read stored value
                    if (entry->entry.type == IntrinsicType::INT)
                    {
                        int v = *static_cast<int *>(entry->entry.lvalue_ptr);
                        pushFn(stack, sp, StackValue::makeInt(v));
                    }
                    else
                    {
                        float v = *static_cast<float *>(entry->entry.lvalue_ptr);
                        pushFn(stack, sp, StackValue::makeFloat(v));
                    }
                }
                else
                {
                    // unknown identifier — push default 0
                    pushFn(stack, sp, StackValue::makeInt(0));
                }
                return;
            }

            // ---------------------------------------------------------
            // INT_CONST leaf
            // ---------------------------------------------------------
            if (node->label == Label::INT_CONST)
            {
                pushFn(stack, sp, StackValue::makeInt(node->content.integer));
                return;
            }

            // ---------------------------------------------------------
            // FLOAT_CONST leaf
            // ---------------------------------------------------------
            if (node->label == Label::FLOAT_CONST)
            {
                pushFn(stack, sp, StackValue::makeFloat(node->content.floating));
                return;
            }

            // ---------------------------------------------------------
            // OP nodes
            // ---------------------------------------------------------
            if (node->label == Label::OP)
            {
                int op = node->content.op;

                // =======================
                // ASSIGN OPERATION
                // =======================
                if (op == ASSIGN_OP)
                {
                    // Order on stack:  [..., addr(LHS), value(RHS)]
                    StackValue rhs  = popFn(stack, sp);
                    StackValue addr = popFn(stack, sp);

                    if (!addr.isAddr)
                        std::cerr << "Runtime assign error: expected address\n";

                    // Look up symbol table entry
                    SymbolTableList::SymbolTableNode *entry =
                        searchByAddress(addr.addr);

                    if (!entry)
                        entry = symbol_table.search(node->left->content.token);

                    if (!entry)
                    {
                        // fallback
                        pushFn(stack, sp, rhs);
                        return;
                    }

                    // Apply cast based on actual target storage type
                    if (entry->entry.type == IntrinsicType::INT)
                    {
                        int v = (rhs.type == IntrinsicType::FLOAT)
                                ? (int)rhs.f
                                : rhs.i;

                        *static_cast<int *>(entry->entry.lvalue_ptr) = v;
                        pushFn(stack, sp, StackValue::makeInt(v));
                    }
                    else // FLOAT
                    {
                        float v = (rhs.type == IntrinsicType::INT)
                                 ? (float)rhs.i
                                 : rhs.f;

                        *static_cast<float *>(entry->entry.lvalue_ptr) = v;
                        pushFn(stack, sp, StackValue::makeFloat(v));
                    }
                    return;
                }

                // =======================
                // ARITHMETIC OPERATIONS
                // =======================
                StackValue r = popFn(stack, sp);
                StackValue l = popFn(stack, sp);

                bool makeFloat = (node->computed_type == FLOAT);

                float lf = (l.type == IntrinsicType::FLOAT) ? l.f : (float)l.i;
                float rf = (r.type == IntrinsicType::FLOAT) ? r.f : (float)r.i;

                if (makeFloat)
                {
                    float out = 0.0f;
                    if (op == ADD_OP)      out = lf + rf;
                    else if (op == SUB_OP) out = lf - rf;
                    else if (op == MULT_OP) out = lf * rf;
                    else if (op == DIV_OP)  out = lf / rf;

                    pushFn(stack, sp, StackValue::makeFloat(out));
                }
                else
                {
                    int li = (l.type == IntrinsicType::INT) ? l.i : (int)l.f;
                    int ri = (r.type == IntrinsicType::INT) ? r.i : (int)r.f;

                    int out = 0;
                    if (op == ADD_OP)      out = li + ri;
                    else if (op == SUB_OP) out = li - ri;
                    else if (op == MULT_OP) out = li * ri;
                    else if (op == DIV_OP)  out = li / ri;

                    pushFn(stack, sp, StackValue::makeInt(out));
                }
            }
        }
    }; // struct Exec

    // Run execution
    Exec::execNode(root, stack, sp, push, pop);

    // return top of stack
    if (sp > 0) return stack[sp - 1];
    return StackValue::makeInt(0);
} /* End of runStackMachine */
/*****************************************************/