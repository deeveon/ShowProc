//--------------------------------------------------------------------------------
//	ShowProc - Displays information about active Shell/CLI processes
//
//	Copyright (C) 2025 Matthew Sawyer
//--------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <proto/dos.h>
#include <proto/exec.h>

#include "ShowProc_rev.h"
#include "ShowProc.h"

// Embed version tag into binary
const char* version = VERSTAG;

// Function prototypes
BYTE 	bstrlen(BSTR bstring);
size_t 	bstr2cstr(BSTR bstring, char* buffer, size_t bufsize);

//--------------------------------------------------------------------------------
//	main()
//--------------------------------------------------------------------------------
int main(void)
{
	// ParsePatternNoCase() docs say to make pattern buffer at least double the
	// length of the string to match + 2
	char	pattern[258] = {0};

	struct 	RDArgs*	rdargs;
 	long	opts[OPT_COUNT] = {0};
	struct 	Process* process;
	struct	CommandLineInterface* cli;
	char	cmd[128] = {0};
	int		start = 1;
	int		finish = -1;
	int 	proc_num;
	BYTE	prev_task_pri;
	MODE 	mode = MODE_VERBOSE;
	long	result;
	int		rc = RETURN_OK;

	// Capture current task priority & set the priority a bit higher to reduce
	// the risk of changes occurring while reading the process & CLI info
	prev_task_pri = SetTaskPri(FindTask(NULL), 1);

	// Set the program name for our own process
	SetProgramName(PROGRAM);

	// Get command line arguments
	rdargs = ReadArgs(TEMPLATE, opts, NULL);

	{ // Begin ReadArgs() section

		if (rdargs == NULL) {
			PrintFault(IoErr(), NULL);
			rc = RETURN_FAIL;
			goto exit;
		}

		// VERSION argument
		if (opts[OPT_VERSION]) {
			Printf("%s", VSTRING);
			rc = RETURN_OK;
			goto exit;
		}

		// PROCESS argument 
		if (opts[OPT_PROCESS]) {
			start = finish = *((long*)opts[OPT_PROCESS]);
			if (start < 1 || start > 999) {
				Printf("%s\n", STR_INV_PROC_NUM);
				rc = RETURN_FAIL;
				goto exit;
			}
		}

		// If the user didn't specify a specific process, set finish to the max number of
		// CLIs, but limit to 999 to match the max width of the process number field
		if (start != finish)
			finish = MaxCli() > 1000 ? 999 : MaxCli() - 1;  // -1 because last one is NULL

		// Determine output mode based on arguments received.
		if (opts[OPT_SHORT])	mode = MODE_SHORT;
		if (opts[OPT_CLI])		mode = MODE_SHORT; 			// Same as SHORT
		if (opts[OPT_TCB])		mode = MODE_TCB;			// Overrides SHORT and CLI/ALL
		if (opts[OPT_FULL])		mode = MODE_VERBOSE;		// Overrides SHORT, CLI/ALL and TCB
		if (opts[OPT_COMMAND])	mode = MODE_COMMAND;		// Overrides all others

		// Print header based on output mode
		switch (mode) {
			case MODE_VERBOSE:
				Printf(FORMAT_VERBOSE, VERBOSE_TOP);
				Printf(FORMAT_VERBOSE, VERBOSE_BOT);
				Printf(FORMAT_VERBOSE, VERBOSE_DIV);
				break;
			case MODE_TCB:
				Printf(FORMAT_TCB, TCB_TOP);
				Printf(FORMAT_TCB, TCB_BOT);
				Printf(FORMAT_TCB, TCB_DIV);
				break;
			case MODE_SHORT:
				Printf(FORMAT_SHORT, SHORT_TOP);
				Printf(FORMAT_SHORT, SHORT_BOT);
				Printf(FORMAT_SHORT, SHORT_DIV);
				break;
			case MODE_COMMAND:
				// No header for command mode
				rc = RETURN_WARN;  	// Assume we won't find it
				break;
			default:
				// Shouldn't get here so fail if we do
				rc = RETURN_FAIL;
				goto exit;
				break;
		}

	} // End ReadArgs() section

	// Per the Amiga DOS library docs, FindCliProc() is normally used with Forbid()
	Forbid();

	{ // Begin Forbid() section

		// Loop through all the CLIs
		for (proc_num = start; proc_num <= finish; proc_num++)
		{
			// Process struct has the stack size & pointer to the CLI struct
			process = FindCliProc(proc_num);

			// Make sure we got a valid pointer
			if (process == NULL) {
				// If the user requested a specific process number, show an error
				// message. If we're scanning all processes, just skip it.
				if (start == finish)
					Printf(" %3.3ld %s\n", proc_num, STR_NO_PROCESS);
				continue;	// Go to next process
			}

			// Convert BPTR to CommandLineInterface pointer
			cli = (struct CommandLineInterface*) BADDR((*process).pr_CLI);

			// Make sure we got a valid pointer
			if (cli == NULL) {
				Printf(" %s\n", STR_ERR_GET_CLI);
				continue;	// Go to next process
			}

			// Get the command name
			if (bstr2cstr(cli->cli_CommandName, cmd, sizeof(cmd)) < 0) {
				Printf(" %s\n", STR_ERR_GET_CMD);
				rc = RETURN_FAIL;
				continue;	// Go to next process
			}

			if (mode == MODE_COMMAND)
			{
				// Check if the user-supplied command string contains any wildcards
				result = ParsePatternNoCase((char *)opts[OPT_COMMAND], (STRPTR)pattern, sizeof(pattern));

				// Invalid pattern
				if (result == -1) {
					Printf("%s\n", STR_PAT_TOO_LONG);
					rc = RETURN_FAIL;
					break; // Exit the for loop
				}

				// Wildcards found
				if (result == 1) {
					if (MatchPatternNoCase(pattern, cmd)) {
						// Match found
						Printf("%2ld\n", proc_num);
						rc = RETURN_OK;
						break;	// Mission accomplished, exit the for loop
					} else {
						// No match, go to next process
						rc = RETURN_WARN;
						continue;
					}
				}

				// No wildcards, do a simple string compare
				if (!stricmp((char *)opts[OPT_COMMAND], cmd)) {
					// Match found
					// Print only the CLI number to match the output of STATUS
					Printf("%2ld\n", proc_num);
					rc = RETURN_OK;
					break;	// Mission accomplished, exit the for loop
				}

				// No match, go to next process
				rc = RETURN_WARN;
				continue;
			}

			// Print CLI number
			Printf(" %3.3ld", proc_num);

			// Only print command name if not TCB mode
			if (mode != MODE_TCB) {
				if (bstrlen(cli->cli_CommandName) > 0)
					Printf(" %-35.35b",	cli->cli_CommandName);		// %b prints BSTR strings (AmigaOS only)
				else
					Printf(" %-35.35s",	STR_NO_COMMAND);
			}

			// Print the rest of the process details if not SHORT mode
			if (mode == MODE_VERBOSE || mode == MODE_TCB) 
			{
				//       Pri    Used   Stack  Fail   RC     BG
				Printf(" %3.3ld %4.4ld %6.6ld %6.6ld %4.4ld %4.4ld %4.3s",
					process->pr_Task.tc_Node.ln_Pri,											// Priority
					process->pr_GlobVec ? *(long*)process->pr_GlobVec : 0,						// Global vector
					(long)process->pr_Task.tc_SPUpper - (long)process->pr_Task.tc_SPReg,		// Used stack size
					(long)process->pr_Task.tc_SPUpper - (long)process->pr_Task.tc_SPLower,		// Total stack size
					cli->cli_FailLevel,															// Failat Level
					cli->cli_ReturnCode,														// Last return code
					cli->cli_Background  ? STR_YES : STR_NO);									// Background process?
			}

			// End this output line
			Printf("\n");

			// Check for Ctrl-C break
			if (CheckSignal(SIGBREAKF_CTRL_C))
			{
				// Ctrl-C detected, abort
                PrintFault(ERROR_BREAK, NULL);
                break;	// Exit the for loop
			}
		}

	} // End Forbid() section

	Permit();

exit:

	// Free resources
	if (rdargs)
		FreeArgs(rdargs);

	// Restore previous task priority
	SetTaskPri(FindTask(NULL), prev_task_pri);

	return rc;
}


