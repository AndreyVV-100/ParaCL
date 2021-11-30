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

class Driver {
  FlexLexer *plex_;
  AST::Tree tree;

public:
  Driver(FlexLexer *plex) : plex_(plex), tree{} {}

  parser::token_type yylex(parser::semantic_type *yylval) {
    parser::token_type tt = static_cast<parser::token_type>(plex_->yylex());
    if (tt == yy::parser::token_type::NUMBER)
      yylval->as<int>() = std::stoi(plex_->YYText());
    return tt;
  }

  bool parse() {
    parser parser(this);
    bool res = parser.parse();
    return !res;
  }

  void insert(AST::AbstractNode* other) {
      tree.top_ = other;
  }

  void printout() const {
      tree.PrintTree ("bebra.dot");
  }
};

} // namespace yy
