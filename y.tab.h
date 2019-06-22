/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TIDENTIFIER = 258,
    TINTEGER = 259,
    TDOUBLE = 260,
    TYINT = 261,
    TYDOUBLE = 262,
    TYCHAR = 263,
    TYBOOL = 264,
    TYVOID = 265,
    TYSTRING = 266,
    TEXTERN = 267,
    TLITERAL = 268,
    TCEQ = 269,
    TCNE = 270,
    TCLT = 271,
    TCLE = 272,
    TCGT = 273,
    TCGE = 274,
    TEQUAL = 275,
    TLPAREN = 276,
    TRPAREN = 277,
    TLBRACE = 278,
    TRBRACE = 279,
    TCOMMA = 280,
    TDOT = 281,
    TSEMICOLON = 282,
    TLBRACKET = 283,
    TRBRACKET = 284,
    TQUOTATION = 285,
    TPLUS = 286,
    TMINUS = 287,
    TMUL = 288,
    TDIV = 289,
    TAND = 290,
    TOR = 291,
    TXOR = 292,
    TMOD = 293,
    TNEG = 294,
    TNOT = 295,
    TIF = 296,
    TELSE = 297,
    TFOR = 298,
    TWHILE = 299,
    TRETURN = 300
  };
#endif
/* Tokens.  */
#define TIDENTIFIER 258
#define TINTEGER 259
#define TDOUBLE 260
#define TYINT 261
#define TYDOUBLE 262
#define TYCHAR 263
#define TYBOOL 264
#define TYVOID 265
#define TYSTRING 266
#define TEXTERN 267
#define TLITERAL 268
#define TCEQ 269
#define TCNE 270
#define TCLT 271
#define TCLE 272
#define TCGT 273
#define TCGE 274
#define TEQUAL 275
#define TLPAREN 276
#define TRPAREN 277
#define TLBRACE 278
#define TRBRACE 279
#define TCOMMA 280
#define TDOT 281
#define TSEMICOLON 282
#define TLBRACKET 283
#define TRBRACKET 284
#define TQUOTATION 285
#define TPLUS 286
#define TMINUS 287
#define TMUL 288
#define TDIV 289
#define TAND 290
#define TOR 291
#define TXOR 292
#define TMOD 293
#define TNEG 294
#define TNOT 295
#define TIF 296
#define TELSE 297
#define TFOR 298
#define TWHILE 299
#define TRETURN 300

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 12 "grammar.y" /* yacc.c:1909  */

	NBlock* block;
	NExpression* expr;
	NStatement* stmt;
	NIdentifier* ident;
	NVariableDeclaration* var_decl;
	NArrayIndex* index;
	std::vector<shared_ptr<NVariableDeclaration>>* varvec;
	std::vector<shared_ptr<NExpression>>* exprvec;
	std::string* string;
	int token;

#line 157 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
