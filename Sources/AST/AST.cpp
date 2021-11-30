#include "Node.hpp"
#include <fstream>

namespace AST
{
    Tree :: ~Tree()
    {
        AbstractNode* node_deleter = top_;

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
            delete node_deleter;
            node_deleter = prev;
        }
    }

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
    
    AbstractNode* MakeVal (int val)           { return new Node :: ConstNode{val}; }
    AbstractNode* MakeVar (std::string name)  { return new Node :: VariableNode{name}; }
    AbstractNode* MakeFunc (std::string name) { return new Node :: FunctionCallNode{name}; }
    AbstractNode* MakeORD  (AbstractNode *lhs, AbstractNode *rhs) {return new Node :: OrderNode{lhs, rhs};}
    AbstractNode* MakeCond (AbstractNode* lhs, CondType cond_type, AbstractNode* rhs)
    { 
        AbstractNode* tmp = new Node :: ConditionNode{cond_type};

        if (lhs) lhs->prev_ = tmp;
        if (rhs) rhs->prev_ = tmp;

        tmp->left_ = lhs;
        tmp->right_ = rhs;

        return tmp;
    }
    
    AbstractNode* MakeOp (AbstractNode* lhs, OpType op, AbstractNode* rhs)
    {
        AbstractNode* tmp = new Node :: OperationNode{op};

        if (lhs)
            lhs->prev_ = tmp;

        if (rhs)
            rhs->prev_ = tmp;

        tmp->left_ = lhs;
        tmp->right_ = rhs;

        return tmp;
    }

} // End of namespace AST
