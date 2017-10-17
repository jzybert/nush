#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

#include "parse.h"
#include "vector.h"

void
check_rv(int rv)
{
	if (rv == -1) {
		perror("fail");
		exit(1);
	}
}

void
execute(vector* tokens);

void
cd(vector* cmd)
{
	char* dir = vector_get(cmd, 1);
	int ret;
	ret = chdir(dir);
	if (ret == -1) {
		printf("nush: cd: %s: No such file or directory", dir);
	}
}

void
executeSemi(vector* tokens)
{
	int ii;
	char left[256];
	char right[256];
	strcpy(left, "");
	strcpy(right, "");
	int seenSemi = 0;
	for (ii = 0; ii < tokens->size; ++ii) {
		char* v = vector_get(tokens, ii);
		if (strncmp(v, ";", 1) == 0) {
			seenSemi = 1;
		} else if (seenSemi) {
			strcat(right, v);
			strcat(right, " ");
		} else {
			strcat(left, v);
			strcat(left, " ");
		}
	}
	vector* leftTokens = shell_parse(left);
	execute(leftTokens);
	free_vector(leftTokens);
	vector* rightTokens = shell_parse(right);
	execute(rightTokens);
	free_vector(rightTokens);
}

void
executePipe(vector* tokens)
{
	int cpid1;

	if ((cpid1 = fork())) { // fork 1 parent
		int status;
		waitpid(cpid1, &status, 0);
	} else { // fork 1 child
		int rv;

        	int pipe_fds[2];
        	rv = pipe(pipe_fds);
        	check_rv(rv);

        	int p_read = pipe_fds[0];
        	int p_write = pipe_fds[1];
		
		int cpid2;
		if ((cpid2 = fork())) { // fork 2 parent
			int cpid3;
			if((cpid3 = fork())) { // fork 3 parent
				close(p_read);
				close(p_write);

				int status;
				waitpid(cpid2, &status, 0);
				waitpid(cpid3, &status, 0);
			} else { // fork 3 child
				close(p_write);
				close(0);
				
				rv = dup(p_read);
				check_rv(rv);
				close(p_read);
				int ii; // index of tokens
				int jj = 0; // index of args
				int seenPipe = 0;
				int indexOfPipe = 0;
				char* args[tokens->size];
				for (ii = 0; ii <= tokens->size; ++ii) {
					if (ii < tokens->size) {
						char* v = vector_get(tokens, ii);
						if (strncmp(v, "|\0", 2) == 0) {
							seenPipe = 1;
							indexOfPipe = ii;
						} else if (seenPipe) {
							args[jj] = v;
							jj++;
						}
					} else {
						args[jj] = 0;
					}
				}
				execvp(vector_get(tokens, indexOfPipe + 1), args);
			}
		} else { // fork 2 child
			close(p_read);
                	close(1);

                	rv = dup(p_write);
	               	check_rv(rv);
			close(p_write);
			int ii;
                	char* args[tokens->size + 1];
                	for (ii = 0; ii < tokens->size; ++ii) {
                               	char* v = vector_get(tokens, ii);
                               	if (strncmp(v, "|\0", 2) != 0) {
                                       	args[ii] = v;
                               	} else {
                                       	args[ii] = 0;
                                       	break;
                                }
                	}
			execvp(vector_get(tokens, 0), args);
		}
	}
}

void
executeLogic(vector* tokens, int isAND)
{
	char op[3];
	if (isAND) {
		strcpy(op, "&&");
	} else {
		strcpy(op, "||");
	}

	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		if ((!isAND && WEXITSTATUS(status)) || (isAND && !WEXITSTATUS(status))) {
			int ii; // index of tokens
			int jj = 0; // index of args
			int seenOp = 0;
			int indexOfOp = 0;
			char* args[tokens->size];
			for (ii = 0; ii <= tokens->size; ++ii) {
				if (ii < tokens->size) {
					char* v = vector_get(tokens, ii);
					if (strncmp(v, op, 2) == 0) {
						seenOp = 1;
						indexOfOp = ii;
					} else if (seenOp) {
						args[jj] = v;
						jj++;
					}
				} else {
					args[jj] = 0;
				}
			}
			if (strncmp(vector_get(tokens, indexOfOp + 1), "exit", 4) == 0) {
				exit(0);
			} else {
			execvp(vector_get(tokens, indexOfOp + 1), args);
			}
		}
	} else {
		int ii;
		char* args[tokens->size + 1];
		for (ii = 0; ii < tokens->size; ++ii) {
			char* v = vector_get(tokens, ii);
			if (strncmp(v, op, 2) != 0) {
				args[ii] = v;
			} else {
				args[ii] = 0;
				break;
			}
		}
		execvp(vector_get(tokens, 0), args);
	}
}

