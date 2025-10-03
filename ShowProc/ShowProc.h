#ifndef SHOWPROC_H
#define SHOWPROC_H

// --------------------------------------------------------------------------------
// typedefs, enums & structs
// --------------------------------------------------------------------------------

// Output modes
typedef enum MODE { 
	MODE_VERBOSE, 
	MODE_TCB,
	MODE_SHORT, 
	MODE_COMMAND 
} MODE;

// --------------------------------------------------------------------------------
// Command line template for ReadArgs
// --------------------------------------------------------------------------------
// Orig status template =	"PROCESS/N,FULL/S,TCB/S,CLI=ALL/S,COM=COMMAND/K"
#define TEMPLATE			"VER=VERSION/S,P=PROCESS/N,F=FULL/S,S=SHORT/S," \
							"TCB/S,CLI=ALL/S,COM=COMMAND/K"

#define OPT_VERSION			0		// Show program version and exit
#define OPT_PROCESS			1		// Display specific process number only
#define OPT_FULL			2		// Show full info (default)
#define OPT_SHORT			3		// Just CLI number & command name
#define OPT_TCB				4		// Same as FULL minus command name
#define OPT_CLI				5		// Same as SHORT (for now)
// #define OPT_ALL				6		// Display all processes
#define OPT_COMMAND			6		// TODO: Implement later
#define OPT_COUNT 			7

// --------------------------------------------------------------------------------
// String constants
// --------------------------------------------------------------------------------
#define PROGRAM	"ShowProc"

#define STR_INVALID_PROC_NUM	"Process number must be between 1 and 999"
#define STR_NO_COMMAND 			"No command loaded"
#define STR_NO_PROCESS			"No such process"
#define STR_ERROR_GETTING_CLI	"Error getting CLI info"
#define STR_ERROR_GETTING_CMD	"Error getting command name"
#define STR_YES					"Yes"
#define STR_NO					"No"
#define STR_PATTERN_TOO_LONG	"Pattern too long"

// Table header individual strings (used to stitch together complete headers below)

// Note the quote marks are aligned such that they represent the single space 
// between columns so we make sure it all fits into an 80 character-wide table
//                                    1         2         3         4         5         6         7         8
//                       	 ---------0---------0---------0---------0---------0---------0---------0---------0
#define CLI_HEAD_TOP         ""
#define CLI_HEAD_BOT         "Num"
#define CLI_DIVIDER          "---"

#define CMD_HEAD_TOP		     ""
#define CMD_HEAD_BOT		     "Command Name"
#define CMD_DIVIDER		         "-----------------------------------"

#define PRI_HEAD_TOP		                                         ""
#define PRI_HEAD_BOT		                                         "Pri"
#define PRI_DIVIDER			                                         "---"

#define GV_HEAD_TOP		                                                 ""
#define GV_HEAD_BOT		                                                 "  GV"
#define GV_DIVIDER		                                                 "----"

#define USED_HEAD_TOP		                                                  " Stack"
#define USED_HEAD_BOT		                                                  "  Used"
#define USED_DIVIDER		                                                  "------"

#define SIZE_HEAD_TOP		                                                         " Stack"
#define SIZE_HEAD_BOT		                                                         "  Size"
#define SIZE_DIVIDER		                                                         "------"

#define LVL_HEAD_TOP		                                                                "Fail"
#define LVL_HEAD_BOT		                                                                " Lvl"
#define LVL_DIVIDER		                                                                    "----"

#define RC_HEAD_TOP		                                                                         ""
#define RC_HEAD_BOT		                                                                         "  RC"
#define RC_DIVIDER		                                                                         "----"

#define BG_HEAD_TOP		                                                                              ""
#define BG_HEAD_BOT		                                                                              "  BG"
#define BG_DIVIDER		                                                                              "----"
//                                    1         2         3         4         5         6         7         8
//                       	 ---------0---------0---------0---------0---------0---------0---------0---------0

// Table heading format strings
//                            Num   Command  Pri   GV    Used  Stack Fail  RC    BG
#define FORMAT_VERBOSE		" %3.3s %-35.35s %3.3s %4.4s %6.6s %6.6s %4.4s %4.4s %4.4s\n"
//                            Num   Pri   GV    Used  Stack Fail  RC    BG
#define FORMAT_TCB			" %3.3s %3.3s %4.4s %6.6s %6.6s %4.4s %4.4s %4.4s\n"
//							  Num   Command
#define FORMAT_SHORT		" %3.3s %-35.35s\n"

// Table headings (stitched together from the pieces above)

// Verbose mode
#define VERBOSE_TOP		    CLI_HEAD_TOP, CMD_HEAD_TOP, PRI_HEAD_TOP, GV_HEAD_TOP, USED_HEAD_TOP, \
							SIZE_HEAD_TOP, LVL_HEAD_TOP, RC_HEAD_TOP, BG_HEAD_TOP
#define VERBOSE_BOTTOM		CLI_HEAD_BOT, CMD_HEAD_BOT, PRI_HEAD_BOT, GV_HEAD_BOT, USED_HEAD_BOT, \
							SIZE_HEAD_BOT, LVL_HEAD_BOT, RC_HEAD_BOT, BG_HEAD_BOT
#define VERBOSE_DIVIDER		CLI_DIVIDER, CMD_DIVIDER, PRI_DIVIDER, GV_DIVIDER, USED_DIVIDER, \
							SIZE_DIVIDER, LVL_DIVIDER, RC_DIVIDER, BG_DIVIDER
// TCB mode
#define TCB_TOP				CLI_HEAD_TOP, PRI_HEAD_TOP, GV_HEAD_TOP, USED_HEAD_TOP, SIZE_HEAD_TOP, \
							LVL_HEAD_TOP, RC_HEAD_TOP, BG_HEAD_TOP
#define TCB_BOTTOM			CLI_HEAD_BOT, PRI_HEAD_BOT, GV_HEAD_BOT, USED_HEAD_BOT, SIZE_HEAD_BOT, \
							LVL_HEAD_BOT, RC_HEAD_BOT, BG_HEAD_BOT
#define TCB_DIVIDER			CLI_DIVIDER, PRI_DIVIDER, GV_DIVIDER, USED_DIVIDER, SIZE_DIVIDER, \
							LVL_DIVIDER, RC_DIVIDER, BG_DIVIDER
// Short mode
#define SHORT_TOP			CLI_HEAD_TOP, CMD_HEAD_TOP
#define SHORT_BOTTOM		CLI_HEAD_BOT, CMD_HEAD_BOT
#define SHORT_DIVIDER		CLI_DIVIDER, CMD_DIVIDER


#endif // SHOWPROC_H