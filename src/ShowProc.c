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
#include <proto/wb.h>

#include "ShowProc_rev.h"
#include "ShowProc.h"

// Embed version tag into binary
const char* version = VERSTAG;

//--------------------------------------------------------------------------------
// Function prototypes
//--------------------------------------------------------------------------------
int 	ParseCommandLineArgs(Mode* mode, OutFrmt* format, int* start, int* finish, char* cmd_pat);
BOOL 	SanitizeCommandName(char* cleanName, const char* dirtyName);
int 	PrintShellProcesses(Mode mode, OutFrmt format, int start, int finish, char* cmd_pat);
int 	PrintThisProcess(OutFrmt format, int* taskCount);
int 	PrintTaskList(OutFrmt format, struct List* taskList, int* taskCount);
BOOL 	CheckCommandMatch(const BSTR bstring, const char* cmd_pat);
char* 	GetStateName(UBYTE state);
BOOL 	CheckRequirements(void);
BYTE 	bstrlen(BSTR bstring);
size_t 	bstr2cstr(BSTR bstring, char* buffer, size_t bufsize);


//--------------------------------------------------------------------------------
//	main()
//--------------------------------------------------------------------------------
int main(void)
{
	Mode 	mode = MODE_SYSTEM;				// Default to system tasks/processes only
	OutFrmt format = FORMAT_VERBOSE;		// Default to verbose output
	int		start = 1;						// Process number to start with
	int		finish = -1;					// Process number to finish with (set later)
 	char	cmd_pat[MAX_CMD_NAME_LEN + 1];	// Command pattern for COMMAND argument
	BYTE	prev_program_pri = 0;			// Program priority before we change it
	int		taskCount = 1;					// Number of tasks found
	int		rc;

	// Check minimum Kickstart & AmigaOS version requirements
	if (CheckRequirements() == FALSE)
		return RETURN_FAIL;

	// Set the program name for our own process
	SetProgramName(PROGRAM);

	// Parse command line arguments
	rc = ParseCommandLineArgs(&mode, &format, &start, &finish, cmd_pat);
	if (rc != RETURN_OK)
		goto exit;

	// Capture current program priority & set the priority a bit higher to reduce
	// the risk of changes occurring while reading task/process & CLI info
	prev_program_pri = SetTaskPri(FindTask(NULL), PROGRAM_PRIORITY);

	// Print out Shell/CLI processes
	if (mode == MODE_ALL || mode == MODE_CLI)
	{
		rc = PrintShellProcesses(mode, format, start, finish, cmd_pat);
		if (rc != RETURN_OK)
			goto exit;
	}

	// Print out system tasks/processes
	if (mode == MODE_ALL || mode == MODE_SYSTEM)
	{
		// Only print section header if we're showing both system & CLI processes
		// and format is not COMMAND
		if (mode == MODE_ALL && format != FORMAT_COMMAND)
			Printf("\n%s\n", STR_SYS_HEADING);

		// Print header based on output format
		switch (format) {
			case FORMAT_VERBOSE:
				Printf(SYS_VERBOSE, SYS_VERBOSE_TOP);
				Printf(SYS_VERBOSE, SYS_VERBOSE_BOT);
				Printf(SYS_VERBOSE, SYS_VERBOSE_DIV);
				break;
			case FORMAT_TCB:
				Printf(SYS_TCB, SYS_TCB_TOP);
				Printf(SYS_TCB, SYS_TCB_BOT);
				Printf(SYS_TCB, SYS_TCB_DIV);
				break;
			case FORMAT_SHORT:
				Printf(SYS_SHORT, SYS_SHORT_TOP);
				Printf(SYS_SHORT, SYS_SHORT_BOT);
				Printf(SYS_SHORT, SYS_SHORT_DIV);
				break;
			default:
				// Shouldn't get here so fail if we do
				Printf("%s\n", STR_INV_TASK_FMT);
				return RETURN_FAIL;
				break;
		}

		rc = PrintThisProcess(format, &taskCount);
		if (rc != RETURN_OK)
			goto exit;

		rc = PrintTaskList(format, &SysBase->TaskReady, &taskCount);
		if (rc != RETURN_OK)
			goto exit;

		rc = PrintTaskList(format, &SysBase->TaskWait, &taskCount);
		if (rc != RETURN_OK)
			goto exit;
	}

exit:
	// Restore previous program priority
	SetTaskPri(FindTask(NULL), prev_program_pri);

	return rc;
}


