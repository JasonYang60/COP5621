#ifndef _yy_defines_h_
#define _yy_defines_h_

#define DEFINEFUN 257
#define PRINT 258
#define CONST 259
#define LEFT 260
#define RIGHT 261
#define VARNAME 262
#define TRUE 263
#define FALSE 264
#define INT 265
#define BOOL 266
#define ADD 267
#define MINUS 268
#define MUL 269
#define DIV 270
#define MOD 271
#define SMALLER 272
#define GREATER 273
#define NOTGREATER 274
#define NOTSMALLER 275
#define EQUAL 276
#define AND 277
#define OR 278
#define NOT 279
#define IF 280
#define LET 281
#define STRCONST 282
#define entry 283
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union YYSTYPE {
    int ival;
    char * sval;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;

#endif /* _yy_defines_h_ */
