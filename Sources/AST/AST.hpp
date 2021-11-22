#ifndef HEADER_AST
#define HEADER_AST 1

#include <string>
#include <iostream>

namespace ParaCL
{
	enum class OpType
	{
		ADD = 0,    // +
		SUB = 1,    // -
		MUL = 2,    // *
		DIV = 3,    // /
		REM = 4,    // % - remainder of division

		ASS = 5,    // = - ♂assignment♂
		ORD = 6,    // ; - order

		EQU = 7,    // ==
		EQG = 8,    // >=
		EQL = 9,    // <=
		EQN = 10,   // !=
		STG = 11,   // > - strictly greater
		STL = 12,   // <

		LAND = 13,  // && - logical and
		LOR  = 14,  // ||
		LNOT = 15,  // !

		BAND = 16,  // & - binary and
		BOR  = 17,  // |
		BNOT = 18,  // ~
		BXOR = 19,  // ^
		
		SHL  = 20,  // <<
		SHR  = 21,  // >>

		PLUS  = 22, // + - unary plus
		MINUS = 23, // - - unary minus

		PRE_INC  = 24, // ++x
		POST_INC = 25, // x++
		PRE_DEC  = 26, // --x
		POST_DEC = 27  // x--
	};

	struct AbstractNode;

	struct Tree final
	{
		// ToDo: private?
		AbstractNode* top_;

		Tree () = default;
		Tree (const Tree&) = delete;
		Tree (Tree&&) = delete;
		Tree& operator = (const Tree&) = delete;
		Tree& operator = (Tree&&) = delete;
		void dump() const;
		~Tree();
	};


// ToDo: class or struct, non-private fields? How to get access in child?
struct AbstractNode
{
	enum class NodeType
	{
		VARIABLE      = 0,
		CONST         = 1,
		OPERATION     = 2,
		CONDITION     = 3,
		FUNCTION_CALL = 4,
		FUNCTION_DECL = 5  // Now not used
	};

	AbstractNode* prev_, 
				* left_  = nullptr,
				* right_ = nullptr;
	
	NodeType type_; // const?

	public:

	AbstractNode (AbstractNode* prev, NodeType type):
		prev_ (prev),
		type_ (type)
	{ ; }

	AbstractNode (const AbstractNode&) = delete;
	AbstractNode (AbstractNode&&) = delete;
	AbstractNode& operator = (const AbstractNode&) = delete;
	AbstractNode& operator = (AbstractNode&&) = delete;
	virtual ~AbstractNode() = default;
	virtual void dump() const = 0;

	// ToDo: SetPrev() ?
};

struct VariableNode final : public AbstractNode
{
	// static Scope - now we havn't class for it // const?
	std::string name_;

	VariableNode (std::string name, AbstractNode* prev = nullptr):
		AbstractNode (prev, NodeType :: VARIABLE),
		name_ (name)
	{ ; }

	void dump() const { std::cout << "VarNode <" << name_ << ">" << std::endl;}
};

struct ConstNode final : public AbstractNode
{
	int value_; // TODO: rework for all other types

	ConstNode (int value, AbstractNode* prev = nullptr):
		AbstractNode (prev, NodeType :: CONST),
		value_ (value)
	{ ; }

	~ConstNode() {};

	void dump() const { std::cout << "CONSTNODE <" << value_ << ">" << std::endl;}
};

struct OperationNode final : public AbstractNode
{
	OpType op_type_;

	OperationNode (OpType op_type, AbstractNode* prev = nullptr):
		AbstractNode (prev, NodeType :: OPERATION),
		op_type_ (op_type)
	{ ; }

	~OperationNode() {};
	void dump() const 
	{
		if (op_type_ == OpType::ADD)
			std::cout << "OpNode <+>" << std::endl;
		else
			std::cout << "OpNode <->" << std::endl;
	}
};

struct ConditionNode final : public AbstractNode
{
	enum class CondType
	{
		IF = 0,
		WHILE = 1
	};

	CondType cond_type_;

	ConditionNode (CondType cond_type, AbstractNode* prev = nullptr):
		AbstractNode (prev, NodeType :: CONDITION),
		cond_type_ (cond_type)
	{ ; }

	void dump() const { std::cout << "BranchNode <" << "if or while" << ">" << std::endl;}
};

struct FunctionCallNode final : public AbstractNode
{
	// ToDo: parameters list
	// ToDo: function list

	std::string name_;

	FunctionCallNode (std::string name, AbstractNode* prev = nullptr):
		AbstractNode (prev, NodeType :: FUNCTION_CALL),
		name_ (name)
	{ ; }

	void dump() const { std::cout << "FuncNode <" << name_ << ">" << std::endl;}
};
	AbstractNode* make_val(int val);
	AbstractNode* make_op(AbstractNode* lhs, OpType op, AbstractNode* rhs);
} // End of namespace ParaCL

#endif /* HEADER_AST */