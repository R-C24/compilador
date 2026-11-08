/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"

/* Declaraciones necesarias para conectar con el scanner (Flex) */
extern int yylex(void);
extern int lineaAct;
extern int columnaAct;
extern FILE *yyin;

extern int yyleng;
extern const char *archivoFuente;

char *tiposUsuario[MaxTipos];
int totalTiposUsuario = 0;

char *tiposEstandar[] = {
    "FILE", "DIR", "size_t", "ssize_t", "ptrdiff_t", "wchar_t", "time_t", "clock_t",
    "off_t", "pid_t", "va_list", "bool", "int8_t", "int16_t", "int32_t", "int64_t",
    "uint8_t", "uint16_t", "uint32_t", "uint64_t", "intptr_t", "uintptr_t", NULL
};

int totalErrores = 0;

void yyerror(const char *s);
struct ASTNodo *crearNodoUnario(const char *op, struct ASTNodo *operando);

struct ASTNodo *raizAST = NULL;


#line 104 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TokenInt = 3,                   /* TokenInt  */
  YYSYMBOL_TokenFloat = 4,                 /* TokenFloat  */
  YYSYMBOL_TokenChar = 5,                  /* TokenChar  */
  YYSYMBOL_TokenString = 6,                /* TokenString  */
  YYSYMBOL_TokenID = 7,                    /* TokenID  */
  YYSYMBOL_TokenVoid = 8,                  /* TokenVoid  */
  YYSYMBOL_TokenModificador = 9,           /* TokenModificador  */
  YYSYMBOL_TokenTipoDato = 10,             /* TokenTipoDato  */
  YYSYMBOL_TokenFlecha = 11,               /* TokenFlecha  */
  YYSYMBOL_TokenNombreTipo = 12,           /* TokenNombreTipo  */
  YYSYMBOL_TokenBreak = 13,                /* TokenBreak  */
  YYSYMBOL_TokenContinue = 14,             /* TokenContinue  */
  YYSYMBOL_TokenEnum = 15,                 /* TokenEnum  */
  YYSYMBOL_TokenGoto = 16,                 /* TokenGoto  */
  YYSYMBOL_TokenSizeof = 17,               /* TokenSizeof  */
  YYSYMBOL_TokenStruct = 18,               /* TokenStruct  */
  YYSYMBOL_TokenTypedef = 19,              /* TokenTypedef  */
  YYSYMBOL_TokenUnion = 20,                /* TokenUnion  */
  YYSYMBOL_TokenFor = 21,                  /* TokenFor  */
  YYSYMBOL_TokenIf = 22,                   /* TokenIf  */
  YYSYMBOL_TokenElse = 23,                 /* TokenElse  */
  YYSYMBOL_TokenSwitch = 24,               /* TokenSwitch  */
  YYSYMBOL_TokenWhile = 25,                /* TokenWhile  */
  YYSYMBOL_TokenDo = 26,                   /* TokenDo  */
  YYSYMBOL_TokenCase = 27,                 /* TokenCase  */
  YYSYMBOL_TokenReturn = 28,               /* TokenReturn  */
  YYSYMBOL_TokenDefault = 29,              /* TokenDefault  */
  YYSYMBOL_TokenOpAsignacion = 30,         /* TokenOpAsignacion  */
  YYSYMBOL_TokenOpLogico = 31,             /* TokenOpLogico  */
  YYSYMBOL_TokenOpBits = 32,               /* TokenOpBits  */
  YYSYMBOL_TokenOpIgualdad = 33,           /* TokenOpIgualdad  */
  YYSYMBOL_TokenOpDesplazamiento = 34,     /* TokenOpDesplazamiento  */
  YYSYMBOL_TokenOpAditivos = 35,           /* TokenOpAditivos  */
  YYSYMBOL_TokenOpMultiplicativos = 36,    /* TokenOpMultiplicativos  */
  YYSYMBOL_TokenOpInc = 37,                /* TokenOpInc  */
  YYSYMBOL_TokenError = 38,                /* TokenError  */
  YYSYMBOL_39_ = 39,                       /* '='  */
  YYSYMBOL_40_ = 40,                       /* '?'  */
  YYSYMBOL_41_ = 41,                       /* ':'  */
  YYSYMBOL_UNARIO = 42,                    /* UNARIO  */
  YYSYMBOL_43_ = 43,                       /* '['  */
  YYSYMBOL_44_ = 44,                       /* ']'  */
  YYSYMBOL_45_ = 45,                       /* '.'  */
  YYSYMBOL_46_ = 46,                       /* '}'  */
  YYSYMBOL_47_ = 47,                       /* ';'  */
  YYSYMBOL_48_ = 48,                       /* '('  */
  YYSYMBOL_49_ = 49,                       /* ')'  */
  YYSYMBOL_50_ = 50,                       /* '{'  */
  YYSYMBOL_51_ = 51,                       /* ','  */
  YYSYMBOL_52_ = 52,                       /* '!'  */
  YYSYMBOL_YYACCEPT = 53,                  /* $accept  */
  YYSYMBOL_programa = 54,                  /* programa  */
  YYSYMBOL_funcion = 55,                   /* funcion  */
  YYSYMBOL_retornar = 56,                  /* retornar  */
  YYSYMBOL_bloqueCodigo = 57,              /* bloqueCodigo  */
  YYSYMBOL_sentenciaBloque = 58,           /* sentenciaBloque  */
  YYSYMBOL_sentenciaSimple = 59,           /* sentenciaSimple  */
  YYSYMBOL_condicionIf = 60,               /* condicionIf  */
  YYSYMBOL_loopWhile = 61,                 /* loopWhile  */
  YYSYMBOL_loopDoWhile = 62,               /* loopDoWhile  */
  YYSYMBOL_loopFor = 63,                   /* loopFor  */
  YYSYMBOL_inicializacionFor = 64,         /* inicializacionFor  */
  YYSYMBOL_condicional = 65,               /* condicional  */
  YYSYMBOL_caseSwitch = 66,                /* caseSwitch  */
  YYSYMBOL_listaCasos = 67,                /* listaCasos  */
  YYSYMBOL_caso = 68,                      /* caso  */
  YYSYMBOL_elementoCase = 69,              /* elementoCase  */
  YYSYMBOL_listaParametros = 70,           /* listaParametros  */
  YYSYMBOL_parametro = 71,                 /* parametro  */
  YYSYMBOL_listaDeclaracionVariables = 72, /* listaDeclaracionVariables  */
  YYSYMBOL_declaracionVariable = 73,       /* declaracionVariable  */
  YYSYMBOL_listaIds = 74,                  /* listaIds  */
  YYSYMBOL_declaracionArrays = 75,         /* declaracionArrays  */
  YYSYMBOL_listaDimensiones = 76,          /* listaDimensiones  */
  YYSYMBOL_especificadores = 77,           /* especificadores  */
  YYSYMBOL_tipoCast = 78,                  /* tipoCast  */
  YYSYMBOL_elementoInit = 79,              /* elementoInit  */
  YYSYMBOL_listaElementos = 80,            /* listaElementos  */
  YYSYMBOL_expresion = 81,                 /* expresion  */
  YYSYMBOL_listaArgumentosOpt = 82,        /* listaArgumentosOpt  */
  YYSYMBOL_listaArgumentos = 83            /* listaArgumentos  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1449

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  176
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  381

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   294


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    52,     2,     2,     2,     2,     2,     2,
      48,    49,     2,     2,    51,     2,    45,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    41,    47,
       2,    39,     2,    40,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    43,     2,    44,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    50,     2,    46,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    42
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    75,    75,    79,    83,    88,    95,   100,   108,   113,
     118,   123,   128,   135,   139,   146,   150,   154,   160,   170,
     173,   185,   186,   187,   193,   197,   206,   207,   208,   209,
     210,   211,   212,   213,   217,   221,   222,   223,   229,   233,
     241,   245,   250,   256,   262,   270,   274,   282,   286,   295,
     302,   308,   316,   317,   318,   322,   323,   327,   331,   340,
     341,   348,   352,   361,   366,   370,   377,   378,   379,   386,
     390,   394,   405,   415,   421,   436,   437,   444,   449,   453,
     457,   463,   468,   473,   480,   485,   495,   507,   513,   519,
     531,   545,   553,   558,   564,   570,   580,   585,   590,   601,
     615,   619,   622,   627,   634,   638,   643,   650,   658,   663,
     667,   671,   675,   681,   685,   690,   695,   699,   705,   710,
     717,   724,   729,   736,   744,   749,   754,   759,   763,   770,
     775,   784,   788,   792,   795,   798,   802,   808,   811,   853,
     857,   862,   867,   871,   875,   879,   883,   887,   897,   901,
     905,   909,   913,   917,   921,   925,   930,   934,   938,   947,
     956,   960,   964,   968,   979,   984,   989,   995,  1006,  1019,
    1024,  1028,  1033,  1042,  1043,  1047,  1048
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TokenInt",
  "TokenFloat", "TokenChar", "TokenString", "TokenID", "TokenVoid",
  "TokenModificador", "TokenTipoDato", "TokenFlecha", "TokenNombreTipo",
  "TokenBreak", "TokenContinue", "TokenEnum", "TokenGoto", "TokenSizeof",
  "TokenStruct", "TokenTypedef", "TokenUnion", "TokenFor", "TokenIf",
  "TokenElse", "TokenSwitch", "TokenWhile", "TokenDo", "TokenCase",
  "TokenReturn", "TokenDefault", "TokenOpAsignacion", "TokenOpLogico",
  "TokenOpBits", "TokenOpIgualdad", "TokenOpDesplazamiento",
  "TokenOpAditivos", "TokenOpMultiplicativos", "TokenOpInc", "TokenError",
  "'='", "'?'", "':'", "UNARIO", "'['", "']'", "'.'", "'}'", "';'", "'('",
  "')'", "'{'", "','", "'!'", "$accept", "programa", "funcion", "retornar",
  "bloqueCodigo", "sentenciaBloque", "sentenciaSimple", "condicionIf",
  "loopWhile", "loopDoWhile", "loopFor", "inicializacionFor",
  "condicional", "caseSwitch", "listaCasos", "caso", "elementoCase",
  "listaParametros", "parametro", "listaDeclaracionVariables",
  "declaracionVariable", "listaIds", "declaracionArrays",
  "listaDimensiones", "especificadores", "tipoCast", "elementoInit",
  "listaElementos", "expresion", "listaArgumentosOpt", "listaArgumentos", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-262)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-159)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -262,  1163,  -262,    -3,  -262,   811,  -262,  -262,     2,     5,
     911,    24,  -262,  -262,  -262,  -262,  -262,    44,  -262,  -262,
    -262,    85,  -262,  -262,    26,    99,   134,    98,    -1,   101,
    -262,    -1,   108,  -262,   -23,    74,  1065,  -262,   119,     7,
     155,  -262,   185,   189,  -262,    -1,  -262,  -262,    -1,   167,
      -1,    53,  -262,   961,    -6,  -262,  1075,   951,  1065,    27,
     127,   198,   140,   169,  -262,  -262,  -262,  -262,   147,  1101,
    1065,  1065,  1065,  1065,   930,  1065,  1251,   217,  -262,  -262,
      95,  -262,  -262,  -262,   106,   121,   987,  -262,  1246,  -262,
    -262,   139,  -262,  1259,  -262,  1122,  1374,  1374,   199,  -262,
     186,   322,  -262,     4,   200,    86,  -262,   105,   911,  1065,
     930,   260,   260,   260,   260,   260,   208,  1018,   204,    29,
      48,   182,   260,   252,  1065,  1065,  1065,  1065,  1065,  1065,
    1065,  -262,  1065,  1065,  1065,   261,  -262,   207,   251,   271,
     231,  -262,  -262,  1122,  -262,   279,   120,  -262,   188,   224,
     253,   256,   156,  -262,   275,  1374,  -262,  -262,   780,  -262,
     911,   246,   302,  1122,   266,  -262,   286,   280,   -36,  1374,
     292,   309,    60,   312,   342,  1065,   -22,   358,  1065,  -262,
    -262,  1374,  1030,   229,   212,   212,   260,   260,  1374,  1341,
    1358,  -262,   362,  1008,  1065,  -262,   404,   317,   379,   366,
    1122,    65,   -24,   377,   378,   422,     9,    20,   880,   393,
      21,   880,  1044,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  1278,  -262,   414,   246,   318,
    -262,   420,   410,    82,  -262,  1065,   412,   157,  1065,  1065,
     260,   415,  1065,   260,  1065,  -262,   911,  1122,  1374,  1374,
     419,  -262,  -262,  -262,  -262,  -262,   880,  -262,  -262,   423,
     413,   276,   421,  1065,  -262,  -262,  -262,  1065,   424,  1065,
     444,  -262,  1296,   380,  -262,   414,   425,   429,  1122,  -262,
    -262,  1374,   157,   260,   260,  1065,   260,  1389,   249,   324,
     427,  -262,  -262,  -262,   442,  -262,  1065,  -262,  1323,  -262,
     830,   430,  1154,  -262,  1181,   459,  -262,  -262,  -262,  -262,
    1122,   328,   480,   260,  -262,  -262,   911,   530,   880,   426,
    1374,  -262,   580,  1065,  1065,  1065,   194,   880,  -262,  -262,
    -262,   460,   630,   880,  1065,   363,   462,  -262,   278,  -262,
    -262,  1065,  -262,  1404,   260,   260,   488,   680,   486,  -262,
    -262,  1208,   467,  -262,  -262,   469,   880,  -262,   416,   144,
    -262,   472,  -262,   880,  -262,  -262,  -262,  -262,   479,   481,
    -262,  -262,  -262,  -262,  -262,  -262,   330,   730,   474,  -262,
    -262
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,   113,   116,   110,   111,     0,     0,
       0,     0,     5,     6,     3,     4,    79,     0,   104,     7,
     115,   118,   114,   112,     0,     0,     0,   126,     0,   121,
      75,     0,   105,    75,    84,     0,     0,    77,     0,     0,
     120,   117,     0,     0,   128,     0,   122,   106,     0,    84,
       0,     0,    75,     0,     0,    75,     0,     0,     0,     0,
       0,     0,    84,    85,   140,   141,   142,   143,   139,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    80,    78,
       0,   119,   123,   107,     0,     0,     0,   127,     0,   125,
      76,     0,    81,     0,   109,     0,    87,    92,     0,   101,
       0,     0,    67,    70,     0,     0,    96,     0,    66,   173,
       0,   161,   158,   156,   159,   160,   113,   116,   110,     0,
       0,     0,   157,     0,     0,     0,     0,     0,     0,     0,
       0,   164,     0,     0,     0,     0,    83,     0,    88,     0,
       0,   130,   129,     0,   124,    84,     0,   108,   140,   141,
     142,   143,   139,   137,     0,   136,   100,    14,     0,    13,
       0,    69,    72,     0,     0,   103,     0,     0,     0,   175,
       0,   174,     0,     0,   114,     0,   121,     0,     0,   144,
     171,   149,   150,   151,   152,   153,   154,   155,   148,     0,
       0,   172,     0,     0,     0,    89,     0,     0,    84,    94,
       0,     0,   139,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    18,     8,    19,    23,    32,    12,    22,    27,
      29,    31,    30,    28,    21,     0,    68,    73,    71,     0,
     102,     0,     0,     0,   170,     0,     0,   162,     0,     0,
     165,     0,     0,   146,     0,   145,    66,     0,    91,    93,
       0,    94,    82,   138,    25,    24,     0,    33,    34,     0,
       0,     0,     0,     0,    36,    19,    37,     0,     0,     0,
       0,    15,    17,     0,    26,    74,     0,     0,     0,     9,
      19,   176,   163,   167,   166,     0,   147,   169,     0,     0,
       0,    39,    38,    19,     0,    52,    55,    53,     0,    19,
       0,     0,     0,    19,     0,     0,    16,    10,    20,    97,
       0,     0,     0,   168,    86,    95,    66,     0,     0,     0,
      56,    54,     0,     0,     0,     0,   164,     0,    19,    43,
      35,     0,     0,     0,     0,     0,     0,    11,     0,    51,
      50,    55,    44,   151,   154,   155,    40,     0,     0,    46,
      45,     0,     0,    98,    90,     0,     0,    35,     0,     0,
      59,    48,    99,     0,    41,    63,    64,    65,     0,     0,
      57,    60,    47,    49,    19,    19,     0,     0,     0,    58,
      33
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -262,  -262,   522,  -262,  -261,   367,  -174,  -262,  -262,  -262,
    -262,  -262,   183,  -262,  -262,   170,  -262,  -106,   381,   -26,
       0,    42,  -262,  -156,   -10,   -35,   357,  -140,    -9,  -262,
    -262
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,   215,   216,   273,   308,   218,   219,   220,   221,
     222,   296,   319,   223,   359,   360,   368,   101,   102,    53,
     224,    63,    16,    61,    17,    18,   153,   154,   225,   170,
     171
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      31,    15,   168,   197,   301,   227,    49,    56,    78,    27,
     260,   161,    29,   233,   241,   160,    57,   256,    58,   312,
      59,   262,   268,   229,   109,    60,    88,    76,    52,    93,
      98,    32,   317,    44,   266,    50,   176,   270,   322,   120,
     162,    92,   332,    91,    19,    80,    91,    38,    96,    97,
     103,    34,    28,    90,    79,    30,    90,   261,    80,    39,
     111,   112,   113,   114,   115,   121,   122,   347,   263,   269,
      51,    99,   275,    54,    33,   172,    45,    96,    91,    30,
      35,    62,   291,    91,   177,    36,   155,    84,    90,   164,
      85,    37,    38,    90,    40,    41,   236,   178,   103,    87,
     169,   121,   138,    42,    80,    43,    46,   289,   166,   237,
      50,   254,   255,   376,   377,   181,   182,   183,   184,   185,
     186,   187,    38,   188,   189,   190,   329,   198,   100,   279,
     165,   139,   280,    39,   155,     4,     5,     6,   311,     7,
     288,    47,     8,   140,   340,     9,   145,    11,    48,   167,
     103,    52,   141,   346,   155,    77,    50,    80,    55,   350,
      64,    65,    66,    67,    68,    81,   240,   142,    38,   243,
     335,   358,    80,   369,    69,   146,   -66,   -66,   -66,    86,
      36,    58,   364,   107,   248,   249,    37,    38,   108,   373,
     370,   155,    82,   123,    73,   109,    83,    64,    65,    66,
      67,    68,  -135,   272,   109,    74,    86,  -135,    58,    75,
     338,    69,   124,   125,   126,   127,   128,   129,   130,   131,
      80,   132,   133,   123,   137,   134,   281,   135,   243,   283,
     284,   179,   157,   286,  -131,   287,   103,   104,   155,  -131,
     123,   105,    74,   156,   173,   106,    75,   129,   130,   131,
     163,    91,   298,   175,   300,   134,   192,   135,   302,   180,
     304,   297,   127,   128,   129,   130,   131,   196,   191,   155,
    -132,   123,   134,   286,   135,  -132,   313,   294,   195,    64,
      65,    66,    67,    68,     4,     5,     6,   320,     7,    59,
     193,     8,   194,    69,     9,    10,    11,   131,   314,  -133,
     160,   155,  -134,   134,  -133,   135,   103,  -134,    70,   228,
     230,    71,    72,    73,   343,   344,   345,   115,    57,   232,
      58,   199,    59,   295,    74,   351,   200,   354,    75,   160,
     231,   201,   320,    64,    65,    66,    67,   202,     4,     5,
       6,   234,     7,   378,   204,     8,   205,    69,     9,    10,
      11,   206,   207,   208,   209,   210,   211,   -61,   212,   -61,
     235,   238,    70,   251,   276,    71,    72,    73,   200,   200,
     315,   158,   159,   160,   336,   200,   -61,   264,    74,   200,
     265,   201,    75,    64,    65,    66,    67,   202,     4,     5,
       6,   239,     7,   203,   204,     8,   205,    69,     9,    10,
      11,   206,   207,   208,   209,   210,   211,   242,   212,   352,
     246,   250,    70,   252,   200,    71,    72,    73,    86,   365,
      58,   366,   107,   367,   257,   258,   307,   264,    74,   259,
     265,   201,    75,    64,    65,    66,    67,   202,     4,     5,
       6,   267,     7,   203,   204,     8,   205,    69,     9,    10,
      11,   206,   207,   208,   209,   210,   211,   105,   212,   277,
     278,   282,    70,   293,   285,    71,    72,    73,   290,   305,
     292,   299,   309,   341,   303,   316,   330,   264,    74,   310,
     265,   201,    75,    64,    65,    66,    67,   202,     4,     5,
       6,   318,     7,   203,   204,     8,   205,    69,     9,    10,
      11,   206,   207,   208,   209,   210,   211,   334,   212,   353,
     348,   356,    70,   358,   362,    71,    72,    73,   363,   372,
     374,   380,   375,    14,   355,   217,   337,   264,    74,   371,
     265,   201,    75,    64,    65,    66,    67,   202,     4,     5,
       6,   226,     7,   203,   204,     8,   205,    69,     9,    10,
      11,   206,   207,   208,   209,   210,   211,   253,   212,     0,
       0,     0,    70,     0,     0,    71,    72,    73,     0,     0,
       0,     0,     0,     0,     0,     0,   339,   264,    74,     0,
     265,   201,    75,    64,    65,    66,    67,   202,     4,     5,
       6,     0,     7,   203,   204,     8,   205,    69,     9,    10,
      11,   206,   207,   208,   209,   210,   211,     0,   212,     0,
       0,     0,    70,     0,     0,    71,    72,    73,     0,     0,
       0,     0,     0,     0,     0,     0,   342,   264,    74,     0,
     265,   201,    75,    64,    65,    66,    67,   202,     4,     5,
       6,     0,     7,   203,   204,     8,   205,    69,     9,    10,
      11,   206,   207,   208,   209,   210,   211,     0,   212,     0,
       0,     0,    70,     0,     0,    71,    72,    73,     0,     0,
       0,     0,     0,     0,     0,     0,   349,   264,    74,     0,
     265,   201,    75,    64,    65,    66,    67,   202,     4,     5,
       6,     0,     7,   203,   204,     8,   205,    69,     9,    10,
      11,   206,   207,   208,   209,   210,   211,     0,   212,     0,
       0,     0,    70,     0,     0,    71,    72,    73,     0,     0,
       0,     0,     0,     0,     0,     0,   357,   264,    74,     0,
     265,   201,    75,    64,    65,    66,    67,   202,     4,     5,
       6,     0,     7,   203,   204,     8,   205,    69,     9,    10,
      11,   206,   207,   208,   209,   210,   211,     0,   212,     0,
       0,     0,    70,     0,     0,    71,    72,    73,     0,     0,
       0,     0,     0,     0,     0,     0,   379,   264,    74,     0,
     265,   201,    75,    64,    65,    66,    67,   202,     4,     5,
       6,     0,     7,   203,   204,     8,   205,    69,     9,    10,
      11,   206,   207,   208,   209,   210,   211,     0,   212,     0,
       0,     0,    70,     0,     0,    71,    72,    73,     0,    20,
      21,    22,     0,    23,     0,     0,    24,   213,    74,    25,
     214,    26,    75,    64,    65,    66,    67,   202,     0,     0,
       0,   123,     0,   203,   204,     0,   205,    69,     0,     0,
       0,   206,   207,   208,   209,   210,   211,     0,   212,     0,
     124,   125,   323,   127,   128,   324,   325,   326,     0,   132,
     133,     0,     0,   134,     0,   135,     0,   264,    74,   327,
     328,     0,    75,    64,    65,    66,    67,   202,     0,     0,
       0,     0,     0,   203,   204,     0,   205,    69,     0,     0,
       0,   206,   207,   208,   209,   210,   211,     0,   212,     0,
       0,     0,    70,     0,     0,    71,    72,    73,     0,     4,
       5,     6,     0,     7,     0,     0,     8,   264,    74,     9,
     265,    11,    75,    64,    65,    66,    67,    68,   116,   117,
     118,     0,     7,     0,     0,     8,     0,    69,   119,     0,
      11,     0,     0,     0,    64,    65,    66,    67,    68,     0,
       0,     0,    70,     0,     0,    71,    72,    73,    69,     4,
       5,     6,     0,     7,     0,     0,     8,     0,    74,     9,
      10,    11,    75,    70,     0,     0,    71,    72,    73,     0,
      64,    65,    66,    67,    68,     0,     0,     0,     0,    74,
       0,    95,     0,    75,    69,     0,     0,    89,     0,     0,
       0,    64,    65,    66,    67,    68,     0,     0,     0,    70,
       0,     0,    71,    72,    73,    69,    20,    21,   174,     0,
      23,     0,     0,    24,     0,    74,    25,   143,    26,    75,
      70,   123,     0,    71,    72,    73,     0,    64,    65,    66,
      67,    68,     0,     0,     0,     0,    74,     0,   247,     0,
      75,    69,   126,   127,   128,   129,   130,   131,    64,    65,
      66,    67,    68,   134,     0,   135,    70,     0,     0,    71,
      72,    73,    69,     4,     5,     6,     0,     7,     0,     0,
       8,   271,    74,     9,    10,    11,    75,    70,     0,     0,
      71,    72,    73,     0,    64,    65,    66,    67,    68,     0,
       0,     0,     0,    74,     0,     0,     0,    75,    69,     0,
       0,    94,     0,     0,     0,   148,   149,   150,   151,   152,
       0,     0,     0,    70,     0,     0,    71,    72,    73,    69,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   110,
       0,     0,     0,    75,    70,     0,     0,    71,    72,    73,
       0,     0,     0,     2,     3,   123,     0,     0,     0,     0,
      74,     4,     5,     6,    75,     7,     0,     0,     8,     0,
       0,     9,    10,    11,   124,   125,   126,   127,   128,   129,
     130,   131,   123,   132,   133,     0,     0,   134,     0,   135,
       0,     0,     0,   331,     0,     0,     0,     0,     0,    12,
      13,   124,   125,   126,   127,   128,   129,   130,   131,   123,
     132,   133,     0,     0,   134,     0,   135,     0,     0,     0,
     333,     0,     0,     0,     0,     0,     0,     0,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,   133,     0,
       0,   134,     0,   135,     4,     5,     6,   361,     7,     0,
       0,     8,   123,     0,     9,    10,    11,     4,     5,     6,
       0,     7,     0,     0,     8,     0,     0,     9,    10,    11,
       0,   124,   125,   126,   127,   128,   129,   130,   131,   123,
     132,   133,   144,     0,   134,     0,   135,     0,   136,     0,
       0,     0,     0,     0,     0,   147,     0,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,   133,     0,
       0,   134,     0,   135,     0,   274,   124,   125,   126,   127,
     128,   129,   130,   131,   123,   132,   133,     0,     0,   134,
       0,   135,     0,   306,     0,     0,     0,     0,     0,     0,
       0,     0,   123,   124,   125,   126,   127,   128,   129,   130,
     131,     0,   132,   133,     0,     0,   134,     0,   135,   123,
     321,   124,   125,   126,   127,   128,   129,   130,   131,     0,
     132,   133,   244,     0,   134,   123,   135,     0,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,   133,     0,
     123,   134,   245,   135,   124,   125,   126,   127,   128,   129,
     130,   131,     0,   132,   133,   123,     0,   134,     0,   135,
     125,   126,   127,   128,   129,   130,   131,     0,     0,   133,
       0,     0,   134,     0,   135,     0,     0,  -158,  -158,  -158,
    -158,   131,     0,     0,     0,     0,     0,   134,     0,   135
};

static const yytype_int16 yycheck[] =
{
      10,     1,   108,   143,   265,   161,     7,    33,     1,     7,
       1,     7,     7,    49,    36,    51,    39,    41,    41,   280,
      43,     1,     1,   163,    48,    48,    52,    36,    50,    55,
       3,     7,   293,     7,   208,    36,     7,   211,   299,    74,
      36,    47,   303,    53,    47,    51,    56,    48,    57,    58,
      60,     7,    50,    53,    47,    50,    56,    48,    51,    17,
      69,    70,    71,    72,    73,    74,    75,   328,    48,    48,
      28,    44,   228,    31,    50,   110,    50,    86,    88,    50,
      36,     7,   256,    93,    36,    41,    95,    45,    88,     3,
      48,    47,    48,    93,     9,    10,    36,    49,   108,    46,
     109,   110,     7,    18,    51,    20,     7,   247,     3,    49,
      36,    46,    47,   374,   375,   124,   125,   126,   127,   128,
     129,   130,    48,   132,   133,   134,   300,     7,     1,    47,
      44,    36,    50,    91,   143,     8,     9,    10,   278,    12,
     246,     7,    15,    48,   318,    18,     7,    20,    50,    44,
     160,    50,    46,   327,   163,    36,    36,    51,    50,   333,
       3,     4,     5,     6,     7,    10,   175,    46,    48,   178,
     310,    27,    51,    29,    17,    36,    49,    50,    51,    39,
      41,    41,   356,    43,   193,   194,    47,    48,    48,   363,
      46,   200,     7,    11,    37,    48,     7,     3,     4,     5,
       6,     7,    46,   212,    48,    48,    39,    51,    41,    52,
     316,    17,    30,    31,    32,    33,    34,    35,    36,    37,
      51,    39,    40,    11,     7,    43,   235,    45,   237,   238,
     239,    49,    46,   242,    46,   244,   246,    39,   247,    51,
      11,    43,    48,    44,    36,    47,    52,    35,    36,    37,
      50,   261,   261,    49,   263,    43,    49,    45,   267,     7,
     269,   261,    33,    34,    35,    36,    37,    36,     7,   278,
      46,    11,    43,   282,    45,    51,   285,     1,     7,     3,
       4,     5,     6,     7,     8,     9,    10,   296,    12,    43,
      39,    15,    41,    17,    18,    19,    20,    37,    49,    46,
      51,   310,    46,    43,    51,    45,   316,    51,    32,     7,
      44,    35,    36,    37,   323,   324,   325,   326,    39,    39,
      41,    46,    43,    47,    48,   334,    51,    49,    52,    51,
      44,     1,   341,     3,     4,     5,     6,     7,     8,     9,
      10,    49,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      51,    49,    32,    46,    46,    35,    36,    37,    51,    51,
      46,    49,    50,    51,    46,    51,    46,    47,    48,    51,
      50,     1,    52,     3,     4,     5,     6,     7,     8,     9,
      10,    49,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    49,    28,    46,
      48,     7,    32,    47,    51,    35,    36,    37,    39,     3,
      41,     5,    43,     7,    47,    47,    46,    47,    48,     7,
      50,     1,    52,     3,     4,     5,     6,     7,     8,     9,
      10,    48,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    43,    28,    39,
      50,    49,    32,    50,    49,    35,    36,    37,    49,    25,
      47,    50,    47,    47,    50,    48,    46,    47,    48,    50,
      50,     1,    52,     3,     4,     5,     6,     7,     8,     9,
      10,    49,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    48,    28,    47,
      50,    23,    32,    27,    47,    35,    36,    37,    49,    47,
      41,    47,    41,     1,   341,   158,    46,    47,    48,   359,
      50,     1,    52,     3,     4,     5,     6,     7,     8,     9,
      10,   160,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,   200,    28,    -1,
      -1,    -1,    32,    -1,    -1,    35,    36,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    -1,
      50,     1,    52,     3,     4,     5,     6,     7,     8,     9,
      10,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    -1,    28,    -1,
      -1,    -1,    32,    -1,    -1,    35,    36,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    -1,
      50,     1,    52,     3,     4,     5,     6,     7,     8,     9,
      10,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    -1,    28,    -1,
      -1,    -1,    32,    -1,    -1,    35,    36,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    -1,
      50,     1,    52,     3,     4,     5,     6,     7,     8,     9,
      10,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    -1,    28,    -1,
      -1,    -1,    32,    -1,    -1,    35,    36,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    -1,
      50,     1,    52,     3,     4,     5,     6,     7,     8,     9,
      10,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    -1,    28,    -1,
      -1,    -1,    32,    -1,    -1,    35,    36,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    -1,
      50,     1,    52,     3,     4,     5,     6,     7,     8,     9,
      10,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    -1,    28,    -1,
      -1,    -1,    32,    -1,    -1,    35,    36,    37,    -1,     8,
       9,    10,    -1,    12,    -1,    -1,    15,    47,    48,    18,
      50,    20,    52,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    11,    -1,    13,    14,    -1,    16,    17,    -1,    -1,
      -1,    21,    22,    23,    24,    25,    26,    -1,    28,    -1,
      30,    31,    32,    33,    34,    35,    36,    37,    -1,    39,
      40,    -1,    -1,    43,    -1,    45,    -1,    47,    48,    49,
      50,    -1,    52,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    -1,    -1,    13,    14,    -1,    16,    17,    -1,    -1,
      -1,    21,    22,    23,    24,    25,    26,    -1,    28,    -1,
      -1,    -1,    32,    -1,    -1,    35,    36,    37,    -1,     8,
       9,    10,    -1,    12,    -1,    -1,    15,    47,    48,    18,
      50,    20,    52,     3,     4,     5,     6,     7,     8,     9,
      10,    -1,    12,    -1,    -1,    15,    -1,    17,    18,    -1,
      20,    -1,    -1,    -1,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    32,    -1,    -1,    35,    36,    37,    17,     8,
       9,    10,    -1,    12,    -1,    -1,    15,    -1,    48,    18,
      19,    20,    52,    32,    -1,    -1,    35,    36,    37,    -1,
       3,     4,     5,     6,     7,    -1,    -1,    -1,    -1,    48,
      -1,    50,    -1,    52,    17,    -1,    -1,    46,    -1,    -1,
      -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,    32,
      -1,    -1,    35,    36,    37,    17,     8,     9,    10,    -1,
      12,    -1,    -1,    15,    -1,    48,    18,    50,    20,    52,
      32,    11,    -1,    35,    36,    37,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    -1,    48,    -1,    50,    -1,
      52,    17,    32,    33,    34,    35,    36,    37,     3,     4,
       5,     6,     7,    43,    -1,    45,    32,    -1,    -1,    35,
      36,    37,    17,     8,     9,    10,    -1,    12,    -1,    -1,
      15,    47,    48,    18,    19,    20,    52,    32,    -1,    -1,
      35,    36,    37,    -1,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    -1,    48,    -1,    -1,    -1,    52,    17,    -1,
      -1,    46,    -1,    -1,    -1,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    32,    -1,    -1,    35,    36,    37,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,
      -1,    -1,    -1,    52,    32,    -1,    -1,    35,    36,    37,
      -1,    -1,    -1,     0,     1,    11,    -1,    -1,    -1,    -1,
      48,     8,     9,    10,    52,    12,    -1,    -1,    15,    -1,
      -1,    18,    19,    20,    30,    31,    32,    33,    34,    35,
      36,    37,    11,    39,    40,    -1,    -1,    43,    -1,    45,
      -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,    -1,    46,
      47,    30,    31,    32,    33,    34,    35,    36,    37,    11,
      39,    40,    -1,    -1,    43,    -1,    45,    -1,    -1,    -1,
      49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    -1,    39,    40,    -1,
      -1,    43,    -1,    45,     8,     9,    10,    49,    12,    -1,
      -1,    15,    11,    -1,    18,    19,    20,     8,     9,    10,
      -1,    12,    -1,    -1,    15,    -1,    -1,    18,    19,    20,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    11,
      39,    40,    46,    -1,    43,    -1,    45,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    11,    30,    31,
      32,    33,    34,    35,    36,    37,    -1,    39,    40,    -1,
      -1,    43,    -1,    45,    -1,    47,    30,    31,    32,    33,
      34,    35,    36,    37,    11,    39,    40,    -1,    -1,    43,
      -1,    45,    -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    11,    30,    31,    32,    33,    34,    35,    36,
      37,    -1,    39,    40,    -1,    -1,    43,    -1,    45,    11,
      47,    30,    31,    32,    33,    34,    35,    36,    37,    -1,
      39,    40,    41,    -1,    43,    11,    45,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    -1,    39,    40,    -1,
      11,    43,    44,    45,    30,    31,    32,    33,    34,    35,
      36,    37,    -1,    39,    40,    11,    -1,    43,    -1,    45,
      31,    32,    33,    34,    35,    36,    37,    -1,    -1,    40,
      -1,    -1,    43,    -1,    45,    -1,    -1,    33,    34,    35,
      36,    37,    -1,    -1,    -1,    -1,    -1,    43,    -1,    45
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    54,     0,     1,     8,     9,    10,    12,    15,    18,
      19,    20,    46,    47,    55,    73,    75,    77,    78,    47,
       8,     9,    10,    12,    15,    18,    20,     7,    50,     7,
      50,    77,     7,    50,     7,    36,    41,    47,    48,    74,
       9,    10,    18,    20,     7,    50,     7,     7,    50,     7,
      36,    74,    50,    72,    74,    50,    72,    39,    41,    43,
      48,    76,     7,    74,     3,     4,     5,     6,     7,    17,
      32,    35,    36,    37,    48,    52,    81,    36,     1,    47,
      51,    10,     7,     7,    74,    74,    39,    46,    72,    46,
      73,    77,    47,    72,    46,    50,    81,    81,     3,    44,
       1,    70,    71,    77,    39,    43,    47,    43,    48,    48,
      48,    81,    81,    81,    81,    81,     8,     9,    10,    18,
      78,    81,    81,    11,    30,    31,    32,    33,    34,    35,
      36,    37,    39,    40,    43,    45,    47,     7,     7,    36,
      48,    46,    46,    50,    46,     7,    36,    46,     3,     4,
       5,     6,     7,    79,    80,    81,    44,    46,    49,    50,
      51,     7,    36,    50,     3,    44,     3,    44,    70,    81,
      82,    83,    78,    36,    10,    49,     7,    36,    49,    49,
       7,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,     7,    49,    39,    41,     7,    36,    80,     7,    46,
      51,     1,     7,    13,    14,    16,    21,    22,    23,    24,
      25,    26,    28,    47,    50,    55,    56,    58,    59,    60,
      61,    62,    63,    66,    73,    81,    71,    76,     7,    80,
      44,    44,    39,    49,    49,    51,    36,    49,    49,    49,
      81,    36,    49,    81,    41,    44,    48,    50,    81,    81,
       7,    46,    47,    79,    46,    47,    41,    47,    47,     7,
       1,    48,     1,    48,    47,    50,    59,    48,     1,    48,
      59,    47,    81,    57,    47,    76,    46,    39,    50,    47,
      50,    81,    49,    81,    81,    49,    81,    81,    70,    80,
      49,    59,    47,    50,     1,    47,    64,    73,    81,    50,
      81,    57,    81,    50,    81,    25,    47,    46,    58,    47,
      50,    80,    57,    81,    49,    46,    48,    57,    49,    65,
      81,    47,    57,    32,    35,    36,    37,    49,    50,    59,
      46,    49,    57,    49,    48,    80,    46,    46,    70,    46,
      59,    47,    46,    81,    81,    81,    59,    57,    50,    46,
      59,    81,    46,    47,    49,    65,    23,    46,    27,    67,
      68,    49,    47,    49,    59,     3,     5,     7,    69,    29,
      46,    68,    47,    59,    41,    41,    57,    57,    13,    46,
      47
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    53,    54,    54,    54,    54,    54,    54,    55,    55,
      55,    55,    55,    55,    55,    56,    56,    56,    56,    57,
      57,    58,    58,    58,    58,    58,    59,    59,    59,    59,
      59,    59,    59,    59,    59,    59,    59,    59,    59,    59,
      60,    60,    60,    60,    60,    61,    61,    62,    62,    63,
      63,    63,    64,    64,    64,    65,    65,    66,    66,    67,
      67,    68,    68,    69,    69,    69,    70,    70,    70,    71,
      71,    71,    71,    71,    71,    72,    72,    73,    73,    73,
      73,    73,    73,    73,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    75,    75,    75,    75,
      76,    76,    76,    76,    77,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    79,    79,    79,    79,    79,    79,    80,    80,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    82,    82,    83,    83
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     3,     6,     7,
       8,     9,     6,     5,     5,     2,     3,     2,     1,     0,
       2,     1,     1,     1,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     2,     2,     3,     1,     2,     3,     3,
       5,     7,     6,     4,     5,     5,     5,     7,     6,     8,
       5,     5,     1,     1,     2,     0,     1,     7,    10,     1,
       2,     4,     6,     1,     1,     1,     0,     1,     3,     2,
       1,     3,     2,     3,     4,     0,     2,     2,     3,     1,
       3,     4,     7,     4,     1,     2,     7,     3,     3,     4,
       9,     5,     3,     5,     5,     7,     4,     8,    10,    11,
       3,     2,     4,     3,     1,     2,     3,     4,     5,     4,
       1,     1,     2,     1,     2,     2,     1,     3,     2,     4,
       3,     2,     3,     4,     5,     4,     2,     4,     3,     5,
       5,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       1,     1,     1,     1,     3,     4,     4,     5,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     2,     2,
       2,     2,     4,     5,     2,     4,     5,     5,     6,     5,
       4,     3,     3,     0,     1,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* programa: %empty  */
