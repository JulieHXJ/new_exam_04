#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int ft_popen(const char *file, char *const av[], int type)
{
    if (!file || !av || (type != 'r' && type != 'w'))
    {
        return -1;
    }
    int fd[2];
    if (pipe(fd) < 0)
    {
        return -1;
    }
    pid_t pid = fork();
    if (pid < 0)
    {
        close(fd[0]);
        close(fd[1]);
        return (-1);
    }
    
    if ((pid == 0))
    {
        if (type == 'r')
        {
            close (fd[0]);
            if (dup2(fd[1], STDOUT_FILENO) < 0)
                exit(-1);
        }
        else
        {
            close (fd[1]);
            if (dup2(fd[0], STDIN_FILENO) < 0)
                exit (-1);
        }
        close(fd[1]);
        close (fd[0]);
        execvp(file, av);
        exit(-1);
    }
    if (type == 'r')
    {
        close(fd[1]);
        return (fd[0]);
    }
    else
    {
        close(fd[0]);
        return (fd[1]);
    }
    
}


int    picoshell(char **cmds[])
{
    int i = 0;
    int fd[2];
    int in_fd = 0;
    pid_t pid;
    int has_next = 0;
    while (cmds[i])
    {
        has_next = cmds[i+1] != NULL;
        if (has_next && pipe(fd) < 0)
        {
            return -1;
        }
        
        pid = fork();
        if (pid < 0)
        {
            if (has_next)
            {
                close(fd[0]);
                close(fd[1]);
            }
            if (in_fd != 0)
            {
                close (in_fd);
            }
            return -1;
            
        }

        if (pid == 0)
        {
            if (in_fd != 0)
            {
                if (dup2(in_fd, STDIN_FILENO) < 0)
                {
                    exit(-1);
                }
                close(in_fd);
            }
            if (has_next)
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
            }
            execvp(cmds[i][0], cmds[i]);
            exit(-1);
            
        }
        
        //parent
        if (in_fd != 0)
        {
            close(in_fd);
        }
        if (has_next)
        {
            close(fd[1]);
        }
        in_fd = fd[0];
        i++;
    }
    //wait
    int status;
    int ret = 0;
    while(wait(&status > 0))
    {
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        {
            ret = 1;
        }
    }
    return ret;
}


