#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc , char *argv[])
{
    int pipefd[2];
    int pipefd2[2];
    pipe(pipefd);
    pipe(pipefd2);

    int pid = fork();
    if(pid == 0)
    {
        // 关闭写端
        close(pipefd[1]);
        // 关闭读端
        close(pipefd2[0]);
        char buf[1024] = {0};
        read(pipefd[0],buf,sizeof(buf));
        close(pipefd[0]);
        printf("%d: received ping\n",getpid());
       
        write(pipefd2[1],"a",2);
        close(pipefd2[1]);
    }
    else if(pid > 0)
    {
        // 父进程

        close(pipefd[0]);
         // 关闭写端
        close(pipefd2[1]);
        

        write(pipefd[1],"a",2);
        close(pipefd[1]);

        char buf[1024] = {0};
        read(pipefd2[0],buf,sizeof(buf));

        printf("%d: received pong\n",getpid());
        close(pipefd2[0]);

    }
    exit(0);
}