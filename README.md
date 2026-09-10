# Custom Unix Shell

A lightweight Unix-like command-line shell implemented in **C** as part of the Operating Systems coursework at **IIT Gandhinagar**.

The project focuses on fundamental operating-system concepts including **process creation, program execution, process synchronization, signal handling, terminal control, and child-process management**.

## Features

* Built-in commands:

  * `cd` — Change the current working directory
  * `pwd` — Display the current working directory
  * `echo` — Print arguments to the terminal
  * `kill` — Send `SIGTERM` to a specified process
  * `exit` — Safely terminate the shell

* External command execution using:

  * `fork()`
  * `execvp()`
  * `waitpid()`

* Process management and cleanup using:

  * `kill()`
  * `waitpid()`

* Signal handling using `sigaction`

* Terminal configuration and restoration using `termios`

* Ctrl+Q-based shell termination

* Command parsing with support for command-line arguments

## Operating System Concepts

### 1. Process Creation

External commands are executed by creating a child process using `fork()`.

The child process then replaces its execution image with the requested program using `execvp()`.

```text
Shell Process
      |
    fork()
   /      \
Parent    Child
  |         |
waitpid()  execvp()
             |
       External Program
```

### 2. Process Synchronization

The shell uses `waitpid()` to wait for the active child process to terminate before continuing with the next command.

This ensures that the shell maintains control over its active child process and properly synchronizes process execution.

### 3. Signal Handling

The shell installs a signal handler using `sigaction()` and uses terminal configuration through `termios` to support **Ctrl+Q-based termination**.

Before exiting, the shell restores the original terminal configuration.

### 4. Child Process Cleanup

The shell tracks the currently active child process and performs explicit cleanup during termination.

The active child is terminated using `kill()` and subsequently reaped using `waitpid()` to prevent it from remaining active after the shell exits.

## Supported Commands

| Command | Description             | Example            |
| ------- | ----------------------- | ------------------ |
| `pwd`   | Print current directory | `pwd`              |
| `cd`    | Change directory        | `cd /tmp`          |
| `echo`  | Print text              | `echo Hello World` |
| `kill`  | Terminate a process     | `kill 1234`        |
| `exit`  | Exit the shell          | `exit`             |

The shell also supports execution of external programs available through the system's `PATH`.

Examples:

```bash
ls
date
whoami
cat file.txt
```

## Build and Run

### Requirements

* Linux or another POSIX-compatible environment
* GCC
* Standard POSIX system libraries

### Compile

```bash
gcc shell.c -o shell
```

### Run

```bash
./shell
```

## Example

```text
24110054:/home/user/custom-unix-shell$ pwd
/home/user/custom-unix-shell

24110054:/home/user/custom-unix-shell$ echo Hello World
Hello World

24110054:/home/user/custom-unix-shell$ cd ..
24110054:/home/user$ pwd
/home/user

24110054:/home/user$ ls
custom-unix-shell
```

Press **Ctrl+Q** to terminate the shell.

## Project Structure

```text
custom-unix-shell/
├── shell.c
├── README.md
└── .gitignore
```

## Technologies

* **C**
* **Linux / POSIX**
* **GCC**
* **Unix System Calls**
* **Process Management**
* **Signal Handling**
* **Terminal Control**

## Limitations

This project intentionally implements a lightweight shell rather than a full-featured shell such as Bash.

It currently does not implement features such as:

* Pipes (`|`)
* Input/output redirection (`>`, `<`)
* Background execution (`&`)
* Job control
* Shell scripting

## Learning Outcomes

This project provided practical experience with:

* Unix process creation and execution
* Parent-child process relationships
* Process synchronization
* POSIX signals
* Terminal configuration
* System calls and low-level C programming
* Safe process cleanup
* Command parsing and execution

---

## Author

**Aryan Meshram**
B.Tech-M.Tech, Computer Science & Engineering
IIT Gandhinagar
