#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <proto/dos.h>
#include <proto/exec.h>

#define PROGRAM	"ShowProc"

// String constants
#define STR_INVALID_PROC_NUM	"Process number must be between 1 and 999"
#define STR_NO_COMMAND 			"No command loaded"
#define STR_NO_PROCESS			"No such process"
#define STR_ERROR_GETTING_CLI	"Error getting CLI info"
#define STR_YES					"Yes"
#define STR_NO					"No"

// Command line template for ReadArgs()
// Orig status template =	"PROCESS/N,FULL/S,TCB/S,CLI=ALL/S,COM=COMMAND/K"
#define TEMPLATE			"P=PROCESS/N,F=FULL/S,S=SHORT/S,T=TCB/S,CLI=ALL/S,COM=COMMAND/K"

#define OPT_PROCESS			0	// Display specific process number only
#define OPT_FULL			1	// Show full info (default)
#define OPT_SHORT			2	// Just CLI number & command name
#define OPT_TCB				3	// Same as FULL minus command name
#define OPT_CLI				4	// Same as SHORT (for now)
// #define OPT_ALL				5	// Display all processes (Same as CLI for now though to match status)
#define OPT_COMMAND			5	// TODO: Implement later
#define OPT_COUNT 			6

// Output column headers
//                                    1         2         3         4         5         6         7         8
//                       	 ---------0---------0---------0---------0---------0---------0---------0---------0
#define CLI_HEADER_TOP		 ""
#define CLI_HEADER_BOTTOM	 "Num"
#define CLI_DIVIDER			 "---"

#define CMD_HEADER_TOP		     ""
#define CMD_HEADER_BOTTOM	     "Process Command Name"
#define CMD_DIVIDER		         "-----------------------------------"

#define PRI_HEADER_TOP		                                         ""
#define PRI_HEADER_BOTTOM	                                         "Pri"
#define PRI_DIVIDER			                                         "---"

#define USED_HEADER_TOP		                                             " Stack"
#define USED_HEADER_BOTTOM	                                             "  Used"
#define USED_DIVIDER		                                             "------"

#define SIZE_HEADER_TOP		                                                    " Stack"
#define SIZE_HEADER_BOTTOM	                                                    "  Size"
#define SIZE_DIVIDER		                                                    "------"

#define LVL_HEADER_TOP		                                                           "Fail"
#define LVL_HEADER_BOTTOM	                                                           " Lvl"
#define LVL_DIVIDER		                                                               "----"

#define RC_HEADER_TOP		                                                                ""
#define RC_HEADER_BOTTOM	                                                                "  RC"
#define RC_DIVIDER		                                                                    "----"

#define BG_HEADER_TOP		                                                                     ""
#define BG_HEADER_BOTTOM	                                                                     "  BG"
#define BG_DIVIDER		                                                                         "----"
//                                    1         2         3         4         5         6         7         8
//                       	 ---------0---------0---------0---------0---------0---------0---------0---------0

// Function prototypes
BYTE bstrlen(BSTR bstring);


