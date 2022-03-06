#pragma once

#include <string>
#include <iostream>
#include <string>

namespace AST
{

enum class NodeType
{
    VARIABLE      = 0,
    CONST         = 1,
    OPERATION     = 2,
    CONDITION     = 3,
    FUNCTION_CALL = 4,
    FUNCTION_DECL = 5,  // Now not used
    ORDER_OP      = 6,
    SCOPE         = 7
};

enum OpType // ToDo: print name, move this
{
    ADD = 0,            // +
    SUB = 1,            // -
    MUL = 2,            // *
    DIV = 3,            // /
    REM = 4,            // % - remainder of division

    ASS = 5,            // = - ♂assignment♂

    EQU = 7,            // ==
    EQG = 8,            // >=
    EQL = 9,            // <=
    EQN = 10,           // !=
    STG = 11,           // > - strictly greater
    STL = 12,           // <

    LAND = 13,          // && - logical and
    LOR  = 14,          // ||
    LNOT = 1015,        // !

    BAND = 16,          // & - binary and
    BOR  = 17,          // |
    BNOT = 1018,        // ~
    BXOR = 19,          // ^

    SHL  = 20,          // <<
    SHR  = 21,          // >>

    PLUS  = 1022,       // + - unary plus
    MINUS = 1023,       // - - unary minus

    PRE_INC  = 1024,    // ++x
    POST_INC = 1025,    // x++
    PRE_DEC  = 1026,    // --x
    POST_DEC = 1027     // x--
};

enum class CondType
{
    IF = 0,
    WHILE = 1
};

// ToDo: class or struct, non-private fields? How to get access in child?
struct AbstractNode
{
    AbstractNode* prev_, 
                * left_  = nullptr,
                * right_ = nullptr;
    
    NodeType type_; // const?

    int lineno_;

    public:

    AbstractNode (NodeType type, int lineno, AbstractNode* left = nullptr, AbstractNode* right = nullptr, AbstractNode* prev = nullptr):
        prev_ (prev),
        left_ (left),
        right_(right),
        type_ (type),
        lineno_ (lineno)
    { 
        if (left_)   left_->prev_ = this;
        if (right_) right_->prev_ = this;
    }
    
    virtual ~AbstractNode();

    AbstractNode (const AbstractNode&) = delete;
    AbstractNode (AbstractNode&&) = delete;
    AbstractNode& operator = (const AbstractNode&) = delete;
    AbstractNode& operator = (AbstractNode&&) = delete;

    void PrintTypeName          (std::ostream& output) const;
    void GraphPrintConnections  (std::ostream& output) const;
    void GraphPrintHeaders      (std::ostream& output) const;
    virtual void GraphPrintInfo (std::ostream& output) const = 0;

    // ToDo: SetPrev() ?
};

class Tree final
{
    AbstractNode* top_ = nullptr;

public:
    Tree (const Tree&) = delete;
    Tree (Tree&&) = delete;
    Tree& operator = (const Tree&) = delete;
    Tree& operator = (Tree&&) = delete;

    Tree() = default;
    ~Tree()
    {
        delete top_;
    }

    void ChangeTop (AbstractNode* new_top)
    {
        delete top_;
        top_ = new_top;
    }

    const AbstractNode* GetTop() // ToDo: const annotation? How to ban delete GetTop();?
    {
        return top_;
    }

    void PrintTree (const std::string& filename) const; // ToDo: output filename?
    void PrintTree (std::ostream& output = std::cout) const;
};

    AbstractNode* MakeVal   (int val,            int lineno);
    AbstractNode* MakeVar   (std::string name,   int lineno);
    AbstractNode* MakeFunc  (std::string name,   int lineno, AbstractNode* lhs);
    AbstractNode* MakeScope (                    int lineno, AbstractNode* lhs);
    AbstractNode* MakeORD   (                    int lineno, AbstractNode* lhs, AbstractNode* rhs);
    AbstractNode* MakeOp    (OpType op,          int lineno, AbstractNode* lhs, AbstractNode* rhs);
    AbstractNode* MakeCond  (CondType cond_type, int lineno, AbstractNode* lhs, AbstractNode* rhs);

} // End of namespace AST
