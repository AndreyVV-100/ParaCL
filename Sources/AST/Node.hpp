#include "AST.hpp"

namespace Node
{

using namespace AST; // ToDo: It's OK?

struct VariableNode final : public AbstractNode
{
    // static Scope - now we havn't class for it // const?
    std::string name_;

    VariableNode (std::string name, int lineno, AbstractNode* prev = nullptr):
        AbstractNode (prev, NodeType :: VARIABLE, lineno),
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

    ConstNode (int value, int lineno, AbstractNode* prev = nullptr):
        AbstractNode (prev, NodeType :: CONST, lineno),
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

    OperationNode (OpType op_type, int lineno, AbstractNode* prev = nullptr):
        AbstractNode (prev, NodeType :: OPERATION, lineno),
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

    ConditionNode (CondType cond_type, int lineno, AbstractNode* prev = nullptr):
        AbstractNode (prev, NodeType :: CONDITION, lineno),
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

    FunctionCallNode (std::string name, int lineno, AbstractNode* prev = nullptr):
        AbstractNode (prev, NodeType :: FUNCTION_CALL, lineno),
        name_ (name)
    { ; }

    void GraphPrintInfo (std::ostream& output) const override
    {
        output << name_;
        return;
    }
};

struct OrderNode final : public AbstractNode
{
    OrderNode (AbstractNode *left, AbstractNode *right, int lineno, AbstractNode *prev = nullptr):
        AbstractNode (prev, NodeType::ORDER_OP, lineno) 
    {
        left_  = left;
        right_ = right;
        
        if (left_ != nullptr)
            left_->prev_ = this;

        if (right_ != nullptr)
            right_->prev_ = this;
    }

    void GraphPrintInfo (std::ostream& output) const override
    {
        output << ";\n";
        return;
    }
};

} // end of namespace Node
