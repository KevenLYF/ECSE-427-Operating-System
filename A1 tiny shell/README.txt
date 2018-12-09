Instructions for running tiny_shell.c

General tips:
1. Type exit to exit the tshell
2. This tshell handles several errors that I can think of, that is, ideally it won't crash. And it always prints the error message and command usage.
3. The tshell can take input both from user (terminal) and input file.

By using makefile

    test system()

        > make all
        > ./tshell
            or
        > ./tshell input.txt
    
    test fork() version

        > make fork
        > ./tshell
            or
        > ./tshell input.txt

    test vfork() version

        > make vfork
        > ./tshell
            or
        > ./tshell input.txt

    test clone() version

        > make clone
        > ./tshell
            or
        > ./tshell input.txt

        note: one can also test cd by 
        > cd [path]
        and do "pwd" to check current directory

    test pipe() version

        > make pipe

        In first shell instance

        > ./tshell myfifo W
        > ls

        this is to write output of the "ls" command to myfifo

        In second shell instance

        > ./tshell myfifo R
        > wc

        this is to read from myfifo instead of stdin
        this will return the result of "wc" to terminal
