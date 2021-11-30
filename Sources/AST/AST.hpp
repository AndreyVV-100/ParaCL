#include <string>
#include <iostream>

namespace ParaCL
{

enum class OpType // ToDo: print name, move this
{
    ADD = 0,    // +
    SUB = 1,    // -
    MUL = 2,    // *
    DIV = 3,    // /
    REM = 4,    // % - remainder of division

    ASS = 5,    // = - ♂assignment♂
    ORD = 6,    // ; - order

    EQU = 7,    // ==
    EQG = 8,    // >=
    EQL = 9,    // <=
    EQN = 10,   // !=
    STG = 11,   // > - strictly greater
    STL = 12,   // <

    LAND = 13,  // && - logical and
    LOR  = 14,  // ||
    LNOT = 15,  // !

    BAND = 16,  // & - binary and
    BOR  = 17,  // |
    BNOT = 18,  // ~
    BXOR = 19,  // ^
    
    SHL  = 20,  // <<
    SHR  = 21,  // >>

    PLUS  = 22, // + - unary plus
    MINUS = 23, // - - unary minus

    PRE_INC  = 24, // ++x
    POST_INC = 25, // x++
    PRE_DEC  = 26, // --x
    POST_DEC = 27  // x--
};

enum class CondType
{
    IF = 0,
    WHILE = 1
};

struct AbstractNode;

struct Tree final
{
    // ToDo: private?
    AbstractNode* top_ = nullptr;

    Tree (const Tree&) = delete;
    Tree (Tree&&) = delete;
    Tree& operator = (const Tree&) = delete;
    Tree& operator = (Tree&&) = delete;

    Tree() = default;
    ~Tree();

    void PrintTree (const std::string& filename) const; // ToDo: output filename?
    void PrintTree (std::ostream& output = std::cout) const;
};

// ToDo: class or struct, non-private fields? How to get access in child?
struct AbstractNode
{
    enum class NodeType
    {
        VARIABLE      = 0,
        CONST         = 1,
        OPERATION     = 2,
        CONDITION     = 3,
        FUNCTION_CALL = 4,
        FUNCTION_DECL = 5  // Now not used
    };

    AbstractNode* prev_, 
                * left_  = nullptr,
                * right_ = nullptr;
    
    NodeType type_; // const?

    public:

    AbstractNode (AbstractNode* prev, NodeType type):
        prev_ (prev),
        type_ (type)
    { ; }
    virtual ~AbstractNode() = default;

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

struct VariableNode final : public AbstractNode
{
    // static Scope - now we havn't class for it // const?
    std::string name_;

    VariableNode (std::string name, AbstractNode* prev = nullptr):
        AbstractNode (prev, NodeType :: VARIABLE),
        name_ (name)
    { ; }

    void GraphPrintInfo (std::ostream& output) const override
    {
        output << name_;
        return;
    }
};

struct ConstNode final : public AbstractNode
{
    int value_; // TODO: rework for all other types

    ConstNode (int value, AbstractNode* prev = nullptr):
        AbstractNode (prev, NodeType :: CONST),
        value_ (value)
    { ; }

    void GraphPrintInfo (std::ostream& output) const override
    {
        output << value_;
        return;
    }
};

struct OperationNode final : public AbstractNode
{
    OpType op_type_;

    OperationNode (OpType op_type, AbstractNode* prev = nullptr):
        AbstractNode (prev, NodeType :: OPERATION),
        op_type_ (op_type)
    { ; }

    void GraphPrintInfo (std::ostream& output) const override
    {
        output << static_cast <int> (op_type_);
        return;
    }
};

struct ConditionNode final : public AbstractNode
{
    CondType cond_type_;

    ConditionNode (CondType cond_type, AbstractNode* prev = nullptr):
        AbstractNode (prev, NodeType :: CONDITION),
        cond_type_ (cond_type)
    { ; }

    void GraphPrintInfo (std::ostream& output) const override
    {
        output << static_cast <int> (cond_type_);
        return;
    }
};

struct FunctionCallNode final : public AbstractNode
{
    // ToDo: parameters list
    // ToDo: function list

    std::string name_;

    FunctionCallNode (std::string name, AbstractNode* prev = nullptr):
        AbstractNode (prev, NodeType :: FUNCTION_CALL),
        name_ (name)
    { ; }

    void GraphPrintInfo (std::ostream& output) const override
    {
        output << name_;
        return;
    }
};
    AbstractNode* MakeVal  (int val);
    AbstractNode* MakeVar  (std::string name);
    AbstractNode* MakeFunc (std::string name);
    AbstractNode* MakeOp   (AbstractNode* lhs, OpType op, AbstractNode* rhs);
    AbstractNode* MakeCond (AbstractNode* lhs, CondType cond_type, AbstractNode* rhs);
} // End of namespace ParaCL