#line 75 "parser.y"
    {
        (yyval.nodo) = crearNodo(nodoPrograma, NULL, NULL, NULL);
        raizAST = (yyval.nodo);
    }
#line 1912 "parser.tab.c"
    break;

  case 3: /* programa: programa funcion  */
#line 79 "parser.y"
                       {
        (yyval.nodo) = crearNodo(nodoPrograma, (yyvsp[-1].nodo), (yyvsp[0].nodo), "programa");
        raizAST = (yyval.nodo);
    }
#line 1921 "parser.tab.c"
    break;

  case 4: /* programa: programa declaracionVariable  */
#line 83 "parser.y"
                                   {
        (yyval.nodo) = crearNodo(nodoPrograma, (yyvsp[-1].nodo), (yyvsp[0].nodo), "programa");
        raizAST = (yyval.nodo);
    }
#line 1930 "parser.tab.c"
    break;

  case 5: /* programa: programa '}'  */
#line 88 "parser.y"
                   {
        yyerror("sintaxis inválida: '}' extra o fuera de cualquier función");
        yyerrok;
        (yyval.nodo) = (yyvsp[-1].nodo); // Conserva lo procesado hasta el momento
        raizAST = (yyval.nodo);
    }
#line 1941 "parser.tab.c"
    break;

  case 6: /* programa: programa ';'  */
