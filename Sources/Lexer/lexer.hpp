#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include "location.hh"

class ParaCLlexer final: public yyFlexLexer
{
    yy::location location{};

public:
    ParaCLlexer() = default;

    void setlocation()
    {
        location.begin.line = location.end.line = lineno();
        int endCol = location.end.column;

        for (size_t i = 0; yytext[i]; ++i)
        {
            switch (yytext[i])
            {
                case ' ':
                    location.begin.column++;
                    location.end.column++;
                    break;
                case '\n':
                case '\v':
                case '\r':
                    location.begin.column = location.end.column = 1;
                    location.end.line = lineno();
                    break;
                case '\t':
                    location.begin.column += 8;
                    location.end.column += 8;
                    break;
                default: 
                    location.begin.column = endCol;
                    location.end.column = location.begin.column + YYLeng();
            }
        }
    }

    yy::location getlocation() const { return location; }

   int yylex() override;
};
