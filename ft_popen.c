/* Allowed functions: pipe, fork, dup2, execvp, close, exit

write the following function:

    int    ft_popen(const char file, char const argv[], char type)

The function must launch the executable file with the arguments argv (using execvp).
If the type is 'r' the function must return a file descriptor connected to the output of the command.
If the type is 'w' the function must return a file descriptor connected to the input of the command.
In case of error or invalid parameter the function must return -1.

example:

int main() {
    int fd = ft_popen("ls", (char const[]){"ls", NULL}, 'r');

    charline;
    while(line = get_next_line(fd))
        ft_putstr(line);
}

Hint: Do not leak file descriptors! */

#include <unistd.h>
#include <stdlib.h>




//logic: 
//if type is r - parent should receive (read from) the command's output, so the pipe should write to the command's stdout
//if type is w - parent should send (write) to the command's input, so the pipe should read from the command's stdin
int ft_popen(const char *file, char *const av[], int type)
{
    if(!file || !av || (type != 'r' && type !='w' ))
        return -1;
    int fd[2];
    if (pipe(fd) < 0)
        return -1;
    pid_t pid = fork();
    if (pid < 0) {
        close(fd[0]);//read end
        close(fd[1]);// wirte end
        return -1;
    }
    //child process
    if(pid == 0) {
        if(type == 'r') 
        {
            close(fd[0]);
            if(dup2(fd[1], STDOUT_FILENO) < 0)//Redirect the command’s stdout to pipe’s write end.
                exit (-1);
            close(fd[1]);
        } else {
            close(fd[1]);
            if(dup2(fd[0], STDIN_FILENO) < 0) //Redirect the command’s stdin to pipe’s read end.
                exit (-1);
            close(fd[0]);
        }
        execvp(file, av);
        exit (-1);
    }
    //parent process
    if (type == 'r') {
        close(fd[1]);// Close write-end
        return (fd[0]);// Return read-end of the pipe
    } else {
        close(fd[0]);// Close read-end
        return (fd[1]);// Return write-end of the pipe
    }
}


/*#include <stdio.h>
#include <string.h>
int main()
{
	//int fd = open("texte", O_RDONLY);
	int fd = ft_popen("lss", (char *const[]){"ls", NULL}, 'r');

	char buf[1];
	while(read(fd, buf, 1))
		write(1, buf, 1);

	close(fd);
	return (0);
}*/ 