#line 95 "parser.y"
                   {
        (yyval.nodo) = (yyvsp[-1].nodo);
        raizAST = (yyval.nodo);
    }
#line 1950 "parser.tab.c"
    break;

  case 7: /* programa: programa error ';'  */
#line 100 "parser.y"
                         {
        yyerrok;
        (yyval.nodo) = (yyvsp[-2].nodo);
        raizAST = (yyval.nodo);
    }
#line 1960 "parser.tab.c"
    break;

  case 8: /* funcion: especificadores TokenID '(' listaParametros ')' ';'  */
#line 108 "parser.y"
                                                        {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, (yyvsp[-5].nodo), (yyvsp[-2].nodo), (yyvsp[-4].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoFuncion, cabecera, NULL, (yyvsp[-4].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 1970 "parser.tab.c"
    break;

  case 9: /* funcion: especificadores TokenOpMultiplicativos TokenID '(' listaParametros ')' ';'  */
#line 113 "parser.y"
                                                                                 {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, (yyvsp[-6].nodo), (yyvsp[-2].nodo), (yyvsp[-4].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoFuncion, cabecera, NULL, (yyvsp[-4].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 1980 "parser.tab.c"
    break;

  case 10: /* funcion: especificadores TokenID '(' listaParametros ')' '{' bloqueCodigo '}'  */
#line 118 "parser.y"
                                                                           {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, (yyvsp[-7].nodo), (yyvsp[-4].nodo), (yyvsp[-6].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoFuncion, cabecera, (yyvsp[-1].nodo), (yyvsp[-6].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 1990 "parser.tab.c"
    break;

  case 11: /* funcion: especificadores TokenOpMultiplicativos TokenID '(' listaParametros ')' '{' bloqueCodigo '}'  */
#line 123 "parser.y"
                                                                                                  {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, (yyvsp[-8].nodo), (yyvsp[-4].nodo), (yyvsp[-6].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoFuncion, cabecera, (yyvsp[-1].nodo), (yyvsp[-6].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 2000 "parser.tab.c"
    break;

  case 12: /* funcion: especificadores TokenID '(' listaParametros ')' sentenciaBloque  */
#line 128 "parser.y"
                                                                      {
        yyerror("se esperaba '{' al inicio del cuerpo de la función");
        yyerrok;
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, (yyvsp[-5].nodo), (yyvsp[-2].nodo), (yyvsp[-4].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoFuncion, cabecera, (yyvsp[0].nodo), (yyvsp[-4].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 2012 "parser.tab.c"
    break;

  case 13: /* funcion: especificadores TokenID '(' listaParametros '{'  */
#line 135 "parser.y"
                                                      {
        yyerror("falta ')' en la declaración de la función");
        YYABORT; // Bloquea errores en cascada
    }
#line 2021 "parser.tab.c"
    break;

  case 14: /* funcion: especificadores TokenID '(' error '}'  */
#line 139 "parser.y"
                                            {
        yyerror("error de sintaxis en la firma de la función");
        YYABORT;
    }
#line 2030 "parser.tab.c"
    break;

  case 15: /* retornar: TokenReturn ';'  */
#line 146 "parser.y"
                    {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, "return");
        raizAST = (yyval.nodo);
    }
#line 2039 "parser.tab.c"
    break;

  case 16: /* retornar: TokenReturn expresion ';'  */
#line 150 "parser.y"
                                {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-1].nodo), NULL, "return");
        raizAST = (yyval.nodo);
    }
#line 2048 "parser.tab.c"
    break;

  case 17: /* retornar: TokenReturn expresion  */
#line 154 "parser.y"
                            {  // Sin punto y coma al final de línea
        yyerror("falta ';' después de return");
        yyerrok;
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[0].nodo), NULL, "return");
        raizAST = (yyval.nodo);
    }
#line 2059 "parser.tab.c"
    break;

  case 18: /* retornar: TokenReturn  */
#line 160 "parser.y"
                  { // Return vacío sin punto y coma
        yyerror("falta ';' después de return");
        yyerrok;
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, "return");
        raizAST = (yyval.nodo);
    }
#line 2070 "parser.tab.c"
    break;

  case 19: /* bloqueCodigo: %empty  */
#line 170 "parser.y"
    {
        (yyval.nodo) = NULL;
    }
#line 2078 "parser.tab.c"
    break;

  case 20: /* bloqueCodigo: bloqueCodigo sentenciaBloque  */
#line 174 "parser.y"
    {
        if ((yyvsp[-1].nodo) == NULL) {
            (yyval.nodo) = (yyvsp[0].nodo);
        } else {
            (yyval.nodo) = crearNodo(nodoBloqueCodigo, (yyvsp[-1].nodo), (yyvsp[0].nodo), "bloque");
        }
        raizAST = (yyval.nodo);
    }
#line 2091 "parser.tab.c"
    break;

  case 21: /* sentenciaBloque: declaracionVariable  */
#line 185 "parser.y"
                          { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 2097 "parser.tab.c"
    break;

  case 22: /* sentenciaBloque: sentenciaSimple  */
#line 186 "parser.y"
                          { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 2103 "parser.tab.c"
    break;

  case 23: /* sentenciaBloque: funcion  */
#line 187 "parser.y"
              {
        yyerror("se esperaba '}' al final de la función anterior");
        yyerrok;
        (yyval.nodo) = (yyvsp[0].nodo);
        raizAST = (yyval.nodo);
    }
#line 2114 "parser.tab.c"
    break;

  case 24: /* sentenciaBloque: error ';'  */
#line 193 "parser.y"
                {
        yyerrok;
        (yyval.nodo) = NULL;
    }
#line 2123 "parser.tab.c"
    break;

  case 25: /* sentenciaBloque: error '}'  */
#line 197 "parser.y"
                {
        yyerrok;
        (yyval.nodo) = NULL;
    }
#line 2132 "parser.tab.c"
    break;

  case 26: /* sentenciaSimple: expresion ';'  */
#line 206 "parser.y"
                           { (yyval.nodo) = (yyvsp[-1].nodo); raizAST = (yyval.nodo); }
#line 2138 "parser.tab.c"
    break;

  case 27: /* sentenciaSimple: condicionIf  */
#line 207 "parser.y"
                           { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 2144 "parser.tab.c"
    break;

  case 28: /* sentenciaSimple: caseSwitch  */
#line 208 "parser.y"
                           { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 2150 "parser.tab.c"
    break;

  case 29: /* sentenciaSimple: loopWhile  */
#line 209 "parser.y"
                           { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 2156 "parser.tab.c"
    break;

  case 30: /* sentenciaSimple: loopFor  */
#line 210 "parser.y"
                           { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 2162 "parser.tab.c"
    break;

  case 31: /* sentenciaSimple: loopDoWhile  */
#line 211 "parser.y"
                           { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 2168 "parser.tab.c"
    break;

  case 32: /* sentenciaSimple: retornar  */
#line 212 "parser.y"
                           { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 2174 "parser.tab.c"
    break;

  case 33: /* sentenciaSimple: TokenBreak ';'  */
#line 213 "parser.y"
                     {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, "break");
        raizAST = (yyval.nodo);
    }
#line 2183 "parser.tab.c"
    break;

  case 34: /* sentenciaSimple: TokenContinue ';'  */
#line 217 "parser.y"
                        {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, "continue");
        raizAST = (yyval.nodo);
    }
#line 2192 "parser.tab.c"
    break;

  case 35: /* sentenciaSimple: '{' bloqueCodigo '}'  */
#line 221 "parser.y"
                           { (yyval.nodo) = (yyvsp[-1].nodo); raizAST = (yyval.nodo); }
#line 2198 "parser.tab.c"
    break;

  case 36: /* sentenciaSimple: ';'  */
#line 222 "parser.y"
                           { (yyval.nodo) = NULL; }
#line 2204 "parser.tab.c"
    break;

  case 37: /* sentenciaSimple: TokenElse sentenciaSimple  */
#line 223 "parser.y"
                                {
        yyerror("'else' sin un 'if' previo");
        yyerrok;
        (yyval.nodo) = (yyvsp[0].nodo);
        raizAST = (yyval.nodo);
    }
#line 2215 "parser.tab.c"
    break;

  case 38: /* sentenciaSimple: TokenGoto TokenID ';'  */
#line 229 "parser.y"
                            {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, "goto");
        raizAST = (yyval.nodo);
    }
#line 2224 "parser.tab.c"
    break;

  case 39: /* sentenciaSimple: TokenID ':' sentenciaSimple  */
#line 233 "parser.y"
                                  {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-2].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, idNodo, (yyvsp[0].nodo), "etiqueta");
        raizAST = (yyval.nodo);
    }
#line 2234 "parser.tab.c"
    break;

  case 40: /* condicionIf: TokenIf '(' expresion ')' sentenciaSimple  */
#line 241 "parser.y"
                                              {
        (yyval.nodo) = crearNodo(nodoCondicionIf, (yyvsp[-2].nodo), (yyvsp[0].nodo), "if");
        raizAST = (yyval.nodo);
    }
#line 2243 "parser.tab.c"
    break;

  case 41: /* condicionIf: TokenIf '(' expresion ')' sentenciaSimple TokenElse sentenciaSimple  */
#line 245 "parser.y"
                                                                          {
        struct ASTNodo *ifParcial = crearNodo(nodoCondicionIf, (yyvsp[-4].nodo), (yyvsp[-2].nodo), "if");
        (yyval.nodo) = crearNodo(nodoCondicionIf, ifParcial, (yyvsp[0].nodo), "if-else");
        raizAST = (yyval.nodo);
    }
#line 2253 "parser.tab.c"
    break;

  case 42: /* condicionIf: TokenIf '(' expresion '{' bloqueCodigo '}'  */
#line 250 "parser.y"
                                                 {
        yyerror("se esperaba ')' antes del token '{'");
        yyerrok;
        (yyval.nodo) = crearNodo(nodoCondicionIf, (yyvsp[-3].nodo), (yyvsp[-1].nodo), "if-error");
        raizAST = (yyval.nodo);
    }
#line 2264 "parser.tab.c"
    break;

  case 43: /* condicionIf: TokenIf '(' expresion sentenciaSimple  */
#line 256 "parser.y"
                                            {
        yyerror("se esperaba ')' después de la condición del 'if'");
        yyerrok;
        (yyval.nodo) = crearNodo(nodoCondicionIf, (yyvsp[-1].nodo), (yyvsp[0].nodo), "if-error");
        raizAST = (yyval.nodo);
    }
#line 2275 "parser.tab.c"
    break;

  case 44: /* condicionIf: TokenIf error '{' bloqueCodigo '}'  */
#line 262 "parser.y"
                                         {
        yyerrok;
        (yyval.nodo) = crearNodo(nodoCondicionIf, NULL, (yyvsp[-1].nodo), "if-error");
        raizAST = (yyval.nodo);
    }
#line 2285 "parser.tab.c"
    break;

  case 45: /* loopWhile: TokenWhile '(' expresion ')' sentenciaSimple  */
#line 270 "parser.y"
                                                 {
        (yyval.nodo) = crearNodo(nodoLoopWhile, (yyvsp[-2].nodo), (yyvsp[0].nodo), "while");
        raizAST = (yyval.nodo);
    }
#line 2294 "parser.tab.c"
    break;

  case 46: /* loopWhile: TokenWhile error '{' bloqueCodigo '}'  */
#line 274 "parser.y"
                                            {
        yyerrok;
        (yyval.nodo) = crearNodo(nodoLoopWhile, NULL, (yyvsp[-1].nodo), "while-error");
        raizAST = (yyval.nodo);
    }
#line 2304 "parser.tab.c"
    break;

  case 47: /* loopDoWhile: TokenDo sentenciaSimple TokenWhile '(' expresion ')' ';'  */
#line 282 "parser.y"
                                                             {
        (yyval.nodo) = crearNodo(nodoLoopDoWhile, (yyvsp[-5].nodo), (yyvsp[-2].nodo), "do-while");
        raizAST = (yyval.nodo);
    }
#line 2313 "parser.tab.c"
    break;

  case 48: /* loopDoWhile: TokenDo sentenciaSimple TokenWhile '(' expresion ')'  */
#line 286 "parser.y"
                                                           {
        yyerror("falta ';' al final del do-while");
        yyerrok;
        (yyval.nodo) = crearNodo(nodoLoopDoWhile, (yyvsp[-4].nodo), (yyvsp[-1].nodo), "do-while");
        raizAST = (yyval.nodo);
    }
#line 2324 "parser.tab.c"
    break;

  case 49: /* loopFor: TokenFor '(' inicializacionFor condicional ';' condicional ')' sentenciaSimple  */
#line 295 "parser.y"
                                                                                   {
        struct ASTNodo *condInc = crearNodo(nodoLoopFor, (yyvsp[-4].nodo), (yyvsp[-2].nodo), "for-cond-inc");
        struct ASTNodo *cabeceraFor = crearNodo(nodoLoopFor, (yyvsp[-5].nodo), condInc, "for-cabecera");
        (yyval.nodo) = crearNodo(nodoLoopFor, cabeceraFor, (yyvsp[0].nodo), "for");
        raizAST = (yyval.nodo);
    }
#line 2335 "parser.tab.c"
    break;

  case 50: /* loopFor: TokenFor '(' error ')' sentenciaSimple  */
#line 302 "parser.y"
                                             {
        yyerrok;
        (yyval.nodo) = crearNodo(nodoLoopFor, NULL, (yyvsp[0].nodo), "for-error");
        raizAST = (yyval.nodo);
    }
#line 2345 "parser.tab.c"
    break;

  case 51: /* loopFor: TokenFor error '{' bloqueCodigo '}'  */
#line 308 "parser.y"
                                          {
        yyerrok;
        (yyval.nodo) = crearNodo(nodoLoopFor, NULL, (yyvsp[-1].nodo), "for-error");
        raizAST = (yyval.nodo);
    }
#line 2355 "parser.tab.c"
    break;

  case 52: /* inicializacionFor: ';'  */
#line 316 "parser.y"
        { (yyval.nodo) = NULL; raizAST = (yyval.nodo); }
#line 2361 "parser.tab.c"
    break;

  case 53: /* inicializacionFor: declaracionVariable  */
#line 317 "parser.y"
                          { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 2367 "parser.tab.c"
    break;

  case 54: /* inicializacionFor: expresion ';'  */
#line 318 "parser.y"
                    { (yyval.nodo) = (yyvsp[-1].nodo); raizAST = (yyval.nodo); }
#line 2373 "parser.tab.c"
    break;

  case 55: /* condicional: %empty  */
#line 322 "parser.y"
    { (yyval.nodo) = NULL; raizAST = (yyval.nodo); }
#line 2379 "parser.tab.c"
    break;

  case 56: /* condicional: expresion  */
#line 323 "parser.y"
                { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 2385 "parser.tab.c"
    break;

  case 57: /* caseSwitch: TokenSwitch '(' expresion ')' '{' listaCasos '}'  */
#line 327 "parser.y"
                                                     {
        (yyval.nodo) = crearNodo(nodoCaseSwitch, (yyvsp[-4].nodo), (yyvsp[-1].nodo), "switch");
        raizAST = (yyval.nodo);
    }
#line 2394 "parser.tab.c"
    break;

  case 58: /* caseSwitch: TokenSwitch '(' expresion ')' '{' listaCasos TokenDefault ':' bloqueCodigo '}'  */
#line 331 "parser.y"
                                                                                     {
        struct ASTNodo *nodoDef = crearNodo(nodoCaso, NULL, (yyvsp[-1].nodo), "default");
        struct ASTNodo *listaCompleta = crearNodo(nodoListaCasos, (yyvsp[-4].nodo), nodoDef, "lista-casos");
        (yyval.nodo) = crearNodo(nodoCaseSwitch, (yyvsp[-7].nodo), listaCompleta, "switch-default");
        raizAST = (yyval.nodo);
    }
#line 2405 "parser.tab.c"
    break;

  case 59: /* listaCasos: caso  */
#line 340 "parser.y"
         { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo);}
#line 2411 "parser.tab.c"
    break;

  case 60: /* listaCasos: listaCasos caso  */
#line 341 "parser.y"
                      {
        (yyval.nodo) = crearNodo(nodoListaCasos, (yyvsp[-1].nodo), (yyvsp[0].nodo), "lista-casos");
        raizAST = (yyval.nodo);
    }
#line 2420 "parser.tab.c"
    break;

  case 61: /* caso: TokenCase elementoCase ':' bloqueCodigo  */
#line 348 "parser.y"
                                            {
        (yyval.nodo) = crearNodo(nodoCaso, (yyvsp[-2].nodo), (yyvsp[0].nodo), "case");
        raizAST = (yyval.nodo);
    }
#line 2429 "parser.tab.c"
    break;

  case 62: /* caso: TokenCase elementoCase ':' bloqueCodigo TokenBreak ';'  */
#line 352 "parser.y"
                                                             {
        struct ASTNodo *nodoBreak = crearNodo(nodoExpresion, NULL, NULL, "break");
        struct ASTNodo *cuerpoConBreak = crearNodo(nodoBloqueCodigo, (yyvsp[-2].nodo), nodoBreak, "bloque-break");
        (yyval.nodo) = crearNodo(nodoCaso, (yyvsp[-4].nodo), cuerpoConBreak, "case-break");
        raizAST = (yyval.nodo);
    }
#line 2440 "parser.tab.c"
    break;

  case 63: /* elementoCase: TokenInt  */
#line 361 "parser.y"
             {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, NULL);
        (yyval.nodo)->valorNumerico = (yyvsp[0].tokenInfo).valor;
        raizAST = (yyval.nodo);
    }
#line 2450 "parser.tab.c"
    break;

  case 64: /* elementoCase: TokenChar  */
#line 366 "parser.y"
                {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 2459 "parser.tab.c"
    break;

  case 65: /* elementoCase: TokenID  */
#line 370 "parser.y"
              {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 2468 "parser.tab.c"
    break;

  case 66: /* listaParametros: %empty  */
#line 377 "parser.y"
    { (yyval.nodo) = NULL; raizAST = (yyval.nodo); }
#line 2474 "parser.tab.c"
    break;

  case 67: /* listaParametros: parametro  */
#line 378 "parser.y"
                { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 2480 "parser.tab.c"
    break;

  case 68: /* listaParametros: listaParametros ',' parametro  */
#line 379 "parser.y"
                                    {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), "lista-parametros");
        raizAST = (yyval.nodo);
    }
#line 2489 "parser.tab.c"
    break;

  case 69: /* parametro: especificadores TokenID  */
#line 386 "parser.y"
                              {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-1].nodo), NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 2498 "parser.tab.c"
    break;

  case 70: /* parametro: especificadores  */
#line 390 "parser.y"
                      {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[0].nodo), NULL, "parametro-tipo");
        raizAST = (yyval.nodo);
    }
#line 2507 "parser.tab.c"
    break;

  case 71: /* parametro: especificadores TokenOpMultiplicativos TokenID  */
#line 394 "parser.y"
                                                     {
        if (strcmp((yyvsp[-1].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), ptrNodo, "parametro-puntero");
        } else {
            yyerror("se esperaba '*' en la declaración del parámetro");
            (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), NULL, (yyvsp[0].tokenInfo).lexema);
        }
        raizAST = (yyval.nodo);
    }
#line 2523 "parser.tab.c"
    break;

  case 72: /* parametro: especificadores TokenOpMultiplicativos  */
#line 405 "parser.y"
                                             {
        if (strcmp((yyvsp[0].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *ptrNodo = crearNodoUnario("*", NULL);
            (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-1].nodo), ptrNodo, "parametro-puntero-anon");
        } else {
            yyerror("se esperaba '*' en la declaración del parámetro");
            (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-1].nodo), NULL, "parametro-error");
        }
        raizAST = (yyval.nodo);
    }
#line 2538 "parser.tab.c"
    break;

  case 73: /* parametro: especificadores TokenID listaDimensiones  */
#line 415 "parser.y"
                                               {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-1].tokenInfo).lexema);
        struct ASTNodo *arrNodo = crearNodo(nodoExpresion, idNodo, (yyvsp[0].nodo), "parametro-array");
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), arrNodo, "parametro");
        raizAST = (yyval.nodo);
    }
#line 2549 "parser.tab.c"
    break;

  case 74: /* parametro: especificadores TokenOpMultiplicativos TokenID listaDimensiones  */
#line 421 "parser.y"
                                                                      {
        if (strcmp((yyvsp[-2].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-1].tokenInfo).lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            struct ASTNodo *arrNodo = crearNodo(nodoExpresion, ptrNodo, (yyvsp[0].nodo), "parametro-array");
            (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-3].nodo), arrNodo, "parametro-puntero");
        } else {
            yyerror("se esperaba '*' en la declaración del parámetro");
            (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-3].nodo), NULL, (yyvsp[-1].tokenInfo).lexema);
        }
        raizAST = (yyval.nodo);
    }