//--------------------------------------------------------------------------------
//	Parses command line arguments
//--------------------------------------------------------------------------------
int ParseCommandLineArgs(Mode* mode, OutFrmt* format, int* start, int* finish, char* cmd_pat)
{
	struct 	RDArgs*	rdargs;
 	long	opts[OPT_COUNT] = {0};
	int		rc = RETURN_OK;

	// Parse command line arguments
	rdargs = ReadArgs(TEMPLATE, opts, NULL);

	if (rdargs == NULL) {
		PrintFault(IoErr(), NULL);
		rc = RETURN_FAIL;
		goto cleanup;
	}

	// VERSION argument
	if (opts[OPT_VERSION]) {
		Printf("%s", VSTRING);
		rc = RETURN_WARN;  // Not an error, but we want to exit after showing version
		goto cleanup;
	}

	// Determine mode based on arguments received
	if (opts[OPT_CLI])		*mode = MODE_CLI;				// CLI processes only
	if (opts[OPT_SYS])		*mode = MODE_SYSTEM;			// System processes only
	if (opts[OPT_ALL])		*mode = MODE_ALL;				// All processes (default)

	// Determine output format based on arguments received
	if (opts[OPT_SHORT])	*format = FORMAT_SHORT;
	if (opts[OPT_TCB])		*format = FORMAT_TCB;			// Overrides SHORT and CLI/ALL
	if (opts[OPT_FULL])		*format = FORMAT_VERBOSE;		// Overrides SHORT, CLI/ALL and TCB

	// Handle the PROCESS argument
	if (opts[OPT_PROCESS]) {
		*mode = MODE_CLI;									// PROCESS only applies to CLI processes
		*start = *finish = *((long*)opts[OPT_PROCESS]);
		if (*start < 1 || *start > 999) {
			Printf("%s\n", STR_INV_PROC_NUM);
			rc = RETURN_FAIL;
			goto cleanup;
		}
	}

	// If the user didn't specify a specific process, set finish to the max number of
	// CLIs, but limit to 999 to match the max width of the process number field
	if (*start != *finish)
		*finish = MaxCli() > 1000 ? 999 : MaxCli() - 1;  	// -1 because last one is NULL

	// If COMMAND argument is given, override mode & format
	if (opts[OPT_COMMAND]) {
		*mode = MODE_CLI;									// Command search only applies to CLI processes
		*format = FORMAT_COMMAND;							// Overrides all other formats
		// *cmd_pat = (char*)opts[OPT_COMMAND];
		if (!SanitizeCommandName(cmd_pat, (char*)opts[OPT_COMMAND])) {
			rc = RETURN_FAIL;
			goto cleanup;
		}
		*start = 1;
		*finish = MaxCli() > 1000 ? 999 : MaxCli() - 1;  	// Search all CLIs
	}

cleanup:

	if (rdargs)
		FreeArgs(rdargs);

	return rc;
}