//--------------------------------------------------------------------------------
//	Returns the length of a BCPL string or -1 if the pointer is NULL.
//--------------------------------------------------------------------------------
BYTE bstrlen(BSTR bstring)
{
	char* str;

	// If the BCPL string pointer is NULL or the pointer conversion to C string
	// failed, return -1
	if (bstring == NULL || (str = BADDR(bstring)) == NULL)
		return -1;

	// Return the length of the BCPL string (first byte is length)
	return str[0];
}


//--------------------------------------------------------------------------------
//	Converts a BCPL string to a C string. 
//	Returns TRUE if successful, otherwise FALSE.
//--------------------------------------------------------------------------------
size_t bstr2cstr(BSTR bstring, char* buffer, size_t bufsize)
{
	char* str;
	size_t len;

	// If the BCPL string pointer is NULL or the pointer conversion to C string
	// failed, return FALSE
	if (bstring == NULL || (str = BADDR(bstring)) == NULL)
		return (size_t) -1;

	// Get the length of the BCPL string (first byte is length)
	len = str[0];

	// Make sure it will fit into the provided buffer
	if (len >= bufsize)
		len = bufsize - 1;

	// Copy the string to the buffer and null terminate it
	memcpy(buffer, str+1, len);
	buffer[len] = '\0';

	// Conversion successful
	return len;
}
