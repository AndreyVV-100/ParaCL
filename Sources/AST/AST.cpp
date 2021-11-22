#include "AST.hpp"

namespace ParaCL
{
    AbstractNode* make_val(int val) { return new ConstNode{val}; }
    AbstractNode* make_op(AbstractNode* lhs, OpType op, AbstractNode* rhs)
    {
        AbstractNode* tmp = new OperationNode{op};
        tmp->left_  = lhs;
        tmp->right_ = rhs;
        return tmp;
    }


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

    void Tree::dump() const
    {
        top_->dump();
        
        if (top_->left_)
            top_->left_->dump();
        if (top_->right_)
            top_->right_->dump();
    }


} // End of namespace ParaCL
