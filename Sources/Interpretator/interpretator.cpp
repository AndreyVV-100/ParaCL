#include <interpretator.hpp>

namespace interpretator
{
//scope

scope * scope::create_nested_scope ()
{
    scope* new_scope = new scope;

    scope_vec.push_back (new_scope);

    return new_scope;
};

basic_variable* scope::find (std::string name) const
{
    basic_variable* res = nullptr;

    auto find_res = objects.find (name);

    if (find_res != objects.end())
        res = find_res->second;

    else if (upper_scope != nullptr)
        res = upper_scope->find (name);

    return res;
}

//other funcs

void start_interpretate (AST::Tree &tree)
{
    scope *global_scope = new scope;
    scope *cur_scope    = global_scope;

    AST::AbstractNode *cur_node = tree.top_;
    AST::AbstractNode *cur_exec_node = nullptr;

    while (cur_node != nullptr)
    {
        if (cur_node->type_ != AST::AbstractNode::NodeType::ORDER_OP)
            throw ERRORS::MISSED_SEMICOLON;

        cur_exec_node = cur_node->left_;

        if (cur_exec_node != nullptr)
        {
            process_node (cur_scope, cur_exec_node, global_scope);
        }

        cur_node = cur_node->right_;
    }

    delete global_scope;
}

int process_node (scope *scope_, AST::AbstractNode *node, scope *global_scope)
{
    if (node == nullptr)
        return 0;

    switch (node->type_)
    {
        case AST::AbstractNode::NodeType::VARIABLE:
        {
            AST::VariableNode *var_node = static_cast <AST::VariableNode*> (node);

            basic_variable* find_res = scope_->find (var_node->name_);
            
            if (find_res == nullptr)
                throw ERRORS::UNKNOWN_VARIABLE;

            variable <int> *var = static_cast <variable <int> *> (find_res);

            return var->value;
        }

        case AST::AbstractNode::NodeType::CONST:
        {
            AST::ConstNode *const_node = static_cast <AST::ConstNode*> (node);

            return const_node->value_;
        }

        case AST::AbstractNode::NodeType::OPERATION:
            return process_operation_node (scope_, node, global_scope);
        
        case AST::AbstractNode::NodeType::CONDITION:
            return process_condition_node (scope_, node, global_scope);

        case AST::AbstractNode::NodeType::FUNCTION_CALL:
            return process_funccall_node (scope_, node, global_scope);

        default:
            throw ERRORS::UNKNOWN_NODETYPE;
    }

    return 0;
}

int process_operation_node (scope *scope_, AST::AbstractNode *node_, scope *global_scope)
{
    AST::OperationNode *node = static_cast <AST::OperationNode*> (node_);

    int left_val = 0, right_val = 0;

    if ((node->op_type_ / 1000) == 1)
    {
        if (node->left_ == nullptr)
            throw ERRORS::INCORRECT_TREE;
    }

    else 
    {
        if (node->left_ == nullptr or node->right_ == nullptr)
            throw ERRORS::INCORRECT_TREE;
    }

    left_val  = process_node (scope_, node_->left_, global_scope);
    right_val = process_node (scope_, node->right_, global_scope);

    switch (node->op_type_)
    {
        case AST::OpType::ADD:
            return left_val + right_val;
        
        case AST::OpType::SUB:
            return left_val - right_val;
 
        case AST::OpType::MUL:
            return left_val * right_val;

        case AST::OpType::DIV:
            return left_val / right_val;

        case AST::OpType::REM:
            return left_val % right_val;

        case AST::OpType::ASS:
            return process_assignment (scope_, node_, global_scope);

        case AST::OpType::EQU:
            return left_val == right_val;

        case AST::OpType::EQG:
            return left_val >= right_val;

        case AST::OpType::EQL:
            return left_val <= right_val;

        case AST::OpType::EQN:
            return left_val != right_val;

        case AST::OpType::STG:
            return left_val > right_val;

        case AST::OpType::STL:
            return left_val < right_val;

        case AST::OpType::LAND:
            return left_val && right_val;

        case AST::OpType::LOR:
            return left_val || right_val;

        case AST::OpType::LNOT:
            return !left_val;

        case AST::OpType::BAND:
            return left_val & right_val;

        case AST::OpType::BOR:
            return left_val | right_val;

        case AST::OpType::BNOT:
            return ~left_val;

        case AST::OpType::BXOR:
            return left_val ^ right_val;

        case AST::OpType::SHL:
            return left_val << right_val;

        case AST::OpType::SHR:
            return left_val >> right_val;

        case AST::OpType::PLUS:
            return left_val;

        case AST::OpType::MINUS:
            return -left_val;

        case AST::OpType::PRE_INC:
            return process_pre_increment (scope_, node, 1);

        case AST::OpType::POST_INC:
            return process_post_increment (scope_, node, 1);

        case AST::OpType::PRE_DEC:
            return process_pre_increment (scope_, node, -1);

        case AST::OpType::POST_DEC:
            return process_post_increment (scope_, node, -1);

        default:
            throw ERRORS::UNKNOWN_OPERATION;
    }

    return 0;
}

int process_assignment (scope *scope_, AST::AbstractNode *node_, scope *global_scope)
{
    if (node_->left_->type_ != AST::AbstractNode::NodeType::VARIABLE)
            throw ERRORS::NOT_A_VARIABLE;

    AST::VariableNode *var_node = static_cast <AST::VariableNode*> (node_);

    basic_variable *abs_var = scope_->find (var_node->name_);

    if (abs_var == nullptr)
    {
        variable <int> *new_var = new variable <int>;

        scope_->objects.insert ({var_node->name_, static_cast <basic_variable*> (new_var)});

        abs_var = static_cast <basic_variable*> (new_var);
    }

    variable <int> *var_value = static_cast <variable <int> *> (abs_var);

    int right_res = process_node (scope_, node_->right_, global_scope);

    var_value->value = right_res;

    return right_res;
}

int process_pre_increment (scope *scope_, AST::AbstractNode *node_, int extra_val)
{
    if (node_->left_->type_ != AST::AbstractNode::NodeType::VARIABLE)
        throw ERRORS::NOT_A_VARIABLE;

    AST::VariableNode *var_node = static_cast <AST::VariableNode*> (node_);

    basic_variable *abs_var = scope_->find (var_node->name_);

    if (abs_var == nullptr)
        throw ERRORS::UNKNOWN_VARIABLE;

    variable <int> *var_value = static_cast <variable <int> *> (abs_var);

    var_value->value += extra_val;

    return var_value->value;
}

int process_post_increment (scope *scope_, AST::AbstractNode *node_, int extra_val)
{
   if (node_->left_->type_ != AST::AbstractNode::NodeType::VARIABLE)
        throw ERRORS::NOT_A_VARIABLE;

    AST::VariableNode *var_node = static_cast <AST::VariableNode*> (node_);

    basic_variable *abs_var = scope_->find (var_node->name_);

    if (abs_var == nullptr)
        throw ERRORS::UNKNOWN_VARIABLE;

    variable <int> *var_value = static_cast <variable <int> *> (abs_var);

    int res = var_value->value;

    var_value->value += extra_val;

    return res; 
}

int process_condition_node (scope *scope_, AST::AbstractNode *node_, scope *global_scope)
{
    if (node_->left_ == nullptr)
        throw ERRORS::EMPTY_CONDITION;

    scope *cur_scope = scope_->create_nested_scope();

    AST::ConditionNode *cond_node = static_cast <AST::ConditionNode*> (node_);

    switch (cond_node->cond_type_)
    {
        case AST::ConditionNode::CondType::IF:
            if (process_node (cur_scope, node_->left_, global_scope))
                process_node (cur_scope, node_->right_, global_scope);

            break;

        case AST::ConditionNode::CondType::WHILE:
            while (process_node (cur_scope, node_->left_, global_scope))
                process_node (cur_scope, node_->right_, global_scope);

            break;
        
        default:
            throw ERRORS::UNKNOWN_COND_OP;
    }

    scope_->scope_vec.pop_back ();

    delete cur_scope;

    return 1;
}

int process_funccall_node (scope  *scope_, AST::AbstractNode *node, scope *global_scope)
{
    int res = 0;

    AST::FunctionCallNode *func_node = static_cast <AST::FunctionCallNode*> (node);

    if (!func_node->name_.compare (std::string {"?"}))
    {
        std::cin >> res;
    }

    else if (!func_node->name_.compare (std::string {"print"}))
    {
        std::cout << process_node (scope_, node->left_, global_scope) << std::endl;
    }

    else 
        throw ERRORS::UNKNOWN_FUNC;

    return res;
}

};