#line 2566 "parser.tab.c"
    break;

  case 75: /* listaDeclaracionVariables: %empty  */
#line 436 "parser.y"
    { (yyval.nodo) = NULL; raizAST = (yyval.nodo);}
#line 2572 "parser.tab.c"
    break;

  case 76: /* listaDeclaracionVariables: listaDeclaracionVariables declaracionVariable  */
#line 437 "parser.y"
                                                    {
        (yyval.nodo) = crearNodo(nodoBloqueCodigo, (yyvsp[-1].nodo), (yyvsp[0].nodo), "lista-declaraciones");
        raizAST = (yyval.nodo);
    }
#line 2581 "parser.tab.c"
    break;

  case 77: /* declaracionVariable: especificadores ';'  */
#line 444 "parser.y"
                        {
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-1].nodo), NULL, "def-tipo");
        raizAST = (yyval.nodo);
    }
#line 2590 "parser.tab.c"
    break;

  case 78: /* declaracionVariable: especificadores listaIds ';'  */
#line 449 "parser.y"
                                   {
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-2].nodo), (yyvsp[-1].nodo), "declaracion-multiple");
        raizAST = (yyval.nodo);
    }
#line 2599 "parser.tab.c"
    break;

  case 79: /* declaracionVariable: declaracionArrays  */
