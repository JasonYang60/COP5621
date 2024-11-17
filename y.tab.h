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
#define GETINT 267
#define GETBOOL 268
#define ADD 269
#define MINUS 270
#define MUL 271
#define DIV 272
#define MOD 273
#define SMALLER 274
#define GREATER 275
#define NOTGREATER 276
#define NOTSMALLER 277
#define EQUAL 278
#define AND 279
#define OR 280
#define NOT 281
#define IF 282
#define LET 283
#define STRCONST 284
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
