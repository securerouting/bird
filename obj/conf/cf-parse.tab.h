/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

/* Line 1685 of yacc.c  */
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



/* Line 1685 of yacc.c  */
#line 377 "cf-parse.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE cf_lval;