#line 453 "parser.y"
                        {
        (yyval.nodo) = (yyvsp[0].nodo);
        raizAST = (yyval.nodo);
    }
#line 2608 "parser.tab.c"
    break;

  case 80: /* declaracionVariable: especificadores listaIds error  */
#line 457 "parser.y"
                                     {
        yyclearin;
        yyerrok;
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-2].nodo), (yyvsp[-1].nodo), "declaracion-multiple");
        raizAST = (yyval.nodo);
    }
#line 2619 "parser.tab.c"
    break;

  case 81: /* declaracionVariable: TokenTypedef especificadores listaIds ';'  */
#line 463 "parser.y"
                                                {
        registrarTipoDeclarado((yyvsp[-1].nodo));
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-2].nodo), (yyvsp[-1].nodo), "typedef");
        raizAST = (yyval.nodo);
    }
#line 2629 "parser.tab.c"
    break;

  case 82: /* declaracionVariable: especificadores TokenID '=' '{' listaElementos '}' ';'  */
#line 468 "parser.y"
                                                             {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-5].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, idNodo, (yyvsp[-2].nodo), "=");
        raizAST = (yyval.nodo);
    }
#line 2639 "parser.tab.c"
    break;

  case 83: /* declaracionVariable: especificadores ':' expresion ';'  */
