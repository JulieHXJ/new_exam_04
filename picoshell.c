/* Assignment name:    picoshell
Expected files:        picoshell.c
Allowed functions:    close, fork, wait, exit, execvp, dup2, pipe
___

Write the following function:

int    picoshell(char *cmds[]);

The goal of this function is to execute a pipeline. It must execute each
commands [sic] of cmds and connect the output of one to the input of the
next command (just like a shell).

Cmds contains a null-terminated list of valid commands. Each rows [sic]
of cmds are an argv array directly usable for a call to execvp. The first
arguments [sic] of each command is the command name or path and can be passed
directly as the first argument of execvp.

If any error occur [sic], The function must return 1 (you must of course
close all the open fds before). otherwise the function must wait all child
processes and return 0. You will find in this directory a file main.c which
contain [sic] something to help you test your function.


Examples: 
./picoshell /bin/ls "|" /usr/bin/grep picoshell
picoshell
./picoshell echo 'squalala' "|" cat "|" sed 's/a/b/g'
squblblb/
*/




#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


int picoshell(char **cmds[])
{
    int i = 0;
    int fd[2];
    int in_fd = 0;//the previous command's output and the next command's input
    // if in_fd is 0, it means the input is from stdin
    // if in_fd is not 0, it means the input is from the previous command's output
    pid_t pid;
    int has_next = 0;


    while (cmds[i])
    {
        // Create a pipe if there's a next command
        has_next = cmds[i + 1] != NULL;
        if (has_next && pipe(fd) == -1)
        {
                return 1;
        }



        pid = fork();
        // create child failed
        if (pid < 0)
        {
            if (has_next)
            {
                //means i created pipe already
                close(fd[0]);
                close(fd[1]);
            }
            if (in_fd != 0)
            {
                //means it was left from previous command
                close(in_fd);
            }
            return 1;
        }


        //chile process
        if (pid == 0) 
        {
            // if the input is from last command
            if (in_fd != 0)
            {
                //redirect to pipe’s read end"
                if (dup2(in_fd, 0) == -1)
                    exit(1);
                close(in_fd);//close after using
            }

            //redirect stdout to the pipe out
            if (has_next) 
            {
                if (dup2(fd[1], 1) == -1)
                    exit(1);
                close(fd[0]);//unused
                close(fd[1]);//close after dup
            }
            execvp(cmds[i][0], cmds[i]);
            exit(1); 
        }

        // Parent

        // close the previous input fd
        if (in_fd != 0)
        {
            close(in_fd);
        }
        // close the pipe's write end
        if (has_next)
        {
            close(fd[1]);
        }
        // store the read end of the pipe for the next command
        in_fd = fd[0];
        i++;
    }

    //waiting for all children
    int status;
    int ret = 0;
    while (wait(&status) > 0)
    {
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
            ret = 1;
    }
    return ret;
}

#include <string.h>
int main(int argc, char *argv[])
{
    int cmd_size = 1;
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "|"))
        {
            cmd_size++;
        }
        
    }
    char ***cmds = calloc(cmd_size + 1, sizeof(char**));
    if (!cmds)
        return 1;
    cmds[0] = argv + 1;
    int cmd_i = 1;
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "|"))
        {
            cmds[cmd_i] = argv + i + 1;
            argv[i] = NULL; 
            cmd_i++;
        }
    }
    int ret = picoshell(cmds);
    // if (ret == 1)
    // {
    //     perror("picoshell");
    // }
    free(cmds);
    return ret;
}

