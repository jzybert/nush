# nush
**Northeastern University shell**

This project was our first challenge assignment in CS 3650 Computer Systems at Northeastern University. 

## Goals
Create a Unix shell that supported:
1. Programs to execute
2. Built-in commands
3. Command line arguments to programs / builtins
4. Operators

### Programs to Execute
```
-bash-4.2$ ./nush
nush$ head -n 2 /usr/share/dict/words
A
A's
nush$
```

### Built-in Commands
```
nush$ pwd
/home/jzybert/cs3650/challenge01
nush$ cd tests
nush$ pwd
/home/jzybert/cs3650/challenge01/tests
nush$ exit
-bash-4.2$
```

### Command Line Args
```
-bash-4.2$ ./nush tests/01-echo-one.sh
one
-bash-4.2$
```

### Operators
```
nush$ sort tests/sample.txt | tail -n 3
remorselessly
revolutionized
underplays
nush$
```

In total, nush supports seven operators:
1. Redirect input: sort < foo.txt
2. Redirect output: sort foo.txt > output.txt
3. Pipe: sort foo.txt | uniq
4. Background: sleep 10 &
5. AND: true && echo one
6. OR: false || echo one
7. Semicolon: echo one; echo two

## How to run:
1. Clone this reposotory locally
2. Via command line, go to the repository
3. Type make and hit enter
	* Type ./nush <command-line-args>
	* Type ./nush and enter commands