void
executeBackground(vector* tokens)
{
	int cpid;
        if ((cpid = fork())) {
                // don't wait
        } else {
                int ii;
                char* args[tokens->size + 1];
                for (ii = 0; ii <= tokens->size; ++ii) {
                        if (ii < tokens->size) {
                                args[ii] = vector_get(tokens, ii);
                        } else {
                                args[ii] = 0;
                        }
                }
                execvp(vector_get(tokens, 0), args);
        }
}

void
executeRedirectIn(vector* tokens)
{
	int cpid;

	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);				
	} else {
		int ii; // tokens index
                int jj = 0; // args index
		char* args[tokens->size + 1];
                for (ii = 0; ii <= tokens->size; ++ii) {
                        if (ii < tokens->size) {
				char* v = vector_get(tokens, ii);
				if (strncmp(v, "<", 1) != 0) {
					args[jj] = v;
					jj++;
				}
                        } else {
				args[jj] = 0;
			}
                }
                execvp(vector_get(tokens, 0), args);
	}	
}

void
executeRedirectOut(vector* tokens)
{
	int rv;

	int pipe_fds[2];
	rv = pipe(pipe_fds);
	check_rv(rv);

	int p_read = pipe_fds[0];
	int p_write = pipe_fds[1];
	int cpid;

	if ((cpid = fork())) {
		close(p_write);

		char fileToWrite[100];
		int ii;
		for (ii = 0; ii < tokens->size; ++ii) {
			if (strncmp(vector_get(tokens, ii), ">", 1) == 0) {
				strcpy(fileToWrite, vector_get(tokens, ii + 1));
				break;
			}
		}
		FILE* fp = fopen(fileToWrite, "w");
		char temp[256];
		rv = read(p_read, temp, 256);
		check_rv(rv);
		fprintf(fp, temp);
		fclose(fp);
		
	} else {
		close(p_read);
		close(1);

		rv = dup(p_write);
		check_rv(rv);

		int ii; // tokens index
                int jj = 0; // args index
                char* args[tokens->size + 1];
                for (ii = 0; ii <= tokens->size; ++ii) {
                        if (ii < tokens->size) {
                                char* v = vector_get(tokens, ii);
                                if (strncmp(v, ">", 1) != 0) {
                                        args[jj] = v;
                                        jj++;
                                } else {
					args[jj] = 0;
					break;
				}
                        } else {
                                args[jj] = 0;
                        }
                }
                execvp(vector_get(tokens, 0), args);
	}
}

void
executeNormal(vector* tokens) 
{
	if (strncmp(vector_get(tokens, 0), "cd", 2) == 0) {
		cd(tokens);
		return;
	}

	if (strncmp(vector_get(tokens, 0), "exit", 4) == 0) {
		exit(0);
	}

	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
	} else {
		int ii;
		char* args[tokens->size + 1];
		for (ii = 0; ii <= tokens->size; ++ii) {
			if (ii < tokens->size) {
				args[ii] = vector_get(tokens, ii);
			} else {
				args[ii] = 0;
			}
		}
		execvp(vector_get(tokens, 0), args);
	}
}

void
execute(vector* tokens)
{
	int cpid;

	if (vector_contains(tokens, ";", 1)) {
		executeSemi(tokens);
	}
	else if (vector_contains(tokens, "|\0", 2)) {
		executePipe(tokens);
	}
	else if (vector_contains(tokens, "&&", 2)) {
		executeLogic(tokens, 1);
	}
	else if (vector_contains(tokens, "||", 2)) {
		executeLogic(tokens, 0);
	}
	else if (vector_contains(tokens, "&", 1)) {
		executeBackground(tokens);
	}
	else if (vector_contains(tokens, "<", 1)) {
		executeRedirectIn(tokens);
	}
	else if (vector_contains(tokens, ">", 1)) {
		executeRedirectOut(tokens);
	}
	else {
		executeNormal(tokens);
	}
}

int
main(int argc, char* argv[])
{
    	char cmd[256];

    	if (argc == 1) {
		printf("nush$ ");
        	while(fgets(cmd, 256, stdin) != NULL) {
        		fflush(stdout);
			if (cmd[0] != '\n') {
				vector* tokens = shell_parse(cmd);
				execute(tokens);
				free_vector(tokens);
			}
			printf("nush$ ");
		}
    	}
    	else {
		FILE* pFile;
		pFile = fopen(argv[1], "r");
		if (pFile != NULL) {
			while(fgets(cmd, 256, pFile) != NULL) {
				fflush(stdout);
				vector* tokens = shell_parse(cmd);
				execute(tokens);
				free_vector(tokens);
			}
		}
		fclose(pFile);
    	}
    	return 0;
}
