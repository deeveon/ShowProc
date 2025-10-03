# ShowProc

`ShowProc` is a replacement for the AmigaOS `status` command in the `SYS:C`
drawer that displays process information for all Shell/CLI processes.

## Improvements over `status`

* Uses a table view for better readability
* Displays full information by default with options to show less detail
* The `COMMAND` argument supports the use of standard AmigaOS wildcards.
For example, `Workb#?` will match the Workbench process.
* Displays additional information including:
  * How much of the stack is in use
  * Failat level
  * Last return code
  * Whether or not it is a background process

## AmigaOS Compatibility

* AmigaOS 1.3 (V34) or above (m68k only)
* Kickstart 2.04 (V37) or above

Tested configurations:

* AmigaOS 3.2.3 (V47) with Kickstart 3.2.3 ROM (V47)
* AmigaOS 2.04 (V37) with Kickstart 2.04 ROM (V37)
* AmigaOS 1.34 (V33) with Kickstart 2.04 ROM (V37)

## Installation Instructions

Copy the `ShowProc` executable to the `SYS:C` drawer and the `ShowProc.help` 
file to the `HELP:English/Sys/Commands` drawer.

Optionally, add an alias to the `S:Shell-startup` script to redirect
the AmigaOS `status` command to use `ShowProc` instead. For example:

	alias status ShowProc

## Source Code

The source code for the application is available on GitHub at 
https://github.com/deeveon/AmigaOS in the `ShowProc` folder. 

SAS/C 6.58 was used during development and there probably are some SAS/C 
exclusive functions used.

## License

The app and source code are free to use under the terms of the MIT License
located in the AmigaOS (root) folder.

## Version History

| Version  | Description
|----------|-----------------
| 37.1	   | Initial version
