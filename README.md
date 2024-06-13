# My Custom Shell

## Overview

My Custom Shell is a simple command-line interpreter implemented in C. It supports basic Linux commands such as `ls`, `cat`, and `sleep`. Additionally, it includes a few built-in commands like `history`, `cd`, `pwd`, and `exit`, implemented without relying on their Unix defaults. This shell handles command execution in the foreground using `fork()` and `exec()`, and supports custom paths specified at launch.

## Features

- **Custom Command Paths**: Accepts any number of command-line arguments representing paths that may contain executable commands.
- **Command Execution**: Executes basic Unix commands using `fork()` and `exec()`.
- **Built-In Commands**:
  - `history`: Displays the list of commands entered during the session.
  - `cd`: Changes the current directory.
  - `pwd`: Prints the current working directory.
  - `exit`: Exits the shell.
- **Foreground Execution**: All commands run in the foreground, with the parent process waiting for the child process to complete.
- **Error Handling**: Appropriately handles errors and prints relevant messages.

## Usage

To use My Custom Shell, compile the source code and run the executable. The shell will prompt for commands, which you can enter interactively.

### Compilation
`gcc -o myshell myshell.c`

## Running the Shell
When running the shell, you can specify custom paths as arguments. These paths will be searched for executable commands.
`./myshell /path/to/custom/commands /another/path`


## Example Usage
`$ ./myshell /usr/local/bin /home/user/bin`

`$ pwd`

/home/user

`$ ls`

file1.txt file2.txt

`$ cd /home/user/Documents`

`$ pwd`

/home/user/Documents

`$ history`

`pwd`

`ls`

`cd /home/user/Documents`

`pwd`

`$ exit`



