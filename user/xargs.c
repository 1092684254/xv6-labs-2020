#include "kernel/types.h"
#include "user/user.h"



int main(int argc, char *argv[])
{
    int i;
    int j = 0;
    char *pi_buf[32];       //储存所有参数的字符串指针数组
    char total_buf[256];    //储存所有输入的缓存区
    char *buf = total_buf;  //指向当前处理到的缓存区位置
    char *pi;               //指向当前处理字符串的开头
    pi = buf;               
    for(i = 1; i < argc; ++i)   //首先载入 xargs 后指令自带的参数
    {
        // pi_buf是个字符串指针数组，二维的，相当于，第0行存储argv[1],第1行存储argv[2]
        pi_buf[j++] = argv[i]; 
    }

    int len;
    int sum = 0;
    while((len = read(0, buf, 32)) > 0)//循环读入标准输入 0
    {
        sum += len; //当标准输入长度大于256时退出
        if(sum > 256)
        {
            printf("the args is too long!\n");
            exit(0);
        }

        for(i = 0; i < len; ++i)    //处理该轮读入的数据
        {
            if(buf[i] == ' ')//读到了一个字符串的结尾
            {
                buf[i] = 0; //手动将字符串结尾置为'\0': c风格字符串以'\0’为结尾
                pi_buf[j++] = pi;   //在参数数组中加入新的参数字符串
                pi = &buf[i+1]; //pi指向下一个字符串的开头（如果有的话）
            }
            else if(buf[i] == '\n') //读到了一行的结尾
            {
                buf[i] = 0;
                pi_buf[j++] = pi;
                pi = &buf[i + 1];
                //上同读到字符串结尾的操作

                //将参数列表尾置0， exec要求参数列表第一个为指令本身，列表尾为0，即{cmd,arg1,arg2,..,0}
                pi_buf[j] = 0;  
                if(fork() == 0) //启动子进程执行命令
                {
                    exec(argv[1], pi_buf);
                    exit(0);
                }
                else
                {
                    //处理完标准输出的一行，将参数列表置回初始状态
                    //{cmd, arg1, arg2, extern_arg1, extern_arg1,.. , 0} -> {cmd, arg1, arg2, }
                    j = argc - 1;
                    wait(0);
                }
            }
        }
        //当前缓冲区处理位置更新
        buf = buf + len;
    }

    exit(0);
    return 0;

}