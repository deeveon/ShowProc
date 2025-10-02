# ShowProc

ShowProc is a replacement for the AmigaOS "status" command in the SYS:c drawer.

## Improvements
* Much better readability
* Additional information including:
  * How much of the stack size is being used
  * Last return code
  * Failat level
  * Whether or not it is a background process
  * Defaults to full information display with an option to show less detail

## Current limitations
* No global vector info (seems from the DOS RKM it's basically obsolete anyway)
* No COMMAND option (yet)
* No Ctrl-C support

## Wish List
* COM=COMMAND option
* Ctrl-C handling for the app
* Ability to send a break to a process (e.g. ShowProc PROCESS=4 BREAK)
* Optimization
* Non-CLI/Shell based processes
