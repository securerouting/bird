/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         cf_parse
#define yylex           cf_lex
#define yyerror         cf_error
#define yylval          cf_lval
#define yychar          cf_char
#define yydebug         cf_debug
#define yynerrs         cf_nerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "cf-parse.y"

/* Headers from ../../conf/confbase.Y */

#define PARSER 1

#include "nest/bird.h"
#include "conf/conf.h"
#include "lib/resource.h"
#include "lib/socket.h"
#include "lib/timer.h"
#include "lib/string.h"
#include "nest/protocol.h"
#include "nest/iface.h"
#include "nest/route.h"
#include "nest/cli.h"
#include "filter/filter.h"

/* FIXME: Turn on YYERROR_VERBOSE and work around lots of bison bugs? */

/* Defines from ../../conf/confbase.Y */

static void
check_u16(unsigned val)
{
  if (val > 0xFFFF)
    cf_error("Value %d out of range (0-65535)", val);
}

/* Headers from ../../sysdep/unix/config.Y */

#include "lib/unix.h"
#include <stdio.h>

/* Headers from ../../sysdep/unix/krt.Y */

#include "lib/krt.h"

/* Defines from ../../sysdep/unix/krt.Y */

#define THIS_KRT ((struct krt_config *) this_proto)
#define THIS_KIF ((struct kif_config *) this_proto)

/* Headers from ../../sysdep/linux/netlink/netlink.Y */

/* Headers from ../../nest/config.Y */

#include "nest/rt-dev.h"
#include "nest/password.h"
#include "nest/cmds.h"
#include "lib/lists.h"

/* Defines from ../../nest/config.Y */

static struct proto_config *this_proto;
static struct iface_patt *this_ipatt;
static struct iface_patt_node *this_ipn;
static list *this_p_list;
static struct password_item *this_p_item;
static int password_id;

static inline void
reset_passwords(void)
{
  this_p_list = NULL;
}

static inline list *
get_passwords(void)
{
  list *rv = this_p_list;
  this_p_list = NULL;
  return rv;
}

#define DIRECT_CFG ((struct rt_dev_config *) this_proto)

/* Headers from ../../filter/config.Y */

/* Defines from ../../filter/config.Y */

#define P(a,b) ((a << 8) | b)

static inline u32 pair(u32 a, u32 b) { return (a << 16) | b; }
static inline u32 pair_a(u32 p) { return p >> 16; }
static inline u32 pair_b(u32 p) { return p & 0xFFFF; }


/*
 * Sets and their items are during parsing handled as lists, linked
 * through left ptr. The first item in a list also contains a pointer
 * to the last item in a list (right ptr). For convenience, even items
 * are handled as one-item lists. Lists are merged by f_merge_items().
 */

static inline struct f_tree *
f_new_item(struct f_val from, struct f_val to)
{
  struct f_tree *t = f_new_tree();
  t->right = t;
  t->from = from;
  t->to = to;
  return t;
}

static inline struct f_tree *
f_merge_items(struct f_tree *a, struct f_tree *b)
{
  if (!a) return b;
  a->right->left = b;
  a->right = b->right;
  b->right = NULL;
  return a;
}

static inline struct f_tree *
f_new_pair_item(int fa, int ta, int fb, int tb)
{
  struct f_tree *t = f_new_tree();
  t->right = t;
  t->from.type = t->to.type = T_PAIR;
  t->from.val.i = pair(fa, fb);
  t->to.val.i = pair(ta, tb);
  return t;
}

static inline struct f_tree *
f_new_pair_set(int fa, int ta, int fb, int tb)
{
  struct f_tree *lst = NULL;
  int i;

  if ((fa == ta) || ((fb == 0) && (tb == 0xFFFF)))
    return f_new_pair_item(fa, ta, fb, tb);
  
  if ((ta < fa) || (tb < fb))
    cf_error( "From value cannot be higher that To value in pair sets");

  for (i = fa; i <= ta; i++)
    lst = f_merge_items(lst, f_new_pair_item(i, i, fb, tb));

  return lst;
}

#define EC_ALL 0xFFFFFFFF

static struct f_tree *
f_new_ec_item(u32 kind, u32 ipv4_used, u32 key, u32 vf, u32 vt)
{
  u64 fm, to;

  if (ipv4_used || (key >= 0x10000)) {
    check_u16(vf);
    if (vt == EC_ALL)
      vt = 0xFFFF;
    else
      check_u16(vt);
  }

  if (kind == EC_GENERIC) {
    fm = ec_generic(key, vf);
    to = ec_generic(key, vt);
  }
  else if (ipv4_used) {
    fm = ec_ip4(kind, key, vf);
    to = ec_ip4(kind, key, vt);
  }
  else if (key < 0x10000) {
    fm = ec_as2(kind, key, vf);
    to = ec_as2(kind, key, vt);
  }
  else {
    fm = ec_as4(kind, key, vf);
    to = ec_as4(kind, key, vt);
  }

  struct f_tree *t = f_new_tree();
  t->right = t;
  t->from.type = t->to.type = T_EC;
  t->from.val.ec = fm;
  t->to.val.ec = to;
  return t;
}

static inline struct f_inst *
f_generate_empty(struct f_inst *dyn)
{ 
  struct f_inst *e = f_new_inst();
  e->code = 'E';

  switch (dyn->aux & EAF_TYPE_MASK) {
    case EAF_TYPE_AS_PATH:
      e->aux = T_PATH;
      break;
    case EAF_TYPE_INT_SET:
      e->aux = T_CLIST;
      break;
    case EAF_TYPE_EC_SET:
      e->aux = T_ECLIST;
      break;
    default:
      cf_error("Can't empty that attribute");
  }

  dyn->code = P('e','S');
  dyn->a1.p = e;
  return dyn;
}


static inline struct f_inst *
f_generate_dpair(struct f_inst *t1, struct f_inst *t2)
{
  struct f_inst *rv;

  if ((t1->code == 'c') && (t2->code == 'c')) {
    if ((t1->aux != T_INT) || (t2->aux != T_INT))
      cf_error( "Can't operate with value of non-integer type in pair constructor");

    check_u16(t1->a2.i);
    check_u16(t2->a2.i);

    rv = f_new_inst();
    rv->code = 'c';
    rv->aux = T_PAIR;
    rv->a2.i = pair(t1->a2.i, t2->a2.i);
  }
  else {
    rv = f_new_inst();
    rv->code = P('m', 'p');
    rv->a1.p = t1;
    rv->a2.p = t2;
  }

  return rv;
}

static inline struct f_inst *
f_generate_ec(u16 kind, struct f_inst *tk, struct f_inst *tv)
{
  struct f_inst *rv;
  int c1 = 0, c2 = 0, ipv4_used = 0;
  u32 key = 0, val2 = 0;

  if (tk->code == 'c') {
    c1 = 1;

    if (tk->aux == T_INT) {
      ipv4_used = 0; key = tk->a2.i;
    }
    else if (tk->aux == T_QUAD) {
      ipv4_used = 1; key = tk->a2.i;
    }
    else
      cf_error("Can't operate with key of non-integer/IPv4 type in EC constructor");
  }

#ifndef IPV6
  /* IP->Quad implicit conversion */
  else if (tk->code == 'C') {
    c1 = 1;
    struct f_val *val = tk->a1.p;
    if (val->type == T_IP) {
      ipv4_used = 1; key = ipa_to_u32(val->val.px.ip);
    }
    else
      cf_error("Can't operate with key of non-integer/IPv4 type in EC constructor");
  }
#endif

  if (tv->code == 'c') {
    if (tv->aux != T_INT)
      cf_error("Can't operate with value of non-integer type in EC constructor");
    c2 = 1;
    val2 = tv->a2.i;
  }

  if (c1 && c2) {
    u64 ec;
  
    if (kind == EC_GENERIC) {
      ec = ec_generic(key, val2);
    }
    else if (ipv4_used) {
      check_u16(val2);
      ec = ec_ip4(kind, key, val2);
    }
    else if (key < 0x10000) {
      ec = ec_as2(kind, key, val2);
    }
    else {
      check_u16(val2);
      ec = ec_as4(kind, key, val2);
    }

    NEW_F_VAL;
    rv = f_new_inst();
    rv->code = 'C';
    rv->a1.p = val;    
    val->type = T_EC;
    val->val.ec = ec;
  }
  else {
    rv = f_new_inst();
    rv->code = P('m','c');
    rv->aux = kind;
    rv->a1.p = tk;
    rv->a2.p = tv;
  }

  return rv;
}



/* Headers from ../../proto/bgp/config.Y */

#include "proto/bgp/bgp.h"

/* Defines from ../../proto/bgp/config.Y */

#define BGP_CFG ((struct bgp_config *) this_proto)

/* Headers from ../../proto/ospf/config.Y */

#include "proto/ospf/ospf.h"

/* Defines from ../../proto/ospf/config.Y */

#define OSPF_CFG ((struct ospf_config *) this_proto)
#define OSPF_PATT ((struct ospf_iface_patt *) this_ipatt)

static struct ospf_area_config *this_area;
static struct nbma_node *this_nbma;
static list *this_nets;
static struct area_net_config *this_pref;
static struct ospf_stubnet_config *this_stubnet; 

#ifdef OSPFv2
static void
ospf_iface_finish(void)
{
  struct ospf_iface_patt *ip = OSPF_PATT;

  if (ip->deadint == 0)
    ip->deadint = ip->deadc * ip->helloint;

  ip->passwords = get_passwords();

  if ((ip->autype == OSPF_AUTH_CRYPT) && (ip->helloint < 5))
    log(L_WARN "Hello or poll interval less that 5 makes cryptographic authenication prone to replay attacks");

  if ((ip->autype == OSPF_AUTH_NONE) && (ip->passwords != NULL))
    log(L_WARN "Password option without authentication option does not make sense");
}
#endif

#ifdef OSPFv3
static void
ospf_iface_finish(void)
{
  struct ospf_iface_patt *ip = OSPF_PATT;

  if (ip->deadint == 0)
    ip->deadint = ip->deadc * ip->helloint;

  if ((ip->autype != OSPF_AUTH_NONE) || (get_passwords() != NULL))
    cf_error("Authentication not supported in OSPFv3");
}
#endif

static void
ospf_area_finish(void)
{
  if ((this_area->areaid == 0) && (this_area->type != OPT_E))
    cf_error("Backbone area cannot be stub/NSSA");

  if (this_area->summary && (this_area->type == OPT_E))
    cf_error("Only Stub/NSSA areas can use summary propagation");

  if (this_area->default_nssa && ((this_area->type != OPT_N) || ! this_area->summary))
    cf_error("Only NSSA areas with summary propagation can use NSSA default route");

  if ((this_area->default_cost & LSA_EXT_EBIT) && ! this_area->default_nssa)
    cf_error("Only NSSA default route can use type 2 metric");
}

static void
ospf_proto_finish(void)
{
  struct ospf_config *cf = OSPF_CFG;

  if (EMPTY_LIST(cf->area_list))
    cf_error( "No configured areas in OSPF");

  int areano = 0;
  int backbone = 0;
  struct ospf_area_config *ac;
  WALK_LIST(ac, cf->area_list)
  {
    areano++;
    if (ac->areaid == 0)
     backbone = 1;
  }
  cf->abr = areano > 1;

  if (cf->abr && !backbone)
  {
    struct ospf_area_config *ac = cfg_allocz(sizeof(struct ospf_area_config));
    add_head(&cf->area_list, NODE ac);
    init_list(&ac->patt_list);
    init_list(&ac->net_list);
    init_list(&ac->enet_list);
    init_list(&ac->stubnet_list);
  }

  if (!cf->abr && !EMPTY_LIST(cf->vlink_list))
    cf_error( "Vlinks cannot be used on single area router");
}

static inline void
check_defcost(int cost)
{
  if ((cost <= 0) || (cost >= LSINFINITY))
   cf_error("Default cost must be in range 1-%d", LSINFINITY);
}

/* Headers from ../../proto/pipe/config.Y */

#include "proto/pipe/pipe.h"

/* Defines from ../../proto/pipe/config.Y */

#define PIPE_CFG ((struct pipe_config *) this_proto)

/* Headers from ../../proto/rip/config.Y */

#include "proto/rip/rip.h"
#include "nest/iface.h"

/* Defines from ../../proto/rip/config.Y */

#define RIP_CFG ((struct rip_proto_config *) this_proto)
#define RIP_IPATT ((struct rip_patt *) this_ipatt)

/* Headers from ../../proto/static/config.Y */

#include "proto/static/static.h"

/* Defines from ../../proto/static/config.Y */

#define STATIC_CFG ((struct static_config *) this_proto)
static struct static_route *this_srt, *this_srt_nh, *last_srt_nh;



/* Line 189 of yacc.c  */
#line 536 "cf-parse.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     END = 258,
     CLI_MARKER = 259,
     INVALID_TOKEN = 260,
     ELSECOL = 261,
     DDOT = 262,
     GEQ = 263,
     LEQ = 264,
     NEQ = 265,
     AND = 266,
     OR = 267,
     PO = 268,
     PC = 269,
     NUM = 270,
     ENUM = 271,
     RTRID = 272,
     IPA = 273,
     SYM = 274,
     TEXT = 275,
     PREFIX_DUMMY = 276,
     DEFINE = 277,
     ON = 278,
     OFF = 279,
     YES = 280,
     NO = 281,
     LOG = 282,
     SYSLOG = 283,
     ALL = 284,
     DEBUG = 285,
     TRACE = 286,
     INFO = 287,
     REMOTE = 288,
     WARNING = 289,
     ERROR = 290,
     AUTH = 291,
     FATAL = 292,
     BUG = 293,
     STDERR = 294,
     SOFT = 295,
     TIMEFORMAT = 296,
     ISO = 297,
     SHORT = 298,
     LONG = 299,
     BASE = 300,
     NAME = 301,
     CONFIGURE = 302,
     DOWN = 303,
     KERNEL = 304,
     PERSIST = 305,
     SCAN = 306,
     TIME = 307,
     LEARN = 308,
     DEVICE = 309,
     ROUTES = 310,
     ASYNC = 311,
     TABLE = 312,
     KRT_PREFSRC = 313,
     KRT_REALM = 314,
     ROUTER = 315,
     ID = 316,
     PROTOCOL = 317,
     TEMPLATE = 318,
     PREFERENCE = 319,
     DISABLED = 320,
     DIRECT = 321,
     INTERFACE = 322,
     IMPORT = 323,
     EXPORT = 324,
     FILTER = 325,
     NONE = 326,
     STATES = 327,
     FILTERS = 328,
     PASSWORD = 329,
     FROM = 330,
     PASSIVE = 331,
     TO = 332,
     EVENTS = 333,
     PACKETS = 334,
     PROTOCOLS = 335,
     INTERFACES = 336,
     PRIMARY = 337,
     STATS = 338,
     COUNT = 339,
     FOR = 340,
     COMMANDS = 341,
     PREEXPORT = 342,
     GENERATE = 343,
     LISTEN = 344,
     BGP = 345,
     V6ONLY = 346,
     DUAL = 347,
     ADDRESS = 348,
     PORT = 349,
     PASSWORDS = 350,
     DESCRIPTION = 351,
     RELOAD = 352,
     IN = 353,
     OUT = 354,
     MRTDUMP = 355,
     MESSAGES = 356,
     RESTRICT = 357,
     MEMORY = 358,
     IGP_METRIC = 359,
     SHOW = 360,
     STATUS = 361,
     SUMMARY = 362,
     ROUTE = 363,
     SYMBOLS = 364,
     DUMP = 365,
     RESOURCES = 366,
     SOCKETS = 367,
     NEIGHBORS = 368,
     ATTRIBUTES = 369,
     ECHO = 370,
     DISABLE = 371,
     ENABLE = 372,
     RESTART = 373,
     FUNCTION = 374,
     PRINT = 375,
     PRINTN = 376,
     UNSET = 377,
     RETURN = 378,
     ACCEPT = 379,
     REJECT = 380,
     QUITBIRD = 381,
     INT = 382,
     BOOL = 383,
     IP = 384,
     PREFIX = 385,
     PAIR = 386,
     QUAD = 387,
     EC = 388,
     SET = 389,
     STRING = 390,
     BGPMASK = 391,
     BGPPATH = 392,
     CLIST = 393,
     ECLIST = 394,
     IF = 395,
     THEN = 396,
     ELSE = 397,
     CASE = 398,
     TRUE = 399,
     FALSE = 400,
     RT = 401,
     RO = 402,
     UNKNOWN = 403,
     GENERIC = 404,
     GW = 405,
     NET = 406,
     MASK = 407,
     PROTO = 408,
     SOURCE = 409,
     SCOPE = 410,
     CAST = 411,
     DEST = 412,
     LEN = 413,
     DEFINED = 414,
     ADD = 415,
     DELETE = 416,
     CONTAINS = 417,
     RESET = 418,
     PREPEND = 419,
     FIRST = 420,
     LAST = 421,
     MATCH = 422,
     EMPTY = 423,
     WHERE = 424,
     EVAL = 425,
     LOCAL = 426,
     NEIGHBOR = 427,
     AS = 428,
     HOLD = 429,
     CONNECT = 430,
     RETRY = 431,
     KEEPALIVE = 432,
     MULTIHOP = 433,
     STARTUP = 434,
     VIA = 435,
     NEXT = 436,
     HOP = 437,
     SELF = 438,
     DEFAULT = 439,
     PATH = 440,
     METRIC = 441,
     START = 442,
     DELAY = 443,
     FORGET = 444,
     WAIT = 445,
     AFTER = 446,
     BGP_PATH = 447,
     BGP_LOCAL_PREF = 448,
     BGP_MED = 449,
     BGP_ORIGIN = 450,
     BGP_NEXT_HOP = 451,
     BGP_ATOMIC_AGGR = 452,
     BGP_AGGREGATOR = 453,
     BGP_COMMUNITY = 454,
     BGP_EXT_COMMUNITY = 455,
     RR = 456,
     RS = 457,
     CLIENT = 458,
     CLUSTER = 459,
     AS4 = 460,
     ADVERTISE = 461,
     IPV4 = 462,
     CAPABILITIES = 463,
     LIMIT = 464,
     PREFER = 465,
     OLDER = 466,
     MISSING = 467,
     LLADDR = 468,
     DROP = 469,
     IGNORE = 470,
     REFRESH = 471,
     INTERPRET = 472,
     COMMUNITIES = 473,
     BGP_ORIGINATOR_ID = 474,
     BGP_CLUSTER_LIST = 475,
     IGP = 476,
     GATEWAY = 477,
     RECURSIVE = 478,
     MED = 479,
     TTL = 480,
     SECURITY = 481,
     BGPSEC = 482,
     OSPF = 483,
     AREA = 484,
     OSPF_METRIC1 = 485,
     OSPF_METRIC2 = 486,
     OSPF_TAG = 487,
     OSPF_ROUTER_ID = 488,
     RFC1583COMPAT = 489,
     STUB = 490,
     TICK = 491,
     COST = 492,
     COST2 = 493,
     RETRANSMIT = 494,
     HELLO = 495,
     TRANSMIT = 496,
     PRIORITY = 497,
     DEAD = 498,
     TYPE = 499,
     BROADCAST = 500,
     BCAST = 501,
     NONBROADCAST = 502,
     NBMA = 503,
     POINTOPOINT = 504,
     PTP = 505,
     POINTOMULTIPOINT = 506,
     PTMP = 507,
     SIMPLE = 508,
     AUTHENTICATION = 509,
     STRICT = 510,
     CRYPTOGRAPHIC = 511,
     ELIGIBLE = 512,
     POLL = 513,
     NETWORKS = 514,
     HIDDEN = 515,
     VIRTUAL = 516,
     CHECK = 517,
     LINK = 518,
     RX = 519,
     BUFFER = 520,
     LARGE = 521,
     NORMAL = 522,
     STUBNET = 523,
     TAG = 524,
     EXTERNAL = 525,
     LSADB = 526,
     ECMP = 527,
     WEIGHT = 528,
     NSSA = 529,
     TRANSLATOR = 530,
     STABILITY = 531,
     TOPOLOGY = 532,
     STATE = 533,
     PIPE = 534,
     PEER = 535,
     MODE = 536,
     OPAQUE = 537,
     TRANSPARENT = 538,
     RIP = 539,
     INFINITY = 540,
     PERIOD = 541,
     GARBAGE = 542,
     TIMEOUT = 543,
     MULTICAST = 544,
     QUIET = 545,
     NOLISTEN = 546,
     VERSION1 = 547,
     PLAINTEXT = 548,
     MD5 = 549,
     HONOR = 550,
     NEVER = 551,
     ALWAYS = 552,
     RIP_METRIC = 553,
     RIP_TAG = 554,
     STATIC = 555,
     PROHIBIT = 556,
     MULTIPATH = 557
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 458 "cf-parse.y"

  int i;
  u32 i32;
  ip_addr a;
  struct symbol *s;
  char *t;
  struct rtable_config *r;
  struct f_inst *x;
  struct filter *f;
  struct f_tree *e;
  struct f_trie *trie;
  struct f_val v;
  struct f_path_mask *h;
  struct password_item *p;
  struct rt_show_data *ra;
  void *g;
  bird_clock_t time;
  struct prefix px;
  struct proto_spec ps;
  struct timeformat *tf;



