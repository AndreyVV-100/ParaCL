#pragma once

#include <iostream>
#include <numeric>

#include "parser.tab.hh"
#include <FlexLexer.h>
#include "interpretator.hpp"

namespace yy {

class Driver final {
	FlexLexer *plex_;
	AST::Tree tree;
	std::vector<std::string> errors;
public:
	Driver(): plex_(new yyFlexLexer), tree{} {}

	parser::token_type yylex(parser::semantic_type *yylval) // ToDo: move to driver.cpp?
	{
		parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());

		switch (tt)
		{
			case yy::parser::token_type::NUMBER:
			case yy::parser::token_type::ZERO:			
				yylval->build<int>() = std::stoi(plex_->YYText());
				break;
			case yy::parser::token_type::WORD:
			case yy::parser::token_type::PRINT:
			case yy::parser::token_type::SCAN:
			case yy::parser::token_type::FUNC:
				yylval->build<std::string>() = plex_->YYText();
				break;
			case yy::parser::token_type::LEXERR:
				push_error(std::string("unrecognized character: "));
		}

		return tt;
	}

	void parse()
	{
		parser parser(this);
		parser.parse();

		if (errors.size()) 
			throw errors;
	}

	void insert(AST::AbstractNode* other) { tree.top_ = other; }

	void push_error (std::string error)
	{
		errors.push_back (
			   std::string("line: ") + std::to_string(plex_->lineno()) +
			   std::string(" | error: ") + error
	    );
	}

	void push_err_text(std::string error)
	{
		push_error (
			error + std::string("\"") +
			std::string(plex_->YYText()) + "\""
		);
	}

	void interpretate ()
    {
        interpretator::start_interpretate (tree);
    }

	void printout() const { tree.PrintTree ("graph.dot"); }

	~Driver() { delete plex_; }
};

} // namespace yy
