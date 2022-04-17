#include <unordered_map>
#include <string>
#include "AST.hpp"

namespace semantic
{
    enum class TYPES
    {
        TYPE_INT = 1001
    };

    struct basic_variable
    {
        TYPES var_type = TYPES::TYPE_INT;
        
        virtual ~basic_variable() {}
    };

    template <typename T>
    struct variable final : public basic_variable
    {
        T value = {};

        ~variable () = default;
    };

    class Scope final
    {
        /* 
            // for iterative interpreter or interpreter with functions:
            Scope* logical_prev_ = nullptr;  // for find()
            Scope* physical_prev_ = nullptr; // for return to previous function
            static Scope* global_scope_; // for call functions

            // always logical_prev_ == physical_prev_ (if, while, ect.) || logical_prev_ == global_scope_ (call functions)
            // But it's working only if all functions located in global_scope_. With namespaces and ect. - death.

            // Now it's cumbersome and useless functional, but in the next stages - OK. 
            // Without functions always logical_prev_ == physical_prev_, now it's named prev_.

            public:

            void initialize_global_scope_by_me ()
            {
                // ToDo: if (global_scope) throw error;
                // But at start global_scope == nullptr ?
                global_scope_ = this;
            }

            // Now we don't use this functional because we don't know all problems, related with scopes.
        */

        Scope* prev_ = nullptr;
        std::unordered_map <std::string, basic_variable*> objects_;

        public:

        Scope (Scope* prev):
            prev_ (prev)
        { ; }

        Scope (const Scope &rhs, Scope* prev):
            prev_ (prev)
        {
            if (&rhs != this)
                objects_ = rhs.objects_;
        }

        void SetPrevScope (Scope* prev)
        {
            if (!prev_)
                prev_ = prev;
            // ToDo: else throw exception?
        }

        Scope (const Scope &rhs) = delete;
        Scope (Scope &&rhs) = delete;

        Scope & operator = (const Scope &rhs) = delete;
        Scope & operator = (Scope && rhs) = delete;

        basic_variable* find (std::string name) const;
    };

} // end of namespace semantic