#line 473 "parser.y"
                                        {
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-3].nodo), (yyvsp[-1].nodo), "bitfield-anon");
        raizAST = (yyval.nodo);
    }
#line 2648 "parser.tab.c"
    break;

  case 84: /* listaIds: TokenID  */
#line 480 "parser.y"
              {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, idNodo, NULL, "var-simple");
        raizAST = (yyval.nodo);
    }
#line 2658 "parser.tab.c"
    break;

  case 85: /* listaIds: TokenOpMultiplicativos listaIds  */
#line 485 "parser.y"
                                      {
        if (strcmp((yyvsp[-1].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *nodoPtr = crearNodoUnario("*", (yyvsp[0].nodo));
            (yyval.nodo) = crearNodo(nodoDeclaracionVariable, nodoPtr, NULL, "var-puntero");
        } else {
            yyerror("se esperaba '*' en la declaración de puntero");
            (yyval.nodo) = (yyvsp[0].nodo);
        }
        raizAST = (yyval.nodo);
    }
#line 2673 "parser.tab.c"
    break;

  case 86: /* listaIds: '(' TokenOpMultiplicativos TokenID ')' '(' listaParametros ')'  */
#line 495 "parser.y"
                                                                     {
        if (strcmp((yyvsp[-5].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-4].tokenInfo).lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            (yyval.nodo) = crearNodo(nodoDeclaracionVariable, ptrNodo, (yyvsp[-1].nodo), "puntero-funcion");
        } else {
            yyerror("se esperaba '*' en la declaración del puntero a función");
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-4].tokenInfo).lexema);
            (yyval.nodo) = crearNodo(nodoDeclaracionVariable, idNodo, (yyvsp[-1].nodo), "puntero-funcion");
        }
        raizAST = (yyval.nodo);
    }
#line 2690 "parser.tab.c"
    break;

  case 87: /* listaIds: TokenID '=' expresion  */
#line 507 "parser.y"
                            {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-2].tokenInfo).lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, (yyvsp[0].nodo), "=");
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init");
        raizAST = (yyval.nodo);
    }
#line 2701 "parser.tab.c"
    break;

  case 88: /* listaIds: listaIds ',' TokenID  */
#line 513 "parser.y"
                           {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, idNodo, NULL, "var-simple");
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-2].nodo), nuevaVar, "lista-ids");
        raizAST = (yyval.nodo);
    }
#line 2712 "parser.tab.c"
    break;

  case 89: /* listaIds: listaIds ',' TokenOpMultiplicativos TokenID  */
#line 519 "parser.y"
                                                  {
        if (strcmp((yyvsp[-1].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, ptrNodo, NULL, "var-puntero");
            (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-3].nodo), nuevaVar, "lista-ids");
        } else {
            yyerror("se esperaba '*' en la declaración de puntero");
            (yyval.nodo) = (yyvsp[-3].nodo);
        }
        raizAST = (yyval.nodo);
    }
#line 2729 "parser.tab.c"
    break;

  case 90: /* listaIds: listaIds ',' '(' TokenOpMultiplicativos TokenID ')' '(' listaParametros ')'  */
#line 531 "parser.y"
                                                                                  {
        if (strcmp((yyvsp[-5].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-4].tokenInfo).lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, ptrNodo, (yyvsp[-1].nodo), "puntero-funcion");
            (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-8].nodo), nuevaVar, "lista-ids");
        } else {
            yyerror("se esperaba '*' en la declaración del puntero a función");
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-4].tokenInfo).lexema);
            struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, idNodo, (yyvsp[-1].nodo), "puntero-funcion");
            (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-8].nodo), nuevaVar, "lista-ids");
        }
        raizAST = (yyval.nodo);
    }
#line 2748 "parser.tab.c"
    break;

  case 91: /* listaIds: listaIds ',' TokenID '=' expresion  */
#line 545 "parser.y"
                                         {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-2].tokenInfo).lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, (yyvsp[0].nodo), "=");
        struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init");
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-4].nodo), nuevaVar, "lista-ids");
        raizAST = (yyval.nodo);
    }
#line 2760 "parser.tab.c"
    break;

  case 92: /* listaIds: TokenID ':' expresion  */
#line 553 "parser.y"
                            {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-2].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, idNodo, (yyvsp[0].nodo), "bitfield");
        raizAST = (yyval.nodo);
    }
#line 2770 "parser.tab.c"
    break;

  case 93: /* listaIds: listaIds ',' TokenID ':' expresion  */
#line 558 "parser.y"
                                         {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-2].tokenInfo).lexema);
        struct ASTNodo *nuevoBitfield = crearNodo(nodoDeclaracionVariable, idNodo, (yyvsp[0].nodo), "bitfield");
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-4].nodo), nuevoBitfield, "lista-ids");
        raizAST = (yyval.nodo);
    }
#line 2781 "parser.tab.c"
    break;

  case 94: /* listaIds: TokenID '=' '{' listaElementos '}'  */
#line 564 "parser.y"
                                         {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-4].tokenInfo).lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, (yyvsp[-1].nodo), "=");
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init-bloque");
        raizAST = (yyval.nodo);
    }
#line 2792 "parser.tab.c"
    break;

  case 95: /* listaIds: listaIds ',' TokenID '=' '{' listaElementos '}'  */
#line 570 "parser.y"
                                                      {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-4].tokenInfo).lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, (yyvsp[-1].nodo), "=");
        struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init-bloque");
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, (yyvsp[-6].nodo), nuevaVar, "lista-ids");
        raizAST = (yyval.nodo);
    }
#line 2804 "parser.tab.c"
    break;

  case 96: /* declaracionArrays: especificadores TokenID listaDimensiones ';'  */
#line 580 "parser.y"
                                                 {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, (yyvsp[-3].nodo), (yyvsp[-1].nodo), (yyvsp[-2].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, cabecera, NULL, "matriz");
        raizAST = (yyval.nodo);
    }
#line 2814 "parser.tab.c"
    break;

  case 97: /* declaracionArrays: especificadores TokenID listaDimensiones '=' '{' listaElementos '}' ';'  */
#line 585 "parser.y"
                                                                              {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, (yyvsp[-7].nodo), (yyvsp[-5].nodo), (yyvsp[-6].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoDeclaracionVariable, cabecera, (yyvsp[-2].nodo), "matriz-init");
        raizAST = (yyval.nodo);
    }
#line 2824 "parser.tab.c"
    break;

  case 98: /* declaracionArrays: especificadores TokenOpMultiplicativos TokenID '[' ']' '=' '{' listaElementos '}' ';'  */
#line 590 "parser.y"
                                                                                            {
        if (strcmp((yyvsp[-8].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-7].tokenInfo).lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            (yyval.nodo) = crearNodo(nodoDeclaracionVariable, ptrNodo, (yyvsp[-2].nodo), "arr-puntero-init");
        } else {
            yyerror("se esperaba '*' en la declaración del arreglo de punteros");
            (yyval.nodo) = NULL;
        }
        raizAST = (yyval.nodo);
    }
#line 2840 "parser.tab.c"
    break;

  case 99: /* declaracionArrays: especificadores TokenOpMultiplicativos TokenID '[' TokenInt ']' '=' '{' listaElementos '}' ';'  */
#line 601 "parser.y"
                                                                                                     {
        if (strcmp((yyvsp[-9].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-8].tokenInfo).lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            (yyval.nodo) = crearNodo(nodoDeclaracionVariable, ptrNodo, (yyvsp[-2].nodo), "arr-puntero-init");
        } else {
            yyerror("se esperaba '*' en la declaración del arreglo de punteros");
            (yyval.nodo) = NULL;
        }
        raizAST = (yyval.nodo);
    }
#line 2856 "parser.tab.c"
    break;

  case 100: /* listaDimensiones: '[' TokenInt ']'  */
#line 615 "parser.y"
                     {
        struct ASTNodo *tam = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-1].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, tam, "dimension");
    }
#line 2865 "parser.tab.c"
    break;

  case 101: /* listaDimensiones: '[' ']'  */
#line 619 "parser.y"
              {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, "dimension");
    }
#line 2873 "parser.tab.c"
    break;

  case 102: /* listaDimensiones: listaDimensiones '[' TokenInt ']'  */
#line 622 "parser.y"
                                        {
        struct ASTNodo *tam = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-1].tokenInfo).lexema);
        struct ASTNodo *nuevaDim = crearNodo(nodoExpresion, NULL, tam, "dimension");
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-3].nodo), nuevaDim, "lista-dimensiones");
    }
#line 2883 "parser.tab.c"
    break;

  case 103: /* listaDimensiones: listaDimensiones '[' ']'  */
#line 627 "parser.y"
                               {
        struct ASTNodo *nuevaDim = crearNodo(nodoExpresion, NULL, NULL, "dimension");
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), nuevaDim, "lista-dimensiones");
    }
#line 2892 "parser.tab.c"
    break;

  case 104: /* especificadores: tipoCast  */
#line 634 "parser.y"
             { (yyval.nodo) = (yyvsp[0].nodo); }
