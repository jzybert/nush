# nush
**Northeastern University shell**

This project was our first challenge assignment in CS 3650 Computer Systems at Northeastern University. 

## Goals
Create a Unix shell that supported:
	* Programs to execute
	* Built-in commands
	* Command line arguments to programs / builtins
	* Operators

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
	* Redirect input: sort < foo.txt
	* Redirect output: sort foo.txt > output.txt
	* Pipe: sort foo.txt | uniq
	* Background: sleep 10 &
	* AND: true && echo one
	* OR: false || echo one
	* Semicolon: echo one; echo two