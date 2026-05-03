# Basic Shell

A simple command-line shell implementation written in C. This shell supports basic file system operations and can run in interactive mode or process commands from a file.

## Compatibility

This shell is designed for Unix-like operating systems, including Linux and macOS. It uses POSIX system calls and may not work on Windows without additional tools to ensure compatibility.

## Build Instructions

To build the shell, ensure you have a C compiler (e.g., GCC) installed. Run the following command in the project directory:

```
make
```

This will compile the source files and create an executable named `pseudo-shell`.

To clean up the build artifacts, run:

```
make clean
```

## Usage Instructions

### Interactive Mode

Run the shell in interactive mode by executing:

```
./pseudo-shell
```

You will see a prompt `>>> ` where you can enter commands. Type `exit` to quit the shell.

### File Mode

To process commands from a file, use:

```
./pseudo-shell -f <filename>
```

In file mode, the shell reads commands from the specified file and writes output to `output.txt`.

### Supported Commands

The shell supports the following basic commands:

- `ls` - List directory contents (no arguments)
- `pwd` - Print working directory (no arguments)
- `mkdir <directory>` - Create a directory
- `cd <directory>` - Change directory
- `cp <source> <destination>` - Copy a file
- `mv <source> <destination>` - Move or rename a file
- `rm <file>` - Remove a file
- `cat <file>` - Display file contents
- `exit` - Exit the shell

Note: Commands have limited parameter support as implemented in this basic version.