/* Line 214 of yacc.c  */
#line 898 "cf-parse.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 910 "cf-parse.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  54
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2227

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  324
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  211
/* YYNRULES -- Number of rules.  */
#define YYNRULES  680
/* YYNRULES -- Number of states.  */
#define YYNSTATES  1278

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   557

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    31,     2,     2,     2,    30,     2,     2,
     314,   315,    28,    26,   320,    27,    32,    29,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   317,   316,
      23,    22,    24,   321,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   322,     2,   323,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   318,     2,   319,    25,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     9,    10,    13,    15,    19,    21,
      27,    33,    35,    37,    39,    41,    43,    44,    46,    48,
      51,    53,    55,    58,    61,    63,    65,    66,    71,    74,
      75,    77,    80,    82,    84,    88,    90,    94,    96,    98,
     100,   102,   104,   106,   108,   110,   112,   117,   121,   123,
     125,   127,   129,   132,   137,   141,   145,   149,   153,   158,
     161,   162,   164,   167,   170,   174,   177,   181,   184,   188,
     192,   196,   201,   203,   205,   207,   212,   213,   216,   219,
     222,   224,   226,   229,   231,   233,   234,   236,   240,   241,
     244,   247,   250,   253,   256,   259,   262,   266,   269,   272,
     274,   276,   278,   280,   284,   288,   289,   291,   293,   296,
     297,   299,   303,   305,   309,   312,   316,   320,   324,   325,
     329,   331,   333,   337,   339,   343,   345,   347,   349,   351,
     353,   355,   357,   359,   363,   365,   369,   371,   373,   378,
     380,   381,   385,   390,   392,   395,   396,   402,   408,   414,
     420,   425,   429,   433,   438,   444,   446,   447,   451,   456,
     461,   462,   465,   469,   473,   477,   480,   483,   486,   490,
     494,   497,   500,   502,   504,   509,   513,   517,   521,   525,
     529,   533,   537,   542,   544,   546,   548,   549,   551,   555,
     559,   563,   567,   572,   577,   582,   587,   590,   592,   594,
     596,   598,   599,   601,   602,   607,   610,   612,   614,   616,
     618,   620,   622,   624,   626,   628,   630,   632,   634,   637,
     640,   641,   645,   647,   651,   653,   655,   657,   660,   664,
     667,   672,   673,   679,   680,   682,   684,   687,   689,   693,
     695,   697,   699,   701,   703,   705,   709,   711,   713,   715,
     717,   719,   723,   725,   731,   743,   745,   747,   749,   752,
     754,   762,   772,   780,   782,   784,   786,   790,   792,   794,
     796,   800,   802,   806,   808,   812,   816,   818,   821,   824,
     831,   833,   837,   838,   843,   847,   849,   853,   857,   861,
     864,   867,   870,   873,   874,   877,   880,   881,   883,   885,
     887,   889,   891,   893,   895,   899,   903,   905,   907,   913,
     921,   922,   927,   929,   931,   933,   935,   937,   939,   941,
     943,   945,   949,   953,   957,   961,   965,   969,   973,   977,
     981,   985,   989,   993,   997,  1001,  1004,  1009,  1011,  1013,
    1015,  1017,  1020,  1023,  1027,  1031,  1038,  1042,  1046,  1050,
    1054,  1060,  1067,  1074,  1081,  1088,  1093,  1095,  1097,  1099,
    1101,  1103,  1105,  1107,  1108,  1110,  1114,  1116,  1120,  1121,
    1123,  1128,  1135,  1140,  1144,  1150,  1156,  1161,  1168,  1172,
    1175,  1181,  1187,  1196,  1205,  1214,  1223,  1226,  1230,  1234,
    1240,  1247,  1254,  1261,  1266,  1271,  1277,  1284,  1291,  1297,
    1301,  1306,  1312,  1318,  1324,  1330,  1335,  1340,  1346,  1352,
    1358,  1364,  1370,  1376,  1382,  1389,  1396,  1405,  1412,  1419,
    1425,  1430,  1436,  1441,  1447,  1452,  1458,  1464,  1470,  1473,
    1477,  1481,  1483,  1486,  1489,  1494,  1497,  1499,  1502,  1507,
    1508,  1512,  1515,  1517,  1520,  1524,  1528,  1532,  1536,  1539,
    1543,  1544,  1550,  1551,  1557,  1560,  1563,  1565,  1570,  1572,
    1574,  1575,  1579,  1582,  1585,  1588,  1593,  1595,  1596,  1600,
    1601,  1604,  1607,  1611,  1614,  1617,  1621,  1624,  1627,  1630,
    1632,  1636,  1639,  1642,  1645,  1648,  1651,  1654,  1658,  1661,
    1664,  1667,  1670,  1673,  1676,  1679,  1682,  1686,  1689,  1693,
    1696,  1700,  1704,  1709,  1712,  1715,  1718,  1722,  1726,  1730,
    1732,  1733,  1736,  1740,  1742,  1743,  1745,  1748,  1749,  1752,
    1754,  1756,  1759,  1763,  1764,  1765,  1769,  1770,  1774,  1778,
    1780,  1781,  1786,  1793,  1800,  1807,  1815,  1822,  1830,  1837,
    1840,  1844,  1848,  1854,  1859,  1864,  1867,  1871,  1875,  1880,
    1885,  1890,  1896,  1902,  1907,  1911,  1916,  1921,  1926,  1931,
    1933,  1935,  1937,  1939,  1941,  1943,  1945,  1947,  1948,  1951,
    1954,  1955,  1959,  1960,  1964,  1965,  1969,  1972,  1976,  1980,
    1986,  1992,  1996,  1999,  2002,  2006,  2008,  2011,  2015,  2019,
    2023,  2027,  2030,  2033,  2036,  2041,  2043,  2045,  2047,  2049,
    2051,  2053,  2055,  2057,  2059,  2061,  2063,  2065,  2067,  2069,
    2071,  2073,  2075,  2077,  2079,  2081,  2083,  2085,  2087,  2089,
    2091,  2093,  2095,  2097,  2099,  2101,  2103,  2105,  2107,  2109,
    2111,  2113,  2115,  2117,  2119,  2121,  2123,  2125,  2127,  2129,
    2131,  2133,  2135,  2137,  2139,  2141,  2144,  2147,  2150,  2153,
    2156,  2159,  2162,  2165,  2169,  2173,  2177,  2181,  2185,  2189,
    2193,  2195,  2197,  2199,  2201,  2203,  2205,  2207,  2209,  2211,
    2213,  2215,  2217,  2219,  2221,  2223,  2225,  2227,  2229,  2231,
    2233
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     325,     0,    -1,   326,     3,    -1,     4,   530,    -1,    -1,
     326,   529,    -1,    15,    -1,   314,   463,   315,    -1,    19,
      -1,    33,    19,    22,   327,   316,    -1,    33,    19,    22,
      18,   316,    -1,   327,    -1,    34,    -1,    36,    -1,    35,
      -1,    37,    -1,    -1,    18,    -1,    19,    -1,   330,   333,
      -1,   331,    -1,   330,    -1,    29,   327,    -1,   317,   330,
      -1,    20,    -1,    20,    -1,    -1,    38,   338,   339,   316,
      -1,    57,    20,    -1,    -1,    20,    -1,    39,   337,    -1,
      50,    -1,    40,    -1,   318,   340,   319,    -1,   341,    -1,
     340,   320,   341,    -1,    41,    -1,    42,    -1,    43,    -1,
      44,    -1,    45,    -1,    46,    -1,    47,    -1,    48,    -1,
      49,    -1,   111,    91,   379,   316,    -1,   111,    20,   316,
      -1,   119,    -1,    73,    -1,    56,    -1,    38,    -1,   343,
      20,    -1,   343,    20,   327,    20,    -1,   343,    53,    54,
      -1,   343,    53,    55,    -1,    52,   344,   316,    -1,    58,
     349,     3,    -1,    58,    51,   349,     3,    -1,    59,     3,
      -1,    -1,    20,    -1,   361,    60,    -1,    61,   329,    -1,
      62,    63,   327,    -1,    64,   329,    -1,    65,    66,   329,
      -1,   361,    65,    -1,    62,    63,   327,    -1,    93,   335,
     332,    -1,    60,    68,   327,    -1,    71,    72,   356,   316,
      -1,    15,    -1,    17,    -1,    18,    -1,   100,   101,   358,
     316,    -1,    -1,   358,   359,    -1,   104,   330,    -1,   105,
     327,    -1,   102,    -1,   103,    -1,    68,    19,    -1,    73,
      -1,    74,    -1,    -1,    19,    -1,    19,    86,    19,    -1,
      -1,    75,   327,    -1,    76,   329,    -1,    41,   376,    -1,
     111,   379,    -1,    79,   364,    -1,    80,   364,    -1,    68,
     365,    -1,    71,    72,   356,    -1,   107,    20,    -1,    81,
     427,    -1,   428,    -1,    40,    -1,    82,    -1,    19,    -1,
      41,    91,   376,    -1,    41,    97,   327,    -1,    -1,    20,
      -1,   332,    -1,    20,   332,    -1,    -1,    27,    -1,   367,
     369,   368,    -1,   370,    -1,   371,   320,   370,    -1,   361,
      77,    -1,   372,   362,   318,    -1,   373,   363,   316,    -1,
     373,   375,   316,    -1,    -1,    78,   374,   371,    -1,    40,
      -1,    35,    -1,   318,   377,   319,    -1,   378,    -1,   377,
     320,   378,    -1,    83,    -1,    66,    -1,    84,    -1,    92,
      -1,    89,    -1,    90,    -1,    40,    -1,    35,    -1,   318,
     380,   319,    -1,   381,    -1,   380,   320,   381,    -1,    83,
      -1,   112,    -1,   106,   318,   383,   319,    -1,   384,    -1,
      -1,   384,   316,   383,    -1,   385,   318,   386,   319,    -1,
     385,    -1,    85,    20,    -1,    -1,    99,    86,   334,   316,
     386,    -1,    99,    88,   334,   316,   386,    -1,   135,    86,
     334,   316,   386,    -1,   135,    88,   334,   316,   386,    -1,
      72,   327,   316,   386,    -1,   116,   117,     3,    -1,   116,
     114,     3,    -1,   116,    91,   418,     3,    -1,   116,    91,
      40,   418,     3,    -1,    19,    -1,    -1,   116,    92,     3,
      -1,   116,    92,   118,     3,    -1,   116,   119,   395,     3,
      -1,    -1,   395,   331,    -1,   395,    96,   332,    -1,   395,
      68,    19,    -1,   395,    81,   427,    -1,   395,   428,    -1,
     395,    40,    -1,   395,    93,    -1,   395,   396,    19,    -1,
     395,    73,    19,    -1,   395,    94,    -1,   395,    95,    -1,
      98,    -1,    80,    -1,   116,   120,   391,     3,    -1,   121,
     122,     3,    -1,   121,   123,     3,    -1,   121,    92,     3,
      -1,   121,   124,     3,    -1,   121,   125,     3,    -1,   121,
      66,     3,    -1,   121,    91,     3,    -1,   126,   406,   407,
       3,    -1,    40,    -1,    35,    -1,    15,    -1,    -1,    15,
      -1,   127,   417,     3,    -1,   128,   417,     3,    -1,   129,
     417,     3,    -1,   108,   417,     3,    -1,   108,   109,   417,
       3,    -1,   108,   110,   417,     3,    -1,    41,   417,   376,
       3,    -1,   111,   417,   379,     3,    -1,   113,     3,    -1,
      19,    -1,    40,    -1,    20,    -1,    19,    -1,    -1,    20,
      -1,    -1,    81,    19,   420,   426,    -1,   181,   463,    -1,
     138,    -1,   139,    -1,   140,    -1,   141,    -1,   142,    -1,
     143,    -1,   144,    -1,   146,    -1,   147,    -1,   148,    -1,
     149,    -1,   150,    -1,   422,   145,    -1,   422,    19,    -1,
      -1,   423,   316,   424,    -1,   423,    -1,   423,   316,   425,
      -1,   430,    -1,    19,    -1,   426,    -1,   180,   463,    -1,
     314,   425,   315,    -1,   314,   315,    -1,   424,   318,   433,
     319,    -1,    -1,   130,    19,   432,   429,   430,    -1,    -1,
     434,    -1,   469,    -1,   434,   469,    -1,   469,    -1,   318,
     433,   319,    -1,    18,    -1,   327,    -1,    17,    -1,   436,
      -1,    16,    -1,    15,    -1,   314,   463,   315,    -1,    17,
      -1,   436,    -1,    16,    -1,   463,    -1,   439,    -1,   439,
       7,   439,    -1,    28,    -1,   314,   440,   320,   440,   315,
      -1,   314,   440,   320,   440,   315,     7,   314,   439,   320,
     439,   315,    -1,   463,    -1,   157,    -1,   158,    -1,   159,
      15,    -1,   160,    -1,   314,   443,   320,   442,   320,   442,
     315,    -1,   314,   443,   320,   442,   320,   442,     7,   442,
     315,    -1,   314,   443,   320,   442,   320,    28,   315,    -1,
     441,    -1,   444,    -1,   437,    -1,   437,     7,   437,    -1,
     441,    -1,   444,    -1,   438,    -1,   438,     7,   438,    -1,
     445,    -1,   447,   320,   445,    -1,   446,    -1,   448,   320,
     446,    -1,    18,    29,    15,    -1,   449,    -1,   449,    26,
      -1,   449,    27,    -1,   449,   318,    15,   320,    15,   319,
      -1,   450,    -1,   451,   320,   450,    -1,    -1,   452,   448,
     317,   433,    -1,   452,     6,   433,    -1,   461,    -1,   314,
     463,   315,    -1,    13,   455,    14,    -1,    29,   456,    29,
      -1,    15,   455,    -1,    28,   455,    -1,   321,   455,    -1,
     453,   455,    -1,    -1,    15,   456,    -1,   321,   456,    -1,
      -1,    15,    -1,   155,    -1,   156,    -1,    20,    -1,   436,
      -1,   449,    -1,    17,    -1,   322,   447,   323,    -1,   322,
     451,   323,    -1,    16,    -1,   454,    -1,   314,   463,   320,
     463,   315,    -1,   314,   443,   320,   463,   320,   463,   315,
      -1,    -1,    19,   314,   468,   315,    -1,    19,    -1,    86,
      -1,   161,    -1,   162,    -1,   164,    -1,   165,    -1,   166,
      -1,   167,    -1,   168,    -1,   314,   463,   315,    -1,   463,
      26,   463,    -1,   463,    27,   463,    -1,   463,    28,   463,
      -1,   463,    29,   463,    -1,   463,    11,   463,    -1,   463,
      12,   463,    -1,   463,    22,   463,    -1,   463,    10,   463,
      -1,   463,    23,   463,    -1,   463,     9,   463,    -1,   463,
      24,   463,    -1,   463,     8,   463,    -1,   463,    25,   463,
      -1,    31,   463,    -1,   170,   314,   463,   315,    -1,   461,
      -1,   457,    -1,   458,    -1,    75,    -1,   459,   462,    -1,
     459,   534,    -1,   463,    32,   140,    -1,   463,    32,   169,
      -1,   463,    32,   163,   314,   463,   315,    -1,   463,    32,
     176,    -1,   463,    32,   177,    -1,    26,   179,    26,    -1,
      27,   179,    27,    -1,    27,    27,   179,    27,    27,    -1,
     175,   314,   463,   320,   463,   315,    -1,   171,   314,   463,
     320,   463,   315,    -1,   172,   314,   463,   320,   463,   315,
      -1,    81,   314,   463,   320,   463,   315,    -1,    19,   314,
     468,   315,    -1,   137,    -1,   135,    -1,   136,    -1,    46,
      -1,   131,    -1,   132,    -1,   463,    -1,    -1,   465,    -1,
     465,   320,   466,    -1,   463,    -1,   463,   320,   467,    -1,
      -1,   467,    -1,   151,   463,   152,   435,    -1,   151,   463,
     152,   435,   153,   435,    -1,    19,    22,   463,   316,    -1,
     134,   463,   316,    -1,   459,   534,    22,   463,   316,    -1,
     459,   462,    22,   463,   316,    -1,    75,    22,   463,   316,
      -1,   133,   314,   459,   534,   315,   316,    -1,   464,   466,
     316,    -1,   460,   316,    -1,   154,   463,   318,   452,   319,
      -1,   459,   534,    32,   179,   316,    -1,   459,   534,    32,
     175,   314,   463,   315,   316,    -1,   459,   534,    32,   171,
     314,   463,   315,   316,    -1,   459,   534,    32,   172,   314,
     463,   315,   316,    -1,   459,   534,    32,    81,   314,   463,
     315,   316,    -1,   361,   101,    -1,   470,   362,   318,    -1,
     471,   363,   316,    -1,   471,   182,   184,   327,   316,    -1,
     471,   182,   330,   184,   327,   316,    -1,   471,   183,   330,
     184,   327,   316,    -1,   471,   212,   215,    72,   356,   316,
      -1,   471,   212,   214,   316,    -1,   471,   213,   214,   316,
      -1,   471,   185,    63,   327,   316,    -1,   471,   190,   185,
      63,   327,   316,    -1,   471,   186,   187,    63,   327,   316,
      -1,   471,   188,    63,   327,   316,    -1,   471,   189,   316,
      -1,   471,   189,   327,   316,    -1,   471,   192,   193,   194,
     316,    -1,   471,   223,   224,   194,   316,    -1,   471,   223,
     224,   225,   316,    -1,   471,   223,   224,   226,   316,    -1,
     471,   233,    77,   316,    -1,   471,   233,   234,   316,    -1,
     471,   196,   197,   329,   316,    -1,   471,   235,   197,   329,
     316,    -1,   471,   232,   197,   329,   316,    -1,   471,   221,
     222,   329,   316,    -1,   471,   195,   205,   327,   316,    -1,
     471,   195,   204,   327,   316,    -1,   471,   165,   104,   330,
     316,    -1,   471,   198,   199,    63,   327,   316,    -1,   471,
      46,   200,    63,   327,   316,    -1,   471,    46,   201,    63,
     327,   320,   327,   316,    -1,   471,   127,   202,    46,   329,
     316,    -1,   471,   128,   119,   227,   329,   316,    -1,   471,
     128,   216,   329,   316,    -1,   471,   219,   329,   316,    -1,
     471,   217,   218,   329,   316,    -1,   471,    85,    20,   316,
      -1,   471,   119,   220,   327,   316,    -1,   471,    87,   329,
     316,    -1,   471,   228,   229,   329,   316,    -1,   471,   232,
      68,   365,   316,    -1,   471,   236,   237,   329,   316,    -1,
     361,   239,    -1,   472,   362,   318,    -1,   473,   474,   316,
      -1,   363,    -1,   245,   329,    -1,   283,   329,    -1,   283,
     329,   220,   327,    -1,   247,   327,    -1,   476,    -1,   240,
     356,    -1,   475,   318,   477,   319,    -1,    -1,   477,   478,
     316,    -1,   246,   329,    -1,   285,    -1,   118,   329,    -1,
     195,   285,   329,    -1,   195,   248,   327,    -1,   195,   249,
     327,    -1,   246,   248,   327,    -1,   286,   329,    -1,   286,
     287,   327,    -1,    -1,   270,   479,   318,   490,   319,    -1,
      -1,   281,   480,   318,   490,   319,    -1,   279,   481,    -1,
      78,   501,    -1,   485,    -1,   482,   318,   483,   319,    -1,
     482,    -1,   331,    -1,    -1,   483,   484,   316,    -1,   271,
     329,    -1,   118,   329,    -1,   248,   327,    -1,   488,   318,
     486,   319,    -1,   488,    -1,    -1,   486,   487,   316,    -1,
      -1,   251,   327,    -1,   250,   327,    -1,   252,   199,   327,
      -1,   201,   327,    -1,   254,   327,    -1,   254,    95,   327,
      -1,   265,    82,    -1,   265,   264,    -1,   265,   267,    -1,
     382,    -1,   272,   274,   356,    -1,   248,   327,    -1,   251,
     327,    -1,   269,   327,    -1,   250,   327,    -1,   201,   327,
      -1,   254,   327,    -1,   254,    95,   327,    -1,   255,   256,
      -1,   255,   257,    -1,   255,   258,    -1,   255,   259,    -1,
     255,   260,    -1,   255,   261,    -1,   255,   262,    -1,   255,
     263,    -1,   252,   199,   327,    -1,   253,   327,    -1,   266,
     258,   329,    -1,   246,   329,    -1,   273,   274,   329,    -1,
     283,   284,   327,    -1,   124,   318,   494,   319,    -1,   265,
      82,    -1,   265,   264,    -1,   265,   267,    -1,   275,   276,
     277,    -1,   275,   276,   278,    -1,   275,   276,   327,    -1,
     382,    -1,    -1,   490,   491,    -1,   492,   493,   316,    -1,
     331,    -1,    -1,   271,    -1,   280,   327,    -1,    -1,   494,
     495,    -1,   496,    -1,   497,    -1,    18,   316,    -1,    18,
     268,   316,    -1,    -1,    -1,   499,   489,   316,    -1,    -1,
     318,   499,   319,    -1,   498,   371,   500,    -1,    20,    -1,
      -1,   116,   239,   391,     3,    -1,   116,   239,   124,   391,
     502,     3,    -1,   116,   239,    78,   391,   502,     3,    -1,
     116,   239,   288,   391,   502,     3,    -1,   116,   239,   288,
      40,   391,   502,     3,    -1,   116,   239,   289,   391,   502,
       3,    -1,   116,   239,   289,    40,   391,   502,     3,    -1,
     116,   239,   282,   391,   502,     3,    -1,   361,   290,    -1,
     511,   362,   318,    -1,   512,   363,   316,    -1,   512,   291,
      68,    19,   316,    -1,   512,   292,   293,   316,    -1,   512,
     292,   294,   316,    -1,   361,   295,    -1,   513,   362,   318,
      -1,   514,   363,   316,    -1,   514,   296,   327,   316,    -1,
     514,   105,   327,   316,    -1,   514,   297,   327,   316,    -1,
     514,   298,    63,   327,   316,    -1,   514,   299,    63,   327,
     316,    -1,   514,   265,   515,   316,    -1,   514,   382,   316,
      -1,   514,   306,   308,   316,    -1,   514,   306,   183,   316,
      -1,   514,   306,   307,   316,    -1,   514,    78,   521,   316,
      -1,   304,    -1,   305,    -1,    82,    -1,   256,    -1,   300,
      -1,   301,    -1,   302,    -1,   303,    -1,    -1,   197,   327,
      -1,   292,   516,    -1,    -1,   518,   517,   316,    -1,    -1,
     318,   518,   319,    -1,    -1,   520,   371,   519,    -1,   361,
     311,    -1,   522,   362,   318,    -1,   523,   363,   316,    -1,
     523,   273,   274,   329,   316,    -1,   523,   232,    68,   365,
     316,    -1,   523,   527,   316,    -1,   119,   331,    -1,   191,
     330,    -1,   525,   284,   327,    -1,   525,    -1,   526,   525,
      -1,   524,   191,   330,    -1,   524,   191,    20,    -1,   524,
     313,   526,    -1,   524,   234,   330,    -1,   524,   225,    -1,
     524,   136,    -1,   524,   312,    -1,   116,   311,   391,     3,
      -1,   316,    -1,   328,    -1,   336,    -1,   342,    -1,   345,
      -1,   355,    -1,   357,    -1,   360,    -1,   531,    -1,   366,
      -1,   419,    -1,   421,    -1,   431,    -1,   346,    -1,   347,
      -1,   348,    -1,   387,    -1,   388,    -1,   389,    -1,   390,
      -1,   392,    -1,   393,    -1,   394,    -1,   397,    -1,   398,
      -1,   399,    -1,   400,    -1,   401,    -1,   402,    -1,   403,
      -1,   404,    -1,   405,    -1,   408,    -1,   409,    -1,   410,
      -1,   411,    -1,   412,    -1,   413,    -1,   414,    -1,   415,
      -1,   416,    -1,   503,    -1,   504,    -1,   505,    -1,   506,
      -1,   507,    -1,   508,    -1,   509,    -1,   510,    -1,   528,
      -1,   532,   319,    -1,   533,   319,    -1,   373,   319,    -1,
     471,   319,    -1,   473,   319,    -1,   512,   319,    -1,   514,
     319,    -1,   523,   319,    -1,   350,   362,   318,    -1,   532,
     363,   316,    -1,   532,   351,   316,    -1,   532,   354,   316,
      -1,   352,   362,   318,    -1,   533,   363,   316,    -1,   533,
     353,   316,    -1,    69,    -1,    70,    -1,   115,    -1,     5,
      -1,   206,    -1,   203,    -1,   207,    -1,   205,    -1,   204,
      -1,   208,    -1,   209,    -1,   210,    -1,   230,    -1,   231,
      -1,   211,    -1,   241,    -1,   242,    -1,   243,    -1,   244,
      -1,   309,    -1,   310,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   618,   618,   619,   622,   624,   631,   632,   633,   639,
     643,   652,   653,   654,   655,   656,   657,   663,   664,   671,
     678,   679,   683,   687,   694,   702,   703,   709,   718,   719,
     723,   728,   729,   733,   734,   738,   739,   743,   744,   745,
     746,   747,   748,   749,   750,   751,   757,   758,   767,   768,
     769,   770,   773,   774,   775,   776,   780,   787,   790,   793,
     797,   798,   806,   822,   823,   827,   834,   840,   853,   857,
     870,   883,   889,   890,   891,   902,   904,   906,   910,   911,
     912,   913,   920,   929,   930,   934,   940,   944,   954,   956,
     960,   961,   962,   963,   964,   965,   966,   967,   971,   972,
     973,   974,   978,   986,   987,   995,  1003,  1004,  1005,  1009,
    1010,  1014,  1019,  1020,  1027,  1035,  1036,  1037,  1041,  1049,
    1055,  1056,  1057,  1061,  1062,  1066,  1067,  1068,  1069,  1070,
    1071,  1077,  1078,  1079,  1083,  1084,  1088,  1089,  1095,  1096,
    1099,  1101,  1105,  1106,  1110,  1128,  1129,  1130,  1131,  1132,
    1133,  1141,  1144,  1147,  1150,  1154,  1155,  1158,  1161,  1164,
    1168,  1174,  1180,  1187,  1192,  1197,  1202,  1206,  1210,  1220,
    1228,  1232,  1239,  1240,  1243,  1247,  1249,  1251,  1253,  1255,
    1257,  1259,  1262,  1268,  1269,  1270,  1274,  1275,  1281,  1283,
    1285,  1287,  1289,  1291,  1295,  1299,  1302,  1306,  1307,  1308,
    1312,  1313,  1314,  1322,  1322,  1332,  1336,  1337,  1338,  1339,
    1340,  1341,  1342,  1343,  1344,  1345,  1346,  1347,  1348,  1369,
    1380,  1381,  1388,  1389,  1396,  1405,  1409,  1413,  1437,  1438,
    1442,  1455,  1455,  1471,  1472,  1475,  1476,  1480,  1483,  1492,
    1505,  1506,  1507,  1508,  1512,  1513,  1514,  1515,  1516,  1520,
    1523,  1524,  1525,  1529,  1532,  1541,  1544,  1545,  1546,  1547,
    1551,  1552,  1553,  1557,  1558,  1559,  1560,  1564,  1565,  1566,
    1567,  1571,  1572,  1576,  1577,  1581,  1588,  1589,  1590,  1591,
    1598,  1599,  1602,  1603,  1610,  1622,  1623,  1627,  1628,  1632,
    1633,  1634,  1635,  1636,  1640,  1641,  1642,  1646,  1647,  1648,
    1649,  1650,  1651,  1652,  1653,  1654,  1655,  1656,  1660,  1661,
    1671,  1675,  1698,  1734,  1736,  1737,  1738,  1739,  1740,  1741,
    1742,  1746,  1747,  1748,  1749,  1750,  1751,  1752,  1753,  1754,
    1755,  1756,  1757,  1758,  1759,  1760,  1761,  1763,  1764,  1765,
    1767,  1769,  1771,  1773,  1774,  1775,  1776,  1777,  1787,  1788,
    1789,  1790,  1791,  1792,  1793,  1798,  1821,  1822,  1823,  1824,
    1825,  1826,  1830,  1833,  1834,  1835,  1844,  1851,  1860,  1861,
    1865,  1871,  1881,  1890,  1896,  1901,  1908,  1913,  1919,  1920,
    1921,  1929,  1930,  1931,  1932,  1933,  1939,  1961,  1962,  1963,
    1964,  1965,  1971,  1972,  1973,  1974,  1975,  1976,  1977,  1978,
    1979,  1980,  1981,  1982,  1983,  1984,  1985,  1986,  1987,  1988,
    1989,  1990,  1991,  1992,  1993,  1994,  1995,  1996,  1997,  1998,
    1999,  2000,  2001,  2002,  2003,  2004,  2005,  2006,  2017,  2028,
    2029,  2033,  2034,  2035,  2036,  2037,  2038,  2041,  2056,  2059,
    2061,  2065,  2066,  2067,  2068,  2069,  2070,  2071,  2072,  2073,
    2074,  2074,  2075,  2075,  2076,  2077,  2078,  2082,  2083,  2087,
    2095,  2097,  2101,  2102,  2103,  2107,  2108,  2111,  2113,  2116,
    2117,  2118,  2119,  2120,  2121,  2122,  2123,  2124,  2125,  2126,
    2129,  2151,  2152,  2153,  2154,  2155,  2156,  2157,  2158,  2159,
    2160,  2161,  2162,  2163,  2164,  2165,  2166,  2167,  2168,  2169,
    2170,  2171,  2172,  2173,  2174,  2175,  2176,  2177,  2178,  2179,
    2182,  2184,  2187,  2189,  2198,  2200,  2201,  2204,  2206,  2210,
    2211,  2213,  2222,  2233,  2253,  2255,  2258,  2260,  2264,  2268,
    2269,  2273,  2276,  2279,  2284,  2287,  2292,  2295,  2298,  2304,
    2312,  2313,  2314,  2319,  2320,  2326,  2333,  2334,  2335,  2336,
    2337,  2338,  2339,  2340,  2341,  2342,  2343,  2344,  2345,  2349,
    2350,  2351,  2356,  2357,  2358,  2359,  2360,  2363,  2364,  2365,
    2368,  2370,  2373,  2375,  2379,  2388,  2395,  2402,  2403,  2404,
    2405,  2406,  2409,  2418,  2425,  2432,  2433,  2437,  2441,  2447,
    2450,  2454,  2455,  2456,  2459,  2466,  2466,  2466,  2466,  2466,
    2466,  2466,  2466,  2466,  2466,  2466,  2466,  2466,  2467,  2467,
    2467,  2467,  2467,  2467,  2467,  2467,  2467,  2467,  2467,  2467,
    2467,  2467,  2467,  2467,  2467,  2467,  2467,  2467,  2467,  2467,
    2467,  2467,  2467,  2467,  2467,  2467,  2467,  2467,  2467,  2467,
    2467,  2467,  2467,  2467,  2467,  2468,  2468,  2468,  2468,  2468,
    2468,  2468,  2468,  2469,  2469,  2469,  2469,  2470,  2470,  2470,
    2471,  2471,  2471,  2472,  2472,  2473,  2474,  2475,  2476,  2477,
    2478,  2479,  2480,  2481,  2482,  2483,  2483,  2483,  2483,  2483,
    2483
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "END", "CLI_MARKER", "INVALID_TOKEN",
  "ELSECOL", "DDOT", "GEQ", "LEQ", "NEQ", "AND", "OR", "PO", "PC", "NUM",
  "ENUM", "RTRID", "IPA", "SYM", "TEXT", "PREFIX_DUMMY", "'='", "'<'",
  "'>'", "'~'", "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'.'", "DEFINE",
  "ON", "OFF", "YES", "NO", "LOG", "SYSLOG", "ALL", "DEBUG", "TRACE",
  "INFO", "REMOTE", "WARNING", "ERROR", "AUTH", "FATAL", "BUG", "STDERR",
  "SOFT", "TIMEFORMAT", "ISO", "SHORT", "LONG", "BASE", "NAME",
  "CONFIGURE", "DOWN", "KERNEL", "PERSIST", "SCAN", "TIME", "LEARN",
  "DEVICE", "ROUTES", "ASYNC", "TABLE", "KRT_PREFSRC", "KRT_REALM",
  "ROUTER", "ID", "PROTOCOL", "TEMPLATE", "PREFERENCE", "DISABLED",
  "DIRECT", "INTERFACE", "IMPORT", "EXPORT", "FILTER", "NONE", "STATES",
  "FILTERS", "PASSWORD", "FROM", "PASSIVE", "TO", "EVENTS", "PACKETS",
  "PROTOCOLS", "INTERFACES", "PRIMARY", "STATS", "COUNT", "FOR",
  "COMMANDS", "PREEXPORT", "GENERATE", "LISTEN", "BGP", "V6ONLY", "DUAL",
  "ADDRESS", "PORT", "PASSWORDS", "DESCRIPTION", "RELOAD", "IN", "OUT",
  "MRTDUMP", "MESSAGES", "RESTRICT", "MEMORY", "IGP_METRIC", "SHOW",
  "STATUS", "SUMMARY", "ROUTE", "SYMBOLS", "DUMP", "RESOURCES", "SOCKETS",
  "NEIGHBORS", "ATTRIBUTES", "ECHO", "DISABLE", "ENABLE", "RESTART",
  "FUNCTION", "PRINT", "PRINTN", "UNSET", "RETURN", "ACCEPT", "REJECT",
  "QUITBIRD", "INT", "BOOL", "IP", "PREFIX", "PAIR", "QUAD", "EC", "SET",
  "STRING", "BGPMASK", "BGPPATH", "CLIST", "ECLIST", "IF", "THEN", "ELSE",
  "CASE", "TRUE", "FALSE", "RT", "RO", "UNKNOWN", "GENERIC", "GW", "NET",
  "MASK", "PROTO", "SOURCE", "SCOPE", "CAST", "DEST", "LEN", "DEFINED",
  "ADD", "DELETE", "CONTAINS", "RESET", "PREPEND", "FIRST", "LAST",
  "MATCH", "EMPTY", "WHERE", "EVAL", "LOCAL", "NEIGHBOR", "AS", "HOLD",
  "CONNECT", "RETRY", "KEEPALIVE", "MULTIHOP", "STARTUP", "VIA", "NEXT",
  "HOP", "SELF", "DEFAULT", "PATH", "METRIC", "START", "DELAY", "FORGET",
  "WAIT", "AFTER", "BGP_PATH", "BGP_LOCAL_PREF", "BGP_MED", "BGP_ORIGIN",
  "BGP_NEXT_HOP", "BGP_ATOMIC_AGGR", "BGP_AGGREGATOR", "BGP_COMMUNITY",
  "BGP_EXT_COMMUNITY", "RR", "RS", "CLIENT", "CLUSTER", "AS4", "ADVERTISE",
  "IPV4", "CAPABILITIES", "LIMIT", "PREFER", "OLDER", "MISSING", "LLADDR",
  "DROP", "IGNORE", "REFRESH", "INTERPRET", "COMMUNITIES",
  "BGP_ORIGINATOR_ID", "BGP_CLUSTER_LIST", "IGP", "GATEWAY", "RECURSIVE",
  "MED", "TTL", "SECURITY", "BGPSEC", "OSPF", "AREA", "OSPF_METRIC1",
  "OSPF_METRIC2", "OSPF_TAG", "OSPF_ROUTER_ID", "RFC1583COMPAT", "STUB",
  "TICK", "COST", "COST2", "RETRANSMIT", "HELLO", "TRANSMIT", "PRIORITY",
  "DEAD", "TYPE", "BROADCAST", "BCAST", "NONBROADCAST", "NBMA",
  "POINTOPOINT", "PTP", "POINTOMULTIPOINT", "PTMP", "SIMPLE",
  "AUTHENTICATION", "STRICT", "CRYPTOGRAPHIC", "ELIGIBLE", "POLL",
  "NETWORKS", "HIDDEN", "VIRTUAL", "CHECK", "LINK", "RX", "BUFFER",
  "LARGE", "NORMAL", "STUBNET", "TAG", "EXTERNAL", "LSADB", "ECMP",
  "WEIGHT", "NSSA", "TRANSLATOR", "STABILITY", "TOPOLOGY", "STATE", "PIPE",
  "PEER", "MODE", "OPAQUE", "TRANSPARENT", "RIP", "INFINITY", "PERIOD",
  "GARBAGE", "TIMEOUT", "MULTICAST", "QUIET", "NOLISTEN", "VERSION1",
  "PLAINTEXT", "MD5", "HONOR", "NEVER", "ALWAYS", "RIP_METRIC", "RIP_TAG",
  "STATIC", "PROHIBIT", "MULTIPATH", "'('", "')'", "';'", "':'", "'{'",
  "'}'", "','", "'?'", "'['", "']'", "$accept", "config", "conf_entries",
  "expr", "definition", "bool", "ipa", "prefix", "prefix_or_ipa", "pxlen",
  "datetime", "text_or_none", "log_config", "syslog_name", "log_file",
  "log_mask", "log_mask_list", "log_cat", "mrtdump_base",
  "timeformat_which", "timeformat_spec", "timeformat_base",
  "cmd_CONFIGURE", "cmd_CONFIGURE_SOFT", "cmd_DOWN", "cfg_name",
  "kern_proto_start", "kern_item", "kif_proto_start", "kif_item",
  "nl_item", "rtrid", "idval", "listen", "listen_opts", "listen_opt",
  "newtab", "proto_start", "proto_name", "proto_item", "imexport",
  "rtable", "debug_default", "iface_patt_node_init",
  "iface_patt_node_body", "iface_negate", "iface_patt_node",
  "iface_patt_list", "dev_proto_start", "dev_proto", "dev_iface_init",
  "dev_iface_patt", "debug_mask", "debug_list", "debug_flag",
  "mrtdump_mask", "mrtdump_list", "mrtdump_flag", "password_list",
  "password_items", "password_item", "password_item_begin",
  "password_item_params", "cmd_SHOW_STATUS", "cmd_SHOW_MEMORY",
  "cmd_SHOW_PROTOCOLS", "cmd_SHOW_PROTOCOLS_ALL", "optsym",
  "cmd_SHOW_INTERFACES", "cmd_SHOW_INTERFACES_SUMMARY", "cmd_SHOW_ROUTE",
  "r_args", "export_or_preexport", "cmd_SHOW_SYMBOLS",
  "cmd_DUMP_RESOURCES", "cmd_DUMP_SOCKETS", "cmd_DUMP_INTERFACES",
  "cmd_DUMP_NEIGHBORS", "cmd_DUMP_ATTRIBUTES", "cmd_DUMP_ROUTES",
  "cmd_DUMP_PROTOCOLS", "cmd_ECHO", "echo_mask", "echo_size",
  "cmd_DISABLE", "cmd_ENABLE", "cmd_RESTART", "cmd_RELOAD",
  "cmd_RELOAD_IN", "cmd_RELOAD_OUT", "cmd_DEBUG", "cmd_MRTDUMP",
  "cmd_RESTRICT", "proto_patt", "proto_patt2", "filter_def", "$@1",
  "filter_eval", "type", "one_decl", "decls", "declsn", "filter_body",
  "filter", "where_filter", "function_params", "function_body",
  "function_def", "$@2", "cmds", "cmds_int", "block", "fipa", "set_atom",
  "switch_atom", "pair_expr", "pair_atom", "pair_item", "ec_expr",
  "ec_kind", "ec_item", "set_item", "switch_item", "set_items",
  "switch_items", "fprefix_s", "fprefix", "fprefix_set", "switch_body",
  "bgp_path_expr", "bgp_path", "bgp_path_tail1", "bgp_path_tail2",
  "constant", "constructor", "rtadot", "function_call", "symbol",
  "static_attr", "term", "break_command", "print_one", "print_list",
  "var_listn", "var_list", "cmd", "bgp_proto_start", "bgp_proto",
  "ospf_proto_start", "ospf_proto", "ospf_proto_item", "ospf_area_start",
  "ospf_area", "ospf_area_opts", "ospf_area_item", "$@3", "$@4",
  "ospf_stubnet", "ospf_stubnet_start", "ospf_stubnet_opts",
  "ospf_stubnet_item", "ospf_vlink", "ospf_vlink_opts", "ospf_vlink_item",
  "ospf_vlink_start", "ospf_iface_item", "pref_list", "pref_item",
  "pref_base", "pref_opt", "ipa_list", "ipa_item", "ipa_el", "ipa_ne",
  "ospf_iface_start", "ospf_iface_opts", "ospf_iface_opt_list",
  "ospf_iface", "opttext", "cmd_SHOW_OSPF", "cmd_SHOW_OSPF_NEIGHBORS",
  "cmd_SHOW_OSPF_INTERFACE", "cmd_SHOW_OSPF_TOPOLOGY",
  "cmd_SHOW_OSPF_TOPOLOGY_ALL", "cmd_SHOW_OSPF_STATE",
  "cmd_SHOW_OSPF_STATE_ALL", "cmd_SHOW_OSPF_LSADB", "pipe_proto_start",
  "pipe_proto", "rip_cfg_start", "rip_cfg", "rip_auth", "rip_mode",
  "rip_iface_item", "rip_iface_opts", "rip_iface_opt_list",
  "rip_iface_init", "rip_iface", "static_proto_start", "static_proto",
  "stat_route0", "stat_multipath1", "stat_multipath", "stat_route",
  "cmd_SHOW_STATIC", "conf", "cli_cmd", "proto", "kern_proto", "kif_proto",
  "dynamic_attr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,    61,    60,    62,   126,    43,    45,    42,    47,
      37,    33,    46,   277,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   289,   290,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345,   346,   347,   348,   349,   350,   351,   352,   353,
     354,   355,   356,   357,   358,   359,   360,   361,   362,   363,
     364,   365,   366,   367,   368,   369,   370,   371,   372,   373,
     374,   375,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   396,   397,   398,   399,   400,   401,   402,   403,
     404,   405,   406,   407,   408,   409,   410,   411,   412,   413,
     414,   415,   416,   417,   418,   419,   420,   421,   422,   423,
     424,   425,   426,   427,   428,   429,   430,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   461,   462,   463,
     464,   465,   466,   467,   468,   469,   470,   471,   472,   473,
     474,   475,   476,   477,   478,   479,   480,   481,   482,   483,
     484,   485,   486,   487,   488,   489,   490,   491,   492,   493,
     494,   495,   496,   497,   498,   499,   500,   501,   502,   503,
     504,   505,   506,   507,   508,   509,   510,   511,   512,   513,
     514,   515,   516,   517,   518,   519,   520,   521,   522,   523,
     524,   525,   526,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,   540,   541,   542,   543,
     544,   545,   546,   547,   548,   549,   550,   551,   552,   553,
     554,   555,   556,   557,    40,    41,    59,    58,   123,   125,
      44,    63,    91,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   324,   325,   325,   326,   326,   327,   327,   327,   328,
     328,   329,   329,   329,   329,   329,   329,   330,   330,   331,
     332,   332,   333,   333,   334,   335,   335,   336,   337,   337,
     338,   338,   338,   339,   339,   340,   340,   341,   341,   341,
     341,   341,   341,   341,   341,   341,   342,   342,   343,   343,
     343,   343,   344,   344,   344,   344,   345,   346,   347,   348,
     349,   349,   350,   351,   351,   351,   351,   352,   353,   353,
     354,   355,   356,   356,   356,   357,   358,   358,   359,   359,
     359,   359,   360,   361,   361,   362,   362,   362,   363,   363,
     363,   363,   363,   363,   363,   363,   363,   363,   364,   364,
     364,   364,   365,   366,   366,   367,   368,   368,   368,   369,
     369,   370,   371,   371,   372,   373,   373,   373,   374,   375,
     376,   376,   376,   377,   377,   378,   378,   378,   378,   378,
     378,   379,   379,   379,   380,   380,   381,   381,   382,   382,
     383,   383,   384,   384,   385,   386,   386,   386,   386,   386,
     386,   387,   388,   389,   390,   391,   391,   392,   393,   394,
     395,   395,   395,   395,   395,   395,   395,   395,   395,   395,
     395,   395,   396,   396,   397,   398,   399,   400,   401,   402,
     403,   404,   405,   406,   406,   406,   407,   407,   408,   409,
     410,   411,   412,   413,   414,   415,   416,   417,   417,   417,
     418,   418,   418,   420,   419,   421,   422,   422,   422,   422,
     422,   422,   422,   422,   422,   422,   422,   422,   422,   423,
     424,   424,   425,   425,   426,   427,   427,   428,   429,   429,
     430,   432,   431,   433,   433,   434,   434,   435,   435,   436,
     437,   437,   437,   437,   438,   438,   438,   438,   438,   439,
     440,   440,   440,   441,   441,   442,   443,   443,   443,   443,
     444,   444,   444,   445,   445,   445,   445,   446,   446,   446,
     446,   447,   447,   448,   448,   449,   450,   450,   450,   450,
     451,   451,   452,   452,   452,   453,   453,   454,   454,   455,
     455,   455,   455,   455,   456,   456,   456,   457,   457,   457,
     457,   457,   457,   457,   457,   457,   457,   457,   458,   458,
     459,   460,   461,   462,   462,   462,   462,   462,   462,   462,
     462,   463,   463,   463,   463,   463,   463,   463,   463,   463,
     463,   463,   463,   463,   463,   463,   463,   463,   463,   463,
     463,   463,   463,   463,   463,   463,   463,   463,   463,   463,
     463,   463,   463,   463,   463,   463,   464,   464,   464,   464,
     464,   464,   465,   466,   466,   466,   467,   467,   468,   468,
     469,   469,   469,   469,   469,   469,   469,   469,   469,   469,
     469,   469,   469,   469,   469,   469,   470,   471,   471,   471,
     471,   471,   471,   471,   471,   471,   471,   471,   471,   471,
     471,   471,   471,   471,   471,   471,   471,   471,   471,   471,
     471,   471,   471,   471,   471,   471,   471,   471,   471,   471,
     471,   471,   471,   471,   471,   471,   471,   471,   472,   473,
     473,   474,   474,   474,   474,   474,   474,   475,   476,   477,
     477,   478,   478,   478,   478,   478,   478,   478,   478,   478,
     479,   478,   480,   478,   478,   478,   478,   481,   481,   482,
     483,   483,   484,   484,   484,   485,   485,   486,   486,   487,
     487,   487,   487,   487,   487,   487,   487,   487,   487,   487,
     488,   489,   489,   489,   489,   489,   489,   489,   489,   489,
     489,   489,   489,   489,   489,   489,   489,   489,   489,   489,
     489,   489,   489,   489,   489,   489,   489,   489,   489,   489,
     490,   490,   491,   492,   493,   493,   493,   494,   494,   495,
     495,   496,   497,   498,   499,   499,   500,   500,   501,   502,
     502,   503,   504,   505,   506,   507,   508,   509,   510,   511,
     512,   512,   512,   512,   512,   513,   514,   514,   514,   514,
     514,   514,   514,   514,   514,   514,   514,   514,   514,   515,
     515,   515,   516,   516,   516,   516,   516,   517,   517,   517,
     518,   518,   519,   519,   520,   521,   522,   523,   523,   523,
     523,   523,   524,   525,   525,   526,   526,   527,   527,   527,
     527,   527,   527,   527,   528,   529,   529,   529,   529,   529,
     529,   529,   529,   529,   529,   529,   529,   529,   530,   530,
     530,   530,   530,   530,   530,   530,   530,   530,   530,   530,
     530,   530,   530,   530,   530,   530,   530,   530,   530,   530,
     530,   530,   530,   530,   530,   530,   530,   530,   530,   530,
     530,   530,   530,   530,   530,   531,   531,   531,   531,   531,
     531,   531,   531,   532,   532,   532,   532,   533,   533,   533,
     534,   534,   534,   534,   534,   534,   534,   534,   534,   534,
     534,   534,   534,   534,   534,   534,   534,   534,   534,   534,
     534
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     0,     2,     1,     3,     1,     5,
       5,     1,     1,     1,     1,     1,     0,     1,     1,     2,
       1,     1,     2,     2,     1,     1,     0,     4,     2,     0,
       1,     2,     1,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     3,     1,     1,
       1,     1,     2,     4,     3,     3,     3,     3,     4,     2,
       0,     1,     2,     2,     3,     2,     3,     2,     3,     3,
       3,     4,     1,     1,     1,     4,     0,     2,     2,     2,
       1,     1,     2,     1,     1,     0,     1,     3,     0,     2,
       2,     2,     2,     2,     2,     2,     3,     2,     2,     1,
       1,     1,     1,     3,     3,     0,     1,     1,     2,     0,
       1,     3,     1,     3,     2,     3,     3,     3,     0,     3,
       1,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     3,     1,     1,     4,     1,
       0,     3,     4,     1,     2,     0,     5,     5,     5,     5,
       4,     3,     3,     4,     5,     1,     0,     3,     4,     4,
       0,     2,     3,     3,     3,     2,     2,     2,     3,     3,
       2,     2,     1,     1,     4,     3,     3,     3,     3,     3,
       3,     3,     4,     1,     1,     1,     0,     1,     3,     3,
       3,     3,     4,     4,     4,     4,     2,     1,     1,     1,
       1,     0,     1,     0,     4,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       0,     3,     1,     3,     1,     1,     1,     2,     3,     2,
       4,     0,     5,     0,     1,     1,     2,     1,     3,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     3,     1,     5,    11,     1,     1,     1,     2,     1,
       7,     9,     7,     1,     1,     1,     3,     1,     1,     1,
       3,     1,     3,     1,     3,     3,     1,     2,     2,     6,
       1,     3,     0,     4,     3,     1,     3,     3,     3,     2,
       2,     2,     2,     0,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     1,     1,     5,     7,
       0,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     4,     1,     1,     1,
       1,     2,     2,     3,     3,     6,     3,     3,     3,     3,
       5,     6,     6,     6,     6,     4,     1,     1,     1,     1,
       1,     1,     1,     0,     1,     3,     1,     3,     0,     1,
       4,     6,     4,     3,     5,     5,     4,     6,     3,     2,
       5,     5,     8,     8,     8,     8,     2,     3,     3,     5,
       6,     6,     6,     4,     4,     5,     6,     6,     5,     3,
       4,     5,     5,     5,     5,     4,     4,     5,     5,     5,
       5,     5,     5,     5,     6,     6,     8,     6,     6,     5,
       4,     5,     4,     5,     4,     5,     5,     5,     2,     3,
       3,     1,     2,     2,     4,     2,     1,     2,     4,     0,
       3,     2,     1,     2,     3,     3,     3,     3,     2,     3,
       0,     5,     0,     5,     2,     2,     1,     4,     1,     1,
       0,     3,     2,     2,     2,     4,     1,     0,     3,     0,
       2,     2,     3,     2,     2,     3,     2,     2,     2,     1,
       3,     2,     2,     2,     2,     2,     2,     3,     2,     2,
       2,     2,     2,     2,     2,     2,     3,     2,     3,     2,
       3,     3,     4,     2,     2,     2,     3,     3,     3,     1,
       0,     2,     3,     1,     0,     1,     2,     0,     2,     1,
       1,     2,     3,     0,     0,     3,     0,     3,     3,     1,
       0,     4,     6,     6,     6,     7,     6,     7,     6,     2,
       3,     3,     5,     4,     4,     2,     3,     3,     4,     4,
       4,     5,     5,     4,     3,     4,     4,     4,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     2,     2,
       0,     3,     0,     3,     0,     3,     2,     3,     3,     5,
       5,     3,     2,     2,     3,     1,     2,     3,     3,     3,
       3,     2,     2,     2,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       4,     0,     0,     0,     0,    60,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   608,   609,   610,   611,
     612,   613,   614,   615,   616,   617,   618,   619,   620,   621,
     622,   623,   624,   625,   626,   627,   628,   629,   630,   631,
     632,   633,   634,   635,   636,   637,   638,   639,   640,   641,
     642,   643,   644,     3,     1,     2,     0,     0,     0,     0,
       0,     0,    83,    84,     0,     0,     0,     0,   310,   595,
     596,   597,   598,   599,    85,    85,   600,   601,   602,     0,
     604,    85,    88,   605,   606,   607,    85,    88,    85,    88,
      85,    88,    85,    88,    85,    88,     5,   603,    88,    88,
     197,   199,   198,     0,    61,    60,     0,    59,     0,     0,
       0,     0,   196,   201,     0,     0,     0,   160,   156,   156,
     156,     0,     0,     0,     0,     0,     0,     0,   185,   184,
     183,   186,     0,     0,     0,     0,    30,    29,    32,     0,
       0,     0,    51,    50,    49,    48,     0,     0,    82,     0,
     203,    76,     0,     0,   231,   293,   297,   306,   303,   239,
     312,   300,     0,     0,   296,   310,   340,     0,   298,   299,
       0,     0,     0,     0,   310,     0,   301,   302,   307,   338,
     339,     0,   337,   205,    86,     0,     0,    62,    67,   114,
     386,   428,   539,   545,   576,     0,     0,     0,     0,     0,
      16,   118,     0,     0,     0,     0,   647,     0,     0,     0,
       0,     0,    16,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    16,     0,     0,     0,     0,     0,     0,     0,   648,
       0,     0,     0,    16,     0,    16,   649,   431,     0,     0,
     436,     0,     0,     0,   650,     0,     0,   574,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   651,     0,     0,
     139,   143,     0,     0,     0,     0,   652,     0,     0,     0,
       0,    16,     0,    16,     0,   645,     0,     0,     0,     0,
      26,   646,     0,     0,   121,   120,     0,     0,     0,    57,
       0,     0,   191,   132,   131,     0,     0,   200,   202,   201,
       0,   157,     0,   152,   151,     0,   155,     0,   156,   156,
     156,   156,   156,     0,     0,   180,   181,   177,   175,   176,
     178,   179,   187,     0,   188,   189,   190,     0,     0,    31,
      33,     0,     0,   103,     6,     8,   310,   104,    52,     0,
      56,    72,    73,    74,     0,   220,     0,    47,     0,     0,
     293,   312,   293,   310,   293,   293,     0,   285,     0,   310,
       0,     0,     0,   296,   296,     0,   335,   310,   310,   310,
     310,   310,   256,   257,     0,   259,     0,     0,   243,   241,
     310,   240,   242,   265,   263,   264,   271,     0,   276,   280,
       0,   663,   660,   661,   313,   662,   314,   315,   316,   317,
     318,   319,   320,   665,   668,   667,   664,   666,   669,   670,
     671,   674,   672,   673,   675,   676,   677,   678,   679,   680,
     341,   342,   310,   310,   310,   310,   310,   310,   310,   310,
     310,   310,   310,   310,   310,     0,     0,   653,   657,   115,
      91,   102,    95,     0,    89,    12,    14,    13,    15,    11,
      90,   105,   100,   220,   101,   310,    93,    99,    94,    97,
      92,   116,   117,   387,     0,     0,     0,     0,     0,     0,
       0,    16,     0,    17,    18,     0,     0,     0,     0,     0,
       0,   399,     0,     0,     0,     0,     0,    16,     0,     0,
       0,     0,    16,     0,    16,     0,    16,     0,    16,     0,
       0,    16,    16,   388,   429,   437,   432,   435,   433,   430,
     439,   540,     0,     0,     0,   541,   546,   105,     0,   144,
       0,   140,   561,   559,   560,     0,     0,     0,     0,     0,
       0,     0,     0,   547,   554,   145,   577,     0,   582,     0,
      16,   578,   592,     0,   591,     0,   593,     0,   581,     0,
      63,     0,    65,    16,   655,   656,   654,     0,    25,     0,
     659,   658,   126,   125,   127,   129,   130,   128,     0,   123,
     194,    58,   192,   193,   136,   137,     0,   134,   195,     0,
     153,   158,   159,   166,     0,     0,   173,   220,   167,   170,
     171,     0,   172,   161,     0,   165,   174,   530,   530,   530,
     156,   530,   156,   530,   531,   594,   182,     0,     0,    28,
      37,    38,    39,    40,    41,    42,    43,    44,    45,     0,
      35,    27,     0,     0,    54,    55,    71,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,     0,
       0,     0,   204,   224,    80,    81,     0,     0,    75,    77,
      46,     0,   220,   289,   290,     0,   291,   292,   287,   275,
     366,   369,     0,   348,     0,   349,   294,   295,   288,     0,
       0,     0,     0,     0,   258,   310,   321,   310,   252,   250,
       0,     0,   249,     0,     0,   304,   277,   278,     0,     0,
     305,   333,   331,   329,   326,   327,   328,   330,   332,   334,
     322,   323,   324,   325,   343,     0,   344,   346,   347,    87,
      96,   109,   112,   119,   225,   226,    98,   227,     0,     0,
     422,   424,     0,    16,    16,     0,     0,     0,     0,     0,
       0,     0,     0,   400,     0,     0,     0,     0,     0,     0,
     393,     0,   394,     0,   420,     0,     0,     0,     0,     0,
       0,     0,   405,   406,     0,     0,     0,     0,     0,   543,
     544,   572,   558,   549,     0,     0,   553,   548,   550,     0,
       0,   556,   557,   555,     0,     0,     0,     0,     0,     0,
      19,     0,     0,   588,   587,   590,     0,   585,   589,    70,
      64,    66,    68,    21,    20,    69,   122,     0,   133,     0,
     154,   163,   169,   164,   162,   168,   529,     0,     0,     0,
     530,     0,   530,     0,    10,     9,    34,     0,     7,    53,
     219,   218,   220,   310,    78,    79,   229,   222,     0,   232,
     286,   310,   355,     0,   310,   336,   310,   310,   310,     0,
       0,   310,   310,   310,   239,   266,   272,     0,     0,   281,
     310,   110,     0,   105,     0,     0,   423,     0,     0,   419,
     413,   389,     0,     0,   395,     0,   398,     0,   401,   412,
     411,   407,     0,     0,   421,   410,   402,   403,   404,   425,
     426,   409,   408,   427,   434,   523,    16,     0,    16,   450,
       0,     0,   452,   442,    16,   438,     0,   456,   466,   542,
     570,   575,   138,   140,   551,   552,     0,     0,     0,     0,
       0,   142,    22,    23,   580,   579,   583,     0,   586,   124,
     135,   533,   532,   538,     0,   534,     0,   536,    36,   221,
       0,   359,     0,   360,   361,     0,   310,   357,   358,   356,
     310,   310,     0,   310,     0,     0,   310,   235,     0,   228,
     367,   350,     0,     0,     0,     0,   310,   308,   251,   249,
       0,     0,   255,     0,     0,   106,   107,   111,   113,   415,
       0,   417,   418,   390,   391,   397,   396,   414,   392,   105,
     455,   443,     0,     0,    16,     0,   441,     0,     0,   459,
     454,   458,     0,     0,   448,   440,   467,   567,   141,   145,
      24,     0,     0,     0,     0,   584,   535,   537,   310,   310,
     310,   310,     0,     0,     0,   230,   236,     0,     0,   379,
     362,   364,     0,   223,   354,   352,   353,   351,     0,   253,
     310,     0,   345,   108,     0,   526,   445,   446,   444,   447,
     510,   480,   460,   510,   449,   469,     0,     0,   573,     0,
     150,   145,   145,   145,   145,     0,     0,     0,     0,   373,
     310,   282,   310,   310,     0,   310,   378,   309,     0,     0,
       0,   279,   416,   524,   528,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   465,   479,     0,   568,   562,   563,
     564,   565,   566,   569,   571,   146,   147,   148,   149,   372,
     311,   376,     0,   310,   370,   237,     0,     0,     0,     0,
       0,     0,     0,     0,   365,   310,   262,   310,   260,     0,
     451,   513,   511,   514,    16,     0,    16,   457,     0,   453,
     473,   471,   470,     0,     0,   474,   476,   477,   478,   468,
       0,     0,   310,   310,   244,   248,   246,   310,   380,   247,
     269,   267,   268,   273,     0,   375,   374,   310,   310,   310,
     310,   381,     0,     0,     0,     0,    16,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     527,   509,     0,   515,     0,     0,   463,   464,   462,   461,
     472,   475,   377,   238,   371,   284,   249,     0,   310,     0,
       0,     0,     0,     0,   310,   261,   517,   485,   499,   481,
     484,   482,     0,   497,     0,   486,   488,   489,   490,   491,
     492,   493,   494,   495,   503,   504,   505,    16,   483,    16,
       0,     0,   525,   516,   512,   245,   310,   270,   283,   274,
       0,     0,     0,     0,     0,     0,   496,   487,   498,   500,
     506,   507,   508,   501,     0,   385,   383,   384,   382,   254,
       0,   502,   518,   519,   520,     0,   521,   522
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,   459,    70,   460,   547,   804,   805,   790,
    1011,   569,    71,   339,   139,   342,   629,   630,    72,   146,
     147,    73,    16,    17,    18,   106,    74,   286,    75,   292,
     287,    76,   354,    77,   356,   659,    78,    79,   185,   207,
     466,   452,    80,   721,   977,   862,   722,   723,    81,    82,
     461,   208,   297,   578,   579,   306,   586,   587,   269,   774,
     270,   271,   787,    19,    20,    21,    22,   317,    23,    24,
      25,   315,   604,    26,    27,    28,    29,    30,    31,    32,
      33,    34,   131,   333,    35,    36,    37,    38,    39,    40,
      41,    42,    43,   103,   310,    83,   355,    84,   649,   650,
     651,   838,   725,   726,   467,   662,   653,    85,   359,   952,
     953,  1114,   176,   393,  1160,   689,   690,   394,   971,   691,
     395,   396,  1163,   397,  1164,   177,   399,   400,  1116,   365,
     178,   366,   375,   179,   180,   181,   955,   182,   430,   969,
     956,  1031,  1032,   671,   672,   957,    86,    87,    88,    89,
     248,   249,   250,   767,   906,   997,  1002,  1000,  1001,  1086,
    1138,   907,  1055,  1096,   908,  1192,  1085,  1132,  1133,  1195,
    1255,  1272,  1273,  1274,   989,  1129,  1084,   990,   817,    44,
      45,    46,    47,    48,    49,    50,    51,    90,    91,    92,
      93,   535,  1103,  1059,  1007,   911,   527,   528,    94,    95,
     278,   797,   798,   279,    52,    96,    53,    97,    98,    99,
     431
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -1038
static const yytype_int16 yypact[] =
{
     148,  1767,   147,    83,   389,   220,   265,   517,   389,   337,
     760,   705,   478,   389,   389,   389, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038,   333,   368,   186,   575,
     364,   318, -1038, -1038,   400,   325,    41,   424,  1107, -1038,
   -1038, -1038, -1038, -1038,   448,   448, -1038, -1038, -1038,  1510,
   -1038,   448,  1226, -1038, -1038, -1038,   448,  1836,   448,  1849,
     448,  1185,   448,  1739,   448,  1640, -1038, -1038,  1623,   832,
   -1038, -1038, -1038,    32, -1038,   457,   503, -1038,   389,   389,
     519,    60, -1038,   568,   131,   537,   603, -1038,   595,   104,
     595,   615,   617,   625,   646,   652,   658,   674, -1038, -1038,
   -1038,   541,   686,   692,   702,   631, -1038,   559, -1038,   -19,
      32,    63, -1038, -1038, -1038, -1038,   219,   390, -1038,   594,
   -1038, -1038,   392,    60, -1038,    70, -1038, -1038, -1038,   683,
     415, -1038,   551,    50,     4,  1107, -1038,   417, -1038, -1038,
     420,   427,   429,   430,   818,    96, -1038, -1038, -1038, -1038,
   -1038,  1627, -1038,  2134,   659,   432,   438, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038,   440,    32,   736,   690,    63,
     461, -1038,    26,    26,   743,    60, -1038,   451,   454,   446,
      79,   752,   461,   554,   581,   -85,   673,   108,   434,   718,
     597,   724,    43,   604,   598,   312,   591,   599,   329,   576,
     582,   461,   572,   584,   570,   -22,   -29,   612,   573, -1038,
     495,   494,   594,   461,    63,   461, -1038, -1038,   497,   502,
   -1038,   505,   747,   280, -1038,   508,   507, -1038,   801,    63,
     514,   -53,    63,    63,   763,   776,  -107, -1038,   524,   525,
   -1038,   530,   532,   434,   774,   580, -1038,   539,   367,   540,
     785,   461,   794,   461,   796, -1038,   544,   547,   548,   802,
     846, -1038,   552,   556, -1038, -1038,   676,   864,   867, -1038,
     878,   881, -1038, -1038, -1038,    90,   882, -1038, -1038,   583,
     883, -1038,   884, -1038, -1038,  1900, -1038,   886,   595,   595,
     595,   341,   439,   893,   894, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038,   898, -1038, -1038, -1038,   118,   885, -1038,
   -1038,  1511,   586, -1038, -1038, -1038,  1107, -1038,    63,   579,
   -1038, -1038, -1038, -1038,   588,  1264,   -34, -1038,   590,   596,
      70, -1038,    70,  1107,    70,    70,   895, -1038,   900,   999,
     887,   735,   896,     4,     4,   892,   909,  1107,  1107,  1107,
    1107,  1107, -1038, -1038,   912, -1038,   618,   209, -1038, -1038,
     647, -1038, -1038,   917, -1038, -1038, -1038,  -124,     6, -1038,
     189, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038,  1107,  1107,  1107,  1107,  1107,  1107,  1107,  1107,
    1107,  1107,  1107,  1107,  1107,   899,   925, -1038, -1038, -1038,
   -1038, -1038, -1038,   594, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038,   577, -1038,  1107, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038,   890,   897,   629,   630,    63,   901,
     721,   461,   434, -1038, -1038,    63,   765,   773,    63,   902,
      63, -1038,   643,   903,   768,    63,    63,   461,   904,   648,
     891,   654,   461,   655,   461,   257,   461,   736,   461,   663,
     664,   461,   461, -1038, -1038, -1038, -1038, -1038,   762, -1038,
   -1038, -1038,   964,   668,   669, -1038, -1038, -1038,   670, -1038,
     679,   918, -1038, -1038, -1038,   681,   695,   697,    63,    63,
     704,   706,   707, -1038, -1038,   307, -1038,    28, -1038,   736,
     461, -1038, -1038,   383, -1038,   434, -1038,   811, -1038,    63,
   -1038,    63, -1038,   461, -1038, -1038, -1038,    63, -1038,   434,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,   324, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038,   327, -1038, -1038,  1003,
   -1038, -1038, -1038, -1038,   989,  1002, -1038,   577, -1038, -1038,
   -1038,   434, -1038, -1038,  1005, -1038, -1038,  1007,  1007,  1007,
     595,  1007,   595,  1007, -1038, -1038, -1038,   716,   722, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,   352,
   -1038, -1038,  1220,  1016, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,    45,
     730,   719, -1038, -1038, -1038, -1038,   434,    63, -1038, -1038,
   -1038,  1867,  1264, -1038, -1038,  1261, -1038, -1038, -1038, -1038,
     234, -1038,   725, -1038,  1022, -1038, -1038, -1038, -1038,   309,
    1300,   412,   437,   462, -1038,  1107, -1038,  1107, -1038,  1043,
     741,   746,  1220,   207,   396, -1038, -1038, -1038,  1048,  1052,
   -1038,  2163,  2163,  2163,  2188,  2188,  2163,  2163,  2163,  2163,
     510,   510,   909,   909, -1038,   758, -1038, -1038, -1038, -1038,
   -1038,  1046, -1038,   757, -1038, -1038, -1038,  2134,    63,    63,
   -1038, -1038,   771,   461,   461,   775,   780,   782,    63,    63,
     787,    63,   788, -1038,    63,   789,   792,   793,   795,    63,
   -1038,   594, -1038,   798, -1038,   799,   800,   805,   812,   813,
     814,   815, -1038, -1038,   819,   821,    63,  1660,   823, -1038,
   -1038,  -159, -1038, -1038,   759,   833, -1038, -1038, -1038,   834,
     836, -1038, -1038, -1038,    63,   414,   442,   791,    63,   434,
   -1038,   837,   848, -1038, -1038, -1038,   434,   828,   811, -1038,
   -1038, -1038, -1038,    28, -1038, -1038, -1038,   676, -1038,    90,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038,  1114,  1115,  1145,
    1007,  1162,  1007,  1164, -1038, -1038, -1038,  1511, -1038, -1038,
   -1038, -1038,  1264,   435, -1038, -1038, -1038,   852,   857, -1038,
   -1038,  1107, -1038,  1146,  1107, -1038,  1107,  1107,  1107,   523,
    1339,  1107,   720,  1107, -1038, -1038, -1038,   856,   683, -1038,
    1107, -1038,   650, -1038,   861,   859, -1038,   865,   873, -1038,
   -1038, -1038,   874,   875, -1038,   876, -1038,   879, -1038, -1038,
   -1038, -1038,   889,   907, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038,   461,   256,    68, -1038,
     932,   434, -1038, -1038,   143, -1038,   911, -1038,   862, -1038,
   -1038, -1038, -1038,   918, -1038, -1038,   919,  1191,  1191,  1191,
    1191, -1038, -1038, -1038, -1038, -1038, -1038,    63,   828, -1038,
   -1038, -1038, -1038, -1038,  1209, -1038,  1211, -1038, -1038, -1038,
       0, -1038,  1193, -1038, -1038,   905,  1107, -1038, -1038, -1038,
    1107,  1107,   914,     9,  1627,   923,  1038, -1038,  1264, -1038,
   -1038, -1038,  1367,  1408,  1433,  1458,  1107, -1038, -1038,  2134,
     910,   920,  2134,  1201,  1484,   434, -1038, -1038, -1038, -1038,
      63, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038,    63,    63,   461,    63, -1038,   916,   594, -1038,
   -1038,   933,   936,    63, -1038, -1038, -1038,  -143, -1038,   307,
   -1038,   934,   939,   941,   942, -1038, -1038, -1038,  1107,   999,
    1107, -1038,   613,  2075,   553, -1038, -1038,  1195,   152, -1038,
    2134,   946,   943, -1038, -1038, -1038, -1038, -1038,  1514,  1213,
    1066,   949, -1038, -1038,   958,   273, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038,  1090,    63,   297, -1038,   959,
   -1038,   307,   307,   307,   307,   675,   961,   908,  1656, -1038,
    1631, -1038,  1107,  1107,   336,  1038, -1038, -1038,   966,   980,
      11, -1038, -1038, -1038, -1038,    20,   -44,    24,    63,    63,
      63,  1019,     1,    17, -1038, -1038,   965, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038,   983,   435,  1147, -1038,    75,  1134,  1175,   985,
     993,  1001,  1004,   987, -1038,  1107, -1038,  1107, -1038,  1865,
   -1038, -1038, -1038,  -224,   461,    63,   461, -1038,  1000, -1038,
   -1038, -1038, -1038,    63,    63, -1038, -1038, -1038, -1038, -1038,
    1014,   998,  1631,    34, -1038, -1038, -1038,   647, -1038, -1038,
    1312, -1038, -1038, -1038,   111, -1038, -1038,  1107,  1107,  1107,
    1107, -1038,  1011,  1020,  1018,    63,   461,    63,    63,    63,
    1121,    63,    44,  1271,   211,  1076,    63,  1064,  1063,  1059,
   -1038, -1038,  1029, -1038,    63,  1030, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038,  1539,   102,    34,   270,
    1569,  1594,  1619,  1647,  1107, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038,    63, -1038,    63, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038,   461, -1038,   461,
     178,    63, -1038, -1038, -1038, -1038,  1107, -1038, -1038, -1038,
    1037,  1040,  1041,  1042,  1044,    27, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038,  1539, -1038, -1038, -1038, -1038, -1038,
     -14, -1038, -1038, -1038, -1038,  1053, -1038, -1038
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1038, -1038, -1038,  -134, -1038,  -208,  -206,  -271,  -588, -1038,
    -227, -1038, -1038, -1038, -1038, -1038, -1038,   543, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038,  1267, -1038, -1038, -1038, -1038,
   -1038, -1038,  -237, -1038, -1038, -1038, -1038, -1038,   906,  1642,
    1170,  -252, -1038, -1038, -1038, -1038,   511,  -521, -1038, -1038,
   -1038, -1038,    52, -1038,   574,   -52, -1038,   578,  -917,   469,
    -517, -1038,  -756, -1038, -1038, -1038, -1038,  -111, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038,   644,  1074, -1038, -1038, -1038, -1038,  -631,
     565,   426,  1031,   803,  1070, -1038,   739, -1038, -1038,  -958,
   -1038,   246,  -174,   729,   208,  -834,   571, -1037,  -946,  1250,
    -994,   731,   217, -1038, -1038,  -172,   728, -1038, -1038, -1038,
   -1038,   349,   308, -1038, -1038,  -823, -1038,   -95,   474,   -68,
   -1038, -1038,   363,   605,   428,  -922, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038,   386, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,  -422, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
   -1038,   651, -1038, -1038, -1038, -1038, -1038, -1038, -1038, -1038,
    -918
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -369
static const yytype_int16 yytable[] =
{
     183,   392,   548,   398,   477,   515,   771,   347,   323,   324,
     954,   486,   487,   814,   775,  -234,   344,   968,  1127,   373,
     345,   340,  1018,   503,  -234,  -234,  -234,  -234,   940,   532,
     837,  1026,   696,   697,   480,   516,  1028,   518,   483,   484,
    -233,   391,   483,   484,   603,  1270,   507,  1193,   509,  -233,
    -233,  -233,  -233,   940,  1056,   941,  1194,   788,   344,   344,
     367,   152,   345,   345,   830,   454,   462,   294,   654,   655,
     656,   657,   295,   560,  1134,   562,   540,   371,   344,  1161,
     941,  1153,   345,   344,   942,   360,    55,   345,   492,   361,
    1154,  1155,  1156,   854,  1080,   303,  1144,   376,   362,  1146,
     304,   358,   455,   456,   457,   458,   387,   463,   464,   942,
     517,   344,   388,   389,   159,   345,    56,  1154,  1155,  1156,
     854,    57,  1162,   316,    58,   530,   483,   484,   536,   537,
     954,   481,   153,   344,   311,    59,   617,   345,  1095,  1224,
     943,   944,   945,   946,   947,   948,   949,    54,  1115,  1057,
    1112,    60,     1,   470,    61,  1151,    62,    63,   344,   910,
     950,   863,   345,   951,    64,   943,   944,   945,   946,   947,
     948,   949,  1161,   584,  1073,   508,  1058,   455,   456,   457,
     458,  1173,   318,    65,  1074,   950,   818,   819,   951,   821,
     831,   823,   343,   344,    66,  1205,   694,   345,  1068,   695,
     541,   542,   585,   618,  1135,   510,   465,   607,   608,   609,
     611,   613,  1191,    67,   633,  1162,   720,   432,   433,   434,
     435,   436,   344,   388,   389,   854,   345,  1136,   319,   372,
    1115,   437,   438,   439,   440,   441,   442,   443,   444,   348,
     104,   445,   432,   433,   434,   435,   436,   954,   450,   312,
    1248,   533,   534,  1060,  1275,   760,   437,   438,   439,   440,
     441,   442,   443,   444,    68,   367,   445,   367,   107,   367,
     367,   105,   349,   735,   976,  1137,   736,   140,   632,   474,
     475,  1147,   658,   141,  1148,  1154,  1155,  1156,   854,   748,
     954,  1172,   485,  1234,   753,   665,   755,   791,   759,   341,
     761,   670,  1276,   764,   765,  1105,  1106,  1107,  1108,   679,
     680,   681,   682,   683,  1019,   346,   995,   432,   433,   434,
     435,   436,   692,  -234,   698,   374,  1128,   837,  -234,   954,
     954,   437,   438,   439,   440,   441,   442,   443,   444,  1130,
     112,   445,   792,  1139,   732,   789,  1271,   794,  -233,   795,
     296,   737,   135,  -233,   740,   801,   742,   346,   346,   491,
     316,   746,   747,   803,   701,   702,   703,   704,   705,   706,
     707,   708,   709,   710,   711,   712,   713,   346,   305,   784,
    1254,   610,   346,   148,   363,   954,   320,  1043,   136,  1157,
     149,   364,   321,   322,  1158,   803,   775,   727,   934,    69,
     936,   483,   484,   793,   779,   780,   785,   137,   100,   101,
     390,   344,   388,   389,   854,   345,  1246,  1119,   138,   150,
     432,   433,   434,   435,   436,   799,   151,   800,  1208,   102,
    1003,  1209,   346,   802,   437,   438,   439,   440,   441,   442,
     443,   444,   786,   154,   445,   432,   433,   434,   435,   436,
     834,   756,   483,   484,   940,  1260,  1261,   346,   316,   437,
     438,   439,   440,   441,   442,   443,   444,   184,  1045,   445,
     432,   433,   434,   435,   436,  1235,   344,   104,  1236,   612,
     345,   941,   757,   758,   437,   438,   439,   440,   441,   442,
     443,   444,   346,   128,   445,   455,   456,   457,   458,   820,
     917,   822,   918,   552,   992,   993,   299,  1120,  1121,   699,
     942,  1122,   700,   129,   883,  1123,   495,   496,   130,   392,
     392,   346,   302,   835,   686,   867,   868,   398,   919,   687,
     920,   432,   433,   434,   435,   436,   100,   101,   443,   444,
     313,   994,   445,   499,   500,   437,   438,   439,   440,   441,
     442,   443,   444,  1098,   841,   445,   332,   102,   553,   391,
     391,   432,   433,   434,   435,   436,   943,   944,   945,   946,
     947,   948,   949,   523,   524,   437,   438,   439,   440,   441,
     442,   443,   444,   923,  1157,   445,   950,   307,   308,   951,
     926,  1083,   554,   863,   864,   865,   724,  1099,  1100,  1101,
    1102,   555,   307,   308,   872,   873,   314,   875,   309,   351,
     877,   352,   353,   142,   316,   882,   338,   849,   325,   850,
     326,   432,   433,   434,   435,   436,   108,   109,   327,   844,
     999,   143,   894,   634,   635,   437,   438,   439,   440,   441,
     442,   443,   444,   806,   807,   445,   808,   809,   144,   328,
     916,   110,   111,   337,   922,   329,   803,   132,   133,   134,
     155,   330,   156,   157,   158,   159,   160,   161,   483,   484,
     975,   826,   827,   162,   163,   688,   164,   331,   165,   556,
     557,   676,   677,   432,   433,   434,   435,   436,   991,   334,
     996,  1012,  1013,  1014,   145,   335,  1004,   437,   438,   439,
     440,   441,   442,   443,   444,   336,   350,   445,   357,   663,
     390,   664,   368,   666,   667,   637,   638,   639,   640,   641,
     642,   643,   166,   644,   645,   646,   647,   648,   167,   369,
     370,   377,   846,   155,   378,   156,   157,   158,   159,   160,
     161,   379,   572,   380,   381,   446,   162,   163,   688,   164,
     447,   165,   300,   301,  -233,   451,   448,   847,   449,   573,
     574,  1051,   453,   469,   473,   575,   576,   471,   577,   803,
     472,   121,   476,   670,   478,   346,   962,   482,   963,   964,
     965,   488,   848,   479,   489,   972,  1048,   490,   497,   493,
     501,   494,   974,  1015,   504,   166,   122,   123,   498,   506,
     502,   167,   168,   169,   382,   383,   384,   385,   505,   511,
     512,   513,   514,   519,  1131,   522,  1131,   170,   171,   172,
     520,   529,   173,   521,   525,   526,   538,   124,   125,   126,
     127,   155,   531,   156,   157,   158,   159,   160,   161,   539,
     543,   544,   549,   966,   162,   163,  1044,   164,   545,   165,
     546,   113,   114,   559,   550,   551,   558,   561,  1046,  1047,
     564,  1049,   563,   565,   566,   567,   568,   580,   570,  1054,
     581,  1071,   571,   196,   115,   168,   169,   116,  1022,   117,
     118,   582,  1023,  1024,   583,   588,   590,   591,  1030,   606,
     170,   171,   172,   166,   289,   173,   614,   615,  1038,   167,
     197,   616,   631,   198,   636,   619,   660,   199,   200,   668,
     661,   202,   203,   673,   674,   669,   432,   433,   434,   435,
     436,   678,  1097,   675,   693,   290,  1196,   684,  1198,  1069,
     437,   438,   439,   440,   441,   442,   443,   444,   685,   204,
     445,   445,  1159,   205,   719,   730,   731,   733,   734,   738,
    1065,   670,  1067,   728,  1140,  1141,  1142,   739,  1145,   743,
     729,   174,   745,   751,   750,   741,   744,   749,  1218,   175,
     752,   754,   972,   168,   169,   382,   383,   384,   385,   762,
     763,   186,   766,   768,   769,   770,   772,   195,   170,   171,
     172,  1109,   209,   173,   241,   773,   251,   776,   256,   119,
     272,  1197,   796,   258,  1117,  1118,   810,  1030,   811,  1200,
    1201,   777,   155,   778,   156,   157,   158,   159,   160,   161,
     781,   812,   782,   783,   815,   162,   163,   816,   164,  1258,
     165,  1259,   824,  1159,   174,  1159,   829,   833,   825,   714,
     842,  1217,   175,  1219,  1220,  1221,   832,  1223,  1225,   843,
     851,   155,  1238,   156,   157,   158,   159,   160,   161,   972,
    1243,   852,   715,   857,   162,   163,   853,   164,   716,   165,
     858,   120,   860,   861,   166,   717,   718,   863,   912,   155,
     167,   156,   157,   158,   159,   160,   161,   866,  1256,  1206,
    1257,   869,   162,   163,  1079,   164,   870,   165,   871,  1210,
    1211,  1212,  1213,   874,   876,   878,  1262,  1263,   879,   880,
     921,   881,   927,   166,   884,   885,   886,   931,   932,   167,
     155,   887,   156,   157,   158,   159,   160,   161,   888,   889,
     890,   891,   174,   162,   163,   892,   164,   893,   165,   909,
     175,   166,   432,   433,   434,   435,   436,   167,   933,   913,
     914,   291,   915,   924,   168,   169,   437,   438,   439,   440,
     441,   442,   443,   444,   925,   935,   445,   937,   958,   170,
     171,   172,   959,   961,   173,   258,   973,   979,  1264,   980,
    1006,   981,   166,   432,   433,   434,   435,   436,   167,   982,
     983,   984,   985,   168,   169,   986,   260,   437,   438,   439,
     440,   441,   442,   443,   444,   987,   998,   445,   170,   171,
     172,  1010,  1016,   173,  1017,  1020,  1041,  1072,  1143,  1021,
    1078,   168,   169,   988,  1111,  1039,   196,  1005,   432,   433,
     434,   435,   436,  1025,  1050,  1009,   170,   171,   172,  1029,
    1040,   173,   437,   438,   439,   440,   441,   442,   443,   444,
    1061,  1052,   445,   197,  1053,  1062,   198,  1063,  1064,  1076,
     199,   200,   168,   169,   202,   203,  1075,   196,  1081,   432,
     433,   434,   435,   436,  1082,  1104,  1110,   170,   171,   172,
    1125,  1149,   173,   437,   438,   439,   440,   441,   442,   443,
     444,  1088,   204,   445,   197,  1126,   205,   198,  1150,  1167,
    1152,   199,   200,  1171,   201,   202,   203,  1168,   432,   433,
     434,   435,   436,   174,  -368,  1169,  1199,  1203,  1170,  1207,
    1222,   175,   437,   438,   439,   440,   441,   442,   443,   444,
    1202,  1214,   445,   204,  1237,  1215,  1216,   205,  1239,  1240,
    1089,  1090,  1091,  1241,  1092,  1242,  1244,   432,   433,   434,
     435,   436,   174,  1265,  -363,  1093,  1266,  1267,  1268,  1269,
     175,   437,   438,   439,   440,   441,   442,   443,   444,  1277,
     938,   445,   298,   468,   978,   432,   433,   434,   435,   436,
     174,   929,  1008,   589,  1033,   605,   652,   930,   175,   437,
     438,   439,   440,   441,   442,   443,   444,   939,  1204,   445,
     813,   839,   637,   638,   639,   640,   641,   642,   643,  1094,
     644,   645,   646,   647,   648,  1247,   432,   433,   434,   435,
     436,   174,   855,   970,   386,   856,  1249,   859,  1027,   175,
     437,   438,   439,   440,   441,   442,   443,   444,  1124,  1087,
     445,   432,   433,   434,   435,   436,   960,  1066,     0,   928,
    1165,     0,     0,     0,     0,   437,   438,   439,   440,   441,
     442,   443,   444,     0,     0,   445,   432,   433,   434,   435,
     436,     0,     0,     0,     0,     0,   252,   253,     0,     0,
     437,   438,   439,   440,   441,   442,   443,   444,     0,     0,
     445,  1166,   432,   433,   434,   435,   436,     0,     0,     0,
       0,     0,     0,     0,   254,     0,   437,   438,   439,   440,
     441,   442,   443,   444,     0,     0,   445,     0,     0,     0,
       0,     0,   432,   433,   434,   435,   436,  1226,  1227,  1228,
    1229,  1230,  1231,  1232,  1233,   828,   437,   438,   439,   440,
     441,   442,   443,   444,     0,   206,   445,   432,   433,   434,
     435,   436,   620,   621,   622,   623,   624,   625,   626,   627,
     628,   437,   438,   439,   440,   441,   442,   443,   444,     0,
     187,   445,     0,     0,     0,   188,   840,   432,   433,   434,
     435,   436,     0,     0,     0,     0,     0,   189,     0,     0,
       0,   437,   438,   439,   440,   441,   442,   443,   444,     0,
       0,   445,   432,   433,   434,   435,   436,     0,     0,     0,
       0,   190,     0,     0,     0,   845,   437,   438,   439,   440,
     441,   442,   443,   444,     0,     0,   445,   432,   433,   434,
     435,   436,   401,     0,     0,     0,     0,     0,     0,     0,
       0,   437,   438,   439,   440,   441,   442,   443,   444,     0,
     940,   445,     0,     0,   967,   432,   433,   434,   435,   436,
       0,   401,     0,     0,   196,     0,     0,     0,     0,   437,
     438,   439,   440,   441,   442,   443,   444,   941,     0,   445,
       0,   196,  1034,   280,   281,   282,     0,   283,   284,     0,
       0,   197,     0,     0,   198,     0,   402,   403,   199,   200,
       0,     0,   202,   203,     0,     0,   942,     0,   197,     0,
       0,   198,     0,   404,     0,   199,   200,     0,     0,   202,
     203,     0,     0,  1035,     0,   402,   403,     0,     0,   240,
     204,   247,     0,   255,   205,   268,     0,   277,   895,     0,
     288,   293,   405,     0,     0,     0,     0,   204,  1036,   191,
       0,   205,     0,     0,     0,     0,     0,     0,     0,   273,
       0,     0,   943,   944,   945,   946,   947,   948,   949,     0,
       0,   405,     0,  1037,     0,     0,     0,     0,   896,     0,
     196,     0,   950,     0,     0,   951,     0,     0,   406,   407,
       0,   408,   409,   410,   411,   412,     0,     0,     0,  1042,
     192,     0,     0,     0,     0,   193,     0,   197,     4,     0,
     198,     0,     0,     0,   199,   200,     0,   257,   202,   203,
       0,   194,     0,     0,   258,     5,     6,     0,     0,  1077,
     413,   414,   415,   416,   417,   418,   419,   420,   421,     0,
       0,     0,     0,     0,   259,   260,   204,     0,     0,     0,
     205,     0,     0,     0,  1245,   897,     0,   422,   423,   413,
     414,   415,   416,   417,   418,   419,   420,   421,   424,   425,
     426,   427,   274,     0,     0,     7,     0,   196,     8,     0,
       9,     0,   210,    10,  1250,     0,   422,   423,    11,     0,
     196,     0,     0,    12,    13,    14,    15,   424,   425,   426,
     427,     0,     0,   592,   197,     0,   898,   198,     0,  1251,
       0,   199,   200,   275,     0,   202,   203,   197,   483,   484,
     198,   211,     0,   212,   199,   200,     0,     0,   202,   203,
     899,     0,   900,     0,  1252,     0,   428,   429,     0,   901,
     593,   902,   285,   204,     0,   903,   904,   205,     0,  1113,
     258,     0,     0,     0,     0,   213,   204,     0,     0,   276,
     205,     0,  1253,   214,   215,   428,   429,     0,   594,     0,
       0,   260,     0,   595,     0,     0,     0,     0,     0,   905,
     596,   597,     0,     0,     0,     0,     0,     0,     0,  1174,
       0,     0,     0,   598,   599,   600,   601,     0,   602,     0,
       0,   216,     0,     0,   261,   637,   638,   639,   640,   641,
     642,   643,     0,   644,   645,   646,   647,   648,   217,   218,
       0,   219,   220,     0,   221,   222,   223,     0,   224,     0,
       0,   225,   226,     0,   227,   262,   263,   264,   265,     0,
       0,     0,     0,     0,     0,   266,     0,     0,   228,   229,
       0,     0,     0,   230,     0,   231,     0,   232,   267,   233,
       0,     0,     0,     0,   234,     0,  1175,     0,   235,   236,
       0,   237,   238,     0,     0,     0,     0,     0,     0,     0,
     465,     0,     0,   432,   433,   434,   435,   436,     0,   242,
       0,     0,     0,     0,   243,     0,   244,   437,   438,   439,
     440,   441,   442,   443,   444,     0,     0,   445,     0,     0,
       0,  1176,     0,  1177,     0,  1178,  1179,  1180,  1181,  1182,
    1183,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1184,  1185,   245,     0,  1186,     0,     0,     0,  1187,     0,
    1188,     0,   432,   433,   434,   435,   436,     0,  1189,     0,
       0,     0,     0,     0,     0,   239,   437,   438,   439,   440,
     441,   442,   443,   444,     0,     0,   445,     0,   246,     0,
       0,  -369,  -369,  -369,     0,     0,     0,     0,     0,     0,
       0,     0,   836,     0,  1190,  -369,  -369,  -369,  -369,   441,
     442,   443,   444,     0,     0,   445,   432,   433,   434,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     437,   438,   439,   440,   441,   442,   443,   444,     0,     0,
     445,     0,     0,     0,     0,     0,     0,  1070
};

static const yytype_int16 yycheck[] =
{
      68,   175,   273,   175,   212,   242,   527,   141,   119,   120,
     833,   217,   218,   601,   531,     6,    15,   851,     7,    15,
      19,    40,    22,   231,    15,    16,    17,    18,    19,    82,
     661,   953,    26,    27,   119,   243,   954,   245,    18,    19,
       6,   175,    18,    19,   315,    18,    68,   271,    77,    15,
      16,    17,    18,    19,   197,    46,   280,    29,    15,    15,
     155,    20,    19,    19,    19,   199,    40,    35,   102,   103,
     104,   105,    40,   281,   118,   283,   183,    27,    15,  1116,
      46,     6,    19,    15,    75,    15,     3,    19,   222,    19,
      15,    16,    17,    18,  1040,    35,    95,   165,    28,    82,
      40,   153,    34,    35,    36,    37,   174,    81,    82,    75,
     244,    15,    16,    17,    18,    19,    33,    15,    16,    17,
      18,    38,  1116,    19,    41,   259,    18,    19,   262,   263,
     953,   216,    91,    15,     3,    52,    18,    19,  1055,    95,
     131,   132,   133,   134,   135,   136,   137,     0,  1070,   292,
    1068,    68,     4,   205,    71,  1113,    73,    74,    15,   318,
     151,   320,    19,   154,    81,   131,   132,   133,   134,   135,
     136,   137,  1209,    83,    22,   197,   319,    34,    35,    36,
      37,  1127,    78,   100,    32,   151,   608,   609,   154,   611,
     145,   613,   140,    15,   111,  1153,   320,    19,  1021,   323,
     307,   308,   112,   337,   248,   234,   180,   318,   319,   320,
     321,   322,  1129,   130,   348,  1209,   453,     8,     9,    10,
      11,    12,    15,    16,    17,    18,    19,   271,   124,   179,
    1152,    22,    23,    24,    25,    26,    27,    28,    29,    20,
      20,    32,     8,     9,    10,    11,    12,  1070,   196,   118,
    1208,   304,   305,  1009,   268,   507,    22,    23,    24,    25,
      26,    27,    28,    29,   181,   360,    32,   362,     3,   364,
     365,    51,    53,   481,   862,   319,   482,    91,   346,   200,
     201,   264,   316,    97,   267,    15,    16,    17,    18,   497,
    1113,  1125,   184,    82,   502,   363,   504,   549,   506,   318,
     508,   369,   316,   511,   512,  1061,  1062,  1063,  1064,   377,
     378,   379,   380,   381,   314,   314,   248,     8,     9,    10,
      11,    12,   390,   314,   318,   321,   315,   958,   319,  1152,
    1153,    22,    23,    24,    25,    26,    27,    28,    29,   319,
       3,    32,   550,   319,   478,   317,   319,   553,   314,   555,
     318,   485,    19,   319,   488,   563,   490,   314,   314,   316,
      19,   495,   496,   569,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   314,   318,    72,
    1214,    40,   314,    19,   314,  1208,   282,   975,    20,   314,
      72,   321,   288,   289,   319,   601,   913,   465,   820,   316,
     822,    18,    19,    20,   538,   539,    99,    39,    19,    20,
     314,    15,    16,    17,    18,    19,   314,    81,    50,    19,
       8,     9,    10,    11,    12,   559,   101,   561,   317,    40,
     287,   320,   314,   567,    22,    23,    24,    25,    26,    27,
      28,    29,   135,    19,    32,     8,     9,    10,    11,    12,
     656,   194,    18,    19,    19,   277,   278,   314,    19,    22,
      23,    24,    25,    26,    27,    28,    29,    19,   989,    32,
       8,     9,    10,    11,    12,   264,    15,    20,   267,    40,
      19,    46,   225,   226,    22,    23,    24,    25,    26,    27,
      28,    29,   314,    15,    32,    34,    35,    36,    37,   610,
      86,   612,    88,   136,   248,   249,     3,   171,   172,   320,
      75,   175,   323,    35,   751,   179,   204,   205,    40,   693,
     694,   314,     3,   657,   315,   733,   734,   699,    86,   320,
      88,     8,     9,    10,    11,    12,    19,    20,    28,    29,
       3,   285,    32,   214,   215,    22,    23,    24,    25,    26,
      27,    28,    29,   256,   320,    32,    15,    40,   191,   693,
     694,     8,     9,    10,    11,    12,   131,   132,   133,   134,
     135,   136,   137,   293,   294,    22,    23,    24,    25,    26,
      27,    28,    29,   789,   314,    32,   151,    19,    20,   154,
     796,   318,   225,   320,   728,   729,    19,   300,   301,   302,
     303,   234,    19,    20,   738,   739,     3,   741,    40,    15,
     744,    17,    18,    38,    19,   749,    57,   685,     3,   687,
       3,     8,     9,    10,    11,    12,   109,   110,     3,   320,
     901,    56,   766,    54,    55,    22,    23,    24,    25,    26,
      27,    28,    29,   319,   320,    32,   319,   320,    73,     3,
     784,     7,     8,    22,   788,     3,   862,    13,    14,    15,
      13,     3,    15,    16,    17,    18,    19,    20,    18,    19,
      20,   319,   320,    26,    27,    28,    29,     3,    31,   312,
     313,   373,   374,     8,     9,    10,    11,    12,   896,     3,
     898,   918,   919,   920,   119,     3,   904,    22,    23,    24,
      25,    26,    27,    28,    29,     3,   316,    32,   316,   360,
     314,   362,    29,   364,   365,   138,   139,   140,   141,   142,
     143,   144,    75,   146,   147,   148,   149,   150,    81,   314,
     179,   314,   320,    13,   314,    15,    16,    17,    18,    19,
      20,   314,    66,   314,   314,    86,    26,    27,    28,    29,
     318,    31,   108,   109,   319,    19,   318,   320,   318,    83,
      84,   998,    72,    20,   318,    89,    90,   316,    92,   975,
     316,    66,    20,   841,   220,   314,   844,   104,   846,   847,
     848,    63,   320,   202,   187,   853,   994,    63,   197,   185,
     214,   193,   860,   927,   222,    75,    91,    92,   199,   229,
     218,    81,   155,   156,   157,   158,   159,   160,   224,   197,
     237,   316,   318,   316,  1085,    68,  1087,   170,   171,   172,
     318,    20,   175,   318,   316,   318,    63,   122,   123,   124,
     125,    13,   318,    15,    16,    17,    18,    19,    20,    63,
     316,   316,    68,   320,    26,    27,   980,    29,   318,    31,
     318,    91,    92,    68,   274,   316,   316,    63,   992,   993,
     316,   995,    66,   316,   316,    63,    20,     3,   316,  1003,
       3,   318,   316,    41,   114,   155,   156,   117,   946,   119,
     120,     3,   950,   951,     3,     3,     3,     3,   956,     3,
     170,   171,   172,    75,    62,   175,     3,     3,   966,    81,
      68,     3,   316,    71,   316,    20,   316,    75,    76,    14,
     314,    79,    80,    26,   179,    15,     8,     9,    10,    11,
      12,    29,  1056,    27,     7,    93,  1134,    15,  1136,   316,
      22,    23,    24,    25,    26,    27,    28,    29,   320,   107,
      32,    32,  1116,   111,    19,   316,   316,    46,   227,   184,
    1018,  1019,  1020,    63,  1088,  1089,  1090,   184,  1092,   316,
      63,   314,   194,    72,   316,    63,    63,    63,  1176,   322,
     316,   316,  1040,   155,   156,   157,   158,   159,   160,   316,
     316,    75,   220,    19,   316,   316,   316,    81,   170,   171,
     172,   316,    86,   175,    88,   316,    90,   316,    92,   239,
      94,  1135,   191,    85,  1072,  1073,     3,  1075,    19,  1143,
    1144,   316,    13,   316,    15,    16,    17,    18,    19,    20,
     316,    19,   316,   316,    19,    26,    27,    20,    29,  1237,
      31,  1239,   316,  1207,   314,  1209,    20,   318,   316,   140,
     315,  1175,   322,  1177,  1178,  1179,   316,  1181,  1182,    27,
       7,    13,  1186,    15,    16,    17,    18,    19,    20,  1127,
    1194,   320,   163,    15,    26,    27,   320,    29,   169,    31,
      18,   311,   314,    27,    75,   176,   177,   320,   319,    13,
      81,    15,    16,    17,    18,    19,    20,   316,  1222,  1157,
    1224,   316,    26,    27,    28,    29,   316,    31,   316,  1167,
    1168,  1169,  1170,   316,   316,   316,  1240,  1241,   316,   316,
     319,   316,   284,    75,   316,   316,   316,     3,     3,    81,
      13,   316,    15,    16,    17,    18,    19,    20,   316,   316,
     316,   316,   314,    26,    27,   316,    29,   316,    31,   316,
     322,    75,     8,     9,    10,    11,    12,    81,     3,   316,
     316,   319,   316,   316,   155,   156,    22,    23,    24,    25,
      26,    27,    28,    29,   316,     3,    32,     3,   316,   170,
     171,   172,   315,    27,   175,    85,   320,   316,  1246,   320,
     318,   316,    75,     8,     9,    10,    11,    12,    81,   316,
     316,   316,   316,   155,   156,   316,   106,    22,    23,    24,
      25,    26,    27,    28,    29,   316,   274,    32,   170,   171,
     172,    20,     3,   175,     3,    22,    15,    22,   199,   314,
       7,   155,   156,   316,   316,   315,    41,   316,     8,     9,
      10,    11,    12,   319,   318,   316,   170,   171,   172,   316,
     320,   175,    22,    23,    24,    25,    26,    27,    28,    29,
     316,   318,    32,    68,   318,   316,    71,   316,   316,   316,
      75,    76,   155,   156,    79,    80,   320,    41,   319,     8,
       9,    10,    11,    12,   316,   316,   315,   170,   171,   172,
     314,   316,   175,    22,    23,    24,    25,    26,    27,    28,
      29,   201,   107,    32,    68,   315,   111,    71,   315,   314,
     153,    75,    76,   316,    78,    79,    80,   314,     8,     9,
      10,    11,    12,   314,   315,   314,   316,   319,   314,     7,
     199,   322,    22,    23,    24,    25,    26,    27,    28,    29,
     316,   320,    32,   107,   258,   315,   318,   111,   274,   276,
     250,   251,   252,   284,   254,   316,   316,     8,     9,    10,
      11,    12,   314,   316,   316,   265,   316,   316,   316,   315,
     322,    22,    23,    24,    25,    26,    27,    28,    29,   316,
     827,    32,   105,   203,   863,     8,     9,    10,    11,    12,
     314,   807,   913,   309,   958,   315,   355,   809,   322,    22,
      23,    24,    25,    26,    27,    28,    29,   832,  1152,    32,
     597,   662,   138,   139,   140,   141,   142,   143,   144,   319,
     146,   147,   148,   149,   150,  1207,     8,     9,    10,    11,
      12,   314,   693,   852,   174,   694,  1209,   699,   954,   322,
      22,    23,    24,    25,    26,    27,    28,    29,  1075,  1053,
      32,     8,     9,    10,    11,    12,   841,  1019,    -1,   798,
     316,    -1,    -1,    -1,    -1,    22,    23,    24,    25,    26,
      27,    28,    29,    -1,    -1,    32,     8,     9,    10,    11,
      12,    -1,    -1,    -1,    -1,    -1,   291,   292,    -1,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    -1,    -1,
      32,   316,     8,     9,    10,    11,    12,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   319,    -1,    22,    23,    24,    25,
      26,    27,    28,    29,    -1,    -1,    32,    -1,    -1,    -1,
      -1,    -1,     8,     9,    10,    11,    12,   256,   257,   258,
     259,   260,   261,   262,   263,   315,    22,    23,    24,    25,
      26,    27,    28,    29,    -1,   319,    32,     8,     9,    10,
      11,    12,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    22,    23,    24,    25,    26,    27,    28,    29,    -1,
      60,    32,    -1,    -1,    -1,    65,   315,     8,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    -1,    77,    -1,    -1,
      -1,    22,    23,    24,    25,    26,    27,    28,    29,    -1,
      -1,    32,     8,     9,    10,    11,    12,    -1,    -1,    -1,
      -1,   101,    -1,    -1,    -1,   315,    22,    23,    24,    25,
      26,    27,    28,    29,    -1,    -1,    32,     8,     9,    10,
      11,    12,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    22,    23,    24,    25,    26,    27,    28,    29,    -1,
      19,    32,    -1,    -1,   315,     8,     9,    10,    11,    12,
      -1,     5,    -1,    -1,    41,    -1,    -1,    -1,    -1,    22,
      23,    24,    25,    26,    27,    28,    29,    46,    -1,    32,
      -1,    41,   315,    60,    61,    62,    -1,    64,    65,    -1,
      -1,    68,    -1,    -1,    71,    -1,    69,    70,    75,    76,
      -1,    -1,    79,    80,    -1,    -1,    75,    -1,    68,    -1,
      -1,    71,    -1,    86,    -1,    75,    76,    -1,    -1,    79,
      80,    -1,    -1,   315,    -1,    69,    70,    -1,    -1,    87,
     107,    89,    -1,    91,   111,    93,    -1,    95,    78,    -1,
      98,    99,   115,    -1,    -1,    -1,    -1,   107,   315,   239,
      -1,   111,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   119,
      -1,    -1,   131,   132,   133,   134,   135,   136,   137,    -1,
      -1,   115,    -1,   315,    -1,    -1,    -1,    -1,   118,    -1,
      41,    -1,   151,    -1,    -1,   154,    -1,    -1,   161,   162,
      -1,   164,   165,   166,   167,   168,    -1,    -1,    -1,   315,
     290,    -1,    -1,    -1,    -1,   295,    -1,    68,    41,    -1,
      71,    -1,    -1,    -1,    75,    76,    -1,    78,    79,    80,
      -1,   311,    -1,    -1,    85,    58,    59,    -1,    -1,   315,
     203,   204,   205,   206,   207,   208,   209,   210,   211,    -1,
      -1,    -1,    -1,    -1,   105,   106,   107,    -1,    -1,    -1,
     111,    -1,    -1,    -1,   315,   195,    -1,   230,   231,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   241,   242,
     243,   244,   232,    -1,    -1,   108,    -1,    41,   111,    -1,
     113,    -1,    46,   116,   315,    -1,   230,   231,   121,    -1,
      41,    -1,    -1,   126,   127,   128,   129,   241,   242,   243,
     244,    -1,    -1,     3,    68,    -1,   246,    71,    -1,   315,
      -1,    75,    76,   273,    -1,    79,    80,    68,    18,    19,
      71,    85,    -1,    87,    75,    76,    -1,    -1,    79,    80,
     270,    -1,   272,    -1,   315,    -1,   309,   310,    -1,   279,
      40,   281,   319,   107,    -1,   285,   286,   111,    -1,   318,
      85,    -1,    -1,    -1,    -1,   119,   107,    -1,    -1,   319,
     111,    -1,   315,   127,   128,   309,   310,    -1,    68,    -1,
      -1,   106,    -1,    73,    -1,    -1,    -1,    -1,    -1,   319,
      80,    81,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   124,
      -1,    -1,    -1,    93,    94,    95,    96,    -1,    98,    -1,
      -1,   165,    -1,    -1,   265,   138,   139,   140,   141,   142,
     143,   144,    -1,   146,   147,   148,   149,   150,   182,   183,
      -1,   185,   186,    -1,   188,   189,   190,    -1,   192,    -1,
      -1,   195,   196,    -1,   198,   296,   297,   298,   299,    -1,
      -1,    -1,    -1,    -1,    -1,   306,    -1,    -1,   212,   213,
      -1,    -1,    -1,   217,    -1,   219,    -1,   221,   319,   223,
      -1,    -1,    -1,    -1,   228,    -1,   201,    -1,   232,   233,
      -1,   235,   236,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     180,    -1,    -1,     8,     9,    10,    11,    12,    -1,   240,
      -1,    -1,    -1,    -1,   245,    -1,   247,    22,    23,    24,
      25,    26,    27,    28,    29,    -1,    -1,    32,    -1,    -1,
      -1,   246,    -1,   248,    -1,   250,   251,   252,   253,   254,
     255,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     265,   266,   283,    -1,   269,    -1,    -1,    -1,   273,    -1,
     275,    -1,     8,     9,    10,    11,    12,    -1,   283,    -1,
      -1,    -1,    -1,    -1,    -1,   319,    22,    23,    24,    25,
      26,    27,    28,    29,    -1,    -1,    32,    -1,   319,    -1,
      -1,     8,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   315,    -1,   319,    22,    23,    24,    25,    26,
      27,    28,    29,    -1,    -1,    32,     8,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    -1,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,   152
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     4,   325,   326,    41,    58,    59,   108,   111,   113,
     116,   121,   126,   127,   128,   129,   346,   347,   348,   387,
     388,   389,   390,   392,   393,   394,   397,   398,   399,   400,
     401,   402,   403,   404,   405,   408,   409,   410,   411,   412,
     413,   414,   415,   416,   503,   504,   505,   506,   507,   508,
     509,   510,   528,   530,     0,     3,    33,    38,    41,    52,
      68,    71,    73,    74,    81,   100,   111,   130,   181,   316,
     328,   336,   342,   345,   350,   352,   355,   357,   360,   361,
     366,   372,   373,   419,   421,   431,   470,   471,   472,   473,
     511,   512,   513,   514,   522,   523,   529,   531,   532,   533,
      19,    20,    40,   417,    20,    51,   349,     3,   109,   110,
     417,   417,     3,    91,    92,   114,   117,   119,   120,   239,
     311,    66,    91,    92,   122,   123,   124,   125,    15,    35,
      40,   406,   417,   417,   417,    19,    20,    39,    50,   338,
      91,    97,    38,    56,    73,   119,   343,   344,    19,    72,
      19,   101,    20,    91,    19,    13,    15,    16,    17,    18,
      19,    20,    26,    27,    29,    31,    75,    81,   155,   156,
     170,   171,   172,   175,   314,   322,   436,   449,   454,   457,
     458,   459,   461,   463,    19,   362,   362,    60,    65,    77,
     101,   239,   290,   295,   311,   362,    41,    68,    71,    75,
      76,    78,    79,    80,   107,   111,   319,   363,   375,   362,
      46,    85,    87,   119,   127,   128,   165,   182,   183,   185,
     186,   188,   189,   190,   192,   195,   196,   198,   212,   213,
     217,   219,   221,   223,   228,   232,   233,   235,   236,   319,
     363,   362,   240,   245,   247,   283,   319,   363,   474,   475,
     476,   362,   291,   292,   319,   363,   362,    78,    85,   105,
     106,   265,   296,   297,   298,   299,   306,   319,   363,   382,
     384,   385,   362,   119,   232,   273,   319,   363,   524,   527,
      60,    61,    62,    64,    65,   319,   351,   354,   363,    62,
      93,   319,   353,   363,    35,    40,   318,   376,   349,     3,
     417,   417,     3,    35,    40,   318,   379,    19,    20,    40,
     418,     3,   118,     3,     3,   395,    19,   391,    78,   124,
     282,   288,   289,   391,   391,     3,     3,     3,     3,     3,
       3,     3,    15,   407,     3,     3,     3,    22,    57,   337,
      40,   318,   339,   376,    15,    19,   314,   327,    20,    53,
     316,    15,    17,    18,   356,   420,   358,   316,   379,   432,
      15,    19,    28,   314,   321,   453,   455,   461,    29,   314,
     179,    27,   179,    15,   321,   456,   463,   314,   314,   314,
     314,   314,   157,   158,   159,   160,   443,   463,    16,    17,
     314,   327,   436,   437,   441,   444,   445,   447,   449,   450,
     451,     5,    69,    70,    86,   115,   161,   162,   164,   165,
     166,   167,   168,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   230,   231,   241,   242,   243,   244,   309,   310,
     462,   534,     8,     9,    10,    11,    12,    22,    23,    24,
      25,    26,    27,    28,    29,    32,    86,   318,   318,   318,
     376,    19,   365,    72,   327,    34,    35,    36,    37,   327,
     329,   374,    40,    81,    82,   180,   364,   428,   364,    20,
     379,   316,   316,   318,   200,   201,    20,   329,   220,   202,
     119,   216,   104,    18,    19,   184,   330,   330,    63,   187,
      63,   316,   327,   185,   193,   204,   205,   197,   199,   214,
     215,   214,   218,   329,   222,   224,   229,    68,   197,    77,
     234,   197,   237,   316,   318,   356,   329,   327,   329,   316,
     318,   318,    68,   293,   294,   316,   318,   520,   521,    20,
     327,   318,    82,   304,   305,   515,   327,   327,    63,    63,
     183,   307,   308,   316,   316,   318,   318,   330,   331,    68,
     274,   316,   136,   191,   225,   234,   312,   313,   316,    68,
     329,    63,   329,    66,   316,   316,   316,    63,    20,   335,
     316,   316,    66,    83,    84,    89,    90,    92,   377,   378,
       3,     3,     3,     3,    83,   112,   380,   381,     3,   418,
       3,     3,     3,    40,    68,    73,    80,    81,    93,    94,
      95,    96,    98,   331,   396,   428,     3,   391,   391,   391,
      40,   391,    40,   391,     3,     3,     3,    18,   327,    20,
      41,    42,    43,    44,    45,    46,    47,    48,    49,   340,
     341,   316,   463,   327,    54,    55,   316,   138,   139,   140,
     141,   142,   143,   144,   146,   147,   148,   149,   150,   422,
     423,   424,   426,   430,   102,   103,   104,   105,   316,   359,
     316,   314,   429,   455,   455,   463,   455,   455,    14,    15,
     463,   467,   468,    26,   179,    27,   456,   456,    29,   463,
     463,   463,   463,   463,    15,   320,   315,   320,    28,   439,
     440,   443,   463,     7,   320,   323,    26,    27,   318,   320,
     323,   463,   463,   463,   463,   463,   463,   463,   463,   463,
     463,   463,   463,   463,   140,   163,   169,   176,   177,    19,
     356,   367,   370,   371,    19,   426,   427,   463,    63,    63,
     316,   316,   327,    46,   227,   329,   330,   327,   184,   184,
     327,    63,   327,   316,    63,   194,   327,   327,   329,    63,
     316,    72,   316,   329,   316,   329,   194,   225,   226,   329,
     365,   329,   316,   316,   329,   329,   220,   477,    19,   316,
     316,   371,   316,   316,   383,   384,   316,   316,   316,   327,
     327,   316,   316,   316,    72,    99,   135,   386,    29,   317,
     333,   365,   329,    20,   330,   330,   191,   525,   526,   327,
     327,   329,   327,   330,   331,   332,   319,   320,   319,   320,
       3,    19,    19,   427,   332,    19,    20,   502,   502,   502,
     391,   502,   391,   502,   316,   316,   319,   320,   315,    20,
      19,   145,   316,   318,   330,   327,   315,   423,   425,   430,
     315,   320,   315,    27,   320,   315,   320,   320,   320,   463,
     463,     7,   320,   320,    18,   437,   445,    15,    18,   450,
     314,    27,   369,   320,   327,   327,   316,   329,   329,   316,
     316,   316,   327,   327,   316,   327,   316,   327,   316,   316,
     316,   316,   327,   356,   316,   316,   316,   316,   316,   316,
     316,   316,   316,   316,   327,    78,   118,   195,   246,   270,
     272,   279,   281,   285,   286,   319,   478,   485,   488,   316,
     318,   519,   319,   316,   316,   316,   327,    86,    88,    86,
      88,   319,   327,   330,   316,   316,   330,   284,   525,   378,
     381,     3,     3,     3,   502,     3,   502,     3,   341,   424,
      19,    46,    75,   131,   132,   133,   134,   135,   136,   137,
     151,   154,   433,   434,   459,   460,   464,   469,   316,   315,
     467,    27,   463,   463,   463,   463,   320,   315,   439,   463,
     440,   442,   463,   320,   463,    20,   332,   368,   370,   316,
     320,   316,   316,   316,   316,   316,   316,   316,   316,   498,
     501,   329,   248,   249,   285,   248,   329,   479,   274,   331,
     481,   482,   480,   287,   329,   316,   318,   518,   383,   316,
      20,   334,   334,   334,   334,   327,     3,     3,    22,   314,
      22,   314,   463,   463,   463,   319,   469,   462,   534,   316,
     463,   465,   466,   425,   315,   315,   315,   315,   463,   315,
     320,    15,   315,   332,   327,   371,   327,   327,   329,   327,
     318,   356,   318,   318,   327,   486,   197,   292,   319,   517,
     386,   316,   316,   316,   316,   463,   468,   463,   459,   316,
     152,   318,    22,    22,    32,   320,   316,   315,     7,    28,
     442,   319,   316,   318,   500,   490,   483,   490,   201,   250,
     251,   252,   254,   265,   319,   382,   487,   327,   256,   300,
     301,   302,   303,   516,   316,   386,   386,   386,   386,   316,
     315,   316,   534,   318,   435,   469,   452,   463,   463,    81,
     171,   172,   175,   179,   466,   314,   315,     7,   315,   499,
     319,   331,   491,   492,   118,   248,   271,   319,   484,   319,
     327,   327,   327,   199,    95,   327,    82,   264,   267,   316,
     315,   433,   153,     6,    15,    16,    17,   314,   319,   436,
     438,   441,   444,   446,   448,   316,   316,   314,   314,   314,
     314,   316,   439,   442,   124,   201,   246,   248,   250,   251,
     252,   253,   254,   255,   265,   266,   269,   273,   275,   283,
     319,   382,   489,   271,   280,   493,   329,   327,   329,   316,
     327,   327,   316,   319,   435,   433,   463,     7,   317,   320,
     463,   463,   463,   463,   320,   315,   318,   327,   329,   327,
     327,   327,   199,   327,    95,   327,   256,   257,   258,   259,
     260,   261,   262,   263,    82,   264,   267,   258,   327,   274,
     276,   284,   316,   327,   316,   315,   314,   438,   433,   446,
     315,   315,   315,   315,   439,   494,   327,   327,   329,   329,
     277,   278,   327,   327,   463,   316,   316,   316,   316,   315,
      18,   319,   495,   496,   497,   268,   316,   316
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
	    /* Fall through.  */
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

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1464 of yacc.c  */
#line 618 "cf-parse.y"
    { return 0; ;}
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 619 "cf-parse.y"
    { return 0; ;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 632 "cf-parse.y"
    { (yyval.i) = f_eval_int((yyvsp[(2) - (3)].x)); ;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 633 "cf-parse.y"
    { if ((yyvsp[(1) - (1)].s)->class != SYM_NUMBER) cf_error("Number expected"); else (yyval.i) = (yyvsp[(1) - (1)].s)->aux; ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 639 "cf-parse.y"
    {
     cf_define_symbol((yyvsp[(2) - (5)].s), SYM_NUMBER, NULL);
     (yyvsp[(2) - (5)].s)->aux = (yyvsp[(4) - (5)].i);
   ;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 643 "cf-parse.y"
    {
     cf_define_symbol((yyvsp[(2) - (5)].s), SYM_IPA, cfg_alloc(sizeof(ip_addr)));
     *(ip_addr *)(yyvsp[(2) - (5)].s)->def = (yyvsp[(4) - (5)].a);
   ;}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 652 "cf-parse.y"
    {(yyval.i) = !!(yyvsp[(1) - (1)].i); ;}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 653 "cf-parse.y"
    { (yyval.i) = 1; ;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 654 "cf-parse.y"
    { (yyval.i) = 1; ;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 655 "cf-parse.y"
    { (yyval.i) = 0; ;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 656 "cf-parse.y"
    { (yyval.i) = 0; ;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 657 "cf-parse.y"
    { (yyval.i) = 1; ;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 664 "cf-parse.y"
    {
     if ((yyvsp[(1) - (1)].s)->class != SYM_IPA) cf_error("IP address expected");
     (yyval.a) = *(ip_addr *)(yyvsp[(1) - (1)].s)->def;
   ;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 671 "cf-parse.y"
    {
     if (!ip_is_prefix((yyvsp[(1) - (2)].a), (yyvsp[(2) - (2)].i))) cf_error("Invalid prefix");
     (yyval.px).addr = (yyvsp[(1) - (2)].a); (yyval.px).len = (yyvsp[(2) - (2)].i);
   ;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 679 "cf-parse.y"
    { (yyval.px).addr = (yyvsp[(1) - (1)].a); (yyval.px).len = BITS_PER_IP_ADDRESS; ;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 683 "cf-parse.y"
    {
     if ((yyvsp[(2) - (2)].i) < 0 || (yyvsp[(2) - (2)].i) > BITS_PER_IP_ADDRESS) cf_error("Invalid prefix length %d", (yyvsp[(2) - (2)].i));
     (yyval.i) = (yyvsp[(2) - (2)].i);
   ;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 687 "cf-parse.y"
    {
     (yyval.i) = ipa_mklen((yyvsp[(2) - (2)].a));
     if ((yyval.i) < 0) cf_error("Invalid netmask %I", (yyvsp[(2) - (2)].a));
   ;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 694 "cf-parse.y"
    {
     (yyval.time) = tm_parse_datetime((yyvsp[(1) - (1)].t));
     if (!(yyval.time))
       cf_error("Invalid date and time");
   ;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 702 "cf-parse.y"
    { (yyval.t) = (yyvsp[(1) - (1)].t); ;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 703 "cf-parse.y"
    { (yyval.t) = NULL; ;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 709 "cf-parse.y"
    {
    struct log_config *c = cfg_allocz(sizeof(struct log_config));
    c->fh = (yyvsp[(2) - (4)].g);
    c->mask = (yyvsp[(3) - (4)].i);
    add_tail(&new_config->logfiles, &c->n);
  ;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 718 "cf-parse.y"
    { (yyval.t) = (yyvsp[(2) - (2)].t); ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 719 "cf-parse.y"
    { (yyval.t) = bird_name; ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 723 "cf-parse.y"
    {
     FILE *f = tracked_fopen(new_config->pool, (yyvsp[(1) - (1)].t), "a");
     if (!f) cf_error("Unable to open log file `%s': %m", (yyvsp[(1) - (1)].t));
     (yyval.g) = f;
   ;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 728 "cf-parse.y"
    { (yyval.g) = NULL; new_config->syslog_name = (yyvsp[(2) - (2)].t); ;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 729 "cf-parse.y"
    { (yyval.g) = stderr; ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 733 "cf-parse.y"
    { (yyval.i) = ~0; ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 734 "cf-parse.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); ;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 738 "cf-parse.y"
    { (yyval.i) = 1 << (yyvsp[(1) - (1)].i); ;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 739 "cf-parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) | (1 << (yyvsp[(3) - (3)].i)); ;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 743 "cf-parse.y"
    { (yyval.i) = L_DEBUG[0]; ;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 744 "cf-parse.y"
    { (yyval.i) = L_TRACE[0]; ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 745 "cf-parse.y"
    { (yyval.i) = L_INFO[0]; ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 746 "cf-parse.y"
    { (yyval.i) = L_REMOTE[0]; ;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 747 "cf-parse.y"
    { (yyval.i) = L_WARN[0]; ;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 748 "cf-parse.y"
    { (yyval.i) = L_ERR[0]; ;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 749 "cf-parse.y"
    { (yyval.i) = L_AUTH[0]; ;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 750 "cf-parse.y"
    { (yyval.i) = L_FATAL[0]; ;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 751 "cf-parse.y"
    { (yyval.i) = L_BUG[0]; ;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 757 "cf-parse.y"
    { new_config->proto_default_mrtdump = (yyvsp[(3) - (4)].i); ;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 758 "cf-parse.y"
    {
     FILE *f = tracked_fopen(new_config->pool, (yyvsp[(2) - (3)].t), "a");
     if (!f) cf_error("Unable to open MRTDump file '%s': %m", (yyvsp[(2) - (3)].t));
     new_config->mrtdump_file = fileno(f);
   ;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 767 "cf-parse.y"
    { (yyval.tf) = &new_config->tf_route; ;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 768 "cf-parse.y"
    { (yyval.tf) = &new_config->tf_proto; ;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 769 "cf-parse.y"
    { (yyval.tf) = &new_config->tf_base; ;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 770 "cf-parse.y"
    { (yyval.tf) = &new_config->tf_log; ;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 773 "cf-parse.y"
    { *(yyvsp[(1) - (2)].tf) = (struct timeformat){(yyvsp[(2) - (2)].t), NULL, 0}; ;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 774 "cf-parse.y"
    { *(yyvsp[(1) - (4)].tf) = (struct timeformat){(yyvsp[(2) - (4)].t), (yyvsp[(4) - (4)].t), (yyvsp[(3) - (4)].i)}; ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 775 "cf-parse.y"
    { *(yyvsp[(1) - (3)].tf) = (struct timeformat){"%T", "%F", 20*3600}; ;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 776 "cf-parse.y"
    { *(yyvsp[(1) - (3)].tf) = (struct timeformat){"%F %T", NULL, 0}; ;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 788 "cf-parse.y"
    { cmd_reconfig((yyvsp[(2) - (3)].t), RECONFIG_HARD); ;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 791 "cf-parse.y"
    { cmd_reconfig((yyvsp[(3) - (4)].t), RECONFIG_SOFT); ;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 794 "cf-parse.y"
    { cmd_shutdown(); ;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 797 "cf-parse.y"
    { (yyval.t) = NULL; ;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 806 "cf-parse.y"
    {
#ifndef CONFIG_MULTIPLE_TABLES
     if (cf_krt)
       cf_error("Kernel protocol already defined");
#endif
     cf_krt = this_proto = proto_config_new(&proto_unix_kernel, sizeof(struct krt_config), (yyvsp[(1) - (2)].i));
     this_proto->preference = DEF_PREF_INHERITED;
     THIS_KRT->scan_time = 60;
     THIS_KRT->learn = THIS_KRT->persist = 0;
     krt_scan_construct(THIS_KRT);
     krt_set_construct(THIS_KRT);
   ;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 822 "cf-parse.y"
    { THIS_KRT->persist = (yyvsp[(2) - (2)].i); ;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 823 "cf-parse.y"
    {
      /* Scan time of 0 means scan on startup only */
      THIS_KRT->scan_time = (yyvsp[(3) - (3)].i);
   ;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 827 "cf-parse.y"
    {
      THIS_KRT->learn = (yyvsp[(2) - (2)].i);
#ifndef KRT_ALLOW_LEARN
      if ((yyvsp[(2) - (2)].i))
	cf_error("Learning of kernel routes not supported in this configuration");
#endif
   ;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 834 "cf-parse.y"
    { THIS_KRT->devroutes = (yyvsp[(3) - (3)].i); ;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 840 "cf-parse.y"
    {
     if (cf_kif)
       cf_error("Kernel device protocol already defined");
     cf_kif = this_proto = proto_config_new(&proto_unix_iface, sizeof(struct kif_config), (yyvsp[(1) - (2)].i));
     this_proto->preference = DEF_PREF_DIRECT;
     THIS_KIF->scan_time = 60;
     init_list(&THIS_KIF->primary);
     krt_if_construct(THIS_KIF);
   ;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 853 "cf-parse.y"
    {
      /* Scan time of 0 means scan on startup only */
      THIS_KIF->scan_time = (yyvsp[(3) - (3)].i);
   ;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 857 "cf-parse.y"
    {
     struct kif_primary_item *kpi = cfg_alloc(sizeof (struct kif_primary_item));
     kpi->pattern = (yyvsp[(2) - (3)].t);
     kpi->prefix = (yyvsp[(3) - (3)].px).addr;
     kpi->pxlen = (yyvsp[(3) - (3)].px).len;
     add_tail(&THIS_KIF->primary, &kpi->n);
   ;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 870 "cf-parse.y"
    {
	if ((yyvsp[(3) - (3)].i) <= 0 || (yyvsp[(3) - (3)].i) >= NL_NUM_TABLES)
	  cf_error("Kernel routing table number out of range");
	THIS_KRT->scan.table_id = (yyvsp[(3) - (3)].i);
   ;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 883 "cf-parse.y"
    {
   new_config->router_id = (yyvsp[(3) - (4)].i32);
   ;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 889 "cf-parse.y"
    { (yyval.i32) = (yyvsp[(1) - (1)].i); ;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 891 "cf-parse.y"
    {
#ifndef IPV6
     (yyval.i32) = ipa_to_u32((yyvsp[(1) - (1)].a));
#else
     cf_error("Router IDs must be entered as hexadecimal numbers or IPv4 addresses in IPv6 version");
#endif
   ;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 910 "cf-parse.y"
    { new_config->listen_bgp_addr = (yyvsp[(2) - (2)].a); ;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 911 "cf-parse.y"
    { new_config->listen_bgp_port = (yyvsp[(2) - (2)].i); ;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 912 "cf-parse.y"
    { new_config->listen_bgp_flags = 0; ;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 913 "cf-parse.y"
    { new_config->listen_bgp_flags = 1; ;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 920 "cf-parse.y"
    {
   rt_new_table((yyvsp[(2) - (2)].s));
   ;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 929 "cf-parse.y"
    { (yyval.i) = SYM_PROTO; ;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 930 "cf-parse.y"
    { (yyval.i) = SYM_TEMPLATE; ;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 934 "cf-parse.y"
    {
     struct symbol *s = cf_default_name(this_proto->protocol->template, &this_proto->protocol->name_counter);
     s->class = this_proto->class;
     s->def = this_proto;
     this_proto->name = s->name;
     ;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 940 "cf-parse.y"
    {
     cf_define_symbol((yyvsp[(1) - (1)].s), this_proto->class, this_proto);
     this_proto->name = (yyvsp[(1) - (1)].s)->name;
   ;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 944 "cf-parse.y"
    {
     if (((yyvsp[(3) - (3)].s)->class != SYM_TEMPLATE) && ((yyvsp[(3) - (3)].s)->class != SYM_PROTO)) cf_error("Template or protocol name expected");

     cf_define_symbol((yyvsp[(1) - (3)].s), this_proto->class, this_proto);
     this_proto->name = (yyvsp[(1) - (3)].s)->name;

     proto_copy_config(this_proto, (yyvsp[(3) - (3)].s)->def);
   ;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 956 "cf-parse.y"
    {
     if ((yyvsp[(2) - (2)].i) < 0 || (yyvsp[(2) - (2)].i) > 0xFFFF) cf_error("Invalid preference");
     this_proto->preference = (yyvsp[(2) - (2)].i);
   ;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 960 "cf-parse.y"
    { this_proto->disabled = (yyvsp[(2) - (2)].i); ;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 961 "cf-parse.y"
    { this_proto->debug = (yyvsp[(2) - (2)].i); ;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 962 "cf-parse.y"
    { this_proto->mrtdump = (yyvsp[(2) - (2)].i); ;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 963 "cf-parse.y"
    { this_proto->in_filter = (yyvsp[(2) - (2)].f); ;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 964 "cf-parse.y"
    { this_proto->out_filter = (yyvsp[(2) - (2)].f); ;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 965 "cf-parse.y"
    { this_proto->table = (yyvsp[(2) - (2)].r); ;}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 966 "cf-parse.y"
    { this_proto->router_id = (yyvsp[(3) - (3)].i32); ;}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 967 "cf-parse.y"
    { this_proto->dsc = (yyvsp[(2) - (2)].t); ;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 971 "cf-parse.y"
    { (yyval.f) = (yyvsp[(2) - (2)].f); ;}
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 973 "cf-parse.y"
    { (yyval.f) = FILTER_ACCEPT; ;}
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 974 "cf-parse.y"
    { (yyval.f) = FILTER_REJECT; ;}
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 978 "cf-parse.y"
    {
     if ((yyvsp[(1) - (1)].s)->class != SYM_TABLE) cf_error("Table name expected");
     (yyval.r) = (yyvsp[(1) - (1)].s)->def;
   ;}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 986 "cf-parse.y"
    { new_config->proto_default_debug = (yyvsp[(3) - (3)].i); ;}
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 987 "cf-parse.y"
    { new_config->cli_debug = (yyvsp[(3) - (3)].i); ;}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 995 "cf-parse.y"
    {
     struct iface_patt_node *ipn = cfg_allocz(sizeof(struct iface_patt_node));
     add_tail(&this_ipatt->ipn_list, NODE ipn);
     this_ipn = ipn;
   ;}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 1003 "cf-parse.y"
    { this_ipn->pattern = (yyvsp[(1) - (1)].t); this_ipn->prefix = IPA_NONE; this_ipn->pxlen = 0; ;}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 1004 "cf-parse.y"
    { this_ipn->pattern = NULL; this_ipn->prefix = (yyvsp[(1) - (1)].px).addr; this_ipn->pxlen = (yyvsp[(1) - (1)].px).len; ;}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 1005 "cf-parse.y"
    { this_ipn->pattern = (yyvsp[(1) - (2)].t); this_ipn->prefix = (yyvsp[(2) - (2)].px).addr; this_ipn->pxlen = (yyvsp[(2) - (2)].px).len; ;}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 1009 "cf-parse.y"
    { this_ipn->positive = 1; ;}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 1010 "cf-parse.y"
    { this_ipn->positive = 0; ;}
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 1027 "cf-parse.y"
    {
     this_proto = proto_config_new(&proto_device, sizeof(struct rt_dev_config), (yyvsp[(1) - (2)].i));
     this_proto->preference = DEF_PREF_DIRECT;
     init_list(&DIRECT_CFG->iface_list);
   ;}
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 1041 "cf-parse.y"
    {
     this_ipatt = cfg_allocz(sizeof(struct iface_patt));
     add_tail(&DIRECT_CFG->iface_list, NODE this_ipatt);
     init_list(&this_ipatt->ipn_list);
   ;}
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 1055 "cf-parse.y"
    { (yyval.i) = ~0; ;}
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 1056 "cf-parse.y"
    { (yyval.i) = 0; ;}
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 1057 "cf-parse.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); ;}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 1062 "cf-parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) | (yyvsp[(3) - (3)].i); ;}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 1066 "cf-parse.y"
    { (yyval.i) = D_STATES; ;}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 1067 "cf-parse.y"
    { (yyval.i) = D_ROUTES; ;}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 1068 "cf-parse.y"
    { (yyval.i) = D_FILTERS; ;}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 1069 "cf-parse.y"
    { (yyval.i) = D_IFACES; ;}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 1070 "cf-parse.y"
    { (yyval.i) = D_EVENTS; ;}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 1071 "cf-parse.y"
    { (yyval.i) = D_PACKETS; ;}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 1077 "cf-parse.y"
    { (yyval.i) = ~0; ;}
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 1078 "cf-parse.y"
    { (yyval.i) = 0; ;}
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 1079 "cf-parse.y"
    { (yyval.i) = (yyvsp[(2) - (3)].i); ;}
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 1084 "cf-parse.y"
    { (yyval.i) = (yyvsp[(1) - (3)].i) | (yyvsp[(3) - (3)].i); ;}
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 1088 "cf-parse.y"
    { (yyval.i) = MD_STATES; ;}
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 1089 "cf-parse.y"
    { (yyval.i) = MD_MESSAGES; ;}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 1110 "cf-parse.y"
    {
     if (!this_p_list) {
     	this_p_list = cfg_alloc(sizeof(list));
     	init_list(this_p_list);
        password_id = 1;
     }
     this_p_item = cfg_alloc(sizeof (struct password_item));
     this_p_item->password = (yyvsp[(2) - (2)].t);
     this_p_item->genfrom = 0;
     this_p_item->gento = TIME_INFINITY;
     this_p_item->accfrom = 0;
     this_p_item->accto = TIME_INFINITY;
     this_p_item->id = password_id++;
     add_tail(this_p_list, &this_p_item->n);
   ;}
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 1128 "cf-parse.y"
    { ;}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 1129 "cf-parse.y"
    { this_p_item->genfrom = (yyvsp[(3) - (5)].time); ;}
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 1130 "cf-parse.y"
    { this_p_item->gento = (yyvsp[(3) - (5)].time); ;}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 1131 "cf-parse.y"
    { this_p_item->accfrom = (yyvsp[(3) - (5)].time); ;}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 1132 "cf-parse.y"
    { this_p_item->accto = (yyvsp[(3) - (5)].time); ;}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 1133 "cf-parse.y"
    { this_p_item->id = (yyvsp[(2) - (4)].i); if ((yyvsp[(2) - (4)].i) <= 0) cf_error("Password ID has to be greated than zero."); ;}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 1142 "cf-parse.y"
    { cmd_show_status(); ;}
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 1145 "cf-parse.y"
    { cmd_show_memory(); ;}
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 1148 "cf-parse.y"
    { proto_apply_cmd((yyvsp[(3) - (4)].ps), proto_cmd_show, 0, 0); ;}
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 1151 "cf-parse.y"
    { proto_apply_cmd((yyvsp[(4) - (5)].ps), proto_cmd_show, 0, 1); ;}
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 1155 "cf-parse.y"
    { (yyval.s) = NULL; ;}
    break;

  case 157:

/* Line 1464 of yacc.c  */
#line 1159 "cf-parse.y"
    { if_show(); ;}
    break;

  case 158:

/* Line 1464 of yacc.c  */
#line 1162 "cf-parse.y"
    { if_show_summary(); ;}
    break;

  case 159:

/* Line 1464 of yacc.c  */
#line 1165 "cf-parse.y"
    { rt_show((yyvsp[(3) - (4)].ra)); ;}
    break;

  case 160:

/* Line 1464 of yacc.c  */
#line 1168 "cf-parse.y"
    {
     (yyval.ra) = cfg_allocz(sizeof(struct rt_show_data));
     (yyval.ra)->pxlen = 256;
     (yyval.ra)->filter = FILTER_ACCEPT;
     (yyval.ra)->table = config->master_rtc->table;
   ;}
    break;

  case 161:

/* Line 1464 of yacc.c  */
#line 1174 "cf-parse.y"
    {
     (yyval.ra) = (yyvsp[(1) - (2)].ra);
     if ((yyval.ra)->pxlen != 256) cf_error("Only one prefix expected");
     (yyval.ra)->prefix = (yyvsp[(2) - (2)].px).addr;
     (yyval.ra)->pxlen = (yyvsp[(2) - (2)].px).len;
   ;}
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 1180 "cf-parse.y"
    {
     (yyval.ra) = (yyvsp[(1) - (3)].ra);
     if ((yyval.ra)->pxlen != 256) cf_error("Only one prefix expected");
     (yyval.ra)->prefix = (yyvsp[(3) - (3)].px).addr;
     (yyval.ra)->pxlen = (yyvsp[(3) - (3)].px).len;
     (yyval.ra)->show_for = 1;
   ;}
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 1187 "cf-parse.y"
    {
     (yyval.ra) = (yyvsp[(1) - (3)].ra);
     if ((yyvsp[(3) - (3)].s)->class != SYM_TABLE) cf_error("%s is not a table", (yyvsp[(3) - (3)].s)->name);
     (yyval.ra)->table = ((struct rtable_config *)(yyvsp[(3) - (3)].s)->def)->table;
   ;}
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 1192 "cf-parse.y"
    {
     (yyval.ra) = (yyvsp[(1) - (3)].ra);
     if ((yyval.ra)->filter != FILTER_ACCEPT) cf_error("Filter specified twice");
     (yyval.ra)->filter = (yyvsp[(3) - (3)].f);
   ;}
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 1197 "cf-parse.y"
    {
     (yyval.ra) = (yyvsp[(1) - (2)].ra);
     if ((yyval.ra)->filter != FILTER_ACCEPT) cf_error("Filter specified twice");
     (yyval.ra)->filter = (yyvsp[(2) - (2)].f);
   ;}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 1202 "cf-parse.y"
    {
     (yyval.ra) = (yyvsp[(1) - (2)].ra);
     (yyval.ra)->verbose = 1;
   ;}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 1206 "cf-parse.y"
    {
     (yyval.ra) = (yyvsp[(1) - (2)].ra);
     (yyval.ra)->primary_only = 1;
   ;}
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 1210 "cf-parse.y"
    {
     struct proto_config *c = (struct proto_config *) (yyvsp[(3) - (3)].s)->def;
     (yyval.ra) = (yyvsp[(1) - (3)].ra);
     if ((yyval.ra)->export_mode) cf_error("Protocol specified twice");
     if ((yyvsp[(3) - (3)].s)->class != SYM_PROTO || !c->proto) cf_error("%s is not a protocol", (yyvsp[(3) - (3)].s)->name);
     (yyval.ra)->export_mode = (yyvsp[(2) - (3)].i);
     (yyval.ra)->primary_only = 1;
     (yyval.ra)->export_protocol = c->proto;
     (yyval.ra)->running_on_config = c->proto->cf->global;
   ;}
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 1220 "cf-parse.y"
    {
     struct proto_config *c = (struct proto_config *) (yyvsp[(3) - (3)].s)->def;
     (yyval.ra) = (yyvsp[(1) - (3)].ra);
     if ((yyval.ra)->show_protocol) cf_error("Protocol specified twice");
     if ((yyvsp[(3) - (3)].s)->class != SYM_PROTO || !c->proto) cf_error("%s is not a protocol", (yyvsp[(3) - (3)].s)->name);
     (yyval.ra)->show_protocol = c->proto;
     (yyval.ra)->running_on_config = c->proto->cf->global;
   ;}
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 1228 "cf-parse.y"
    {
     (yyval.ra) = (yyvsp[(1) - (2)].ra);
     (yyval.ra)->stats = 1;
   ;}
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 1232 "cf-parse.y"
    {
     (yyval.ra) = (yyvsp[(1) - (2)].ra);
     (yyval.ra)->stats = 2;
   ;}
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 1239 "cf-parse.y"
    { (yyval.i) = 1; ;}
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 1240 "cf-parse.y"
    { (yyval.i) = 2; ;}
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 1244 "cf-parse.y"
    { cmd_show_symbols((yyvsp[(3) - (4)].s)); ;}
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 1248 "cf-parse.y"
    { rdump(&root_pool); cli_msg(0, ""); ;}
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 1250 "cf-parse.y"
    { sk_dump_all(); cli_msg(0, ""); ;}
    break;

  case 177:

/* Line 1464 of yacc.c  */
#line 1252 "cf-parse.y"
    { if_dump_all(); cli_msg(0, ""); ;}
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 1254 "cf-parse.y"
    { neigh_dump_all(); cli_msg(0, ""); ;}
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 1256 "cf-parse.y"
    { rta_dump_all(); cli_msg(0, ""); ;}
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 1258 "cf-parse.y"
    { rt_dump_all(); cli_msg(0, ""); ;}
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 1260 "cf-parse.y"
    { protos_dump_all(); cli_msg(0, ""); ;}
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 1262 "cf-parse.y"
    {
  cli_set_log_echo(this_cli, (yyvsp[(2) - (4)].i), (yyvsp[(3) - (4)].i));
  cli_msg(0, "");
;}
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 1268 "cf-parse.y"
    { (yyval.i) = ~0; ;}
    break;

  case 184:

/* Line 1464 of yacc.c  */
#line 1269 "cf-parse.y"
    { (yyval.i) = 0; ;}
    break;

  case 186:

/* Line 1464 of yacc.c  */
#line 1274 "cf-parse.y"
    { (yyval.i) = 4096; ;}
    break;

  case 187:

/* Line 1464 of yacc.c  */
#line 1275 "cf-parse.y"
    {
     if ((yyvsp[(1) - (1)].i) < 256 || (yyvsp[(1) - (1)].i) > 65536) cf_error("Invalid log buffer size");
     (yyval.i) = (yyvsp[(1) - (1)].i);
   ;}
    break;

  case 188:

/* Line 1464 of yacc.c  */
#line 1282 "cf-parse.y"
    { proto_apply_cmd((yyvsp[(2) - (3)].ps), proto_cmd_disable, 1, 0); ;}
    break;

  case 189:

/* Line 1464 of yacc.c  */
#line 1284 "cf-parse.y"
    { proto_apply_cmd((yyvsp[(2) - (3)].ps), proto_cmd_enable, 1, 0); ;}
    break;

  case 190:

/* Line 1464 of yacc.c  */
#line 1286 "cf-parse.y"
    { proto_apply_cmd((yyvsp[(2) - (3)].ps), proto_cmd_restart, 1, 0); ;}
    break;

  case 191:

/* Line 1464 of yacc.c  */
#line 1288 "cf-parse.y"
    { proto_apply_cmd((yyvsp[(2) - (3)].ps), proto_cmd_reload, 1, CMD_RELOAD); ;}
    break;

  case 192:

/* Line 1464 of yacc.c  */
#line 1290 "cf-parse.y"
    { proto_apply_cmd((yyvsp[(3) - (4)].ps), proto_cmd_reload, 1, CMD_RELOAD_IN); ;}
    break;

  case 193:

/* Line 1464 of yacc.c  */
#line 1292 "cf-parse.y"
    { proto_apply_cmd((yyvsp[(3) - (4)].ps), proto_cmd_reload, 1, CMD_RELOAD_OUT); ;}
    break;

  case 194:

/* Line 1464 of yacc.c  */
#line 1296 "cf-parse.y"
    { proto_apply_cmd((yyvsp[(2) - (4)].ps), proto_cmd_debug, 1, (yyvsp[(3) - (4)].i)); ;}
    break;

  case 195:

/* Line 1464 of yacc.c  */
#line 1300 "cf-parse.y"
    { proto_apply_cmd((yyvsp[(2) - (4)].ps), proto_cmd_mrtdump, 1, (yyvsp[(3) - (4)].i)); ;}
    break;

  case 196:

/* Line 1464 of yacc.c  */
#line 1303 "cf-parse.y"
    { this_cli->restricted = 1; cli_msg(16, "Access restricted"); ;}
    break;

  case 197:

/* Line 1464 of yacc.c  */
#line 1306 "cf-parse.y"
    { (yyval.ps).ptr = (yyvsp[(1) - (1)].s); (yyval.ps).patt = 0; ;}
    break;

  case 198:

/* Line 1464 of yacc.c  */
#line 1307 "cf-parse.y"
    { (yyval.ps).ptr = NULL; (yyval.ps).patt = 1; ;}
    break;

  case 199:

/* Line 1464 of yacc.c  */
#line 1308 "cf-parse.y"
    { (yyval.ps).ptr = (yyvsp[(1) - (1)].t); (yyval.ps).patt = 1; ;}
    break;

  case 200:

/* Line 1464 of yacc.c  */
#line 1312 "cf-parse.y"
    { (yyval.ps).ptr = (yyvsp[(1) - (1)].s); (yyval.ps).patt = 0; ;}
    break;

  case 201:

/* Line 1464 of yacc.c  */
#line 1313 "cf-parse.y"
    { (yyval.ps).ptr = NULL; (yyval.ps).patt = 1; ;}
    break;

  case 202:

/* Line 1464 of yacc.c  */
#line 1314 "cf-parse.y"
    { (yyval.ps).ptr = (yyvsp[(1) - (1)].t); (yyval.ps).patt = 1; ;}
    break;

  case 203:

/* Line 1464 of yacc.c  */
#line 1322 "cf-parse.y"
    { (yyvsp[(2) - (2)].s) = cf_define_symbol((yyvsp[(2) - (2)].s), SYM_FILTER, NULL); cf_push_scope( (yyvsp[(2) - (2)].s) ); ;}
    break;

  case 204:

/* Line 1464 of yacc.c  */
#line 1323 "cf-parse.y"
    {
     (yyvsp[(2) - (4)].s)->def = (yyvsp[(4) - (4)].f);
     (yyvsp[(4) - (4)].f)->name = (yyvsp[(2) - (4)].s)->name;
     DBG( "We have new filter defined (%s)\n", (yyvsp[(2) - (4)].s)->name );
     cf_pop_scope();
   ;}
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 1332 "cf-parse.y"
    { f_eval_int((yyvsp[(2) - (2)].x)); ;}
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 1336 "cf-parse.y"
    { (yyval.i) = T_INT; ;}
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 1337 "cf-parse.y"
    { (yyval.i) = T_BOOL; ;}
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 1338 "cf-parse.y"
    { (yyval.i) = T_IP; ;}
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 1339 "cf-parse.y"
    { (yyval.i) = T_PREFIX; ;}
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 1340 "cf-parse.y"
    { (yyval.i) = T_PAIR; ;}
    break;

  case 211:

/* Line 1464 of yacc.c  */
#line 1341 "cf-parse.y"
    { (yyval.i) = T_QUAD; ;}
    break;

  case 212:

/* Line 1464 of yacc.c  */
#line 1342 "cf-parse.y"
    { (yyval.i) = T_EC; ;}
    break;

  case 213:

/* Line 1464 of yacc.c  */
#line 1343 "cf-parse.y"
    { (yyval.i) = T_STRING; ;}
    break;

  case 214:

/* Line 1464 of yacc.c  */
#line 1344 "cf-parse.y"
    { (yyval.i) = T_PATH_MASK; ;}
    break;

  case 215:

/* Line 1464 of yacc.c  */
#line 1345 "cf-parse.y"
    { (yyval.i) = T_PATH; ;}
    break;

  case 216:

/* Line 1464 of yacc.c  */
#line 1346 "cf-parse.y"
    { (yyval.i) = T_CLIST; ;}
    break;

  case 217:

/* Line 1464 of yacc.c  */
#line 1347 "cf-parse.y"
    { (yyval.i) = T_ECLIST; ;}
    break;

  case 218:

/* Line 1464 of yacc.c  */
#line 1348 "cf-parse.y"
    { 
	switch ((yyvsp[(1) - (2)].i)) {
	  case T_INT:
	  case T_PAIR:
	  case T_QUAD:
	  case T_EC:
	  case T_IP:
	       (yyval.i) = T_SET;
	       break;

	  case T_PREFIX:
	       (yyval.i) = T_PREFIX_SET;
	    break;

	  default:
		cf_error( "You can't create sets of this type." );
	}
   ;}
    break;

  case 219:

/* Line 1464 of yacc.c  */
#line 1369 "cf-parse.y"
    {
     struct f_val * val = cfg_alloc(sizeof(struct f_val)); 
     val->type = (yyvsp[(1) - (2)].i); 
     (yyvsp[(2) - (2)].s) = cf_define_symbol((yyvsp[(2) - (2)].s), SYM_VARIABLE | (yyvsp[(1) - (2)].i), val);
     DBG( "New variable %s type %x\n", (yyvsp[(2) - (2)].s)->name, (yyvsp[(1) - (2)].i) );
     (yyvsp[(2) - (2)].s)->aux2 = NULL;
     (yyval.s)=(yyvsp[(2) - (2)].s);
   ;}
    break;

  case 220:

/* Line 1464 of yacc.c  */
#line 1380 "cf-parse.y"
    { (yyval.s) = NULL; ;}
    break;

  case 221:

/* Line 1464 of yacc.c  */
#line 1381 "cf-parse.y"
    {
     (yyval.s) = (yyvsp[(1) - (3)].s);
     (yyval.s)->aux2 = (yyvsp[(3) - (3)].s);
   ;}
    break;

  case 222:

/* Line 1464 of yacc.c  */
#line 1388 "cf-parse.y"
    { (yyval.s) = (yyvsp[(1) - (1)].s); ;}
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 1389 "cf-parse.y"
    {
     (yyval.s) = (yyvsp[(1) - (3)].s);
     (yyval.s)->aux2 = (yyvsp[(3) - (3)].s);
   ;}
    break;

  case 224:

/* Line 1464 of yacc.c  */
#line 1396 "cf-parse.y"
    {
     struct filter *f = cfg_alloc(sizeof(struct filter));
     f->name = NULL;
     f->root = (yyvsp[(1) - (1)].x);
     (yyval.f) = f;
   ;}
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 1405 "cf-parse.y"
    {
     if ((yyvsp[(1) - (1)].s)->class != SYM_FILTER) cf_error("No such filter.");
     (yyval.f) = (yyvsp[(1) - (1)].s)->def;
   ;}
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 1413 "cf-parse.y"
    {
     /* Construct 'IF term THEN ACCEPT; REJECT;' */
     struct filter *f = cfg_alloc(sizeof(struct filter));
     struct f_inst *i, *acc, *rej;
     acc = f_new_inst();		/* ACCEPT */
     acc->code = P('p',',');
     acc->a1.p = NULL;
     acc->a2.i = F_ACCEPT;
     rej = f_new_inst();		/* REJECT */
     rej->code = P('p',',');
     rej->a1.p = NULL;
     rej->a2.i = F_REJECT;
     i = f_new_inst();			/* IF */
     i->code = '?';
     i->a1.p = (yyvsp[(2) - (2)].x);
     i->a2.p = acc;
     i->next = rej;
     f->name = NULL;
     f->root = i;
     (yyval.f) = f;
  ;}
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 1437 "cf-parse.y"
    { DBG( "Have function parameters\n" ); (yyval.s)=(yyvsp[(2) - (3)].s); ;}
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 1438 "cf-parse.y"
    { (yyval.s)=NULL; ;}
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 1442 "cf-parse.y"
    {
     if ((yyvsp[(1) - (4)].s)) {
       /* Prepend instruction to clear local variables */
       (yyval.x) = f_new_inst();
       (yyval.x)->code = P('c','v');
       (yyval.x)->a1.p = (yyvsp[(1) - (4)].s);
       (yyval.x)->next = (yyvsp[(3) - (4)].x);
     } else
       (yyval.x) = (yyvsp[(3) - (4)].x);
   ;}
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 1455 "cf-parse.y"
    { DBG( "Beginning of function %s\n", (yyvsp[(2) - (2)].s)->name );
     (yyvsp[(2) - (2)].s) = cf_define_symbol((yyvsp[(2) - (2)].s), SYM_FUNCTION, NULL);
     cf_push_scope((yyvsp[(2) - (2)].s));
   ;}
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 1458 "cf-parse.y"
    {
     (yyvsp[(2) - (5)].s)->def = (yyvsp[(5) - (5)].x);
     (yyvsp[(2) - (5)].s)->aux2 = (yyvsp[(4) - (5)].s);
     DBG("Hmm, we've got one function here - %s\n", (yyvsp[(2) - (5)].s)->name); 
     cf_pop_scope();
   ;}
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 1471 "cf-parse.y"
    { (yyval.x) = NULL; ;}
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 1472 "cf-parse.y"
    { (yyval.x) = (yyvsp[(1) - (1)].x)->next; (yyvsp[(1) - (1)].x)->next = NULL; ;}
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 1475 "cf-parse.y"
    { (yyval.x) = (yyvsp[(1) - (1)].x); (yyvsp[(1) - (1)].x)->next = (yyvsp[(1) - (1)].x); ;}
    break;

  case 236:

/* Line 1464 of yacc.c  */
#line 1476 "cf-parse.y"
    { (yyval.x) = (yyvsp[(2) - (2)].x); (yyvsp[(2) - (2)].x)->next = (yyvsp[(1) - (2)].x)->next ; (yyvsp[(1) - (2)].x)->next = (yyvsp[(2) - (2)].x); ;}
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 1480 "cf-parse.y"
    {
     (yyval.x)=(yyvsp[(1) - (1)].x);
   ;}
    break;

  case 238:

/* Line 1464 of yacc.c  */
#line 1483 "cf-parse.y"
    {
     (yyval.x)=(yyvsp[(2) - (3)].x);
   ;}
    break;

  case 239:

/* Line 1464 of yacc.c  */
#line 1492 "cf-parse.y"
    { (yyval.v).type = T_IP; (yyval.v).val.px.ip = (yyvsp[(1) - (1)].a); ;}
    break;

  case 240:

/* Line 1464 of yacc.c  */
#line 1505 "cf-parse.y"
    { (yyval.v).type = T_INT; (yyval.v).val.i = (yyvsp[(1) - (1)].i); ;}
    break;

  case 241:

/* Line 1464 of yacc.c  */
#line 1506 "cf-parse.y"
    { (yyval.v).type = T_QUAD; (yyval.v).val.i = (yyvsp[(1) - (1)].i32); ;}
    break;

  case 242:

/* Line 1464 of yacc.c  */
#line 1507 "cf-parse.y"
    { (yyval.v) = (yyvsp[(1) - (1)].v); ;}
    break;

  case 243:

/* Line 1464 of yacc.c  */
#line 1508 "cf-parse.y"
    { (yyval.v).type = pair_a((yyvsp[(1) - (1)].i)); (yyval.v).val.i = pair_b((yyvsp[(1) - (1)].i)); ;}
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 1512 "cf-parse.y"
    { (yyval.v).type = T_INT; (yyval.v).val.i = (yyvsp[(1) - (1)].i); ;}
    break;

  case 245:

/* Line 1464 of yacc.c  */
#line 1513 "cf-parse.y"
    { (yyval.v).type = T_INT; (yyval.v).val.i = f_eval_int((yyvsp[(2) - (3)].x)); ;}
    break;

  case 246:

/* Line 1464 of yacc.c  */
#line 1514 "cf-parse.y"
    { (yyval.v).type = T_QUAD; (yyval.v).val.i = (yyvsp[(1) - (1)].i32); ;}
    break;

  case 247:

/* Line 1464 of yacc.c  */
#line 1515 "cf-parse.y"
    { (yyval.v) = (yyvsp[(1) - (1)].v); ;}
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 1516 "cf-parse.y"
    { (yyval.v).type = pair_a((yyvsp[(1) - (1)].i)); (yyval.v).val.i = pair_b((yyvsp[(1) - (1)].i)); ;}
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 1520 "cf-parse.y"
    { (yyval.i) = f_eval_int((yyvsp[(1) - (1)].x)); check_u16((yyval.i)); ;}
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 1523 "cf-parse.y"
    { (yyval.i32) = pair((yyvsp[(1) - (1)].i), (yyvsp[(1) - (1)].i)); ;}
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 1524 "cf-parse.y"
    { (yyval.i32) = pair((yyvsp[(1) - (3)].i), (yyvsp[(3) - (3)].i)); ;}
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 1525 "cf-parse.y"
    { (yyval.i32) = 0xFFFF; ;}
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 1529 "cf-parse.y"
    {
     (yyval.e) = f_new_pair_set(pair_a((yyvsp[(2) - (5)].i32)), pair_b((yyvsp[(2) - (5)].i32)), pair_a((yyvsp[(4) - (5)].i32)), pair_b((yyvsp[(4) - (5)].i32)));
   ;}
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 1532 "cf-parse.y"
    {
     /* Hack: $2 and $4 should be pair_expr, but that would cause shift/reduce conflict */
     if ((pair_a((yyvsp[(2) - (11)].i32)) != pair_b((yyvsp[(2) - (11)].i32))) || (pair_a((yyvsp[(4) - (11)].i32)) != pair_b((yyvsp[(4) - (11)].i32))))
       cf_error("syntax error");
     (yyval.e) = f_new_pair_item(pair_b((yyvsp[(2) - (11)].i32)), pair_b((yyvsp[(4) - (11)].i32)), (yyvsp[(8) - (11)].i), (yyvsp[(10) - (11)].i)); 
   ;}
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 1541 "cf-parse.y"
    { (yyval.i32) = f_eval_int((yyvsp[(1) - (1)].x)); ;}
    break;

  case 256:

/* Line 1464 of yacc.c  */
#line 1544 "cf-parse.y"
    { (yyval.i) = EC_RT; ;}
    break;

  case 257:

/* Line 1464 of yacc.c  */
#line 1545 "cf-parse.y"
    { (yyval.i) = EC_RO; ;}
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 1546 "cf-parse.y"
    { (yyval.i) = (yyvsp[(2) - (2)].i); ;}
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 1547 "cf-parse.y"
    { (yyval.i) = EC_GENERIC; ;}
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 1551 "cf-parse.y"
    { (yyval.e) = f_new_ec_item((yyvsp[(2) - (7)].i), 0, (yyvsp[(4) - (7)].i32), (yyvsp[(6) - (7)].i32), (yyvsp[(6) - (7)].i32)); ;}
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 1552 "cf-parse.y"
    { (yyval.e) = f_new_ec_item((yyvsp[(2) - (9)].i), 0, (yyvsp[(4) - (9)].i32), (yyvsp[(6) - (9)].i32), (yyvsp[(8) - (9)].i32)); ;}
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 1553 "cf-parse.y"
    {  (yyval.e) = f_new_ec_item((yyvsp[(2) - (7)].i), 0, (yyvsp[(4) - (7)].i32), 0, EC_ALL); ;}
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 1559 "cf-parse.y"
    { (yyval.e) = f_new_item((yyvsp[(1) - (1)].v), (yyvsp[(1) - (1)].v)); ;}
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 1560 "cf-parse.y"
    { (yyval.e) = f_new_item((yyvsp[(1) - (3)].v), (yyvsp[(3) - (3)].v)); ;}
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 1566 "cf-parse.y"
    { (yyval.e) = f_new_item((yyvsp[(1) - (1)].v), (yyvsp[(1) - (1)].v)); ;}
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 1567 "cf-parse.y"
    { (yyval.e) = f_new_item((yyvsp[(1) - (3)].v), (yyvsp[(3) - (3)].v)); ;}
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 1572 "cf-parse.y"
    { (yyval.e) = f_merge_items((yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e)); ;}
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 1577 "cf-parse.y"
    { (yyval.e) = f_merge_items((yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e)); ;}
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 1581 "cf-parse.y"
    {
     if (((yyvsp[(3) - (3)].i) < 0) || ((yyvsp[(3) - (3)].i) > MAX_PREFIX_LENGTH) || !ip_is_prefix((yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].i))) cf_error("Invalid network prefix: %I/%d.", (yyvsp[(1) - (3)].a), (yyvsp[(3) - (3)].i));
     (yyval.v).type = T_PREFIX; (yyval.v).val.px.ip = (yyvsp[(1) - (3)].a); (yyval.v).val.px.len = (yyvsp[(3) - (3)].i);
   ;}
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 1588 "cf-parse.y"
    { (yyval.v) = (yyvsp[(1) - (1)].v); ;}
    break;

  case 277:

/* Line 1464 of yacc.c  */
#line 1589 "cf-parse.y"
    { (yyval.v) = (yyvsp[(1) - (2)].v); (yyval.v).val.px.len |= LEN_PLUS; ;}
    break;

  case 278:

/* Line 1464 of yacc.c  */
#line 1590 "cf-parse.y"
    { (yyval.v) = (yyvsp[(1) - (2)].v); (yyval.v).val.px.len |= LEN_MINUS; ;}
    break;

  case 279:

/* Line 1464 of yacc.c  */
#line 1591 "cf-parse.y"
    { 
     if (! ((0 <= (yyvsp[(3) - (6)].i)) && ((yyvsp[(3) - (6)].i) <= (yyvsp[(5) - (6)].i)) && ((yyvsp[(5) - (6)].i) <= MAX_PREFIX_LENGTH))) cf_error("Invalid prefix pattern range: {%d, %d}.", (yyvsp[(3) - (6)].i), (yyvsp[(5) - (6)].i));
     (yyval.v) = (yyvsp[(1) - (6)].v); (yyval.v).val.px.len |= LEN_RANGE | ((yyvsp[(3) - (6)].i) << 16) | ((yyvsp[(5) - (6)].i) << 8);
   ;}
    break;

  case 280:

/* Line 1464 of yacc.c  */
#line 1598 "cf-parse.y"
    { (yyval.trie) = f_new_trie(cfg_mem); trie_add_fprefix((yyval.trie), &((yyvsp[(1) - (1)].v).val.px)); ;}
    break;

  case 281:

/* Line 1464 of yacc.c  */
#line 1599 "cf-parse.y"
    { (yyval.trie) = (yyvsp[(1) - (3)].trie); trie_add_fprefix((yyval.trie), &((yyvsp[(3) - (3)].v).val.px)); ;}
    break;

  case 282:

/* Line 1464 of yacc.c  */
#line 1602 "cf-parse.y"
    { (yyval.e) = NULL; ;}
    break;

  case 283:

/* Line 1464 of yacc.c  */
#line 1603 "cf-parse.y"
    {
     /* Fill data fields */
     struct f_tree *t;
     for (t = (yyvsp[(2) - (4)].e); t; t = t->left)
       t->data = (yyvsp[(4) - (4)].x);
     (yyval.e) = f_merge_items((yyvsp[(1) - (4)].e), (yyvsp[(2) - (4)].e));
   ;}
    break;

  case 284:

/* Line 1464 of yacc.c  */
#line 1610 "cf-parse.y"
    { 
     struct f_tree *t = f_new_tree();
     t->from.type = t->to.type = T_VOID;
     t->right = t;
     t->data = (yyvsp[(3) - (3)].x);
     (yyval.e) = f_merge_items((yyvsp[(1) - (3)].e), t);
 ;}
    break;

  case 285:

/* Line 1464 of yacc.c  */
#line 1622 "cf-parse.y"
    { (yyval.x) = (yyvsp[(1) - (1)].x); ;}
    break;

  case 286:

/* Line 1464 of yacc.c  */
#line 1623 "cf-parse.y"
    { (yyval.x) = (yyvsp[(2) - (3)].x); ;}
    break;

  case 287:

/* Line 1464 of yacc.c  */
#line 1627 "cf-parse.y"
    { (yyval.h) = (yyvsp[(2) - (3)].h); ;}
    break;

  case 288:

/* Line 1464 of yacc.c  */
#line 1628 "cf-parse.y"
    { (yyval.h) = (yyvsp[(2) - (3)].h); ;}
    break;

  case 289:

/* Line 1464 of yacc.c  */
#line 1632 "cf-parse.y"
    { (yyval.h) = cfg_alloc(sizeof(struct f_path_mask)); (yyval.h)->next = (yyvsp[(2) - (2)].h); (yyval.h)->kind = PM_ASN;      (yyval.h)->val = (yyvsp[(1) - (2)].i); ;}
    break;

  case 290:

/* Line 1464 of yacc.c  */
#line 1633 "cf-parse.y"
    { (yyval.h) = cfg_alloc(sizeof(struct f_path_mask)); (yyval.h)->next = (yyvsp[(2) - (2)].h); (yyval.h)->kind = PM_ASTERISK; (yyval.h)->val  = 0; ;}
    break;

  case 291:

/* Line 1464 of yacc.c  */
#line 1634 "cf-parse.y"
    { (yyval.h) = cfg_alloc(sizeof(struct f_path_mask)); (yyval.h)->next = (yyvsp[(2) - (2)].h); (yyval.h)->kind = PM_QUESTION; (yyval.h)->val  = 0; ;}
    break;

  case 292:

/* Line 1464 of yacc.c  */
#line 1635 "cf-parse.y"
    { (yyval.h) = cfg_alloc(sizeof(struct f_path_mask)); (yyval.h)->next = (yyvsp[(2) - (2)].h); (yyval.h)->kind = PM_ASN_EXPR; (yyval.h)->val = (uintptr_t) (yyvsp[(1) - (2)].x); ;}
    break;

  case 293:

/* Line 1464 of yacc.c  */
#line 1636 "cf-parse.y"
    { (yyval.h) = NULL; ;}
    break;

  case 294:

/* Line 1464 of yacc.c  */
#line 1640 "cf-parse.y"
    { (yyval.h) = cfg_alloc(sizeof(struct f_path_mask)); (yyval.h)->next = (yyvsp[(2) - (2)].h); (yyval.h)->kind = PM_ASN;      (yyval.h)->val = (yyvsp[(1) - (2)].i); ;}
    break;

  case 295:

/* Line 1464 of yacc.c  */
#line 1641 "cf-parse.y"
    { (yyval.h) = cfg_alloc(sizeof(struct f_path_mask)); (yyval.h)->next = (yyvsp[(2) - (2)].h); (yyval.h)->kind = PM_ASTERISK; (yyval.h)->val  = 0; ;}
    break;

  case 296:

/* Line 1464 of yacc.c  */
#line 1642 "cf-parse.y"
    { (yyval.h) = NULL; ;}
    break;

  case 297:

/* Line 1464 of yacc.c  */
#line 1646 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'c'; (yyval.x)->aux = T_INT;  (yyval.x)->a2.i = (yyvsp[(1) - (1)].i); ;}
    break;

  case 298:

/* Line 1464 of yacc.c  */
#line 1647 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'c'; (yyval.x)->aux = T_BOOL; (yyval.x)->a2.i = 1;  ;}
    break;

  case 299:

/* Line 1464 of yacc.c  */
#line 1648 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'c'; (yyval.x)->aux = T_BOOL; (yyval.x)->a2.i = 0;  ;}
    break;

  case 300:

/* Line 1464 of yacc.c  */
#line 1649 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'c'; (yyval.x)->aux = T_STRING; (yyval.x)->a2.p = (yyvsp[(1) - (1)].t); ;}
    break;

  case 301:

/* Line 1464 of yacc.c  */
#line 1650 "cf-parse.y"
    { NEW_F_VAL; (yyval.x) = f_new_inst(); (yyval.x)->code = 'C'; (yyval.x)->a1.p = val; *val = (yyvsp[(1) - (1)].v); ;}
    break;

  case 302:

/* Line 1464 of yacc.c  */
#line 1651 "cf-parse.y"
    {NEW_F_VAL; (yyval.x) = f_new_inst(); (yyval.x)->code = 'C'; (yyval.x)->a1.p = val; *val = (yyvsp[(1) - (1)].v); ;}
    break;

  case 303:

/* Line 1464 of yacc.c  */
#line 1652 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'c'; (yyval.x)->aux = T_QUAD;  (yyval.x)->a2.i = (yyvsp[(1) - (1)].i32); ;}
    break;

  case 304:

/* Line 1464 of yacc.c  */
#line 1653 "cf-parse.y"
    { DBG( "We've got a set here..." ); (yyval.x) = f_new_inst(); (yyval.x)->code = 'c'; (yyval.x)->aux = T_SET; (yyval.x)->a2.p = build_tree((yyvsp[(2) - (3)].e)); DBG( "ook\n" ); ;}
    break;

  case 305:

/* Line 1464 of yacc.c  */
#line 1654 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'c'; (yyval.x)->aux = T_PREFIX_SET;  (yyval.x)->a2.p = (yyvsp[(2) - (3)].trie); ;}
    break;

  case 306:

/* Line 1464 of yacc.c  */
#line 1655 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'c'; (yyval.x)->aux = (yyvsp[(1) - (1)].i) >> 16; (yyval.x)->a2.i = (yyvsp[(1) - (1)].i) & 0xffff; ;}
    break;

  case 307:

/* Line 1464 of yacc.c  */
#line 1656 "cf-parse.y"
    { NEW_F_VAL; (yyval.x) = f_new_inst(); (yyval.x)->code = 'C'; val->type = T_PATH_MASK; val->val.path_mask = (yyvsp[(1) - (1)].h); (yyval.x)->a1.p = val; ;}
    break;

  case 308:

/* Line 1464 of yacc.c  */
#line 1660 "cf-parse.y"
    { (yyval.x) = f_generate_dpair((yyvsp[(2) - (5)].x), (yyvsp[(4) - (5)].x)); ;}
    break;

  case 309:

/* Line 1464 of yacc.c  */
#line 1661 "cf-parse.y"
    { (yyval.x) = f_generate_ec((yyvsp[(2) - (7)].i), (yyvsp[(4) - (7)].x), (yyvsp[(6) - (7)].x)); ;}
    break;

  case 311:

/* Line 1464 of yacc.c  */
#line 1675 "cf-parse.y"
    {
     struct symbol *sym;
     struct f_inst *inst = (yyvsp[(3) - (4)].x);
     if ((yyvsp[(1) - (4)].s)->class != SYM_FUNCTION)
       cf_error("You can't call something which is not a function. Really.");
     DBG("You are calling function %s\n", (yyvsp[(1) - (4)].s)->name);
     (yyval.x) = f_new_inst();
     (yyval.x)->code = P('c','a');
     (yyval.x)->a1.p = inst;
     (yyval.x)->a2.p = (yyvsp[(1) - (4)].s)->def;
     sym = (yyvsp[(1) - (4)].s)->aux2;
     while (sym || inst) {
       if (!sym || !inst)
	 cf_error("Wrong number of arguments for function %s.", (yyvsp[(1) - (4)].s)->name);
       DBG( "You should pass parameter called %s\n", sym->name);
       inst->a1.p = sym;
       sym = sym->aux2;
       inst = inst->next;
     }
   ;}
    break;

  case 312:

/* Line 1464 of yacc.c  */
#line 1698 "cf-parse.y"
    {
     (yyval.x) = f_new_inst();
     switch ((yyvsp[(1) - (1)].s)->class) {
       case SYM_NUMBER:
	(yyval.x) = f_new_inst();
	(yyval.x)->code = 'c'; 
	(yyval.x)->aux = T_INT; 
	(yyval.x)->a2.i = (yyvsp[(1) - (1)].s)->aux;
	break;
       case SYM_IPA:
	{ NEW_F_VAL; (yyval.x) = f_new_inst(); (yyval.x)->code = 'C'; (yyval.x)->a1.p = val; val->type = T_IP; val->val.px.ip = * (ip_addr *) ((yyvsp[(1) - (1)].s)->def); }
	break;
       case SYM_VARIABLE | T_BOOL:
       case SYM_VARIABLE | T_INT:
       case SYM_VARIABLE | T_PAIR:
       case SYM_VARIABLE | T_QUAD:
       case SYM_VARIABLE | T_EC:
       case SYM_VARIABLE | T_STRING:
       case SYM_VARIABLE | T_IP:
       case SYM_VARIABLE | T_PREFIX:
       case SYM_VARIABLE | T_PREFIX_SET:
       case SYM_VARIABLE | T_SET:
       case SYM_VARIABLE | T_PATH:
       case SYM_VARIABLE | T_PATH_MASK:
       case SYM_VARIABLE | T_CLIST:
       case SYM_VARIABLE | T_ECLIST:
	 (yyval.x)->code = 'V';
	 (yyval.x)->a1.p = (yyvsp[(1) - (1)].s)->def;
	 (yyval.x)->a2.p = (yyvsp[(1) - (1)].s)->name;
	 break;
       default:
	 cf_error("%s: variable expected.", (yyvsp[(1) - (1)].s)->name );
     }
   ;}
    break;

  case 313:

/* Line 1464 of yacc.c  */
#line 1734 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->aux = T_IP;         (yyval.x)->a2.i = OFFSETOF(struct rta, from);   (yyval.x)->a1.i = 1; ;}
    break;

  case 314:

/* Line 1464 of yacc.c  */
#line 1736 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->aux = T_IP;         (yyval.x)->a2.i = OFFSETOF(struct rta, gw);     (yyval.x)->a1.i = 1; ;}
    break;

  case 315:

/* Line 1464 of yacc.c  */
#line 1737 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->aux = T_PREFIX;     (yyval.x)->a2.i = 0x12345678; /* This is actually ok - T_PREFIX is special-cased. */ ;}
    break;

  case 316:

/* Line 1464 of yacc.c  */
#line 1738 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->aux = T_STRING;     (yyval.x)->a2.i = 0x12345678; /* T_STRING is also special-cased. */ ;}
    break;

  case 317:

/* Line 1464 of yacc.c  */
#line 1739 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->aux = T_ENUM_RTS;   (yyval.x)->a2.i = OFFSETOF(struct rta, source); ;}
    break;

  case 318:

/* Line 1464 of yacc.c  */
#line 1740 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->aux = T_ENUM_SCOPE; (yyval.x)->a2.i = OFFSETOF(struct rta, scope);  (yyval.x)->a1.i = 1; ;}
    break;

  case 319:

/* Line 1464 of yacc.c  */
#line 1741 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->aux = T_ENUM_RTC;   (yyval.x)->a2.i = OFFSETOF(struct rta, cast); ;}
    break;

  case 320:

/* Line 1464 of yacc.c  */
#line 1742 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->aux = T_ENUM_RTD;   (yyval.x)->a2.i = OFFSETOF(struct rta, dest); ;}
    break;

  case 321:

/* Line 1464 of yacc.c  */
#line 1746 "cf-parse.y"
    { (yyval.x) = (yyvsp[(2) - (3)].x); ;}
    break;

  case 322:

/* Line 1464 of yacc.c  */
#line 1747 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = '+';        (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->a2.p = (yyvsp[(3) - (3)].x); ;}
    break;

  case 323:

/* Line 1464 of yacc.c  */
#line 1748 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = '-';        (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->a2.p = (yyvsp[(3) - (3)].x); ;}
    break;

  case 324:

/* Line 1464 of yacc.c  */
#line 1749 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = '*';        (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->a2.p = (yyvsp[(3) - (3)].x); ;}
    break;

  case 325:

/* Line 1464 of yacc.c  */
#line 1750 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = '/';        (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->a2.p = (yyvsp[(3) - (3)].x); ;}
    break;

  case 326:

/* Line 1464 of yacc.c  */
#line 1751 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = '&';        (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->a2.p = (yyvsp[(3) - (3)].x); ;}
    break;

  case 327:

/* Line 1464 of yacc.c  */
#line 1752 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = '|';        (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->a2.p = (yyvsp[(3) - (3)].x); ;}
    break;

  case 328:

/* Line 1464 of yacc.c  */
#line 1753 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('=','='); (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->a2.p = (yyvsp[(3) - (3)].x); ;}
    break;

  case 329:

/* Line 1464 of yacc.c  */
#line 1754 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('!','=');     (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->a2.p = (yyvsp[(3) - (3)].x); ;}
    break;

  case 330:

/* Line 1464 of yacc.c  */
#line 1755 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = '<';        (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->a2.p = (yyvsp[(3) - (3)].x); ;}
    break;

  case 331:

/* Line 1464 of yacc.c  */
#line 1756 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('<','=');     (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->a2.p = (yyvsp[(3) - (3)].x); ;}
    break;

  case 332:

/* Line 1464 of yacc.c  */
#line 1757 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = '<';        (yyval.x)->a1.p = (yyvsp[(3) - (3)].x); (yyval.x)->a2.p = (yyvsp[(1) - (3)].x); ;}
    break;

  case 333:

/* Line 1464 of yacc.c  */
#line 1758 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('<','=');     (yyval.x)->a1.p = (yyvsp[(3) - (3)].x); (yyval.x)->a2.p = (yyvsp[(1) - (3)].x); ;}
    break;

  case 334:

/* Line 1464 of yacc.c  */
#line 1759 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = '~';        (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->a2.p = (yyvsp[(3) - (3)].x); ;}
    break;

  case 335:

/* Line 1464 of yacc.c  */
#line 1760 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = '!'; (yyval.x)->a1.p = (yyvsp[(2) - (2)].x); ;}
    break;

  case 336:

/* Line 1464 of yacc.c  */
#line 1761 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('d','e');  (yyval.x)->a1.p = (yyvsp[(3) - (4)].x); ;}
    break;

  case 337:

/* Line 1464 of yacc.c  */
#line 1763 "cf-parse.y"
    { (yyval.x) = (yyvsp[(1) - (1)].x); ;}
    break;

  case 338:

/* Line 1464 of yacc.c  */
#line 1764 "cf-parse.y"
    { (yyval.x) = (yyvsp[(1) - (1)].x); ;}
    break;

  case 339:

/* Line 1464 of yacc.c  */
#line 1765 "cf-parse.y"
    { (yyval.x) = (yyvsp[(1) - (1)].x); ;}
    break;

  case 340:

/* Line 1464 of yacc.c  */
#line 1767 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'P'; ;}
    break;

  case 341:

/* Line 1464 of yacc.c  */
#line 1769 "cf-parse.y"
    { (yyval.x) = (yyvsp[(2) - (2)].x); (yyval.x)->code = 'a'; ;}
    break;

  case 342:

/* Line 1464 of yacc.c  */
#line 1771 "cf-parse.y"
    { (yyval.x) = (yyvsp[(2) - (2)].x); (yyval.x)->code = P('e','a'); ;}
    break;

  case 343:

/* Line 1464 of yacc.c  */
#line 1773 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('c','p'); (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); (yyval.x)->aux = T_IP; ;}
    break;

  case 344:

/* Line 1464 of yacc.c  */
#line 1774 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'L'; (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); ;}
    break;

  case 345:

/* Line 1464 of yacc.c  */
#line 1775 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('i','M'); (yyval.x)->a1.p = (yyvsp[(1) - (6)].x); (yyval.x)->a2.p = (yyvsp[(5) - (6)].x); ;}
    break;

  case 346:

/* Line 1464 of yacc.c  */
#line 1776 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('a','f'); (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); ;}
    break;

  case 347:

/* Line 1464 of yacc.c  */
#line 1777 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('a','l'); (yyval.x)->a1.p = (yyvsp[(1) - (3)].x); ;}
    break;

  case 348:

/* Line 1464 of yacc.c  */
#line 1787 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'E'; (yyval.x)->aux = T_PATH; ;}
    break;

  case 349:

/* Line 1464 of yacc.c  */
#line 1788 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'E'; (yyval.x)->aux = T_CLIST; ;}
    break;

  case 350:

/* Line 1464 of yacc.c  */
#line 1789 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'E'; (yyval.x)->aux = T_ECLIST; ;}
    break;

  case 351:

/* Line 1464 of yacc.c  */
#line 1790 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('A','p'); (yyval.x)->a1.p = (yyvsp[(3) - (6)].x); (yyval.x)->a2.p = (yyvsp[(5) - (6)].x); ;}
    break;

  case 352:

/* Line 1464 of yacc.c  */
#line 1791 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('C','a'); (yyval.x)->a1.p = (yyvsp[(3) - (6)].x); (yyval.x)->a2.p = (yyvsp[(5) - (6)].x); (yyval.x)->aux = 'a'; ;}
    break;

  case 353:

/* Line 1464 of yacc.c  */
#line 1792 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('C','a'); (yyval.x)->a1.p = (yyvsp[(3) - (6)].x); (yyval.x)->a2.p = (yyvsp[(5) - (6)].x); (yyval.x)->aux = 'd'; ;}
    break;

  case 354:

/* Line 1464 of yacc.c  */
#line 1793 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('C','a'); (yyval.x)->a1.p = (yyvsp[(3) - (6)].x); (yyval.x)->a2.p = (yyvsp[(5) - (6)].x); (yyval.x)->aux = 'f'; ;}
    break;

  case 355:

/* Line 1464 of yacc.c  */
#line 1798 "cf-parse.y"
    {
     struct symbol *sym;
     struct f_inst *inst = (yyvsp[(3) - (4)].x);
     if ((yyvsp[(1) - (4)].s)->class != SYM_FUNCTION)
       cf_error("You can't call something which is not a function. Really.");
     DBG("You are calling function %s\n", (yyvsp[(1) - (4)].s)->name);
     (yyval.x) = f_new_inst();
     (yyval.x)->code = P('c','a');
     (yyval.x)->a1.p = inst;
     (yyval.x)->a2.p = (yyvsp[(1) - (4)].s)->def;
     sym = (yyvsp[(1) - (4)].s)->aux2;
     while (sym || inst) {
       if (!sym || !inst)
	 cf_error("Wrong number of arguments for function %s.", (yyvsp[(1) - (4)].s)->name);
       DBG( "You should pass parameter called %s\n", sym->name);
       inst->a1.p = sym;
       sym = sym->aux2;
       inst = inst->next;
     }
   ;}
    break;

  case 356:

/* Line 1464 of yacc.c  */
#line 1821 "cf-parse.y"
    { (yyval.i) = F_QUITBIRD; ;}
    break;

  case 357:

/* Line 1464 of yacc.c  */
#line 1822 "cf-parse.y"
    { (yyval.i) = F_ACCEPT; ;}
    break;

  case 358:

/* Line 1464 of yacc.c  */
#line 1823 "cf-parse.y"
    { (yyval.i) = F_REJECT; ;}
    break;

  case 359:

/* Line 1464 of yacc.c  */
#line 1824 "cf-parse.y"
    { (yyval.i) = F_ERROR; ;}
    break;

  case 360:

/* Line 1464 of yacc.c  */
#line 1825 "cf-parse.y"
    { (yyval.i) = F_NOP; ;}
    break;

  case 361:

/* Line 1464 of yacc.c  */
#line 1826 "cf-parse.y"
    { (yyval.i) = F_NONL; ;}
    break;

  case 362:

/* Line 1464 of yacc.c  */
#line 1830 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = 'p'; (yyval.x)->a1.p = (yyvsp[(1) - (1)].x); (yyval.x)->a2.p = NULL; ;}
    break;

  case 363:

/* Line 1464 of yacc.c  */
#line 1833 "cf-parse.y"
    { (yyval.x) = NULL; ;}
    break;

  case 364:

/* Line 1464 of yacc.c  */
#line 1834 "cf-parse.y"
    { (yyval.x) = (yyvsp[(1) - (1)].x); ;}
    break;

  case 365:

/* Line 1464 of yacc.c  */
#line 1835 "cf-parse.y"
    {
     if ((yyvsp[(1) - (3)].x)) {
       (yyvsp[(1) - (3)].x)->next = (yyvsp[(3) - (3)].x);
       (yyval.x) = (yyvsp[(1) - (3)].x);
     } else (yyval.x) = (yyvsp[(3) - (3)].x);
   ;}
    break;

  case 366:

/* Line 1464 of yacc.c  */
#line 1844 "cf-parse.y"
    { 
     (yyval.x) = f_new_inst();
     (yyval.x)->code = 's';
     (yyval.x)->a1.p = NULL;
     (yyval.x)->a2.p = (yyvsp[(1) - (1)].x);
     (yyval.x)->next = NULL;
   ;}
    break;

  case 367:

/* Line 1464 of yacc.c  */
#line 1851 "cf-parse.y"
    {
     (yyval.x) = f_new_inst();
     (yyval.x)->code = 's';
     (yyval.x)->a1.p = NULL;
     (yyval.x)->a2.p = (yyvsp[(1) - (3)].x);
     (yyval.x)->next = (yyvsp[(3) - (3)].x);
   ;}
    break;

  case 368:

/* Line 1464 of yacc.c  */
#line 1860 "cf-parse.y"
    { (yyval.x) = NULL; ;}
    break;

  case 369:

/* Line 1464 of yacc.c  */
#line 1861 "cf-parse.y"
    { (yyval.x) = (yyvsp[(1) - (1)].x); ;}
    break;

  case 370:

/* Line 1464 of yacc.c  */
#line 1865 "cf-parse.y"
    {
     (yyval.x) = f_new_inst();
     (yyval.x)->code = '?';
     (yyval.x)->a1.p = (yyvsp[(2) - (4)].x);
     (yyval.x)->a2.p = (yyvsp[(4) - (4)].x);
   ;}
    break;

  case 371:

/* Line 1464 of yacc.c  */
#line 1871 "cf-parse.y"
    {
     struct f_inst *i = f_new_inst();
     i->code = '?';
     i->a1.p = (yyvsp[(2) - (6)].x);
     i->a2.p = (yyvsp[(4) - (6)].x);
     (yyval.x) = f_new_inst();
     (yyval.x)->code = '?';
     (yyval.x)->a1.p = i;
     (yyval.x)->a2.p = (yyvsp[(6) - (6)].x);
   ;}
    break;

  case 372:

/* Line 1464 of yacc.c  */
#line 1881 "cf-parse.y"
    {
     (yyval.x) = f_new_inst();
     DBG( "Ook, we'll set value\n" );
     if (((yyvsp[(1) - (4)].s)->class & ~T_MASK) != SYM_VARIABLE)
       cf_error( "You may set only variables." );
     (yyval.x)->code = 's';
     (yyval.x)->a1.p = (yyvsp[(1) - (4)].s);
     (yyval.x)->a2.p = (yyvsp[(3) - (4)].x);
   ;}
    break;

  case 373:

/* Line 1464 of yacc.c  */
#line 1890 "cf-parse.y"
    {
     (yyval.x) = f_new_inst();
     DBG( "Ook, we'll return the value\n" );
     (yyval.x)->code = 'r';
     (yyval.x)->a1.p = (yyvsp[(2) - (3)].x);
   ;}
    break;

  case 374:

/* Line 1464 of yacc.c  */
#line 1896 "cf-parse.y"
    {
     (yyval.x) = (yyvsp[(2) - (5)].x);
     (yyval.x)->code = P('e','S');
     (yyval.x)->a1.p = (yyvsp[(4) - (5)].x);
   ;}
    break;

  case 375:

/* Line 1464 of yacc.c  */
#line 1901 "cf-parse.y"
    {
     (yyval.x) = (yyvsp[(2) - (5)].x);
     if (!(yyval.x)->a1.i)
       cf_error( "This static attribute is read-only.");
     (yyval.x)->code = P('a','S');
     (yyval.x)->a1.p = (yyvsp[(4) - (5)].x);
   ;}
    break;

  case 376:

/* Line 1464 of yacc.c  */
#line 1908 "cf-parse.y"
    {
     (yyval.x) = f_new_inst();
     (yyval.x)->code = P('P','S');
     (yyval.x)->a1.p = (yyvsp[(3) - (4)].x);
   ;}
    break;

  case 377:

/* Line 1464 of yacc.c  */
#line 1913 "cf-parse.y"
    {
     (yyval.x) = (yyvsp[(4) - (6)].x);
     (yyval.x)->aux = EAF_TYPE_UNDEF | EAF_TEMP;
     (yyval.x)->code = P('e','S');
     (yyval.x)->a1.p = NULL;
   ;}
    break;

  case 378:

/* Line 1464 of yacc.c  */
#line 1919 "cf-parse.y"
    { (yyval.x) = f_new_inst(); (yyval.x)->code = P('p',','); (yyval.x)->a1.p = (yyvsp[(2) - (3)].x); (yyval.x)->a2.i = (yyvsp[(1) - (3)].i); ;}
    break;

  case 379:

/* Line 1464 of yacc.c  */
#line 1920 "cf-parse.y"
    { (yyval.x) = (yyvsp[(1) - (2)].x); ;}
    break;

  case 380:

/* Line 1464 of yacc.c  */
#line 1921 "cf-parse.y"
    {
      (yyval.x) = f_new_inst();
      (yyval.x)->code = P('S','W');
      (yyval.x)->a1.p = (yyvsp[(2) - (5)].x);
      (yyval.x)->a2.p = build_tree( (yyvsp[(4) - (5)].e) );
   ;}
    break;

  case 381:

/* Line 1464 of yacc.c  */
#line 1929 "cf-parse.y"
    { (yyval.x) = f_generate_empty((yyvsp[(2) - (5)].x)); ;}
    break;

  case 382:

/* Line 1464 of yacc.c  */
#line 1930 "cf-parse.y"
    { (yyval.x) = f_generate_complex( P('A','p'), 'x', (yyvsp[(2) - (8)].x), (yyvsp[(6) - (8)].x) ); ;}
    break;

  case 383:

/* Line 1464 of yacc.c  */
#line 1931 "cf-parse.y"
    { (yyval.x) = f_generate_complex( P('C','a'), 'a', (yyvsp[(2) - (8)].x), (yyvsp[(6) - (8)].x) ); ;}
    break;

  case 384:

/* Line 1464 of yacc.c  */
#line 1932 "cf-parse.y"
    { (yyval.x) = f_generate_complex( P('C','a'), 'd', (yyvsp[(2) - (8)].x), (yyvsp[(6) - (8)].x) ); ;}
    break;

  case 385:

/* Line 1464 of yacc.c  */
#line 1933 "cf-parse.y"
    { (yyval.x) = f_generate_complex( P('C','a'), 'f', (yyvsp[(2) - (8)].x), (yyvsp[(6) - (8)].x) ); ;}
    break;

  case 386:

/* Line 1464 of yacc.c  */
#line 1939 "cf-parse.y"
    {
     this_proto = proto_config_new(&proto_bgp, sizeof(struct bgp_config), (yyvsp[(1) - (2)].i));
     this_proto->preference = DEF_PREF_BGP;
     BGP_CFG->hold_time = 240;
     BGP_CFG->connect_retry_time = 120;
     BGP_CFG->initial_hold_time = 240;
     BGP_CFG->compare_path_lengths = 1;
     BGP_CFG->igp_metric = 1;
     BGP_CFG->start_delay_time = 5;
     BGP_CFG->error_amnesia_time = 300;
     BGP_CFG->error_delay_time_min = 60;
     BGP_CFG->error_delay_time_max = 300;
     BGP_CFG->enable_refresh = 1;
     BGP_CFG->enable_as4 = 1;
     BGP_CFG->capabilities = 2;
     BGP_CFG->advertise_ipv4 = 1;
     BGP_CFG->interpret_communities = 1;
     BGP_CFG->default_local_pref = 100;
 ;}
    break;

  case 389:

/* Line 1464 of yacc.c  */
#line 1963 "cf-parse.y"
    { BGP_CFG->local_as = (yyvsp[(4) - (5)].i); ;}
    break;

  case 390:

/* Line 1464 of yacc.c  */
#line 1964 "cf-parse.y"
    { BGP_CFG->source_addr = (yyvsp[(3) - (6)].a); BGP_CFG->local_as = (yyvsp[(5) - (6)].i); ;}
    break;

  case 391:

/* Line 1464 of yacc.c  */
#line 1965 "cf-parse.y"
    {
     if (ipa_nonzero(BGP_CFG->remote_ip)) cf_error("Only one neighbor per BGP instance is allowed");

     BGP_CFG->remote_ip = (yyvsp[(3) - (6)].a);
     BGP_CFG->remote_as = (yyvsp[(5) - (6)].i);
   ;}
    break;

  case 392:

/* Line 1464 of yacc.c  */
#line 1971 "cf-parse.y"
    { BGP_CFG->rr_cluster_id = (yyvsp[(5) - (6)].i32); ;}
    break;

  case 393:

/* Line 1464 of yacc.c  */
#line 1972 "cf-parse.y"
    { BGP_CFG->rr_client = 1; ;}
    break;

  case 394:

/* Line 1464 of yacc.c  */
#line 1973 "cf-parse.y"
    { BGP_CFG->rs_client = 1; ;}
    break;

  case 395:

/* Line 1464 of yacc.c  */
#line 1974 "cf-parse.y"
    { BGP_CFG->hold_time = (yyvsp[(4) - (5)].i); ;}
    break;

  case 396:

/* Line 1464 of yacc.c  */
#line 1975 "cf-parse.y"
    { BGP_CFG->initial_hold_time = (yyvsp[(5) - (6)].i); ;}
    break;

  case 397:

/* Line 1464 of yacc.c  */
#line 1976 "cf-parse.y"
    { BGP_CFG->connect_retry_time = (yyvsp[(5) - (6)].i); ;}
    break;

  case 398:

/* Line 1464 of yacc.c  */
#line 1977 "cf-parse.y"
    { BGP_CFG->keepalive_time = (yyvsp[(4) - (5)].i); ;}
    break;

  case 399:

/* Line 1464 of yacc.c  */
#line 1978 "cf-parse.y"
    { BGP_CFG->multihop = 64; ;}
    break;

  case 400:

/* Line 1464 of yacc.c  */
#line 1979 "cf-parse.y"
    { BGP_CFG->multihop = (yyvsp[(3) - (4)].i); if (((yyvsp[(3) - (4)].i)<1) || ((yyvsp[(3) - (4)].i)>255)) cf_error("Multihop must be in range 1-255"); ;}
    break;

  case 401:

/* Line 1464 of yacc.c  */
#line 1980 "cf-parse.y"
    { BGP_CFG->next_hop_self = 1; ;}
    break;

  case 402:

/* Line 1464 of yacc.c  */
#line 1981 "cf-parse.y"
    { BGP_CFG->missing_lladdr = MLL_SELF; ;}
    break;

  case 403:

/* Line 1464 of yacc.c  */
#line 1982 "cf-parse.y"
    { BGP_CFG->missing_lladdr = MLL_DROP; ;}
    break;

  case 404:

/* Line 1464 of yacc.c  */
#line 1983 "cf-parse.y"
    { BGP_CFG->missing_lladdr = MLL_IGNORE; ;}
    break;

  case 405:

/* Line 1464 of yacc.c  */
#line 1984 "cf-parse.y"
    { BGP_CFG->gw_mode = GW_DIRECT; ;}
    break;

  case 406:

/* Line 1464 of yacc.c  */
#line 1985 "cf-parse.y"
    { BGP_CFG->gw_mode = GW_RECURSIVE; ;}
    break;

  case 407:

/* Line 1464 of yacc.c  */
#line 1986 "cf-parse.y"
    { BGP_CFG->compare_path_lengths = (yyvsp[(4) - (5)].i); ;}
    break;

  case 408:

/* Line 1464 of yacc.c  */
#line 1987 "cf-parse.y"
    { BGP_CFG->med_metric = (yyvsp[(4) - (5)].i); ;}
    break;

  case 409:

/* Line 1464 of yacc.c  */
#line 1988 "cf-parse.y"
    { BGP_CFG->igp_metric = (yyvsp[(4) - (5)].i); ;}
    break;

  case 410:

/* Line 1464 of yacc.c  */
#line 1989 "cf-parse.y"
    { BGP_CFG->prefer_older = (yyvsp[(4) - (5)].i); ;}
    break;

  case 411:

/* Line 1464 of yacc.c  */
#line 1990 "cf-parse.y"
    { BGP_CFG->default_med = (yyvsp[(4) - (5)].i); ;}
    break;

  case 412:

/* Line 1464 of yacc.c  */
#line 1991 "cf-parse.y"
    { BGP_CFG->default_local_pref = (yyvsp[(4) - (5)].i); ;}
    break;

  case 413:

/* Line 1464 of yacc.c  */
#line 1992 "cf-parse.y"
    { BGP_CFG->source_addr = (yyvsp[(4) - (5)].a); ;}
    break;

  case 414:

/* Line 1464 of yacc.c  */
#line 1993 "cf-parse.y"
    { BGP_CFG->start_delay_time = (yyvsp[(5) - (6)].i); ;}
    break;

  case 415:

/* Line 1464 of yacc.c  */
#line 1994 "cf-parse.y"
    { BGP_CFG->error_amnesia_time = (yyvsp[(5) - (6)].i); ;}
    break;

  case 416:

/* Line 1464 of yacc.c  */
#line 1995 "cf-parse.y"
    { BGP_CFG->error_delay_time_min = (yyvsp[(5) - (8)].i); BGP_CFG->error_delay_time_max = (yyvsp[(7) - (8)].i); ;}
    break;

  case 417:

/* Line 1464 of yacc.c  */
#line 1996 "cf-parse.y"
    { BGP_CFG->disable_after_error = (yyvsp[(5) - (6)].i); ;}
    break;

  case 418:

/* Line 1464 of yacc.c  */
#line 1997 "cf-parse.y"
    { BGP_CFG->enable_refresh = (yyvsp[(5) - (6)].i); ;}
    break;

  case 419:

/* Line 1464 of yacc.c  */
#line 1998 "cf-parse.y"
    { BGP_CFG->enable_as4 = (yyvsp[(4) - (5)].i); ;}
    break;

  case 420:

/* Line 1464 of yacc.c  */
#line 1999 "cf-parse.y"
    { BGP_CFG->capabilities = (yyvsp[(3) - (4)].i); ;}
    break;

  case 421:

/* Line 1464 of yacc.c  */
#line 2000 "cf-parse.y"
    { BGP_CFG->advertise_ipv4 = (yyvsp[(4) - (5)].i); ;}
    break;

  case 422:

/* Line 1464 of yacc.c  */
#line 2001 "cf-parse.y"
    { BGP_CFG->password = (yyvsp[(3) - (4)].t); ;}
    break;

  case 423:

/* Line 1464 of yacc.c  */
#line 2002 "cf-parse.y"
    { BGP_CFG->route_limit = (yyvsp[(4) - (5)].i); ;}
    break;

  case 424:

/* Line 1464 of yacc.c  */
#line 2003 "cf-parse.y"
    { BGP_CFG->passive = (yyvsp[(3) - (4)].i); ;}
    break;

  case 425:

/* Line 1464 of yacc.c  */
#line 2004 "cf-parse.y"
    { BGP_CFG->interpret_communities = (yyvsp[(4) - (5)].i); ;}
    break;

  case 426:

/* Line 1464 of yacc.c  */
#line 2005 "cf-parse.y"
    { BGP_CFG->igp_table = (yyvsp[(4) - (5)].r); ;}
    break;

  case 427:

/* Line 1464 of yacc.c  */
#line 2006 "cf-parse.y"
    { BGP_CFG->ttl_security = (yyvsp[(4) - (5)].i); ;}
    break;

  case 428:

/* Line 1464 of yacc.c  */
#line 2017 "cf-parse.y"
    {
     this_proto = proto_config_new(&proto_ospf, sizeof(struct ospf_config), (yyvsp[(1) - (2)].i));
     this_proto->preference = DEF_PREF_OSPF;
     init_list(&OSPF_CFG->area_list);
     init_list(&OSPF_CFG->vlink_list);
     OSPF_CFG->rfc1583 = DEFAULT_RFC1583;
     OSPF_CFG->tick = DEFAULT_OSPFTICK;
  ;}
    break;

  case 432:

/* Line 1464 of yacc.c  */
#line 2034 "cf-parse.y"
    { OSPF_CFG->rfc1583 = (yyvsp[(2) - (2)].i); ;}
    break;

  case 433:

/* Line 1464 of yacc.c  */
#line 2035 "cf-parse.y"
    { OSPF_CFG->ecmp = (yyvsp[(2) - (2)].i) ? DEFAULT_ECMP_LIMIT : 0; ;}
    break;

  case 434:

/* Line 1464 of yacc.c  */
#line 2036 "cf-parse.y"
    { OSPF_CFG->ecmp = (yyvsp[(2) - (4)].i) ? (yyvsp[(4) - (4)].i) : 0; if ((yyvsp[(4) - (4)].i) < 0) cf_error("ECMP limit cannot be negative"); ;}
    break;

  case 435:

/* Line 1464 of yacc.c  */
#line 2037 "cf-parse.y"
    { OSPF_CFG->tick = (yyvsp[(2) - (2)].i); if((yyvsp[(2) - (2)].i)<=0) cf_error("Tick must be greater than zero"); ;}
    break;

  case 437:

/* Line 1464 of yacc.c  */
#line 2041 "cf-parse.y"
    {
  this_area = cfg_allocz(sizeof(struct ospf_area_config));
  add_tail(&OSPF_CFG->area_list, NODE this_area);
  this_area->areaid = (yyvsp[(2) - (2)].i32);
  this_area->default_cost = DEFAULT_STUB_COST;
  this_area->type = OPT_E;
  this_area->transint = DEFAULT_TRANSINT;

  init_list(&this_area->patt_list);
  init_list(&this_area->net_list);
  init_list(&this_area->enet_list);
  init_list(&this_area->stubnet_list);
 ;}
    break;

  case 438:

/* Line 1464 of yacc.c  */
#line 2056 "cf-parse.y"
    { ospf_area_finish(); ;}
    break;

  case 441:

/* Line 1464 of yacc.c  */
#line 2065 "cf-parse.y"
    { this_area->type = (yyvsp[(2) - (2)].i) ? 0 : OPT_E; /* We should remove the option */ ;}
    break;

  case 442:

/* Line 1464 of yacc.c  */
#line 2066 "cf-parse.y"
    { this_area->type = OPT_N; ;}
    break;

  case 443:

/* Line 1464 of yacc.c  */
#line 2067 "cf-parse.y"
    { this_area->summary = (yyvsp[(2) - (2)].i); ;}
    break;

  case 444:

/* Line 1464 of yacc.c  */
#line 2068 "cf-parse.y"
    { this_area->default_nssa = (yyvsp[(3) - (3)].i); ;}
    break;

  case 445:

/* Line 1464 of yacc.c  */
#line 2069 "cf-parse.y"
    { this_area->default_cost = (yyvsp[(3) - (3)].i); check_defcost((yyvsp[(3) - (3)].i)); ;}
    break;

  case 446:

/* Line 1464 of yacc.c  */
#line 2070 "cf-parse.y"
    { this_area->default_cost = (yyvsp[(3) - (3)].i) | LSA_EXT_EBIT; check_defcost((yyvsp[(3) - (3)].i)); ;}
    break;

  case 447:

/* Line 1464 of yacc.c  */
#line 2071 "cf-parse.y"
    { this_area->default_cost = (yyvsp[(3) - (3)].i);  check_defcost((yyvsp[(3) - (3)].i)); ;}
    break;

  case 448:

/* Line 1464 of yacc.c  */
#line 2072 "cf-parse.y"
    { this_area->translator = (yyvsp[(2) - (2)].i); ;}
    break;

  case 449:

/* Line 1464 of yacc.c  */
#line 2073 "cf-parse.y"
    { this_area->transint = (yyvsp[(3) - (3)].i); ;}
    break;

  case 450:

/* Line 1464 of yacc.c  */
#line 2074 "cf-parse.y"
    { this_nets = &this_area->net_list; ;}
    break;

  case 452:

/* Line 1464 of yacc.c  */
#line 2075 "cf-parse.y"
    { this_nets = &this_area->enet_list; ;}
    break;

  case 459:

/* Line 1464 of yacc.c  */
#line 2087 "cf-parse.y"
    {
     this_stubnet = cfg_allocz(sizeof(struct ospf_stubnet_config));
     add_tail(&this_area->stubnet_list, NODE this_stubnet);
     this_stubnet->px = (yyvsp[(1) - (1)].px);
     this_stubnet->cost = COST_D;
   ;}
    break;

  case 462:

/* Line 1464 of yacc.c  */
#line 2101 "cf-parse.y"
    { this_stubnet->hidden = (yyvsp[(2) - (2)].i); ;}
    break;

  case 463:

/* Line 1464 of yacc.c  */
#line 2102 "cf-parse.y"
    { this_stubnet->summary = (yyvsp[(2) - (2)].i); ;}
    break;

  case 464:

/* Line 1464 of yacc.c  */
#line 2103 "cf-parse.y"
    { this_stubnet->cost = (yyvsp[(2) - (2)].i); ;}
    break;

  case 465:

/* Line 1464 of yacc.c  */
#line 2107 "cf-parse.y"
    { ospf_iface_finish(); ;}
    break;

  case 470:

/* Line 1464 of yacc.c  */
#line 2117 "cf-parse.y"
    { OSPF_PATT->helloint = (yyvsp[(2) - (2)].i) ; if (((yyvsp[(2) - (2)].i)<=0) || ((yyvsp[(2) - (2)].i)>65535)) cf_error("Hello interval must be in range 1-65535"); ;}
    break;

  case 471:

/* Line 1464 of yacc.c  */
#line 2118 "cf-parse.y"
    { OSPF_PATT->rxmtint = (yyvsp[(2) - (2)].i) ; if ((yyvsp[(2) - (2)].i)<=0) cf_error("Retransmit int must be greater than zero"); ;}
    break;

  case 472:

/* Line 1464 of yacc.c  */
#line 2119 "cf-parse.y"
    { OSPF_PATT->inftransdelay = (yyvsp[(3) - (3)].i) ; if (((yyvsp[(3) - (3)].i)<=0) || ((yyvsp[(3) - (3)].i)>65535)) cf_error("Transmit delay must be in range 1-65535"); ;}
    break;

  case 473:

/* Line 1464 of yacc.c  */
#line 2120 "cf-parse.y"
    { OSPF_PATT->waitint = (yyvsp[(2) - (2)].i) ; ;}
    break;

  case 474:

/* Line 1464 of yacc.c  */
#line 2121 "cf-parse.y"
    { OSPF_PATT->deadint = (yyvsp[(2) - (2)].i) ; if ((yyvsp[(2) - (2)].i)<=1) cf_error("Dead interval must be greater than one"); ;}
    break;

  case 475:

/* Line 1464 of yacc.c  */
#line 2122 "cf-parse.y"
    { OSPF_PATT->deadc = (yyvsp[(3) - (3)].i) ; if ((yyvsp[(3) - (3)].i)<=1) cf_error("Dead count must be greater than one"); ;}
    break;

  case 476:

/* Line 1464 of yacc.c  */
#line 2123 "cf-parse.y"
    { OSPF_PATT->autype = OSPF_AUTH_NONE ; ;}
    break;

  case 477:

/* Line 1464 of yacc.c  */
#line 2124 "cf-parse.y"
    { OSPF_PATT->autype = OSPF_AUTH_SIMPLE ; ;}
    break;

  case 478:

/* Line 1464 of yacc.c  */
#line 2125 "cf-parse.y"
    { OSPF_PATT->autype = OSPF_AUTH_CRYPT ; ;}
    break;

  case 480:

/* Line 1464 of yacc.c  */
#line 2130 "cf-parse.y"
    {
  if (this_area->areaid == 0) cf_error("Virtual link cannot be in backbone");
  this_ipatt = cfg_allocz(sizeof(struct ospf_iface_patt));
  add_tail(&OSPF_CFG->vlink_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);
  OSPF_PATT->voa = this_area->areaid;
  OSPF_PATT->vid = (yyvsp[(3) - (3)].i32);
  OSPF_PATT->helloint = HELLOINT_D;
  OSPF_PATT->rxmtint = RXMTINT_D;
  OSPF_PATT->inftransdelay = INFTRANSDELAY_D;
  OSPF_PATT->waitint = WAIT_DMH*HELLOINT_D;
  OSPF_PATT->deadc = DEADC_D;
  OSPF_PATT->deadint = 0;
  OSPF_PATT->type = OSPF_IT_VLINK;
  init_list(&OSPF_PATT->nbma_list);
  OSPF_PATT->autype = OSPF_AUTH_NONE;
  reset_passwords();
 ;}
    break;

  case 481:

/* Line 1464 of yacc.c  */
#line 2151 "cf-parse.y"
    { OSPF_PATT->cost = (yyvsp[(2) - (2)].i) ; if (((yyvsp[(2) - (2)].i)<=0) || ((yyvsp[(2) - (2)].i)>65535)) cf_error("Cost must be in range 1-65535"); ;}
    break;

  case 482:

/* Line 1464 of yacc.c  */
#line 2152 "cf-parse.y"
    { OSPF_PATT->helloint = (yyvsp[(2) - (2)].i) ; if (((yyvsp[(2) - (2)].i)<=0) || ((yyvsp[(2) - (2)].i)>65535)) cf_error("Hello interval must be in range 1-65535"); ;}
    break;

  case 483:

/* Line 1464 of yacc.c  */
#line 2153 "cf-parse.y"
    { OSPF_PATT->pollint = (yyvsp[(2) - (2)].i) ; if ((yyvsp[(2) - (2)].i)<=0) cf_error("Poll int must be greater than zero"); ;}
    break;

  case 484:

/* Line 1464 of yacc.c  */
#line 2154 "cf-parse.y"
    { OSPF_PATT->rxmtint = (yyvsp[(2) - (2)].i) ; if ((yyvsp[(2) - (2)].i)<=0) cf_error("Retransmit int must be greater than zero"); ;}
    break;

  case 485:

/* Line 1464 of yacc.c  */
#line 2155 "cf-parse.y"
    { OSPF_PATT->waitint = (yyvsp[(2) - (2)].i) ; ;}
    break;

  case 486:

/* Line 1464 of yacc.c  */
#line 2156 "cf-parse.y"
    { OSPF_PATT->deadint = (yyvsp[(2) - (2)].i) ; if ((yyvsp[(2) - (2)].i)<=1) cf_error("Dead interval must be greater than one"); ;}
    break;

  case 487:

/* Line 1464 of yacc.c  */
#line 2157 "cf-parse.y"
    { OSPF_PATT->deadc = (yyvsp[(3) - (3)].i) ; if ((yyvsp[(3) - (3)].i)<=1) cf_error("Dead count must be greater than one"); ;}
    break;

  case 488:

/* Line 1464 of yacc.c  */
#line 2158 "cf-parse.y"
    { OSPF_PATT->type = OSPF_IT_BCAST ; ;}
    break;

  case 489:

/* Line 1464 of yacc.c  */
#line 2159 "cf-parse.y"
    { OSPF_PATT->type = OSPF_IT_BCAST ; ;}
    break;

  case 490:

/* Line 1464 of yacc.c  */
#line 2160 "cf-parse.y"
    { OSPF_PATT->type = OSPF_IT_NBMA ; ;}
    break;

  case 491:

/* Line 1464 of yacc.c  */
#line 2161 "cf-parse.y"
    { OSPF_PATT->type = OSPF_IT_NBMA ; ;}
    break;

  case 492:

/* Line 1464 of yacc.c  */
#line 2162 "cf-parse.y"
    { OSPF_PATT->type = OSPF_IT_PTP ; ;}
    break;

  case 493:

/* Line 1464 of yacc.c  */
#line 2163 "cf-parse.y"
    { OSPF_PATT->type = OSPF_IT_PTP ; ;}
    break;

  case 494:

/* Line 1464 of yacc.c  */
#line 2164 "cf-parse.y"
    { OSPF_PATT->type = OSPF_IT_PTMP ; ;}
    break;

  case 495:

/* Line 1464 of yacc.c  */
#line 2165 "cf-parse.y"
    { OSPF_PATT->type = OSPF_IT_PTMP ; ;}
    break;

  case 496:

/* Line 1464 of yacc.c  */
#line 2166 "cf-parse.y"
    { OSPF_PATT->inftransdelay = (yyvsp[(3) - (3)].i) ; if (((yyvsp[(3) - (3)].i)<=0) || ((yyvsp[(3) - (3)].i)>65535)) cf_error("Transmit delay must be in range 1-65535"); ;}
    break;

  case 497:

/* Line 1464 of yacc.c  */
#line 2167 "cf-parse.y"
    { OSPF_PATT->priority = (yyvsp[(2) - (2)].i) ; if (((yyvsp[(2) - (2)].i)<0) || ((yyvsp[(2) - (2)].i)>255)) cf_error("Priority must be in range 0-255"); ;}
    break;

  case 498:

/* Line 1464 of yacc.c  */
#line 2168 "cf-parse.y"
    { OSPF_PATT->strictnbma = (yyvsp[(3) - (3)].i) ; ;}
    break;

  case 499:

/* Line 1464 of yacc.c  */
#line 2169 "cf-parse.y"
    { OSPF_PATT->stub = (yyvsp[(2) - (2)].i) ; ;}
    break;

  case 500:

/* Line 1464 of yacc.c  */
#line 2170 "cf-parse.y"
    { OSPF_PATT->check_link = (yyvsp[(3) - (3)].i); ;}
    break;

  case 501:

/* Line 1464 of yacc.c  */
#line 2171 "cf-parse.y"
    { OSPF_PATT->ecmp_weight = (yyvsp[(3) - (3)].i) - 1; if (((yyvsp[(3) - (3)].i)<1) || ((yyvsp[(3) - (3)].i)>256)) cf_error("ECMP weight must be in range 1-256"); ;}
    break;

  case 503:

/* Line 1464 of yacc.c  */
#line 2173 "cf-parse.y"
    { OSPF_PATT->autype = OSPF_AUTH_NONE ; ;}
    break;

  case 504:

/* Line 1464 of yacc.c  */
#line 2174 "cf-parse.y"
    { OSPF_PATT->autype = OSPF_AUTH_SIMPLE ; ;}
    break;

  case 505:

/* Line 1464 of yacc.c  */
#line 2175 "cf-parse.y"
    { OSPF_PATT->autype = OSPF_AUTH_CRYPT ; ;}
    break;

  case 506:

/* Line 1464 of yacc.c  */
#line 2176 "cf-parse.y"
    { OSPF_PATT->rxbuf = OSPF_RXBUF_LARGE ; ;}
    break;

  case 507:

/* Line 1464 of yacc.c  */
#line 2177 "cf-parse.y"
    { OSPF_PATT->rxbuf = OSPF_RXBUF_NORMAL ; ;}
    break;

  case 508:

/* Line 1464 of yacc.c  */
#line 2178 "cf-parse.y"
    { OSPF_PATT->rxbuf = (yyvsp[(3) - (3)].i) ; if (((yyvsp[(3) - (3)].i) < OSPF_RXBUF_MINSIZE) || ((yyvsp[(3) - (3)].i) > OSPF_MAX_PKT_SIZE)) cf_error("Buffer size must be in range 256-65535"); ;}
    break;

  case 513:

/* Line 1464 of yacc.c  */
#line 2190 "cf-parse.y"
    {
   this_pref = cfg_allocz(sizeof(struct area_net_config));
   add_tail(this_nets, NODE this_pref);
   this_pref->px.addr = (yyvsp[(1) - (1)].px).addr;
   this_pref->px.len = (yyvsp[(1) - (1)].px).len;
 ;}
    break;

  case 515:

/* Line 1464 of yacc.c  */
#line 2200 "cf-parse.y"
    { this_pref->hidden = 1; ;}
    break;

  case 516:

/* Line 1464 of yacc.c  */
#line 2201 "cf-parse.y"
    { this_pref->tag = (yyvsp[(2) - (2)].i); ;}
    break;

  case 521:

/* Line 1464 of yacc.c  */
#line 2214 "cf-parse.y"
    {
   this_nbma = cfg_allocz(sizeof(struct nbma_node));
   add_tail(&OSPF_PATT->nbma_list, NODE this_nbma);
   this_nbma->ip=(yyvsp[(1) - (2)].a);
   this_nbma->eligible=0;
 ;}
    break;

  case 522:

/* Line 1464 of yacc.c  */
#line 2223 "cf-parse.y"
    {
   this_nbma = cfg_allocz(sizeof(struct nbma_node));
   add_tail(&OSPF_PATT->nbma_list, NODE this_nbma);
   this_nbma->ip=(yyvsp[(1) - (3)].a);
   this_nbma->eligible=1;
 ;}
    break;

  case 523:

/* Line 1464 of yacc.c  */
#line 2233 "cf-parse.y"
    {
  this_ipatt = cfg_allocz(sizeof(struct ospf_iface_patt));
  add_tail(&this_area->patt_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);
  OSPF_PATT->cost = COST_D;
  OSPF_PATT->helloint = HELLOINT_D;
  OSPF_PATT->pollint = POLLINT_D;
  OSPF_PATT->rxmtint = RXMTINT_D;
  OSPF_PATT->inftransdelay = INFTRANSDELAY_D;
  OSPF_PATT->priority = PRIORITY_D;
  OSPF_PATT->waitint = WAIT_DMH*HELLOINT_D;
  OSPF_PATT->deadc = DEADC_D;
  OSPF_PATT->deadint = 0;
  OSPF_PATT->type = OSPF_IT_UNDEF;
  init_list(&OSPF_PATT->nbma_list);
  OSPF_PATT->autype = OSPF_AUTH_NONE;
  reset_passwords();
 ;}
    break;

  case 528:

/* Line 1464 of yacc.c  */
#line 2264 "cf-parse.y"
    { ospf_iface_finish(); ;}
    break;

  case 530:

/* Line 1464 of yacc.c  */
#line 2269 "cf-parse.y"
    { (yyval.t) = NULL; ;}
    break;

  case 531:

/* Line 1464 of yacc.c  */
#line 2274 "cf-parse.y"
    { ospf_sh(proto_get_named((yyvsp[(3) - (4)].s), &proto_ospf)); ;}
    break;

  case 532:

/* Line 1464 of yacc.c  */
#line 2277 "cf-parse.y"
    { ospf_sh_neigh(proto_get_named((yyvsp[(4) - (6)].s), &proto_ospf), (yyvsp[(5) - (6)].t)); ;}
    break;

  case 533:

/* Line 1464 of yacc.c  */
#line 2280 "cf-parse.y"
    { ospf_sh_iface(proto_get_named((yyvsp[(4) - (6)].s), &proto_ospf), (yyvsp[(5) - (6)].t)); ;}
    break;

  case 534:

/* Line 1464 of yacc.c  */
#line 2285 "cf-parse.y"
    { ospf_sh_state(proto_get_named((yyvsp[(4) - (6)].s), &proto_ospf), 0, 1); ;}
    break;

  case 535:

/* Line 1464 of yacc.c  */
#line 2288 "cf-parse.y"
    { ospf_sh_state(proto_get_named((yyvsp[(5) - (7)].s), &proto_ospf), 0, 0); ;}
    break;

  case 536:

/* Line 1464 of yacc.c  */
#line 2293 "cf-parse.y"
    { ospf_sh_state(proto_get_named((yyvsp[(4) - (6)].s), &proto_ospf), 1, 1); ;}
    break;

  case 537:

/* Line 1464 of yacc.c  */
#line 2296 "cf-parse.y"
    { ospf_sh_state(proto_get_named((yyvsp[(5) - (7)].s), &proto_ospf), 1, 0); ;}
    break;

  case 538:

/* Line 1464 of yacc.c  */
#line 2299 "cf-parse.y"
    { ospf_sh_lsadb(proto_get_named((yyvsp[(4) - (6)].s), &proto_ospf)); ;}
    break;

  case 539:

/* Line 1464 of yacc.c  */
#line 2304 "cf-parse.y"
    {
     this_proto = proto_config_new(&proto_pipe, sizeof(struct pipe_config), (yyvsp[(1) - (2)].i));
     this_proto->preference = DEF_PREF_PIPE;
     PIPE_CFG->mode = PIPE_TRANSPARENT;
  ;}
    break;

  case 542:

/* Line 1464 of yacc.c  */
#line 2314 "cf-parse.y"
    {
     if ((yyvsp[(4) - (5)].s)->class != SYM_TABLE)
       cf_error("Routing table name expected");
     PIPE_CFG->peer = (yyvsp[(4) - (5)].s)->def;
   ;}
    break;

  case 543:

/* Line 1464 of yacc.c  */
#line 2319 "cf-parse.y"
    { PIPE_CFG->mode = PIPE_OPAQUE; ;}
    break;

  case 544:

/* Line 1464 of yacc.c  */
#line 2320 "cf-parse.y"
    { PIPE_CFG->mode = PIPE_TRANSPARENT; ;}
    break;

  case 545:

/* Line 1464 of yacc.c  */
#line 2326 "cf-parse.y"
    {
     this_proto = proto_config_new(&proto_rip, sizeof(struct rip_proto_config), (yyvsp[(1) - (2)].i));
     rip_init_config(RIP_CFG);
   ;}
    break;

  case 548:

/* Line 1464 of yacc.c  */
#line 2335 "cf-parse.y"
    { RIP_CFG->infinity = (yyvsp[(3) - (4)].i); ;}
    break;

  case 549:

/* Line 1464 of yacc.c  */
#line 2336 "cf-parse.y"
    { RIP_CFG->port = (yyvsp[(3) - (4)].i); ;}
    break;

  case 550:

/* Line 1464 of yacc.c  */
#line 2337 "cf-parse.y"
    { RIP_CFG->period = (yyvsp[(3) - (4)].i); ;}
    break;

  case 551:

/* Line 1464 of yacc.c  */
#line 2338 "cf-parse.y"
    { RIP_CFG->garbage_time = (yyvsp[(4) - (5)].i); ;}
    break;

  case 552:

/* Line 1464 of yacc.c  */
#line 2339 "cf-parse.y"
    { RIP_CFG->timeout_time = (yyvsp[(4) - (5)].i); ;}
    break;

  case 553:

/* Line 1464 of yacc.c  */
#line 2340 "cf-parse.y"
    {RIP_CFG->authtype = (yyvsp[(3) - (4)].i); ;}
    break;

  case 555:

/* Line 1464 of yacc.c  */
#line 2342 "cf-parse.y"
    { RIP_CFG->honor = HO_ALWAYS; ;}
    break;

  case 556:

/* Line 1464 of yacc.c  */
#line 2343 "cf-parse.y"
    { RIP_CFG->honor = HO_NEIGHBOR; ;}
    break;

  case 557:

/* Line 1464 of yacc.c  */
#line 2344 "cf-parse.y"
    { RIP_CFG->honor = HO_NEVER; ;}
    break;

  case 559:

/* Line 1464 of yacc.c  */
#line 2349 "cf-parse.y"
    { (yyval.i)=AT_PLAINTEXT; ;}
    break;

  case 560:

/* Line 1464 of yacc.c  */
#line 2350 "cf-parse.y"
    { (yyval.i)=AT_MD5; ;}
    break;

  case 561:

/* Line 1464 of yacc.c  */
#line 2351 "cf-parse.y"
    { (yyval.i)=AT_NONE; ;}
    break;

  case 562:

/* Line 1464 of yacc.c  */
#line 2356 "cf-parse.y"
    { (yyval.i)=IM_BROADCAST; ;}
    break;

  case 563:

/* Line 1464 of yacc.c  */
#line 2357 "cf-parse.y"
    { (yyval.i)=0; ;}
    break;

  case 564:

/* Line 1464 of yacc.c  */
#line 2358 "cf-parse.y"
    { (yyval.i)=IM_QUIET; ;}
    break;

  case 565:

/* Line 1464 of yacc.c  */
#line 2359 "cf-parse.y"
    { (yyval.i)=IM_NOLISTEN; ;}
    break;

  case 566:

/* Line 1464 of yacc.c  */
#line 2360 "cf-parse.y"
    { (yyval.i)=IM_VERSION1 | IM_BROADCAST; ;}
    break;

  case 568:

/* Line 1464 of yacc.c  */
#line 2364 "cf-parse.y"
    { RIP_IPATT->metric = (yyvsp[(2) - (2)].i); ;}
    break;

  case 569:

/* Line 1464 of yacc.c  */
#line 2365 "cf-parse.y"
    { RIP_IPATT->mode |= (yyvsp[(2) - (2)].i); ;}
    break;

  case 574:

/* Line 1464 of yacc.c  */
#line 2379 "cf-parse.y"
    {
     this_ipatt = cfg_allocz(sizeof(struct rip_patt));
     add_tail(&RIP_CFG->iface_list, NODE this_ipatt);
     init_list(&this_ipatt->ipn_list);
     RIP_IPATT->metric = 1;
   ;}
    break;

  case 576:

/* Line 1464 of yacc.c  */
#line 2395 "cf-parse.y"
    {
     this_proto = proto_config_new(&proto_static, sizeof(struct static_config), (yyvsp[(1) - (2)].i));
     static_init_config((struct static_config *) this_proto);
  ;}
    break;

  case 579:

/* Line 1464 of yacc.c  */
#line 2404 "cf-parse.y"
    { STATIC_CFG->check_link = (yyvsp[(4) - (5)].i); ;}
    break;

  case 580:

/* Line 1464 of yacc.c  */
#line 2405 "cf-parse.y"
    { STATIC_CFG->igp_table = (yyvsp[(4) - (5)].r); ;}
    break;

  case 582:

/* Line 1464 of yacc.c  */
#line 2409 "cf-parse.y"
    {
     this_srt = cfg_allocz(sizeof(struct static_route));
     add_tail(&STATIC_CFG->other_routes, &this_srt->n);
     this_srt->net = (yyvsp[(2) - (2)].px).addr;
     this_srt->masklen = (yyvsp[(2) - (2)].px).len;
  ;}
    break;

  case 583:

/* Line 1464 of yacc.c  */
#line 2418 "cf-parse.y"
    {
     last_srt_nh = this_srt_nh;
     this_srt_nh = cfg_allocz(sizeof(struct static_route));
     this_srt_nh->dest = RTD_NONE;
     this_srt_nh->via = (yyvsp[(2) - (2)].a);
     this_srt_nh->if_name = (void *) this_srt; /* really */
   ;}
    break;

  case 584:

/* Line 1464 of yacc.c  */
#line 2425 "cf-parse.y"
    {
     this_srt_nh->masklen = (yyvsp[(3) - (3)].i) - 1; /* really */
     if (((yyvsp[(3) - (3)].i)<1) || ((yyvsp[(3) - (3)].i)>256)) cf_error("Weight must be in range 1-256"); 
   ;}
    break;

  case 585:

/* Line 1464 of yacc.c  */
#line 2432 "cf-parse.y"
    { this_srt->mp_next = this_srt_nh; ;}
    break;

  case 586:

/* Line 1464 of yacc.c  */
#line 2433 "cf-parse.y"
    { last_srt_nh->mp_next = this_srt_nh; ;}
    break;

  case 587:

/* Line 1464 of yacc.c  */
#line 2437 "cf-parse.y"
    {
      this_srt->dest = RTD_ROUTER;
      this_srt->via = (yyvsp[(3) - (3)].a);
   ;}
    break;

  case 588:

/* Line 1464 of yacc.c  */
#line 2441 "cf-parse.y"
    {
      this_srt->dest = RTD_DEVICE;
      this_srt->if_name = (yyvsp[(3) - (3)].t);
      rem_node(&this_srt->n);
      add_tail(&STATIC_CFG->iface_routes, &this_srt->n);
   ;}
    break;

  case 589:

/* Line 1464 of yacc.c  */
#line 2447 "cf-parse.y"
    {
      this_srt->dest = RTD_MULTIPATH;
   ;}
    break;

  case 590:

/* Line 1464 of yacc.c  */
#line 2450 "cf-parse.y"
    {
      this_srt->dest = RTDX_RECURSIVE;
      this_srt->via = (yyvsp[(3) - (3)].a);
   ;}
    break;

  case 591:

/* Line 1464 of yacc.c  */
#line 2454 "cf-parse.y"
    { this_srt->dest = RTD_BLACKHOLE; ;}
    break;

  case 592:

/* Line 1464 of yacc.c  */
#line 2455 "cf-parse.y"
    { this_srt->dest = RTD_UNREACHABLE; ;}
    break;

  case 593:

/* Line 1464 of yacc.c  */
#line 2456 "cf-parse.y"
    { this_srt->dest = RTD_PROHIBIT; ;}
    break;

  case 594:

/* Line 1464 of yacc.c  */
#line 2460 "cf-parse.y"
    { static_show(proto_get_named((yyvsp[(3) - (4)].s), &proto_static)); ;}
    break;

  case 648:

/* Line 1464 of yacc.c  */
#line 2468 "cf-parse.y"
    { bgp_check_config(BGP_CFG); ;}
    break;

  case 649:

/* Line 1464 of yacc.c  */
#line 2468 "cf-parse.y"
    { ospf_proto_finish(); ;}
    break;

  case 651:

/* Line 1464 of yacc.c  */
#line 2468 "cf-parse.y"
    { RIP_CFG->passwords = get_passwords(); ;}
    break;

  case 660:

/* Line 1464 of yacc.c  */
#line 2471 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_IP_ADDRESS, T_IP, EA_KRT_PREFSRC); ;}
    break;

  case 661:

/* Line 1464 of yacc.c  */
#line 2471 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_REALM); ;}
    break;

  case 662:

/* Line 1464 of yacc.c  */
#line 2472 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_GEN_IGP_METRIC); ;}
    break;

  case 663:

/* Line 1464 of yacc.c  */
#line 2472 "cf-parse.y"
    { (yyval.x) = NULL; ;}
    break;

  case 664:

/* Line 1464 of yacc.c  */
#line 2473 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT, T_ENUM_BGP_ORIGIN, EA_CODE(EAP_BGP, BA_ORIGIN)); ;}
    break;

  case 665:

/* Line 1464 of yacc.c  */
#line 2474 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_AS_PATH, T_PATH, EA_CODE(EAP_BGP, BA_AS_PATH)); ;}
    break;

  case 666:

/* Line 1464 of yacc.c  */
#line 2475 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_IP_ADDRESS, T_IP, EA_CODE(EAP_BGP, BA_NEXT_HOP)); ;}
    break;

  case 667:

/* Line 1464 of yacc.c  */
#line 2476 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_CODE(EAP_BGP, BA_MULTI_EXIT_DISC)); ;}
    break;

  case 668:

/* Line 1464 of yacc.c  */
#line 2477 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_CODE(EAP_BGP, BA_LOCAL_PREF)); ;}
    break;

  case 669:

/* Line 1464 of yacc.c  */
#line 2478 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_OPAQUE, T_ENUM_EMPTY, EA_CODE(EAP_BGP, BA_ATOMIC_AGGR)); ;}
    break;

  case 670:

/* Line 1464 of yacc.c  */
#line 2479 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_CODE(EAP_BGP, BA_AGGREGATOR)); ;}
    break;

  case 671:

/* Line 1464 of yacc.c  */
#line 2480 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT_SET, T_CLIST, EA_CODE(EAP_BGP, BA_COMMUNITY)); ;}
    break;

  case 672:

/* Line 1464 of yacc.c  */
#line 2481 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_ROUTER_ID, T_QUAD, EA_CODE(EAP_BGP, BA_ORIGINATOR_ID)); ;}
    break;

  case 673:

/* Line 1464 of yacc.c  */
#line 2482 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT_SET, T_CLIST, EA_CODE(EAP_BGP, BA_CLUSTER_LIST)); ;}
    break;

  case 674:

/* Line 1464 of yacc.c  */
#line 2483 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_EC_SET, T_ECLIST, EA_CODE(EAP_BGP, BA_EXT_COMMUNITY)); ;}
    break;

  case 675:

/* Line 1464 of yacc.c  */
#line 2483 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT | EAF_TEMP, T_INT, EA_OSPF_METRIC1); ;}
    break;

  case 676:

/* Line 1464 of yacc.c  */
#line 2483 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT | EAF_TEMP, T_INT, EA_OSPF_METRIC2); ;}
    break;

  case 677:

/* Line 1464 of yacc.c  */
#line 2483 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT | EAF_TEMP, T_INT, EA_OSPF_TAG); ;}
    break;

  case 678:

/* Line 1464 of yacc.c  */
#line 2483 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_ROUTER_ID | EAF_TEMP, T_QUAD, EA_OSPF_ROUTER_ID); ;}
    break;

  case 679:

/* Line 1464 of yacc.c  */
#line 2483 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT | EAF_TEMP, T_INT, EA_RIP_METRIC); ;}
    break;

  case 680:

/* Line 1464 of yacc.c  */
#line 2483 "cf-parse.y"
    { (yyval.x) = f_new_dynamic_attr(EAF_TYPE_INT | EAF_TEMP, T_INT, EA_RIP_TAG); ;}
    break;



/* Line 1464 of yacc.c  */
#line 7890 "cf-parse.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1684 of yacc.c  */
#line 2485 "cf-parse.y"

/* C Code from ../../conf/confbase.Y */

/* C Code from ../../sysdep/unix/config.Y */

/* C Code from ../../sysdep/unix/krt.Y */

/* C Code from ../../sysdep/linux/netlink/netlink.Y */

/* C Code from ../../nest/config.Y */

/* C Code from ../../proto/bgp/config.Y */

/* C Code from ../../proto/ospf/config.Y */

/* C Code from ../../proto/pipe/config.Y */

/* C Code from ../../proto/rip/config.Y */

/* C Code from ../../proto/static/config.Y */


