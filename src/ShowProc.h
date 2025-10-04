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

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
#define KICKSTART_MIN_VER	37		// Minimum Kickstart version required (37 = 2.04)


// --------------------------------------------------------------------------------
// String constants
// --------------------------------------------------------------------------------
#define PROGRAM				"ShowProc"
#define STR_OS_TOO_OLD		"This program requires Kickstart 2.04 or higher"
#define STR_YES				"Yes"
#define STR_NO				"No"
#define STR_NO_COMMAND 		"No command loaded"
#define STR_NO_PROCESS		"No such process"
#define STR_ERR_GET_CLI		"Error getting CLI info"
#define STR_ERR_GET_CMD		"Error getting command name"
#define STR_INV_PROC_NUM	"Process number must be between 1 and 999"
#define STR_PAT_TOO_LONG	"Pattern too long"

// --------------------------------------------------------------------------------
// Table headings
// --------------------------------------------------------------------------------
#define CLI_TOP				""
#define CLI_BOT				"Num"
#define CLI_DIV				"---"

#define CMD_TOP				""
#define CMD_BOT      		"Command Name"
#define CMD_DIV      		"-----------------------------------"

#define INFO_TOP			"          Stack  Stack Fail"
#define INFO_BOT			"Pri   GV   Used   Size  Lvl   RC   BG"
#define INFO_DIV			"--- ---- ------ ------ ---- ---- ----"

// Heading format strings
#define FORMAT_VERBOSE		" %3s %-35s %-37s\n"
#define FORMAT_TCB			" %3s %-37s\n"
#define FORMAT_SHORT		" %3s %-35s\n"

// Verbose mode
#define VERBOSE_TOP		    CLI_TOP, CMD_TOP, INFO_TOP
#define VERBOSE_BOT			CLI_BOT, CMD_BOT, INFO_BOT
#define VERBOSE_DIV			CLI_DIV, CMD_DIV, INFO_DIV
// TCB mode
#define TCB_TOP				CLI_TOP, INFO_TOP
#define TCB_BOT				CLI_BOT, INFO_BOT
#define TCB_DIV				CLI_DIV, INFO_DIV
// Short mode
#define SHORT_TOP			CLI_TOP, CMD_TOP
#define SHORT_BOT			CLI_BOT, CMD_BOT
#define SHORT_DIV			CLI_DIV, CMD_DIV


#endif // SHOWPROC_H
