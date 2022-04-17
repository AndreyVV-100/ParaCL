#include "semantic.hpp"

namespace semantic
{
    basic_variable* Scope :: find (std::string name) const
    {
        const Scope* search_scope = this;

        while (search_scope)
        {
            auto find_result = search_scope->objects_.find (name);
            
            if (find_result == search_scope->objects_.end())
                search_scope = search_scope->prev_;
            else 
                return find_result->second;
        }

        return nullptr;
    }

    void SemanticAnalysis (AST::AbstractNode* node)
    {
        
    }
} // end of namespace semantic
