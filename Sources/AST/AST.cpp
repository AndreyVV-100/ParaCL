#include "AST.h"

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
} // End of namespace ParaCL
