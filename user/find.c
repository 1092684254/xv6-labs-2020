#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path) //  从字符串 "a/b/c/d" 中获取 字符串 "d",基本原理为从后往前找到第一个'/'
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = '\0';

  return buf;
}

void cmp(char *a, char *b)  //比较两个文件名是否相同并打印路径
{
    if(!strcmp(fmtname(a), b))
        printf("%s\n", a);
}

void
find(char *path, char *target)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){ //根据路径打开文件
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){   //获取文件参数
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type)
  {
    case T_FILE://该文件为文件时
        cmp(path, target);
        break;

    case T_DIR: //该文件为文件夹时
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        /*
        是从fd指向的文件中读取sizeof(de)个字节的数据到de中，
        并判断是否成功读取了sizeof(de)个字节。如果成功，则表达式为真；否则，为假。
        */
        while(read(fd, &de, sizeof(de)) == sizeof(de))
        {
                //若文件夹下文件数量为0，1或者该文件夹名字为"."或".."则不进入，防止套娃
            if(de.inum == 0 || de.inum == 1 || strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0)
                continue;
            memmove(p, de.name, strlen(de.name)); //将文件名追加到路径，递归find
            p[strlen(de.name)] = '\0';
            find(buf, target);
        }
        break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{


    if(argc < 3){
        printf("Please input the right arg!\n");
        exit(0);
    }

    find(argv[1], argv[2]);
    exit(0);
}