#line 2898 "parser.tab.c"
    break;

  case 105: /* tipoCast: TokenUnion TokenID  */
#line 638 "parser.y"
                       {
        struct ASTNodo *unNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, unNodo, NULL, "union");
        raizAST = (yyval.nodo);
    }
#line 2908 "parser.tab.c"
    break;

  case 106: /* tipoCast: TokenModificador TokenUnion TokenID  */
#line 643 "parser.y"
                                          {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-2].tokenInfo).lexema);
        struct ASTNodo *unNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        struct ASTNodo *unionNodo = crearNodo(nodoExpresion, unNodo, NULL, "union");
        (yyval.nodo) = crearNodo(nodoExpresion, mod, unionNodo, "mod-union");
        raizAST = (yyval.nodo);
    }
#line 2920 "parser.tab.c"
    break;

  case 107: /* tipoCast: TokenModificador TokenModificador TokenUnion TokenID  */
#line 650 "parser.y"
                                                           {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-3].tokenInfo).lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, (yyvsp[-2].tokenInfo).lexema);
        struct ASTNodo *unNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        struct ASTNodo *unionNodo = crearNodo(nodoExpresion, unNodo, NULL, "union");
        (yyval.nodo) = crearNodo(nodoExpresion, mod2, unionNodo, "mod-union");
        raizAST = (yyval.nodo);
    }
#line 2933 "parser.tab.c"
    break;

  case 108: /* tipoCast: TokenUnion TokenID '{' listaDeclaracionVariables '}'  */
#line 658 "parser.y"
                                                           {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-3].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, idNodo, (yyvsp[-1].nodo), "def-union");
        raizAST = (yyval.nodo);
    }
#line 2943 "parser.tab.c"
    break;

  case 109: /* tipoCast: TokenUnion '{' listaDeclaracionVariables '}'  */
#line 663 "parser.y"
                                                   {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, (yyvsp[-1].nodo), "def-union-anon");
        raizAST = (yyval.nodo);
    }
#line 2952 "parser.tab.c"
    break;

  case 110: /* tipoCast: TokenTipoDato  */
#line 667 "parser.y"
                    {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 2961 "parser.tab.c"
    break;

  case 111: /* tipoCast: TokenNombreTipo  */
#line 671 "parser.y"
                      {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 2970 "parser.tab.c"
    break;

  case 112: /* tipoCast: TokenModificador TokenNombreTipo  */
#line 675 "parser.y"
                                       {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-1].tokenInfo).lexema);
        struct ASTNodo *tipoNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, mod, tipoNodo, "mod-tipo");
        raizAST = (yyval.nodo);
    }
#line 2981 "parser.tab.c"
    break;

  case 113: /* tipoCast: TokenVoid  */
#line 681 "parser.y"
                {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 2990 "parser.tab.c"
    break;

  case 114: /* tipoCast: TokenModificador TokenTipoDato  */
#line 685 "parser.y"
                                     {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-1].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, mod, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3000 "parser.tab.c"
    break;

  case 115: /* tipoCast: TokenModificador TokenVoid  */
#line 690 "parser.y"
                                 {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-1].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, mod, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3010 "parser.tab.c"
    break;

  case 116: /* tipoCast: TokenModificador  */
#line 695 "parser.y"
                       {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3019 "parser.tab.c"
    break;

  case 117: /* tipoCast: TokenModificador TokenModificador TokenTipoDato  */
#line 699 "parser.y"
                                                      {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-2].tokenInfo).lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, (yyvsp[-1].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, mod2, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3030 "parser.tab.c"
    break;

  case 118: /* tipoCast: TokenModificador TokenModificador  */
#line 705 "parser.y"
                                        {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-1].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, mod1, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3040 "parser.tab.c"
    break;

  case 119: /* tipoCast: TokenModificador TokenModificador TokenModificador TokenTipoDato  */
#line 710 "parser.y"
                                                                       {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-3].tokenInfo).lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, (yyvsp[-2].tokenInfo).lexema);
        struct ASTNodo *mod3 = crearNodo(nodoExpresion, mod2, NULL, (yyvsp[-1].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, mod3, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3052 "parser.tab.c"
    break;

  case 120: /* tipoCast: TokenModificador TokenModificador TokenModificador  */
#line 717 "parser.y"
                                                         {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-2].tokenInfo).lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, (yyvsp[-1].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, mod2, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3063 "parser.tab.c"
    break;

  case 121: /* tipoCast: TokenStruct TokenID  */
#line 724 "parser.y"
                          {
        struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, strNodo, NULL, "struct");
        raizAST = (yyval.nodo);
    }
#line 3073 "parser.tab.c"
    break;

  case 122: /* tipoCast: TokenModificador TokenStruct TokenID  */
#line 729 "parser.y"
                                           {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-2].tokenInfo).lexema);
        struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        struct ASTNodo *structNodo = crearNodo(nodoExpresion, strNodo, NULL, "struct");
        (yyval.nodo) = crearNodo(nodoExpresion, mod, structNodo, "mod-struct");
        raizAST = (yyval.nodo);
    }
#line 3085 "parser.tab.c"
    break;

  case 123: /* tipoCast: TokenModificador TokenModificador TokenStruct TokenID  */
#line 736 "parser.y"
                                                            {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-3].tokenInfo).lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, (yyvsp[-2].tokenInfo).lexema);
        struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        struct ASTNodo *structNodo = crearNodo(nodoExpresion, strNodo, NULL, "struct");
        (yyval.nodo) = crearNodo(nodoExpresion, mod2, structNodo, "mod-struct");
        raizAST = (yyval.nodo);
    }
#line 3098 "parser.tab.c"
    break;

  case 124: /* tipoCast: TokenStruct TokenID '{' listaDeclaracionVariables '}'  */
#line 744 "parser.y"
                                                            {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-3].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, idNodo, (yyvsp[-1].nodo), "def-struct");
        raizAST = (yyval.nodo);
    }
#line 3108 "parser.tab.c"
    break;

  case 125: /* tipoCast: TokenStruct '{' listaDeclaracionVariables '}'  */
#line 749 "parser.y"
                                                    {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, (yyvsp[-1].nodo), "def-struct-anon");
        raizAST = (yyval.nodo);
    }
#line 3117 "parser.tab.c"
    break;

  case 126: /* tipoCast: TokenEnum TokenID  */
#line 754 "parser.y"
                        {
        struct ASTNodo *enNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, enNodo, NULL, "enum");
        raizAST = (yyval.nodo);
    }
#line 3127 "parser.tab.c"
    break;

  case 127: /* tipoCast: TokenEnum '{' listaIds '}'  */
#line 759 "parser.y"
                                 {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, (yyvsp[-1].nodo), "def-enum-anon");
        raizAST = (yyval.nodo);
    }
#line 3136 "parser.tab.c"
    break;

  case 128: /* tipoCast: TokenModificador TokenEnum TokenID  */
#line 763 "parser.y"
                                         {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-2].tokenInfo).lexema);
        struct ASTNodo *enNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        struct ASTNodo *enumNodo = crearNodo(nodoExpresion, enNodo, NULL, "enum");
        (yyval.nodo) = crearNodo(nodoExpresion, mod, enumNodo, "mod-enum");
        raizAST = (yyval.nodo);
    }
#line 3148 "parser.tab.c"
    break;

  case 129: /* tipoCast: TokenEnum TokenID '{' listaIds '}'  */
#line 770 "parser.y"
                                         {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-3].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, idNodo, (yyvsp[-1].nodo), "def-enum");
        raizAST = (yyval.nodo);
    }
#line 3158 "parser.tab.c"
    break;

  case 130: /* tipoCast: TokenModificador TokenEnum '{' listaIds '}'  */
#line 775 "parser.y"
                                                  {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-4].tokenInfo).lexema);
        struct ASTNodo *enumNodo = crearNodo(nodoExpresion, NULL, (yyvsp[-1].nodo), "def-enum-anon");
        (yyval.nodo) = crearNodo(nodoExpresion, mod, enumNodo, "mod-enum");
        raizAST = (yyval.nodo);
    }
#line 3169 "parser.tab.c"
    break;

  case 131: /* elementoInit: TokenInt  */
#line 784 "parser.y"
               {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, NULL);
        (yyval.nodo)->valorNumerico = (yyvsp[0].tokenInfo).valor;
    }
#line 3178 "parser.tab.c"
    break;

  case 132: /* elementoInit: TokenFloat  */
#line 788 "parser.y"
                 {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, NULL);
        (yyval.nodo)->valorNumerico = (yyvsp[0].tokenInfo).valor;
    }
#line 3187 "parser.tab.c"
    break;

  case 133: /* elementoInit: TokenChar  */
#line 792 "parser.y"
                {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
    }
#line 3195 "parser.tab.c"
    break;

  case 134: /* elementoInit: TokenString  */
#line 795 "parser.y"
                  {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
    }
#line 3203 "parser.tab.c"
    break;

  case 135: /* elementoInit: TokenID  */
#line 798 "parser.y"
              {
        /* Permite identificadores como NULL, SIMBOLO_FUNC, etc. */
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
    }
#line 3212 "parser.tab.c"
    break;

  case 136: /* elementoInit: expresion  */
#line 802 "parser.y"
                {
        (yyval.nodo) = (yyvsp[0].nodo);
    }
#line 3220 "parser.tab.c"
    break;

  case 137: /* listaElementos: elementoInit  */
#line 808 "parser.y"
                   {
        (yyval.nodo) = (yyvsp[0].nodo);
    }
#line 3228 "parser.tab.c"
    break;

  case 138: /* listaElementos: listaElementos ',' elementoInit  */
#line 811 "parser.y"
                                      {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), "lista-elementos");
    }
#line 3236 "parser.tab.c"
    break;

  case 139: /* expresion: TokenID  */
#line 853 "parser.y"
            {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3245 "parser.tab.c"
    break;

  case 140: /* expresion: TokenInt  */
#line 857 "parser.y"
               {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        (yyval.nodo)->valorNumerico = (yyvsp[0].tokenInfo).valor;
        raizAST = (yyval.nodo);
    }
#line 3255 "parser.tab.c"
    break;

  case 141: /* expresion: TokenFloat  */
#line 862 "parser.y"
                 {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        (yyval.nodo)->valorNumerico = (yyvsp[0].tokenInfo).valor;
        raizAST = (yyval.nodo);
    }
#line 3265 "parser.tab.c"
    break;

  case 142: /* expresion: TokenChar  */
#line 867 "parser.y"
                {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3274 "parser.tab.c"
    break;

  case 143: /* expresion: TokenString  */
#line 871 "parser.y"
                  {
        (yyval.nodo) = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3283 "parser.tab.c"
    break;

  case 144: /* expresion: '(' expresion ')'  */
#line 875 "parser.y"
                        {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-1].nodo), NULL, "()");
        raizAST = (yyval.nodo);
    }
#line 3292 "parser.tab.c"
    break;

  case 145: /* expresion: expresion '[' expresion ']'  */
#line 879 "parser.y"
                                  {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-3].nodo), (yyvsp[-1].nodo), "indexacion");
        raizAST = (yyval.nodo);
    }
#line 3301 "parser.tab.c"
    break;

  case 146: /* expresion: '(' tipoCast ')' expresion  */
#line 883 "parser.y"
                                              {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), "casteo");
        raizAST = (yyval.nodo);
    }
#line 3310 "parser.tab.c"
    break;

  case 147: /* expresion: '(' tipoCast TokenOpMultiplicativos ')' expresion  */
#line 887 "parser.y"
                                                                     {
        if (strcmp((yyvsp[-2].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *ptrNodo = crearNodoUnario("*", (yyvsp[-3].nodo));
            (yyval.nodo) = crearNodo(nodoExpresion, ptrNodo, (yyvsp[0].nodo), "casteo-puntero");
        } else {
            yyerror("se esperaba '*' en el casteo a puntero");
            (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-3].nodo), (yyvsp[0].nodo), "casteo");
        }
        raizAST = (yyval.nodo);
    }
#line 3325 "parser.tab.c"
    break;

  case 148: /* expresion: expresion '=' expresion  */
#line 897 "parser.y"
                              {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), "=");
        raizAST = (yyval.nodo);
    }
#line 3334 "parser.tab.c"
    break;

  case 149: /* expresion: expresion TokenOpAsignacion expresion  */
