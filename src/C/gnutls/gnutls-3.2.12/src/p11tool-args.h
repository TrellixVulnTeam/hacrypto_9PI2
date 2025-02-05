/*   -*- buffer-read-only: t -*- vi: set ro:
 *
 *  DO NOT EDIT THIS FILE   (p11tool-args.h)
 *
 *  It has been AutoGen-ed  March  4, 2014 at 08:19:39 PM by AutoGen 5.18.2
 *  From the definitions    p11tool-args.def
 *  and the template file   options
 *
 * Generated from AutoOpts 40:1:15 templates.
 *
 *  AutoOpts is a copyrighted work.  This header file is not encumbered
 *  by AutoOpts licensing, but is provided under the licensing terms chosen
 *  by the p11tool author or copyright holder.  AutoOpts is
 *  licensed under the terms of the LGPL.  The redistributable library
 *  (``libopts'') is licensed under the terms of either the LGPL or, at the
 *  users discretion, the BSD license.  See the AutoOpts and/or libopts sources
 *  for details.
 *
 * The p11tool program is copyrighted and licensed
 * under the following terms:
 *
 *  Copyright (C) 2000-2014 Free Software Foundation, and others, all rights reserved.
 *  This is free software. It is licensed for use, modification and
 *  redistribution under the terms of the GNU General Public License,
 *  version 3 or later <http://gnu.org/licenses/gpl.html>
 *
 *  p11tool is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  p11tool is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 *  This file contains the programmatic interface to the Automated
 *  Options generated for the p11tool program.
 *  These macros are documented in the AutoGen info file in the
 *  "AutoOpts" chapter.  Please refer to that doc for usage help.
 */
#ifndef AUTOOPTS_P11TOOL_ARGS_H_GUARD
#define AUTOOPTS_P11TOOL_ARGS_H_GUARD 1
#include "config.h"
#include <autoopts/options.h>

/**
 *  Ensure that the library used for compiling this generated header is at
 *  least as new as the version current when the header template was released
 *  (not counting patch version increments).  Also ensure that the oldest
 *  tolerable version is at least as old as what was current when the header
 *  template was released.
 */
#define AO_TEMPLATE_VERSION 163841
#if (AO_TEMPLATE_VERSION < OPTIONS_MINIMUM_VERSION) \
 || (AO_TEMPLATE_VERSION > OPTIONS_STRUCT_VERSION)
# error option template version mismatches autoopts/options.h header
  Choke Me.
#endif

/**
 *  Enumeration of each option type for p11tool
 */
typedef enum {
    INDEX_OPT_DEBUG              =  0,
    INDEX_OPT_OUTFILE            =  1,
    INDEX_OPT_LIST_TOKENS        =  2,
    INDEX_OPT_EXPORT             =  3,
    INDEX_OPT_EXPORT_CHAIN       =  4,
    INDEX_OPT_LIST_MECHANISMS    =  5,
    INDEX_OPT_LIST_ALL           =  6,
    INDEX_OPT_LIST_ALL_CERTS     =  7,
    INDEX_OPT_LIST_CERTS         =  8,
    INDEX_OPT_LIST_ALL_PRIVKEYS  =  9,
    INDEX_OPT_LIST_PRIVKEYS      = 10,
    INDEX_OPT_LIST_KEYS          = 11,
    INDEX_OPT_LIST_ALL_TRUSTED   = 12,
    INDEX_OPT_INITIALIZE         = 13,
    INDEX_OPT_WRITE              = 14,
    INDEX_OPT_DELETE             = 15,
    INDEX_OPT_GENERATE_RANDOM    = 16,
    INDEX_OPT_GENERATE_RSA       = 17,
    INDEX_OPT_GENERATE_DSA       = 18,
    INDEX_OPT_GENERATE_ECC       = 19,
    INDEX_OPT_LABEL              = 20,
    INDEX_OPT_TRUSTED            = 21,
    INDEX_OPT_PRIVATE            = 22,
    INDEX_OPT_LOGIN              = 23,
    INDEX_OPT_SO_LOGIN           = 24,
    INDEX_OPT_ADMIN_LOGIN        = 25,
    INDEX_OPT_DETAILED_URL       = 26,
    INDEX_OPT_SECRET_KEY         = 27,
    INDEX_OPT_LOAD_PRIVKEY       = 28,
    INDEX_OPT_LOAD_PUBKEY        = 29,
    INDEX_OPT_LOAD_CERTIFICATE   = 30,
    INDEX_OPT_PKCS8              = 31,
    INDEX_OPT_BITS               = 32,
    INDEX_OPT_SEC_PARAM          = 33,
    INDEX_OPT_INDER              = 34,
    INDEX_OPT_INRAW              = 35,
    INDEX_OPT_OUTDER             = 36,
    INDEX_OPT_OUTRAW             = 37,
    INDEX_OPT_PROVIDER           = 38,
    INDEX_OPT_VERSION            = 39,
    INDEX_OPT_HELP               = 40,
    INDEX_OPT_MORE_HELP          = 41
} teOptIndex;
/** count of all options for p11tool */
#define OPTION_CT    42
/** p11tool version */
#define P11TOOL_VERSION       "3.2.12"
/** Full p11tool version text */
#define P11TOOL_FULL_VERSION  "p11tool 3.2.12"

