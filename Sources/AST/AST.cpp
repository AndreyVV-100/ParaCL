#include "Node.hpp"
#include <fstream>

namespace AST
{
    void Tree::PrintTree (const std::string& filename) const
    {
        if (!top_) { return; }

        std::fstream output;
        output.open (filename, std::ios::out);

        if (!output.is_open())
        {
            std::cerr << "Error: unable to open file: " << filename << std::endl;
            return;
        }

        PrintTree (output);
        output.close();
        return;
    }

    void Tree::PrintTree (std::ostream& output) const
    {
        output << "digraph G {\n"
                  "graph [\n"
                  "rankdir = \"HR\"\n"
                  "];\n"
                  "node [\n"
                  "fontsize = \"16\"\n"
                  "shape = \"ellipse\"\n"
                  "];\n";
                //   "edge ["
                //   "];;"

        top_->GraphPrintHeaders     (output);
        top_->GraphPrintConnections (output);
        output << "}\n";
    }

    AbstractNode :: ~AbstractNode()
    {
        if (prev_)
        {
            if (prev_->left_ == this)
                prev_->left_ = nullptr;
            if (prev_->right_ == this)
                prev_->right_ = nullptr;

            prev_ = nullptr;
        }

        AbstractNode* node_deleter = this;

        while (node_deleter)
        {
            // ToDo: copypaste?

            if (node_deleter->left_)
            {
                AbstractNode* left  = node_deleter->left_;
                node_deleter->left_ = nullptr;
                node_deleter = left;
                continue;
            }

            if (node_deleter->right_)
            {
                AbstractNode* right  = node_deleter->right_;
                node_deleter->right_ = nullptr;
                node_deleter = right;
                continue;
            }

            // ToDo: check left_->prev_ == now
            AbstractNode* prev  = node_deleter->prev_;

            if (prev)
                delete node_deleter; // ToDo: It's recursive with deep = 1, OK?

            node_deleter = prev;
        }
    }

    void AbstractNode::GraphPrintConnections (std::ostream& output) const
    {
        // ToDo: copypaste?
        if (left_)
        {
            output << '"' << this << "\" -> \"" << left_ << "\"\n";
            left_->GraphPrintConnections (output);
        }

        if (right_)
        {
            output << '"' << this << "\" -> \"" << right_ << "\"\n";
            right_->GraphPrintConnections (output);
        }
    }

    void AbstractNode::PrintTypeName (std::ostream& output) const // ToDo: It's OK?
    {
        switch (type_)
        {
            case NodeType :: VARIABLE:
                output << "Variable";
                break;

            case NodeType :: CONST:
                output << "Const";
                break;

            case NodeType :: OPERATION:
                output << "Operation";
                break;

            case NodeType :: CONDITION:
                output << "Condition";
                break;

            case NodeType :: FUNCTION_CALL:
                output << "Function call";
                break;

            case NodeType :: FUNCTION_DECL:
                output << "Function declaration";
                break;

            case NodeType :: ORDER_OP:
                output << "Order";
                break;

            case NodeType :: SCOPE:
                output << "Scope";
                break;
        }
    }

    void AbstractNode::GraphPrintHeaders (std::ostream& output) const
    {
        output << '"' << this << "\" [\n" << "label = \"<f0> ";
        PrintTypeName(output);
        output << " | ";
        GraphPrintInfo(output); 
        output << "\"\n"
               << "shape = \"record\"\n"
               << "];\n";

        if (left_)
            left_->GraphPrintHeaders(output);
        if (right_)
            right_->GraphPrintHeaders(output);
        
        return;
    }
    
    AbstractNode* MakeVal   (int val,            int lineno)                                         { return new Node :: ConstNode{val, lineno}; }
    AbstractNode* MakeVar   (std::string name,   int lineno)                                         { return new Node :: VariableNode{name, lineno}; }
    AbstractNode* MakeFunc  (std::string name,   int lineno, AbstractNode* lhs)                      { return new Node :: FunctionCallNode{name, lineno, lhs, nullptr}; }
    AbstractNode* MakeScope (                    int lineno, AbstractNode* lhs)                      { return new Node :: ScopeNode{lineno, lhs}; }
    AbstractNode* MakeORD   (                    int lineno, AbstractNode* lhs, AbstractNode* rhs)   { return new Node :: OrderNode{lineno, lhs, rhs}; }
    AbstractNode* MakeOp    (OpType op,          int lineno, AbstractNode* lhs, AbstractNode* rhs)   { return new Node :: OperationNode{op, lineno, lhs, rhs}; }
    AbstractNode* MakeCond  (CondType cond_type, int lineno, AbstractNode* lhs, AbstractNode* rhs)   { return new Node :: ConditionNode{cond_type, lineno, lhs, rhs}; }
 
} // End of namespace AST
