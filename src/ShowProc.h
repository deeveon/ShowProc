#ifndef SHOWPROC_H
#define SHOWPROC_H

//--------------------------------------------------------------------------------
// typedefs, enums & structs
//--------------------------------------------------------------------------------

// Modes
typedef enum Mode { 
	MODE_ALL,				// Show both system & Shell/CLI processes
	MODE_CLI,				// Show Shell/CLI processes only
	MODE_SYSTEM				// Show system tasks/processes
} Mode;

// Output formats
typedef enum OutFrmt { 
	FORMAT_VERBOSE, 		// Show detailed info about each task/process
	FORMAT_TCB,				// Same as VERBOSE minus name
	FORMAT_SHORT, 			// Show only task/process number & name
	FORMAT_COMMAND			// Show only the Shell/CLI process number
} OutFrmt;

//--------------------------------------------------------------------------------
// Command line template for ReadArgs
//--------------------------------------------------------------------------------
#define TEMPLATE		"VER=VERSION/S,ALL/S,CLI=SHELL/S,SYS=SYSTEM/S," \
						"F=FULL/S,TCB/S,S=SHORT/S," \
						"P=PROCESS/N,COM=COMMAND/K"

#define OPT_VERSION			0			// Show program version and exit
#define OPT_ALL				1			// Show both system & Shell/CLI processes
#define OPT_CLI				2			// Shell/CLI processes only
#define OPT_SYS				3			// System tasks/processes only (default)
#define OPT_FULL			4			// Show full info (default)
#define OPT_TCB				5			// Same as FULL minus name
#define OPT_SHORT			6			// Just number & name
#define OPT_PROCESS			7			// Display specific process number only
#define OPT_COMMAND			8			// Searches for a process by command name
#define OPT_COUNT 			9

//--------------------------------------------------------------------------------
// Constants
//--------------------------------------------------------------------------------
#define KICKSTART_MIN_VER	37		// Min Kickstart version required (37 = 2.04)
#define OS_MIN_VER			37		// Min AmigaOS version required (37 = 2.04)

#define PROGRAM_PRIORITY	11		// High enough to reduce risk of changes while 
									// reading tasks, but not too high to
									// interfere with system operation
#define MAX_CMD_NAME_LEN	102		// Max command name length


//--------------------------------------------------------------------------------
// String constants
//--------------------------------------------------------------------------------
#define PROGRAM				"ShowProc"

// #define STR_CLI_HEADING		Bold "Shell/CLI Processes\n===================" BoldEnd
// #define STR_TASK_HEADING	Bold "System Tasks & Processes\n========================" BoldEnd
#define STR_CLI_HEADING		"Shell/CLI Processes\n==================="
#define STR_SYS_HEADING		"System Tasks & Processes\n========================"
#define STR_NO				"No"
#define STR_YES				"Yes"
#define STR_NO_COMMAND 		"No command loaded"
#define STR_NO_PROCESS		"No such process"

// State names
#define STR_STATE_INVALID	"Invld"
#define STR_STATE_ADDED		"Added"
#define STR_STATE_RUN		"Run"
#define STR_STATE_READY		"Ready"
#define STR_STATE_WAIT		"Wait"
#define STR_STATE_EXCEPT	"Excpt"
#define STR_STATE_REMOVED	"Remvd"
#define STR_STATE_UNDEFINED	"Undef"

// Error messages
#define STR_ERR_GET_CLI			"Error getting CLI info"
#define STR_ERR_GET_CMD			"Error getting command name"
#define STR_ERR_GET_OWN_PROC	"Error getting own process info"
#define STR_ERR_INV_CMD_NAME	"Invalid command name"
#define STR_ERR_INV_POINTER		"Invalid pointer"
#define STR_INV_PROC_NUM		"Process number must be between 1 and 999"
#define STR_INV_TASK_FMT		"Invalid task output format"
#define STR_INV_TASK_LIST		"Invalid task list"
#define STR_INV_TASK_TYPE		"Invalid task type"
#define STR_KS_TOO_OLD			"This program requires Kickstart 2.04 or higher"
#define STR_OS_TOO_OLD			"This program requires AmigaOS 2.04 or higher"
#define STR_INV_CMD_PAT			"Invalid command pattern"

//--------------------------------------------------------------------------------
// CLI table headings
//--------------------------------------------------------------------------------
#define NUM_TOP				""
#define NUM_BOT				"Num"
#define NUM_DIV				"---"

#define NAME_TOP			""
#define CLI_NAME   			"Command Name"
#define NAME_DIV      		"---------------------------------"

#define CLI_INFO_TOP		"           Stack  Stack Fail"
#define CLI_INFO_BOT		" Pri   GV   Used   Size  Lvl  RC   BG"
#define CLI_INFO_DIV		"---- ---- ------ ------ ---- --- ----"

// CLI heading format strings
#define CLI_VERBOSE			" %3s %-33s %-37s\n"
#define CLI_TCB				" %3s %-38s\n"
#define CLI_SHORT			" %3s %-33s\n"

//--------------------------------------------------------------------------------
// System tasks/processes headings
//--------------------------------------------------------------------------------

// NUM NAME PRI USED SIZE STATE 
#define SYS_NAME  			"Task/Process Name"

#define SYS_INFO_TOP		"         CLI        Stack  Stack"
#define SYS_INFO_BOT		" Pri T/P Num State   Used   Size"
#define SYS_INFO_DIV		"---- --- --- ----- ------ ------"

// Task heading format strings
#define SYS_VERBOSE			" %3s %-33s %-29s\n"
#define SYS_TCB				" %3s %-29s\n"
#define SYS_SHORT			" %3s %-33s\n"

//--------------------------------------------------------------------------------
// Output formats
//--------------------------------------------------------------------------------
// Verbose
#define CLI_VERBOSE_TOP	    NUM_TOP, NAME_TOP, CLI_INFO_TOP
#define CLI_VERBOSE_BOT		NUM_BOT, CLI_NAME, CLI_INFO_BOT
#define CLI_VERBOSE_DIV		NUM_DIV, NAME_DIV, CLI_INFO_DIV

#define SYS_VERBOSE_TOP		NUM_TOP, NAME_TOP, SYS_INFO_TOP
#define SYS_VERBOSE_BOT		NUM_BOT, SYS_NAME, SYS_INFO_BOT
#define SYS_VERBOSE_DIV		NUM_DIV, NAME_DIV, SYS_INFO_DIV

// TCB
#define CLI_TCB_TOP			NUM_TOP, CLI_INFO_TOP
#define CLI_TCB_BOT			NUM_BOT, CLI_INFO_BOT
#define CLI_TCB_DIV			NUM_DIV, CLI_INFO_DIV

#define SYS_TCB_TOP			NUM_TOP, SYS_INFO_TOP
#define SYS_TCB_BOT			NUM_BOT, SYS_INFO_BOT
#define SYS_TCB_DIV			NUM_DIV, SYS_INFO_DIV

// Short
#define CLI_SHORT_TOP		NUM_TOP, NAME_TOP
#define CLI_SHORT_BOT		NUM_BOT, CLI_NAME
#define CLI_SHORT_DIV		NUM_DIV, NAME_DIV

#define SYS_SHORT_TOP		NUM_TOP, NAME_TOP
#define SYS_SHORT_BOT		NUM_BOT, SYS_NAME
#define SYS_SHORT_DIV		NUM_DIV, NAME_DIV


#endif // SHOWPROC_H
