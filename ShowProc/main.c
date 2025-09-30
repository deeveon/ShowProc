#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <proto/dos.h>
#include <proto/exec.h>

#define PROGRAM		"ShowProc"

// Command line template
//#define TEMPLATE		"PROC=PROCESS/N,SHORT/S,TCB/S,CLI=ALL/S,COM=COMMAND/K"
#define TEMPLATE		"PROC=PROCESS/N,SHORT/S"

#define OPT_PROCESS		0
#define OPT_SHORT		1
//#define OPT_COMMAND		2  // TODO
#define OPT_COUNT 		2

//                                1         2         3         4         5         6         7         8
//                       ---------0---------0---------0---------0---------0---------0---------0---------0--
#define FULL_HEADER		"                                                   Stack  Stack Fail Last"
#define SHORT_HEADER	" Num Process Command Name                    "
#define SHORT_DIVIDER	" --- ----------------------------------------"
#define EXT_HEADER		                                             " Pri   Used   Size  Lvl   RC   BG"
#define EXT_DIVIDER		                                             " --- ------ ------ ---- ---- ----"

// Function prototypes
long bstrlen(BSTR bstring);


//--------------------------------------------------------------------------------
//	main()
//--------------------------------------------------------------------------------
int main(void)
{
	struct 	RDArgs* rdargs = NULL;
	long	args[OPT_COUNT] = { NULL };
	struct	CommandLineInterface* cli = NULL;
	struct 	Process* process = NULL;
	int 	i = 0;
	int		start = 1;
	int		finish = -1;
	BOOL	verbose = TRUE;
    ULONG 	stack_size;
    long 	used_stack;
	int		rc = EXIT_SUCCESS;

	// Set the program name
	SetProgramName(PROGRAM);

	// Set task priority
	//SetTaskPri(FindTask(NULL), 0);

	// Get command line arguments
	rdargs = ReadArgs(TEMPLATE, args, NULL);

	if (rdargs == NULL) {
		PrintFault(IoErr(), NULL);
		rc = EXIT_FAILURE;
		goto exit;
	}

	// PROC=PROCESS argument 
	if (args[OPT_PROCESS]) {
		long * process_number = (long *) args[OPT_PROCESS];
		start = finish = *process_number;
	}

	// SHORT argument
	if (args[OPT_SHORT]) {
		verbose = FALSE;
	}

	// Per dos library docs, Forbid() is necessary when using FindCliProc()
	Forbid();
	{
		if (start != finish)
			finish = MaxCli() - 1;

		if (verbose) {
			Printf("%s\n",	 FULL_HEADER);
			Printf("%s%s\n", SHORT_HEADER, EXT_HEADER);
			Printf("%s%s\n", SHORT_DIVIDER, EXT_DIVIDER);
		} else {
			Printf("%s\n",	 SHORT_HEADER);
			Printf("%s\n",	 SHORT_DIVIDER);
		}
		
		// Loop through all the CLIs
		for (i = start; i <= finish; i++)
		{
			// Process struct has stack size & pointer to CLI struct
			process = FindCliProc(i);
	
			if (process == NULL) {
				if ((finish - start) == 0)
					Printf(" No such process\n");
				continue;
			}

			// Convert BPTR to CommandLineInterface pointer
			cli = (struct CommandLineInterface*) BADDR((*process).pr_CLI);
			
			// Calculate used & total stack sizes
		    stack_size 	= (long) process->pr_Task.tc_SPUpper - (long) process->pr_Task.tc_SPLower;
		    used_stack	= (long) process->pr_Task.tc_SPUpper - (long) process->pr_Task.tc_SPReg;

			// Show info for this process
			Printf("%4.3ld ", i);									// CLI #

			if (bstrlen(cli->cli_CommandName) > 0)					// Command name
				Printf("%-40.40b", 	cli->cli_CommandName);
			else
				Printf("%-40.40s",	"No command loaded");

			if (verbose) {
				//       Pri    Used   Stack  Fail   RC     BG
				Printf(" %3.3ld %6.6lu %6.6lu %4.4ld %4.4ld %4.3s",
					process->pr_Task.tc_Node.ln_Pri,				// Priority
					used_stack,										// Used stack size
					stack_size,										// Total stack size
					cli->cli_FailLevel,								// Failat Level
					cli->cli_ReturnCode,							// Last return code
					cli->cli_Background  ? "Yes" : "No");			// Background process?
			}

			Printf("\n");
		}
	}
	Permit();

exit:

	// Free resources
	if (rdargs)
		FreeArgs(rdargs);

	return rc;
}


//--------------------------------------------------------------------------------
//	bstrlen()
//--------------------------------------------------------------------------------
long bstrlen(BSTR bstring)
{
	char*	str = NULL;

	str = BADDR(bstring);
	
	if (str == NULL)
		return -1;

	return str[0];
}
