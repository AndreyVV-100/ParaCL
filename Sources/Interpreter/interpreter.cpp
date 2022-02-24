#include "interpreter.hpp"
// ToDo: unificate codestyle?

namespace interpreter
{
//scope

scope * scope::create_nested_scope ()
{
    scope* new_scope = new scope;

    scope_vec.push_back (new_scope);

    new_scope->upper_scope = this;

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

//error processing

std::string interpreter::get_error_message (ERRORS error_code, AST::AbstractNode *node)
{
    int row_number = node->lineno_;

    std::string res = {};

    switch (error_code)
    {
        case ERRORS::MISSED_SEMICOLON:
        {
            res = std::string {"Missed semicolon\n"};
            break;
        }

        case ERRORS::UNKNOWN_FUNC:
        {
            Node::FunctionCallNode *func_node = static_cast <Node::FunctionCallNode*> (node);
            res = std::string {"Unknown function with name \""} + func_node->name_ + std::string {"\"\n"};
            break;
        }

        case ERRORS::UNKNOWN_VARIABLE:
        {
            Node::VariableNode *var_node = static_cast <Node::VariableNode*> (node);
            res = std::string {"Unknown variable \""} + var_node->name_ + std::string {"\"\n"};
            break;
        }

        case ERRORS::INCORRECT_TREE:
        {
            res = std::string {"Incorrect tree error (should be parsing error)\n"};
            break;
        }
        
        case ERRORS::NOT_A_VARIABLE:
        {
            res = std::string {"There isn't variable at leftward in assignment\n"};
            break;
        }

        case ERRORS::UNKNOWN_OPERATION:
        {
            Node::OperationNode* op_node = static_cast <Node::OperationNode*> (node);
            res = std::string {"Unknown operation code \""} + std::to_string (op_node->op_type_) + std::string {"\"\n"};
            break;
        }

        case ERRORS::EMPTY_CONDITION:
        {
            res = std::string {"Empty condition in conditional block\n"};
            break;
        }

        case ERRORS::UNKNOWN_COND_OP:
        {
            Node::ConditionNode* cond_node = static_cast <Node::ConditionNode*> (node);
            res = std::string {"Unknown conditional block \""} + std::to_string (static_cast <int> (cond_node->cond_type_)) + std::string {"\"\n"};
            break;
        }

        case ERRORS::UNKNOWN_NODETYPE:
        {
            res = std::string {"Unknown node type \""} + std::to_string (static_cast <int> (node->type_)) + std::string {"\"\n"};
            break;
        }

        default:
        {
            res = std::string {"Unknown error \""} + std::to_string (static_cast <int> (error_code)) + std::string {"\"\n"};
            break;
        }
    }

    return res;
}

// WARNING: this function always throw exception
void interpreter::process_error (ERRORS error_code, AST::AbstractNode *node)
{
    error_message = get_error_message (error_code, node);
    error_line_number = node->lineno_;
    throw std::runtime_error("interpreter error");
}

//main funcs

void interpreter::interpretate (scope *scope_, AST::AbstractNode *node)
{
    AST::AbstractNode *cur_node = node;
    AST::AbstractNode *cur_exec_node = nullptr;

    while (cur_node != nullptr)
    {
        if (cur_node->type_ != AST::NodeType::ORDER_OP)
            process_error (ERRORS::MISSED_SEMICOLON, node);

        cur_exec_node = cur_node->left_;

        if (cur_exec_node != nullptr)
        {
            process_node (scope_, cur_exec_node);
        }

        cur_node = cur_node->right_;
    }
}

int interpreter::process_node (scope *scope_, AST::AbstractNode *node)
{
    if (node == nullptr)
        return 0;

    switch (node->type_)
    {
        case AST::NodeType::VARIABLE:
        {
            Node::VariableNode *var_node = static_cast <Node::VariableNode*> (node);

            basic_variable* find_res = scope_->find (var_node->name_);
            
            if (find_res == nullptr)
                process_error (ERRORS::UNKNOWN_VARIABLE, node);

            variable <int> *var = static_cast <variable <int> *> (find_res);

            return var->value;
        }

        case AST::NodeType::CONST:
        {
            Node::ConstNode *const_node = static_cast <Node::ConstNode*> (node);

            return const_node->value_;
        }

        case AST::NodeType::OPERATION:
            return process_operation_node (scope_, node);
        
        case AST::NodeType::CONDITION:
            return process_condition_node (scope_, node);

        case AST::NodeType::FUNCTION_CALL:
            return process_funccall_node (scope_, node);

        case AST::NodeType::SCOPE:
            process_scope_node (scope_, node);
            break;

        default:
            process_error (ERRORS::UNKNOWN_NODETYPE, node);
            break;
    }

    return 0;
}

int interpreter::process_operation_node (scope *scope_, AST::AbstractNode *node_)
{
    Node::OperationNode *node = static_cast <Node::OperationNode*> (node_);

    int left_val = 0, right_val = 0;
    
    if ((node->op_type_ / 1000) == 1) // ToDo: 1000 = const, maybe binary?
    {
        if (node->left_ == nullptr)
            process_error (ERRORS::INCORRECT_TREE, node);
    }

    else 
    {
        if (node->left_ == nullptr or node->right_ == nullptr)
            process_error (ERRORS::INCORRECT_TREE, node);
    }

    if (node->op_type_ != AST::OpType::ASS)
    {
        left_val  = process_node (scope_, node_->left_);
        right_val = process_node (scope_, node->right_);
    }
    
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
            return process_assignment (scope_, node_);

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
            process_error (ERRORS::UNKNOWN_OPERATION, node);
            break;
    }

    return 0;
}

int interpreter::process_assignment (scope *scope_, AST::AbstractNode *node_)
{
    if (node_->left_->type_ != AST::NodeType::VARIABLE)
        process_error (ERRORS::NOT_A_VARIABLE, node_);

    Node::VariableNode *var_node = static_cast <Node::VariableNode*> (node_->left_);

    basic_variable *abs_var = scope_->find (var_node->name_);

    int right_res = process_node (scope_, node_->right_);

    if (abs_var == nullptr)
    {
        variable <int> *new_var = new variable <int>;

        scope_->objects.insert ({var_node->name_, static_cast <basic_variable*> (new_var)});

        abs_var = static_cast <basic_variable*> (new_var);
    }

    variable <int> *var_value = static_cast <variable <int> *> (abs_var);

    var_value->value = right_res;

    return right_res;
}

int interpreter::process_pre_increment (scope *scope_, AST::AbstractNode *node_, int extra_val)
{
    if (node_->left_->type_ != AST::NodeType::VARIABLE)
        process_error (ERRORS::NOT_A_VARIABLE, node_);

    Node::VariableNode *var_node = static_cast <Node::VariableNode*> (node_);

    basic_variable *abs_var = scope_->find (var_node->name_);

    if (abs_var == nullptr)
        process_error (ERRORS::UNKNOWN_VARIABLE, node_);

    variable <int> *var_value = static_cast <variable <int> *> (abs_var);

    var_value->value += extra_val;

    return var_value->value;
}

int interpreter::process_post_increment (scope *scope_, AST::AbstractNode *node_, int extra_val)
{
    if (node_->left_->type_ != AST::NodeType::VARIABLE)
        process_error (ERRORS::NOT_A_VARIABLE, node_);

    Node::VariableNode *var_node = static_cast <Node::VariableNode*> (node_);

    basic_variable *abs_var = scope_->find (var_node->name_);

    if (abs_var == nullptr)
        process_error (ERRORS::UNKNOWN_VARIABLE, node_);

    variable <int> *var_value = static_cast <variable <int> *> (abs_var);

    int res = var_value->value;

    var_value->value += extra_val;

    return res; 
}

int interpreter::process_condition_node (scope *scope_, AST::AbstractNode *node_)
{
    if (node_->left_ == nullptr)
        process_error (ERRORS::EMPTY_CONDITION, node_);

    scope *cur_scope = scope_;

    Node::ConditionNode *cond_node = static_cast <Node::ConditionNode*> (node_);

    switch (cond_node->cond_type_)
    {
        case AST::CondType::IF:
            if (process_node (cur_scope, node_->left_))
                process_node (cur_scope, node_->right_);

            break;

        case AST::CondType::WHILE:
            while (process_node (cur_scope, node_->left_))
                process_node (cur_scope, node_->right_);

            break;
        
        default:
            process_error (ERRORS::UNKNOWN_COND_OP, node_);
            break;
    }

    return 1;
}

int interpreter::process_funccall_node (scope  *scope_, AST::AbstractNode *node)
{
    int res = 0;

    Node::FunctionCallNode *func_node = static_cast <Node::FunctionCallNode*> (node);

    if (!func_node->name_.compare (std::string {"?"}))
        std::cin >> res;

    else if (!func_node->name_.compare (std::string {"print"}))
        std::cout << process_node (scope_, node->left_) << std::endl;

    else
        process_error (ERRORS::UNKNOWN_FUNC, node);

    return res;
}

void interpreter::process_scope_node (scope  *scope_, AST::AbstractNode *node)
{
    scope* cur_scope = nullptr;

    cur_scope = scope_->create_nested_scope();
    
    try
    {
        if (node->left_ != nullptr)
            interpretate (cur_scope, node->left_);
    }
    catch(std::runtime_error error)
    {
        scope_->scope_vec.pop_back ();
        delete cur_scope;

        throw error;
    }

    scope_->scope_vec.pop_back ();
    delete cur_scope;
}
};
