# ShowProc

`ShowProc` displays process information for all currently running 
tasks & processes. It is intended to be a replacement for the 
AmigaOS `status` command.

## Improvements over `status`

* Displays information about all tasks and process by default
  with options to display a Shell/CLI processes-specific table.
* Uses table views for better readability
* Displays full information by default with options to show less
  detail
* The `COMMAND` argument supports the use of standard AmigaOS 
  wildcards. For example, `Workb#?` will match the Workbench process.
* Displays additional information including:
  - How much of the stack is in use
  - Failat level
  - Last return code
  - Whether or not it is a background process
* Help file for integration into the AmigaOS 3.2 Help System (i.e.
  type `help ShowProc` in a Shell/CLI to bring up the help)

## Requirements

* AmigaOS/Kickstart 2.04 (V37) or above (m68k only)
* In order to display the full information properly, a Shell/CLI
  window that is at least 80 characters in width is needed.

Tested configurations:

* AmigaOS 3.2.3 (V47)
* AmigaOS 3.1 (V40)
* AmigaOS 2.04 (V37)

## Installation Instructions

1. Copy the `ShowProc` executable to the `SYS:C` drawer
2. Copy the `ShowProc.help` file to the `HELP:English/Sys/Commands`
   drawer (AmigaOS 3.2)

## License

The app and source code are free to use under the terms of the MIT
License. See the LICENSE.txt file for details.

## Source Code

The SAS/C 6.58 source code for the application is available on 
GitHub at https://github.com/deeveon/ShowProc.

## Version History

| Version | Description                                             |
|---------|---------------------------------------------------------|
| 37.1    | Initial release
| 37.2    | Added ability to display all tasks and processes, which is now the default view. It also now checks the version of Workbench/Kickstart at startup and exits gracefully if < 2.04.
| |
