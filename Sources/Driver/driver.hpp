//-----------------------------------------------------------------------------
//
// Source code for MIPT ILab
// Slides: https://sourceforge.net/projects/cpp-lects-rus/files/cpp-graduate/
// Licensed after GNU GPL v3
//
//-----------------------------------------------------------------------------
//
//  Object-oriented parser with some state
//
//----------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <numeric>

#include "parser.tab.hh"
#include <FlexLexer.h>

namespace yy {

class Driver final {
	FlexLexer *plex_;
	ParaCL::Tree tree;

public:
	Driver(): plex_(new yyFlexLexer), tree{} {}

	parser::token_type yylex(parser::semantic_type *yylval)
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
			case yy::parser::token_type::ERR:
				throw std::string{"Parsing error"};
				break;
		}

		return tt;
	}

	bool parse()
	{
		parser parser(this);
		bool res = parser.parse();
		return !res;
	}

	void insert(ParaCL::AbstractNode* other) { tree.top_ = other; }
	void interpretate ();

	void printout() const { tree.PrintTree ("graph.dot"); }

	~Driver() { delete plex_; }
};

} // namespace yy
