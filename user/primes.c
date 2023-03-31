#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/*
首先有一个进程起始时向第一个管道送入2到35，
从子进程开始做的事情就是重复的，有这么几个事情，
从读端口拿到上一个进程送来的第一个数将其输出，
有数字说明需要创建下一个管道和进程继续传数，
本进程继续读数并判断是否是第一个数的倍数，不是则送入刚创建的管道的写端口，
子进程重复这个事情（从读端口拿送来的第一个数……）

*/

void primes(int p[])
{
    int res;
    // 该进程通过一个管道从其左侧邻居读取数据
    if(!read(p[0] , &res, sizeof(res)))
    {
        exit(0);
    }
    fprintf(2,"prime %d\n",res);
    int c[2];
    pipe(c);
    int pid = fork();
    if(pid == 0)
    {
        // 子进程
        close(p[0]);
        close(c[1]);
        //如果是子进程，则循环prime
        primes(c);
    }
    else if (pid > 0)
    {
        // 父进程
        close(c[0]);
        int temp ;
        while (read(p[0],&temp,sizeof(temp)))
        {
           if(temp % res != 0)
           {
            // 并通过另一个管道向其右侧邻居写入数据
            write(c[1] , &temp , sizeof(temp));
           }
        }
        close(p[0]);
        close(c[1]);
        wait((int *)0);
        
    }
}

int main(int argc , char *argv[])
{
    int p0[2];
    pipe(p0);
    // 第一个过程将数字2到35送入管道
    for(int i = 2 ; i <= 35 ; i++)
    {
        write(p0[1] , &i ,sizeof(int));

    }
    close(p0[1]);
    primes(p0);
    exit(0);


}