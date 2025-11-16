# c-shell

The goal of this project was to gain practical experience with process creation, command execution, and user input handling in C at a lower level, without relying on high-level abstractions.

## Features

- Minimal interactive shell loop with a custom prompt
- Retrieval and display of the current working directory
- Parsing of user input into command + arguments
- Execution of external programs using `fork()` + `execvp()`
- Support for built-in commands:
  - `cd`
  - `pwd`
- Clean termination with a farewell message
- Basic error handling for failed system calls

## Known Issues

### **1. Command parsing limitations**

- Commands are split **only by a single space** using `strtok(line, " ")`.
- This causes:
  - Incorrect handling of multiple consecutive spaces
  - No support for quoted strings
  - No support for tabs
  - No correct treatment of escaped characters
- Example problems:
  - `echo     hello` collapses spaces
  - `echo "hello world"` becomes two tokens

### **2. `parse_command()` internal logic is fragile**

- `buffer = tokens;` does nothing useful and is likely an error.
- The allocated `tokens` array size (`MAX_ARGS`) is fixed and may overflow if the user enters too many arguments.
- No validation of return values from `strtok()` beyond the initial NULL check.

### **3. No signal handling**

- Pressing `Ctrl+C` or `Ctrl+Z` may terminate the shell instead of interrupting the foreground program.
- Programs launched by the shell may not behave like expected in a typical Unix shell.

## Future Improvements

### **1. Improve the command parser**

- Support:
  - Quoted strings
  - Escaped characters (`\ `, `\"`, etc.)
  - Tabs as separators
  - Multiple consecutive spaces
- Consider writing a tokenizer from scratch or using `getline()` + manual parsing.

### **2. Add advanced shell features**

- Command history
- Command aliases
- Environment variable expansion (`$HOME`, `$PATH`, etc.)
- Path resolution for executables
- Output/input redirection (`>`, `<`, `>>`)
- Pipelines (`|`)
- Shell scripts (`.sh` execution)
- Background processes (`&`)
- Job control (suspend, resume, list jobs)

### **3. Add signal handling**

- Ignore or handle `SIGINT`, `SIGTSTP`, etc.
- Use custom handlers to prevent the shell from closing unintentionally.

### **4. Improve error handling**

- Print more descriptive errors using `perror()`.
- Return useful exit codes for built-ins.
