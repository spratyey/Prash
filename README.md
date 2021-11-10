# prash

`prash` stands for 'Pratyay's Shell' and is a bash-like UNIX shell written in C.

## How To Use
**Compilation + Execution: Run the following command in the shell directory**
```
make run
```

**Compilation Only : Run the following command in the shell directory**
```
make
```

**To run from any directory**
```
<path_to_shel_dir/prash>
```

---

## Custom Functions
1. **echo**
2. **ls**
3. **cd**
4. **pwd**
5. **pinfo**
6. **repeat**
7. **fg**
8. **bg**
9. **sig**
10. **jobs**
11. `Ctrl + D` logs the user out of prash.
12. `Ctrl + C` interrupts current foreground job
13. `Ctrl + Z` pushed foreground job to background and changes state to 'stopped'.
14. Piping and redirection support.
---

## Assumptions
- Path length does not exceed 4096 characters
- Command length does not exceed 8192 characters
- As clarified, only valid input involving pipes and redirection will be given (no two '>' in a single command, input and output redirection at only the first and last pipes respectively, etc.)

---

## Files
1. **`bgproc.c , .h`** : To deal with background processes
2. **`cd.c, .h`** : To implement the `cd` command
3. **`ls.c, .h`** : To implement the `ls` command
4. **`pinfo.c, .h`** : To implement the `pinfo` command
5. **`commands.c, .h`** : To parse each individual command, and invoke appropriate custom/system commands.
6. **`pwdecho.c, .h`** : To implement the `pwd` and `echo` commands
7. **`shell.c, .h`** : To print the prompt, and get the user's input. Also handles the `repeat` command.
8. **`globalheader.h`** : Has all the necessary system header file inclusion statements, and also declares some global variables using the `extern` keyword, also has the `MAXPATH,MAXCMD` macros.
9. **pipes.h** : Handles redirection and piping, acts as an intermediate layer between the shell and command handler.

---
