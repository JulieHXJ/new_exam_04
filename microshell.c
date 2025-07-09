/**
 * Allowed functions: malloc, free, write, close, fork, waitpid, signal, kill, exit, chdir, execve, dup, dup2, pipe, strcmp, strncmp
 */


#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>  //strcmp, strncmp

// for print error messages
int ft_strlen(char *str)
{
	int i = 0;
	while (str[i])
		i++;
	return (i);
	
}

void print_error(char *str, char *arg)
{
	write(STDERR_FILENO, str, ft_strlen(str));
	if (arg)
	{
		write(STDERR_FILENO, " ", 1);
		write(STDERR_FILENO, arg, ft_strlen(arg));
	}
	write(STDERR_FILENO, "\n", 1);
}

//cd: handle arguments number and wrong path
int buildin_cd(int ac, char **av)
{
	if(ac != 2)
	{
		print_error("error: cd: bad arguments", NULL);
		return (1);
	}
	if (chdir(av[1]) != 0)
	{
		print_error("error: cd: cannot change directory to", av[1]);
		return (1);
	}
	return (0);
}


//"dup2 对接，close 清场"
void pipe_setup(int has_pipe, int *pipe_fd, int i)
{
	if (has_pipe && (dup2(pipe_fd[i], i) < 0 || close(pipe_fd[0]) < 0 || close(pipe_fd[1]) < 0))
	{
		print_error("fatal", NULL);
		exit(1);
	}
}

int ft_execve(char **args, char **envp, int i)
{
	int has_pipe = 0;
	int fd[2];
	pid_t pid;
	int exit_code;

	// check the last arg is |
	if (args[i] && strcmp(args[i], "|") == 0)
		has_pipe = 1;
	// if creating pipe fails
	if (has_pipe && pipe(fd) == -1)
	{
		print_error("fatal", NULL);
		return (1);
	}
	//create child process: child pid = 0, father pid > 0
	pid = fork();
	if (pid < 0)
	{
		print_error("fatal", NULL);
		exit(1);
	}
	// inside child process
	if (pid == 0)
	{
		args[i] = NULL; // 关键：在管道符处截断参数列表！["ls", "-l", "|", ...]变为 ["ls", "-l", NULL]
		pipe_setup(has_pipe, fd, 1);// connect pipe_fd[1] to stdout and close all
		execve(*args, args, envp);//won't return if success
		print_error("cannot execute", *args);
		exit(1);
	}
	//father process
	waitpid(pid, &exit_code, 0);
	pipe_setup(has_pipe, fd, 0); //connect pipe_fd[0] to stdin and close all
	return (WIFEXITED(exit_code) && WEXITSTATUS(exit_code));
}

int main(int ac, char **av, char **envp)
{
	int i = 0;//cmd index
	int exit_code = 0;

	if (ac > 1)
	{
		//while the excutable and command are ture 
		while (av[i] && av[i + 1])
		{
			av = &av[i+1];//move the pointer to next arg
			i = 0;
			while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
			{
				i++;
				//untill find | or ; or \n
			}
			//i is the number of args for this command
			if (strcmp(*av, "cd") == 0)
			{
				exit_code = buildin_cd(i, av);
			}
			else if (i > 0)
			{
				exit_code = ft_execve(av, envp, i);
			}
			
			
		}
		
	}
	return (exit_code);
}