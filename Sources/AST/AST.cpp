#include "AST.hpp"
#include <fstream>

namespace ParaCL
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

    void Tree::PrintTree (const std::string& filename)
    {
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

    void Tree::PrintTree (std::ostream& output)
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
    }

    void AbstractNode::GraphPrintConnections (std::ostream& output)
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

    void AbstractNode::PrintTypeName (std::ostream& output) // ToDo: It's OK?
    {
        switch (type_)
        {
            case VARIABLE:
                output << "Variable";
                break;

            case CONST:
                output << "Const";
                break;

            case OPERATION:
                output << "Operation";
                break;

            case CONDITION:
                output << "Condition";
                break;

            case FUNCTION_CALL:
                output << "Function call";
                break;

            case FUNCTION_DECL:
                output << "Function declaration";
                break;
        }
    }

    void AbstractNode::GraphPrintHeaders (std::ostream& output) override
    {
        output << '"' << this << "\" [\n" << "label = \"<f0> ";
        PrintTypeName();
        output << " | ";
        GraphPrintInfo(); 
        output <<"\"\n"
               << "shape = \"record\"\n"
               << "];\n";

        if (left_)
            left_->GraphPrintHeaders();
        if (right_)
            right_->GraphPrintHeaders();
        
        return;
    }
    
} // End of namespace ParaCL
