#include "AST.hpp"
#include "semantic.hpp"

namespace Node
{

using namespace AST; // ToDo: It's OK?

struct VariableNode final : public AbstractNode
{
    // static Scope - now we havn't class for it // const?
    std::string name_;

    VariableNode (std::string name, int lineno, AbstractNode* prev = nullptr):
        AbstractNode (NodeType :: VARIABLE, lineno, prev),
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
        AbstractNode (NodeType :: CONST, lineno, prev),
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

    OperationNode (OpType op_type, int lineno, AbstractNode* lhs, AbstractNode* rhs, AbstractNode* prev = nullptr):
        AbstractNode (NodeType :: OPERATION, lineno, lhs, rhs, prev),
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

    ConditionNode (CondType cond_type, int lineno, AbstractNode* lhs, AbstractNode* rhs, AbstractNode* prev = nullptr):
        AbstractNode (NodeType :: CONDITION, lineno, lhs, rhs, prev),
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

    FunctionCallNode (std::string name, int lineno, AbstractNode* lhs, AbstractNode* rhs, AbstractNode* prev = nullptr):
        AbstractNode (NodeType :: FUNCTION_CALL, lineno, lhs, rhs, prev),
        name_ (name)
    { ; }

    void GraphPrintInfo (std::ostream& output) const override
    {
        output << name_;
        return;
    }
};

struct OrderNode final: public AbstractNode
{
    OrderNode (int lineno, AbstractNode* lhs, AbstractNode* rhs, AbstractNode *prev = nullptr):
        AbstractNode (NodeType::ORDER_OP, lineno, lhs, rhs, prev)
    { ; }

    void GraphPrintInfo (std::ostream& output) const override
    {
        output << ";";
        return;
    }
};

struct ScopeNode final: public AbstractNode
{
    semantic::Scope scope_;

    ScopeNode (int lineno, AbstractNode *lhs, AbstractNode *prev = nullptr):
        AbstractNode (NodeType::SCOPE, lineno, lhs, prev),
        scope_ (nullptr)
    { ; }

    void GraphPrintInfo (std::ostream& output) const override
    {
        output << "{}";
        return;
    }
};

} // end of namespace Node