//--------------------------------------------------------------------------------
// Sanitizes/validates the specified command name
// Returns TRUE if the name is valid after sanitization, FALSE otherwise.
//--------------------------------------------------------------------------------
BOOL SanitizeCommandName(char* cleanName, const char* dirtyName)
{
	// Validate parameters
	if (dirtyName == NULL || strlen(dirtyName) == 0 || strlen(dirtyName) > MAX_CMD_NAME_LEN) {
		Printf("%s\n", STR_ERR_INV_CMD_NAME);
		return FALSE;
	}

	if (cleanName == NULL) {
		Printf("%s: %s\n", STR_ERR_INV_POINTER, "cleanName");
		return FALSE;
	}

	// Copy the dirty name to the clean name buffer
	strncpy(cleanName, dirtyName, MAX_CMD_NAME_LEN);
	cleanName[MAX_CMD_NAME_LEN] = '\0';				// Ensure null termination

	// ReadArgs already filters out invalid characters, so we don't need to do
	// that here. Instead, we can just check for a trailing colon & remove it.
	if (cleanName[strlen(cleanName) - 1] == ':') {
		cleanName[strlen(cleanName) - 1] = '\0';
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
//	Prints information about Shell/CLI processes
//-----------------------------------------------------------------------------
int PrintShellProcesses(Mode mode, OutFrmt format, int start, int finish, char* cmd_pat)
{
	struct 	Process* process;
	struct 	CommandLineInterface* cli;
	long 	num;
	int		rc = RETURN_OK;

	// Only print section header if we're showing both system & CLI processes
	// and format is not COMMAND
	if (mode == MODE_ALL && format != FORMAT_COMMAND)
		Printf("\n%s\n", STR_CLI_HEADING);

	// Print header based on output format
	switch (format) {
		case FORMAT_VERBOSE:
			Printf(CLI_VERBOSE, CLI_VERBOSE_TOP);
			Printf(CLI_VERBOSE, CLI_VERBOSE_BOT);
			Printf(CLI_VERBOSE, CLI_VERBOSE_DIV);
			break;
		case FORMAT_TCB:
			Printf(CLI_TCB, CLI_TCB_TOP);
			Printf(CLI_TCB, CLI_TCB_BOT);
			Printf(CLI_TCB, CLI_TCB_DIV);
			break;
		case FORMAT_SHORT:
			Printf(CLI_SHORT, CLI_SHORT_TOP);
			Printf(CLI_SHORT, CLI_SHORT_BOT);
			Printf(CLI_SHORT, CLI_SHORT_DIV);
			break;
		case FORMAT_COMMAND:
			// No header for command mode
			rc = RETURN_WARN;  	// Assume we won't find it
			break;
		default:
			// Shouldn't get here so fail if we do
			rc = RETURN_FAIL;
			goto end;
			break;
	}

	// Per the Amiga DOS library docs, FindCliProc() is normally used with Forbid()
	Forbid();
	{
		// Loop through all the CLIs
		for (num = start; num <= finish; num++)
		{
			// Process struct has the stack size & pointer to the CLI struct
			process = FindCliProc(num);

			if (process == NULL) {
				// If the user requested a specific process number, show an error
				// message. If we're scanning all processes, just skip it.
				if (start == finish)
					Printf(" %3.3ld %s\n", num, STR_NO_PROCESS);
				continue;	// Go to next process
			}

			// Convert BPTR to CommandLineInterface pointer
			cli = (struct CommandLineInterface*) BADDR((*process).pr_CLI);

			if (cli == NULL) {
				Printf(" %s\n", STR_ERR_GET_CLI);
				continue;	// Go to next process
			}

			// If in COMMAND mode, check if the command name matches the user-supplied pattern
			if (format == FORMAT_COMMAND)
			{
				if (CheckCommandMatch(cli->cli_CommandName, cmd_pat) == TRUE) {
					// Match found, print only the CLI number 
					// to match the output of STATUS and stop
					Printf("%2ld\n", num);
					rc = RETURN_OK;
					break;	// Exit the for loop
				}
				// No match
				rc = RETURN_WARN;
				continue;	// Go to next process
			}

			// CLI number
			Printf(" %3.3ld", num);

			// Command name if not TCB format
			if (format != FORMAT_TCB) {
				if (bstrlen(cli->cli_CommandName) > 0)
					Printf(" %-33.33b",	cli->cli_CommandName);		// %b prints BSTR strings (AmigaOS only)
				else
					Printf(" %-33.33s",	STR_NO_COMMAND);
			}

			// Print the rest of the process details if not SHORT format
			if (format != FORMAT_SHORT)
			{
				// Priority
				Printf(" %4.4ld", process->pr_Task.tc_Node.ln_Pri);
				// Global vector
				Printf(" %4.4ld", process->pr_GlobVec ? *(long*)process->pr_GlobVec : 0);
				// Stack usage
				Printf(" %6.6ld", (long)process->pr_Task.tc_SPUpper - (long)process->pr_Task.tc_SPReg);
				// Total stack size
				Printf(" %6.6ld", (long)process->pr_Task.tc_SPUpper - (long)process->pr_Task.tc_SPLower);
				// Failat level
				Printf(" %4.4ld", cli->cli_FailLevel);
				// Last return code
				Printf(" %3.3ld", cli->cli_ReturnCode);
				// Background process?
				Printf(" %4.3s", cli->cli_Background  ? STR_YES : STR_NO);
			}

			// End of the line
			Printf("\n");

			// Check for Ctrl-C break
			if (CheckSignal(SIGBREAKF_CTRL_C)) {
                PrintFault(ERROR_BREAK, NULL);
                break;	// Exit the for loop
			}
		}

	} // End Forbid() section
	Permit();

end:
	return rc;
}


//--------------------------------------------------------------------------------
//	Prints information about the current process.
//--------------------------------------------------------------------------------
int PrintThisProcess(OutFrmt format, int* taskCount)
{
	struct 	Process* process;
	struct 	CommandLineInterface* cli;
	int		rc = RETURN_OK;

	// Get our own process structure
	process = (struct Process*)FindTask(NULL);
	if (process == NULL) {
		Printf("%s\n", STR_ERR_GET_OWN_PROC);
		return RETURN_FAIL;
	}

	// Convert BPTR to CommandLineInterface pointer
	cli = (struct CommandLineInterface*) BADDR((*process).pr_CLI);
	if (cli == NULL) {
		Printf("%s\n", STR_ERR_GET_CLI);
		return RETURN_FAIL;
	}

	// Task/Process counter
	Printf(" %3.3ld", (*taskCount)++);

	// Print header based on output format
	if (format != FORMAT_TCB) {
		if (bstrlen(cli->cli_CommandName) > 0) 
			Printf(" %-35.35b",	cli->cli_CommandName);
		else
			Printf(" %-35.35s", process->pr_Task.tc_Node.ln_Name);
	}

	// Print the rest of the process details if not SHORT mode
	if (format != FORMAT_SHORT) 
	{
		// Priority
		Printf(" %4.4ld", process->pr_Task.tc_Node.ln_Pri);
		// Type
		Printf("  %-2.2s", "P");
		// CLI number (unless 0 which means not a CLI process)
		Printf(" %3.3ld", process->pr_TaskNum);
		// Current state
		Printf(" %5.5s", GetStateName(process->pr_Task.tc_State));
		// Used stack size
		Printf(" %6.6ld", (long)process->pr_Task.tc_SPUpper - (long)process->pr_Task.tc_SPReg);
		// Total stack size
		Printf(" %6.6ld", (long)process->pr_Task.tc_SPUpper - (long)process->pr_Task.tc_SPLower);
	}

	// End of the line
	Printf("\n");

	return rc;
}	


//--------------------------------------------------------------------------------
//	Prints information about all tasks & processes in the given task list.
//--------------------------------------------------------------------------------
int PrintTaskList(OutFrmt format, struct List* taskList, int* taskCount)
{
	struct 	Node* node;
	struct 	Task* task;
	struct 	Process* process;
	struct 	CommandLineInterface* cli;
	int 	rc = RETURN_OK;

	if (taskList == NULL) {
		Printf("%s\n", STR_INV_TASK_LIST);
		return RETURN_FAIL;
	}

	Forbid();
	{
		node = taskList->lh_Head;

		// Traverse the task list
		while (node->ln_Succ != NULL)
		{
			// Task count provides a running count of how many tasks/processes there are
			Printf(" %3.3ld", (*taskCount)++);

			// Determine what type of node this is (i.e. task or process)
			switch (node->ln_Type)
			{
				case NT_TASK:
					// Cast the Node pointer to a Task pointer
					task = (struct Task*)node;

					// Print task name if not TCB mode
					if (format != FORMAT_TCB)
						Printf(" %-35.35s", task->tc_Node.ln_Name);

					// Print the rest of the task details if not SHORT mode
					if (format != FORMAT_SHORT) 
					{
						// Priority
						Printf(" %4.4ld", task->tc_Node.ln_Pri);
						// Type
						Printf("  %-2.2s", "T");
						// CLI number (Tasks never have a CLI number)
						Printf(" %3.3s", "");
						// Current state
						Printf(" %5.5s", GetStateName(task->tc_State));
						// Stack usage
						Printf(" %6.6ld", (long)task->tc_SPUpper - (long)task->tc_SPReg);
						// Total stack size
						Printf(" %6.6ld", (long)task->tc_SPUpper - (long)task->tc_SPLower);
					}
					break;

				case NT_PROCESS:
					
					process = (struct Process*)node;

					// Print process name or command name if not TCB mode
					if (format != FORMAT_TCB)
					{
						// TaskNum is 0 if not a CLI process
						if (process->pr_TaskNum == 0) 
							Printf(" %-35.35s", process->pr_Task.tc_Node.ln_Name);
						else
						{
							cli = (struct CommandLineInterface*) BADDR((*process).pr_CLI);
							if (cli == NULL) {
								Printf(" %s\n", STR_ERR_GET_CLI);
								rc = RETURN_FAIL;
								break;	// Exit the switch statement
							}

							// Print the command name if it exists, otherwise print the task name
							if (bstrlen(cli->cli_CommandName) > 0) 
								Printf(" %-35.35b",	cli->cli_CommandName);
							else
								Printf(" %-35.35s", process->pr_Task.tc_Node.ln_Name);
						}
					}

					if (format != FORMAT_SHORT) 
					{
						// Priority
						Printf(" %4.4ld", process->pr_Task.tc_Node.ln_Pri);
						// Type
						Printf("  %-2.2s", "P");

						// CLI number (unless 0 which means not a CLI process)
						if (process->pr_TaskNum != 0)
							Printf(" %3.3ld", process->pr_TaskNum);
						else
							Printf(" %3.3s", "");

						// Current state
						Printf(" %5.5s", GetStateName(process->pr_Task.tc_State));
						// Stack usage
						Printf(" %6.6ld", (long)process->pr_Task.tc_SPUpper - (long)process->pr_Task.tc_SPReg);
						// Total stack size
						Printf(" %6.6ld", (long)process->pr_Task.tc_SPUpper - (long)process->pr_Task.tc_SPLower);
					}
					break;

				default:
					Printf("%s\n", STR_INV_TASK_TYPE);
					// rc = RETURN_FAIL;
					break;
			}

			// End of the line
			Printf("\n");

			// Move to next task
			node = node->ln_Succ;

			// Check for Ctrl-C break
			if (CheckSignal(SIGBREAKF_CTRL_C)) {
                PrintFault(ERROR_BREAK, NULL);
                break;	// Exit the for loop
			}
		}
	} // End Forbid() section
	Permit();

	return rc;
}


//--------------------------------------------------------------------------------
// Checks if the given command pattern matches the command name.
// Returns TRUE if it matches, FALSE if it doesn't or if there's an error.
//--------------------------------------------------------------------------------
BOOL CheckCommandMatch(const BSTR cmd_name, const char* cmd_pat)
{
	// ParsePatternNoCase() docs say to make pattern buffer at least double the
	// length of the string to match + 2
	char	pattern[258] = {0};
	char 	cmd[128] = {0};
	long	result;

	// Validate parameters
	if (cmd_name == NULL || cmd_pat == NULL || bstrlen(cmd_name) <= 0 || strlen(cmd_pat) <= 0)
		return FALSE;

	// Convert the command name from a BSTR to a C string
	if (bstr2cstr(cmd_name, cmd, sizeof(cmd)) < 0) {
		Printf(" %s\n", STR_ERR_GET_CMD);
		return FALSE;
	}

	// Check if the user-supplied command string contains any wildcards
	result = ParsePatternNoCase(cmd_pat, pattern, sizeof(pattern));

	// Invalid pattern
	if (result == -1) {
		Printf("%s\n", STR_INV_CMD_PAT);
		return FALSE;
	}

	// Wildcards found
	if (result == 1)
		return MatchPatternNoCase(pattern, cmd);
	
	// No wildcards, do a simple string compare
	if (result == 0)
		if (stricmp(cmd, cmd_pat) == 0)
			return TRUE;

	// No match was found
	return FALSE;
}


//--------------------------------------------------------------------------------
//	Returns a string representation of the task/process state.
//--------------------------------------------------------------------------------
char* GetStateName(UBYTE state)
{
	switch (state) {
		case TS_INVALID:
			return STR_STATE_INVALID;
		case TS_ADDED:
			return STR_STATE_ADDED;
		case TS_RUN:
			return STR_STATE_RUN;
		case TS_READY:
			return STR_STATE_READY;
		case TS_WAIT:
			return STR_STATE_WAIT;
		case TS_EXCEPT:
			return STR_STATE_EXCEPT;
		case TS_REMOVED:
			return STR_STATE_REMOVED;
		default:
			return STR_STATE_UNDEFINED;
	}
}


//--------------------------------------------------------------------------------
//	Checks that the Kickstart and Workbench versions meet the minimum requirements.
//	Returns TRUE if requirements are met, FALSE otherwise.
//--------------------------------------------------------------------------------
BOOL CheckRequirements(void)
{
	// Check Kickstart version
	if (SysBase->LibNode.lib_Version < KICKSTART_MIN_VER) {
		Printf("%s\n", STR_KS_TOO_OLD);
		return FALSE;
	}

	// Check Workbench version
	if (WorkbenchBase->lib_Version < OS_MIN_VER) {
		Printf("%s\n", STR_OS_TOO_OLD);
		return FALSE;
	}

	return TRUE;
}


//--------------------------------------------------------------------------------
//	Returns the length of a BCPL string or -1 on error.
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
//	Returns the length of the converted string or -1 on error.
//--------------------------------------------------------------------------------
size_t bstr2cstr(BSTR bstring, char* buffer, size_t bufsize)
{
	char* 	str;
	size_t 	len;

	// If the BCPL string pointer is NULL or the pointer
	// conversion failed, return -1
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

	// Return the length of the converted string
	return len;
}
