# ShowProc

`ShowProc` displays process information for any currently running 
Shell/CLI processes. It is intended to be a replacement for the 
AmigaOS `status` command.

## Improvements over `status`

* Uses a table view for better readability
* Displays full information by default with options to show less
  detail
* The `COMMAND` argument supports the use of standard AmigaOS 
  wildcards. For example, `Workb#?` will match the Workbench process.
* Displays additional information including:
  - How much of the stack is in use
  - Failat level
  - Last return code
  - Whether or not it is a background process

## Requirements

* AmigaOS 2.04 (V37) or above (m68k only)
* Shell/CLI that is at least 80 characters in width to display
the full information

Tested configurations:

* AmigaOS 3.2.3 (V47)
* AmigaOS 2.04 (V37)

## Installation Instructions

Copy the `ShowProc` executable to the `SYS:C` drawer and the 
`ShowProc.help` file to the `HELP:English/Sys/Commands` drawer.

## License

The app and source code are free to use under the terms of the MIT
License. See the LICENSE.txt file for details.

## Source Code

The SAS/C 6.58 source code for the application is available on 
GitHub at https://github.com/deeveon/ShowProc.

## Version History

| Version  | Description
|----------|-----------------
| 37.1	   | Initial release
