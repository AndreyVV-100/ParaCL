#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include "Node.hpp"

namespace interpretator
{
enum class TYPES
{
    TYPE_INT = 1001
};

enum class ERRORS
{
    MISSED_SEMICOLON    = 2001,
    UNKNOWN_FUNC        = 2002,
    UNKNOWN_VARIABLE    = 2003,
    INCORRECT_TREE      = 2004,
    NOT_A_VARIABLE      = 2005,
    UNKNOWN_OPERATION   = 2006,
    EMPTY_CONDITION     = 2007,
    UNKNOWN_COND_OP     = 2008,
    UNKNOWN_NODETYPE    = 2010
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

class scope
{
    scope* upper_scope = nullptr;

    public:

    std::vector <scope*> scope_vec; // ToDo: This is a list, it can be easier. Now it's waste of memory

    std::unordered_map <std::string, basic_variable*> objects;

    scope () = default;

    scope (const scope &rhs) = delete;
    scope (scope &&rhs) = delete;

    scope & operator = (const scope &rhs) = delete;
    scope & operator = (scope && rhs) = delete;

    ~scope ()
    {
        for (auto i = objects.begin(); i != objects.end(); ++i)
            delete i->second;

        for (auto i = scope_vec.begin(); i != scope_vec.end(); ++i)
            delete *i;
    }

    scope * create_nested_scope ();

    basic_variable* find (std::string name) const;
};

struct interpreter
{
    scope* global;

    std::string error_message{};
    int error_line_number{};

    interpreter(): global{new scope} {}

    ~interpreter() { delete global; }

    std::string get_error_message (ERRORS error_code, AST::AbstractNode *node);

    void process_error (ERRORS error_code, AST::AbstractNode *node);

    void interpretate (scope *scope_, AST::AbstractNode *node);

    int process_node (scope *scope_, AST::AbstractNode *node);

    int process_operation_node (scope *scope_, AST::AbstractNode *node_);

    int process_assignment (scope *scope_, AST::AbstractNode *node_);

    int process_pre_increment (scope *scope_, AST::AbstractNode *node_, int extra_val);

    int process_post_increment (scope *scope_, AST::AbstractNode *node_, int extra_val);

    int process_condition_node (scope *scope_, AST::AbstractNode *node);

    int process_funccall_node (scope  *scope_, AST::AbstractNode *node);

    void process_scope_node (scope  *scope_, AST::AbstractNode *node);
};

void start_interpretate (const AST::Tree &tree);

}
