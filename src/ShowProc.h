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
#define TEMPLATE			"VER=VERSION/S,P=PROCESS/N,F=FULL/S,S=SHORT/S," \
							"TCB/S,CLI=ALL/S,COM=COMMAND/K"

#define OPT_VERSION			0			// Show program version and exit
#define OPT_PROCESS			1			// Display specific process number only
#define OPT_FULL			2			// Show full info (default)
#define OPT_SHORT			3			// Just CLI number & command name
#define OPT_TCB				4			// Same as FULL minus command name
#define OPT_CLI				5			// Same as SHORT (for now)
#define OPT_COMMAND			6			// Searches for a process by command name
#define OPT_COUNT 			7

// --------------------------------------------------------------------------------
// String constants
// --------------------------------------------------------------------------------
#define PROGRAM					"ShowProc"
#define STR_YES					"Yes"
#define STR_NO					"No"
#define STR_NO_COMMAND 			"No command loaded"
#define STR_NO_PROCESS			"No such process"
#define STR_ERROR_GETTING_CLI	"Error getting CLI info"
#define STR_ERROR_GETTING_CMD	"Error getting command name"
#define STR_INVALID_PROC_NUM	"Process number must be between 1 and 999"
#define STR_PATTERN_TOO_LONG	"Pattern too long"

//                         1         2         3         4         5         6         7         8
//                ---------0---------0---------0---------0---------0---------0---------0---------0
#define CLI_TOP   ""
#define CLI_BOT   "Num"
#define CLI_DIV   "---"

#define CMD_TOP       ""
#define CMD_BOT       "Command Name"
#define CMD_DIV       "-----------------------------------"

#define INFO_TOP                                          "          Stack  Stack Fail"
#define INFO_BOT                                          "Pri   GV   Used   Size  Lvl   RC   BG"
#define INFO_DIV                                          "--- ---- ------ ------ ---- ---- ----"
//                         1         2         3         4         5         6         7         8
//                ---------0---------0---------0---------0---------0---------0---------0---------0

//                            Num   Command  Pri   GV    Used  Stack Fail  RC    BG
#define FORMAT_VERBOSE		" %3.3s %-35.35s %3.3s %4.4s %6.6s %6.6s %4.4s %4.4s %4.4s\n"
//                            Num   Pri   GV    Used  Stack Fail  RC    BG
#define FORMAT_TCB			" %3.3s %3.3s %4.4s %6.6s %6.6s %4.4s %4.4s %4.4s\n"
//							  Num   Command
#define FORMAT_SHORT		" %3.3s %-35.35s\n"

// Verbose mode
#define VERBOSE_TOP		    CLI_TOP, CMD_TOP, INFO_TOP
#define VERBOSE_BOTTOM		CLI_BOT, CMD_BOT, INFO_BOT
#define VERBOSE_DIVIDER		CLI_DIV, CMD_DIV, INFO_DIV
// TCB mode
#define TCB_TOP				CLI_TOP, INFO_TOP
#define TCB_BOTTOM			CLI_BOT, INFO_BOT
#define TCB_DIVIDER			CLI_DIV, INFO_DIV
// Short mode
#define SHORT_TOP			CLI_TOP, CMD_TOP
#define SHORT_BOTTOM		CLI_BOT, CMD_BOT
#define SHORT_DIVIDER		CLI_DIV, CMD_DIV


#endif // SHOWPROC_H