//--------------------------------------------------------------------------------
//	main()
//--------------------------------------------------------------------------------
int main(void)
{
	struct 	RDArgs* rdargs = NULL;
 	long	opts[OPT_COUNT];
	struct	CommandLineInterface* cli = NULL;
	struct 	Process* process = NULL;
	int 	i = 0;
	int		start = 1;
	int		finish = -1;
	BYTE	prev_task_pri = 0;
	BOOL	VERBOSE = TRUE;
	BOOL	TCB = FALSE;
//	BOOL	CLI = FALSE;	// Same as SHORT so not needed (yet)
    ULONG 	stack_size;
    ULONG 	used_stack;
	int		rc = EXIT_SUCCESS;

	// Capture current task priority & set the priority high enough to minimize
	// the chance of a process exiting while we are working with it.
	prev_task_pri = SetTaskPri(FindTask(NULL), 0);

	// Set the program name for our own process
	SetProgramName(PROGRAM);

	// Get command line arguments
	memset((long*)opts, 0, sizeof(opts));
	rdargs = ReadArgs(TEMPLATE, opts, NULL);

	if (rdargs == NULL) {
		PrintFault(IoErr(), NULL);
		rc = EXIT_FAILURE;
		goto exit;
	}

	// PROC=PROCESS/N argument 
	if (opts[OPT_PROCESS]) {
		start = finish = *((long*)opts[OPT_PROCESS]);
		if (start < 1 || start > 999) {
			Printf("%s\n", STR_INVALID_PROC_NUM);
			rc = EXIT_FAILURE;
			goto exit;
		}
	}

	// If user didn't specify a specific process, set finish to max number of CLIs
	if (start != finish)
		finish = MaxCli() - 1;

	// Max process number supported is 999
	if (finish > 999)
			finish = 999;

	// If FULL is set ignore SHORT, TCB and CLI
	if (opts[OPT_FULL])
		VERBOSE = TRUE;
	else
	{
		if (opts[OPT_SHORT])
			VERBOSE = FALSE;
		if (opts[OPT_CLI])
			VERBOSE = FALSE;
		if (opts[OPT_TCB]) {
			TCB = TRUE;				// Will effectively override SHORT & CLI
			VERBOSE = FALSE;
		}
	}

	if (VERBOSE) {
		Printf(" %3.3s %-40.40s %3.3s %6.6s %6.6s %4.4s %4.4s %4.4s\n",
			CLI_HEADER_TOP, CMD_HEADER_TOP, PRI_HEADER_TOP, USED_HEADER_TOP, SIZE_HEADER_TOP,
			LVL_HEADER_TOP, RC_HEADER_TOP, BG_HEADER_TOP);
		Printf(" %3.3s %-40.40s %3.3s %6.6s %6.6s %4.4s %4.4s %4.4s\n",
			CLI_HEADER_BOTTOM, CMD_HEADER_BOTTOM, PRI_HEADER_BOTTOM, USED_HEADER_BOTTOM,
			SIZE_HEADER_BOTTOM, LVL_HEADER_BOTTOM, RC_HEADER_BOTTOM, BG_HEADER_BOTTOM);
		Printf(" %3.3s %-40.40s %3.3s %6.6s %6.6s %4.4s %4.4s %4.4s\n",
			CLI_DIVIDER, CMD_DIVIDER, PRI_DIVIDER, USED_DIVIDER, SIZE_DIVIDER,
			LVL_DIVIDER, RC_DIVIDER, BG_DIVIDER);
	} else {
		if (TCB) {
			Printf(" %3.3s %3.3s %6.6s %6.6s %4.4s %4.4s %4.4s\n",
				CLI_HEADER_TOP, PRI_HEADER_TOP, USED_HEADER_TOP, SIZE_HEADER_TOP,
				LVL_HEADER_TOP, RC_HEADER_TOP, BG_HEADER_TOP);
			Printf(" %3.3s %3.3s %6.6s %6.6s %4.4s %4.4s %4.4s\n",
				CLI_HEADER_BOTTOM, PRI_HEADER_BOTTOM, USED_HEADER_BOTTOM,
				SIZE_HEADER_BOTTOM, LVL_HEADER_BOTTOM, RC_HEADER_BOTTOM, BG_HEADER_BOTTOM);
			Printf(" %3.3s %3.3s %6.6s %6.6s %4.4s %4.4s %4.4s\n",
				CLI_DIVIDER, PRI_DIVIDER, USED_DIVIDER, SIZE_DIVIDER,
				LVL_DIVIDER, RC_DIVIDER, BG_DIVIDER);
		} else {
			Printf(" %3.3s %-40.40s\n", CLI_HEADER_TOP, CMD_HEADER_TOP);
			Printf(" %3.3s %-40.40s\n", CLI_HEADER_BOTTOM, CMD_HEADER_BOTTOM);
			Printf(" %3.3s %-40.40s\n", CLI_DIVIDER, CMD_DIVIDER);
		}
	}
	// Per dos library docs, Forbid() is normally used with FindCliProc()
	Forbid();
	{
		// Loop through all the CLIs
		for (i = start; i <= finish; i++)
		{
			// Process struct has the stack size & pointer to the CLI struct
			process = FindCliProc(i);
	
			if (process == NULL) {
				if (start == finish)
					Printf(" %3.3ld %s\n", i, STR_NO_PROCESS);
				continue;
			}

			// Print CLI number
			Printf(" %3.3ld", i);

			// Convert BPTR to CommandLineInterface pointer
			cli = (struct CommandLineInterface*) BADDR((*process).pr_CLI);

			if (cli == NULL) {
				Printf(" %s\n", STR_ERROR_GETTING_CLI);
				continue;
			}

			// Only print command name if not TCB mode
			if (!TCB) {
				if (bstrlen(cli->cli_CommandName) > 0)
					Printf(" %-40.40b",	cli->cli_CommandName);			// %b prints BSTR strings (AmigaOS only)
				else
					Printf(" %-40.40s",	STR_NO_COMMAND);
			}

			// Print the rest of the process details if not SHORT mode
			if (VERBOSE || TCB) 
			{
				// Calculate used & total stack sizes
			    stack_size 	= (long) process->pr_Task.tc_SPUpper - (long) process->pr_Task.tc_SPLower;
			    used_stack	= (long) process->pr_Task.tc_SPUpper - (long) process->pr_Task.tc_SPReg;

				//       Pri    Used   Stack  Fail   RC     BG
				Printf(" %3.3ld %6.6lu %6.6lu %4.4ld %4.4ld %4.3s",
					process->pr_Task.tc_Node.ln_Pri,					// Priority
					used_stack,											// Used stack size
					stack_size,											// Total stack size
					cli->cli_FailLevel,									// Failat Level
					cli->cli_ReturnCode,								// Last return code
					cli->cli_Background  ? STR_YES : STR_NO);			// Background process?
			}

			// End this output line
			Printf("\n");
		}
	}
	Permit();  // End Forbid() section

exit:

	// Free resources
	if (rdargs) {
		FreeArgs(rdargs);
	}

	// Restore previous task priority
	SetTaskPri(FindTask(NULL), prev_task_pri);

	return rc;
}


//--------------------------------------------------------------------------------
//	bstrlen()
//--------------------------------------------------------------------------------
BYTE bstrlen(BSTR bstring)
{
	char* str = NULL;

	// If the BCPL string pointer is NULL or the pointer conversion to C string
	// failed, return -1
	if (bstring == NULL || (str = BADDR(bstring)) == NULL)
		return -1;

	// Return the length of the BCPL string (first byte is length)
	return str[0];
}