#line 901 "parser.y"
                                            {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), (yyvsp[-1].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3343 "parser.tab.c"
    break;

  case 150: /* expresion: expresion TokenOpLogico expresion  */
#line 905 "parser.y"
                                        {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), (yyvsp[-1].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3352 "parser.tab.c"
    break;

  case 151: /* expresion: expresion TokenOpBits expresion  */
#line 909 "parser.y"
                                      {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), (yyvsp[-1].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3361 "parser.tab.c"
    break;

  case 152: /* expresion: expresion TokenOpIgualdad expresion  */
#line 913 "parser.y"
                                          {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), (yyvsp[-1].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3370 "parser.tab.c"
    break;

  case 153: /* expresion: expresion TokenOpDesplazamiento expresion  */
#line 917 "parser.y"
                                                {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), (yyvsp[-1].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3379 "parser.tab.c"
    break;

  case 154: /* expresion: expresion TokenOpAditivos expresion  */
#line 921 "parser.y"
                                          {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), (yyvsp[-1].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3388 "parser.tab.c"
    break;

  case 155: /* expresion: expresion TokenOpMultiplicativos expresion  */
#line 925 "parser.y"
                                                 {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), (yyvsp[-1].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3397 "parser.tab.c"
    break;

  case 156: /* expresion: TokenOpAditivos expresion  */
#line 930 "parser.y"
                                             {
        (yyval.nodo) = crearNodoUnario((yyvsp[-1].tokenInfo).lexema, (yyvsp[0].nodo));
        raizAST = (yyval.nodo);
    }
#line 3406 "parser.tab.c"
    break;

  case 157: /* expresion: '!' expresion  */
#line 934 "parser.y"
                                 {
        (yyval.nodo) = crearNodoUnario("!", (yyvsp[0].nodo));
        raizAST = (yyval.nodo);
    }
#line 3415 "parser.tab.c"
    break;

  case 158: /* expresion: TokenOpBits expresion  */
#line 938 "parser.y"
                                         {
        if (strcmp((yyvsp[-1].tokenInfo).lexema, "&") != 0 && strcmp((yyvsp[-1].tokenInfo).lexema, "~") != 0) {
            char msg[96];
            snprintf(msg, sizeof(msg), "se esperaba una expresión antes de '%s'", (yyvsp[-1].tokenInfo).lexema);
            yyerror(msg);
        }
        (yyval.nodo) = crearNodoUnario((yyvsp[-1].tokenInfo).lexema, (yyvsp[0].nodo));
        raizAST = (yyval.nodo);
    }
#line 3429 "parser.tab.c"
    break;

  case 159: /* expresion: TokenOpMultiplicativos expresion  */
#line 947 "parser.y"
                                                    {
        if (strcmp((yyvsp[-1].tokenInfo).lexema, "*") != 0) {
            char msg[96];
            snprintf(msg, sizeof(msg), "se esperaba una expresión antes de '%s'", (yyvsp[-1].tokenInfo).lexema);
            yyerror(msg);
        }
        (yyval.nodo) = crearNodoUnario((yyvsp[-1].tokenInfo).lexema, (yyvsp[0].nodo));
        raizAST = (yyval.nodo);
    }
#line 3443 "parser.tab.c"
    break;

  case 160: /* expresion: TokenOpInc expresion  */
#line 956 "parser.y"
                                        {
        (yyval.nodo) = crearNodoUnario((yyvsp[-1].tokenInfo).lexema, (yyvsp[0].nodo));
        raizAST = (yyval.nodo);
    }
#line 3452 "parser.tab.c"
    break;

  case 161: /* expresion: TokenSizeof expresion  */
#line 960 "parser.y"
                                         {
        (yyval.nodo) = crearNodoUnario("sizeof ", (yyvsp[0].nodo));
        raizAST = (yyval.nodo);
    }
#line 3461 "parser.tab.c"
    break;

  case 162: /* expresion: TokenSizeof '(' tipoCast ')'  */
#line 964 "parser.y"
                                                {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-1].nodo), NULL, "sizeof-tipo");
        raizAST = (yyval.nodo);
    }
#line 3470 "parser.tab.c"
    break;

  case 163: /* expresion: TokenSizeof '(' tipoCast TokenOpMultiplicativos ')'  */
#line 968 "parser.y"
                                                                       {
        if (strcmp((yyvsp[-1].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *ptrNodo = crearNodoUnario("*", (yyvsp[-2].nodo));
            (yyval.nodo) = crearNodo(nodoExpresion, ptrNodo, NULL, "sizeof-puntero");
        } else {
            yyerror("se esperaba '*' en el sizeof del puntero");
            (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), NULL, "sizeof-tipo");
        }
        raizAST = (yyval.nodo);
    }
#line 3485 "parser.tab.c"
    break;

  case 164: /* expresion: expresion TokenOpInc  */
#line 979 "parser.y"
                           {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-1].nodo), NULL, (yyvsp[0].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3494 "parser.tab.c"
    break;

  case 165: /* expresion: '(' TokenTipoDato ')' expresion  */
#line 984 "parser.y"
                                                   {
        struct ASTNodo *tipoNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-2].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, tipoNodo, (yyvsp[0].nodo), "casteo");
        raizAST = (yyval.nodo);
    }
#line 3504 "parser.tab.c"
    break;

  case 166: /* expresion: '(' TokenModificador TokenTipoDato ')' expresion  */
#line 989 "parser.y"
                                                                    {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-3].tokenInfo).lexema);
        struct ASTNodo *tipo = crearNodo(nodoExpresion, mod, NULL, (yyvsp[-2].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, tipo, (yyvsp[0].nodo), "casteo");
        raizAST = (yyval.nodo);
    }
#line 3515 "parser.tab.c"
    break;

  case 167: /* expresion: '(' TokenVoid TokenOpMultiplicativos ')' expresion  */
#line 995 "parser.y"
                                                                      {
        if (strcmp((yyvsp[-2].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *tipoNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-3].tokenInfo).lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", tipoNodo);
            (yyval.nodo) = crearNodo(nodoExpresion, ptrNodo, (yyvsp[0].nodo), "casteo-puntero");
        } else {
            yyerror("se esperaba '*' en el casteo de puntero");
            (yyval.nodo) = (yyvsp[0].nodo);
        }
        raizAST = (yyval.nodo);
    }
#line 3531 "parser.tab.c"
    break;

  case 168: /* expresion: '(' TokenStruct TokenID TokenOpMultiplicativos ')' expresion  */
#line 1006 "parser.y"
                                                                                {
        if (strcmp((yyvsp[-2].tokenInfo).lexema, "*") == 0) {
            struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[-3].tokenInfo).lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", strNodo);
            (yyval.nodo) = crearNodo(nodoExpresion, ptrNodo, (yyvsp[0].nodo), "casteo-puntero-struct");
        } else {
            yyerror("se esperaba '*' en el casteo de struct");
            (yyval.nodo) = (yyvsp[0].nodo);
        }
        raizAST = (yyval.nodo);
    }
#line 3547 "parser.tab.c"
    break;

  case 169: /* expresion: expresion '?' expresion ':' expresion  */
#line 1019 "parser.y"
                                            {
        struct ASTNodo *ramas = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), ":");
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-4].nodo), ramas, "?");
        raizAST = (yyval.nodo);
    }
#line 3557 "parser.tab.c"
    break;

  case 170: /* expresion: TokenID '(' listaArgumentosOpt ')'  */
#line 1024 "parser.y"
                                         {
        (yyval.nodo) = crearNodo(nodoLlamadaFuncion, (yyvsp[-1].nodo), NULL, (yyvsp[-3].tokenInfo).lexema);
        raizAST = (yyval.nodo);
    }
#line 3566 "parser.tab.c"
    break;

  case 171: /* expresion: expresion TokenFlecha TokenID  */
#line 1028 "parser.y"
                                    {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), idNodo, "->");
        raizAST = (yyval.nodo);
    }
#line 3576 "parser.tab.c"
    break;

  case 172: /* expresion: expresion '.' TokenID  */
#line 1033 "parser.y"
                            {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, (yyvsp[0].tokenInfo).lexema);
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), idNodo, ".");
        raizAST = (yyval.nodo);
    }
#line 3586 "parser.tab.c"
    break;

  case 173: /* listaArgumentosOpt: %empty  */
#line 1042 "parser.y"
    { (yyval.nodo) = NULL; raizAST = (yyval.nodo); }
#line 3592 "parser.tab.c"
    break;

  case 174: /* listaArgumentosOpt: listaArgumentos  */
#line 1043 "parser.y"
                      { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo); }
#line 3598 "parser.tab.c"
    break;

  case 175: /* listaArgumentos: expresion  */
#line 1047 "parser.y"
              { (yyval.nodo) = (yyvsp[0].nodo); raizAST = (yyval.nodo);}
#line 3604 "parser.tab.c"
    break;

  case 176: /* listaArgumentos: listaArgumentos ',' expresion  */
#line 1048 "parser.y"
                                    {
        (yyval.nodo) = crearNodo(nodoExpresion, (yyvsp[-2].nodo), (yyvsp[0].nodo), "arg");
        raizAST = (yyval.nodo);
    }
#line 3613 "parser.tab.c"
    break;


#line 3617 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1054 "parser.y"


/* Crea un nodo de operador unario prefijo */
struct ASTNodo *crearNodoUnario(const char *op, struct ASTNodo *operando) {
    char buf[64];
    snprintf(buf, sizeof(buf), "unario:%s", op);
    return crearNodo(nodoExpresion, operando, NULL, buf);
}

struct ASTNodo* crearNodo(TipoNodo tipo, struct ASTNodo *izq, struct ASTNodo *der, char *lexema) {
    struct ASTNodo *nuevo = (struct ASTNodo*) malloc(sizeof(struct ASTNodo));
    if (!nuevo) {
        fprintf(stderr, "Error: Memoria agotada al crear nodo del AST\n");
        exit(1);
    }
    nuevo->tipo = tipo;
    nuevo->izq = izq;
    nuevo->der = der;
    nuevo->valorLexema = lexema ? strdup(lexema) : NULL;
    nuevo->valorNumerico = 0.0;
    return nuevo;
}


static void imprimirLineaErrorGCC(const char *archivo, int numLinea, int numColumna, int longitudToken) {
    FILE *f = fopen(archivo, "r");
    if (!f) return;

    char lineaBuffer[1024];
    int lineaActual = 1;

    while (fgets(lineaBuffer, sizeof(lineaBuffer), f)) {
        if (lineaActual == numLinea) {
            lineaBuffer[strcspn(lineaBuffer, "\r\n")] = 0;

            fprintf(stderr, " %5d | %s\n", numLinea, lineaBuffer);
            fprintf(stderr, "       | ");

            for (int i = 1; i < numColumna; i++) {
                if (lineaBuffer[i - 1] == '\t') {
                    fputc('\t', stderr);
                } else {
                    fputc(' ', stderr);
                }
            }

            fputc('^', stderr);
            for (int i = 1; i < longitudToken; i++) {
                fputc('~', stderr);
            }
            fputc('\n', stderr);

            break;
        }
        lineaActual++;
    }
    fclose(f);
}

// Función yyerror adaptada al formato nativo de GCC
void yyerror(const char *s) {
    totalErrores++;

    int columna = yylval.tokenInfo.columna > 0 ? yylval.tokenInfo.columna : columnaAct;
    int lenToken = yyleng > 0 ? yyleng : 1;

    const char *archivo = (archivoFuente && strlen(archivoFuente) > 0)
                          ? archivoFuente
                          : "fuente.c";

    fprintf(stderr, "%s:%d:%d: \033[1;31merror:\033[0m %s\n", archivo, lineaAct, columna, s);
    imprimirLineaErrorGCC(archivo, lineaAct, columna, lenToken);
}

int esTipoUsuario(char *s) {
    for (int i = 0; tiposEstandar[i]; i++){
        if (strcmp(s, tiposEstandar[i]) == 0) {
            return 1;
        }
    }
    for (int i = 0; i < totalTiposUsuario; i++) {
        if (strcmp(s, tiposUsuario[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void registrarTipo(char *s) {
    if (!s || esTipoUsuario(s) || totalTiposUsuario >= MaxTipos) {
        return;
    }
    tiposUsuario[totalTiposUsuario++] = strdup(s);
}

// Registra los nombres declarados en un typedef (la lista de ids)
void registrarTipoDeclarado(struct ASTNodo *n) {
    if (!n) {
        return;
    }
    if (n->valorLexema && strcmp(n->valorLexema, "lista-ids") == 0) {
        registrarTipoDeclarado(n->izq);
        registrarTipoDeclarado(n->der);
        return;
    }
    struct ASTNodo *h = n;
    while (h && h->izq) h = h->izq; // el nombre esta al fondo de la rama izquierda
    if (h && h->valorLexema) {
        registrarTipo(h->valorLexema);
    }
}