/**
 *  Interface defines for all options.  Replace "n" with the UPPER_CASED
 *  option name (as in the teOptIndex enumeration above).
 *  e.g. HAVE_OPT(DEBUG)
 */
#define         DESC(n) (p11toolOptions.pOptDesc[INDEX_OPT_## n])
/** 'true' if an option has been specified in any way */
#define     HAVE_OPT(n) (! UNUSED_OPT(& DESC(n)))
/** The string argument to an option. The argument type must be \"string\". */
#define      OPT_ARG(n) (DESC(n).optArg.argString)
/** Mask the option state revealing how an option was specified.
 *  It will be one and only one of \a OPTST_SET, \a OPTST_PRESET,
 * \a OPTST_DEFINED, \a OPTST_RESET or zero.
 */
#define    STATE_OPT(n) (DESC(n).fOptState & OPTST_SET_MASK)
/** Count of option's occurrances *on the command line*. */
#define    COUNT_OPT(n) (DESC(n).optOccCt)
/** mask of \a OPTST_SET and \a OPTST_DEFINED. */
#define    ISSEL_OPT(n) (SELECTED_OPT(&DESC(n)))
/** 'true' if \a HAVE_OPT would yield 'false'. */
#define ISUNUSED_OPT(n) (UNUSED_OPT(& DESC(n)))
/** 'true' if OPTST_DISABLED bit not set. */
#define  ENABLED_OPT(n) (! DISABLED_OPT(& DESC(n)))
/** number of stacked option arguments.
 *  Valid only for stacked option arguments. */
#define  STACKCT_OPT(n) (((tArgList*)(DESC(n).optCookie))->useCt)
/** stacked argument vector.
 *  Valid only for stacked option arguments. */
#define STACKLST_OPT(n) (((tArgList*)(DESC(n).optCookie))->apzArgs)
/** Reset an option. */
#define    CLEAR_OPT(n) STMTS( \
                DESC(n).fOptState &= OPTST_PERSISTENT_MASK;   \
                if ((DESC(n).fOptState & OPTST_INITENABLED) == 0) \
                    DESC(n).fOptState |= OPTST_DISABLED; \
                DESC(n).optCookie = NULL )
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 *  Enumeration of p11tool exit codes
 */
typedef enum {
    P11TOOL_EXIT_SUCCESS         = 0,
    P11TOOL_EXIT_FAILURE         = 1,
    P11TOOL_EXIT_USAGE_ERROR     = 64,
    P11TOOL_EXIT_LIBOPTS_FAILURE = 70
}   p11tool_exit_code_t;
/**
 *  Interface defines for specific options.
 * @{
 */
#define VALUE_OPT_DEBUG          'd'

#define OPT_VALUE_DEBUG          (DESC(DEBUG).optArg.argInt)
#define VALUE_OPT_OUTFILE        0x1001
#define VALUE_OPT_LIST_TOKENS    0x1002
#define VALUE_OPT_EXPORT         0x1003
#define VALUE_OPT_EXPORT_CHAIN   0x1004
#define VALUE_OPT_LIST_MECHANISMS 0x1005
#define VALUE_OPT_LIST_ALL       0x1006
#define VALUE_OPT_LIST_ALL_CERTS 0x1007
#define VALUE_OPT_LIST_CERTS     0x1008
#define VALUE_OPT_LIST_ALL_PRIVKEYS 0x1009
#define VALUE_OPT_LIST_PRIVKEYS  0x100A
#define VALUE_OPT_LIST_KEYS      0x100B
#define VALUE_OPT_LIST_ALL_TRUSTED 0x100C
#define VALUE_OPT_INITIALIZE     0x100D
#define VALUE_OPT_WRITE          0x100E
#define VALUE_OPT_DELETE         0x100F
#define VALUE_OPT_GENERATE_RANDOM 0x1010

#define OPT_VALUE_GENERATE_RANDOM (DESC(GENERATE_RANDOM).optArg.argInt)
#define VALUE_OPT_GENERATE_RSA   0x1011
#define VALUE_OPT_GENERATE_DSA   0x1012
#define VALUE_OPT_GENERATE_ECC   0x1013
#define VALUE_OPT_LABEL          0x1014
#define VALUE_OPT_TRUSTED        0x1015
#define VALUE_OPT_PRIVATE        0x1016
#define VALUE_OPT_LOGIN          0x1017
#define VALUE_OPT_SO_LOGIN       0x1018
#define VALUE_OPT_ADMIN_LOGIN    0x1019
#define VALUE_OPT_DETAILED_URL   0x101A
#define VALUE_OPT_SECRET_KEY     0x101B
#define VALUE_OPT_LOAD_PRIVKEY   0x101C
#define VALUE_OPT_LOAD_PUBKEY    0x101D
#define VALUE_OPT_LOAD_CERTIFICATE 0x101E
#define VALUE_OPT_PKCS8          '8'
#define VALUE_OPT_BITS           0x101F

#define OPT_VALUE_BITS           (DESC(BITS).optArg.argInt)
#define VALUE_OPT_SEC_PARAM      0x1020
#define VALUE_OPT_INDER          0x1021
#define VALUE_OPT_INRAW          0x1022
#define VALUE_OPT_OUTDER         0x1023
#define VALUE_OPT_OUTRAW         0x1024
#define VALUE_OPT_PROVIDER       0x1025
/** option flag (value) for " (get "val-name") " option */
#define VALUE_OPT_HELP          'h'
/** option flag (value) for " (get "val-name") " option */
#define VALUE_OPT_MORE_HELP     '!'
/** option flag (value) for " (get "val-name") " option */
#define VALUE_OPT_VERSION       'v'
/*
 *  Interface defines not associated with particular options
 */
#define ERRSKIP_OPTERR  STMTS(p11toolOptions.fOptSet &= ~OPTPROC_ERRSTOP)
#define ERRSTOP_OPTERR  STMTS(p11toolOptions.fOptSet |= OPTPROC_ERRSTOP)
#define RESTART_OPT(n)  STMTS( \
                p11toolOptions.curOptIdx = (n); \
                p11toolOptions.pzCurOpt  = NULL )
#define START_OPT       RESTART_OPT(1)
#define USAGE(c)        (*p11toolOptions.pUsageProc)(&p11toolOptions, c)

#ifdef  __cplusplus
extern "C" {
#endif
/*
 *  global exported definitions
 */
#include <gettext.h>


/* * * * * *
 *
 *  Declare the p11tool option descriptor.
 */
extern tOptions p11toolOptions;

#if defined(ENABLE_NLS)
# ifndef _
#   include <stdio.h>
#   ifndef HAVE_GETTEXT
      extern char * gettext(char const *);
#   else
#     include <libintl.h>
#   endif

static inline char* aoGetsText(char const* pz) {
    if (pz == NULL) return NULL;
    return (char*)gettext(pz);
}
#   define _(s)  aoGetsText(s)
# endif /* _() */

# define OPT_NO_XLAT_CFG_NAMES  STMTS(p11toolOptions.fOptSet |= \
                                    OPTPROC_NXLAT_OPT_CFG;)
# define OPT_NO_XLAT_OPT_NAMES  STMTS(p11toolOptions.fOptSet |= \
                                    OPTPROC_NXLAT_OPT|OPTPROC_NXLAT_OPT_CFG;)

# define OPT_XLAT_CFG_NAMES     STMTS(p11toolOptions.fOptSet &= \
                                  ~(OPTPROC_NXLAT_OPT|OPTPROC_NXLAT_OPT_CFG);)
# define OPT_XLAT_OPT_NAMES     STMTS(p11toolOptions.fOptSet &= \
                                  ~OPTPROC_NXLAT_OPT;)

#else   /* ENABLE_NLS */
# define OPT_NO_XLAT_CFG_NAMES
# define OPT_NO_XLAT_OPT_NAMES

# define OPT_XLAT_CFG_NAMES
# define OPT_XLAT_OPT_NAMES

# ifndef _
#   define _(_s)  _s
# endif
#endif  /* ENABLE_NLS */

#ifdef  __cplusplus
}
#endif
#endif /* AUTOOPTS_P11TOOL_ARGS_H_GUARD */

/* p11tool-args.h ends here */
