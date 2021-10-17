#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<ctype.h>
 #include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h> 
#include <fcntl.h>
#include<malloc.h>
#include<string.h>

int count = 0;//插入元素个数计数器

//建单词节点
struct Node{
    char *data;
    struct Node *next;
};

//插入函数
void Insert(struct Node** S,char e[])
{
 
    struct Node *p = NULL;
    p=(struct Node *)malloc(sizeof(struct Node));
    if(p == NULL)
        exit(0);
    p->data = (char *)malloc(strlen(e)+1);
    p->next = NULL;
    strcpy(p->data,e);
    p->next = *S;//将末尾指针指向S
    *S  = p;
    count++;
}
 
 //从文件读取
struct Node* read_txt(struct Node* S, char* txt_name[])
{
    FILE *fp;
    char *StrLine; //每行最大读取的字符数
    fp = fopen(*txt_name,"r");
    while (!feof(fp))
    {
        fgets(StrLine,1024,fp); //读取一行
        StrLine = strncat(StrLine,"\0",1);
        Insert(&S,StrLine);
        //printf("%s", StrLine); //输出
    }
    fclose(fp); //关闭文件
    S = S->next;
    return S;
}

//从键盘读取
struct Node* read_stdin(struct Node* S)
{
    int lines = 5;
    char str[1024];
    for (int i = 0; i < lines; ++i)
    {
        fflush(stdin);
        gets(str);//读取字符串
        Insert(&S,str);//将字符串插在末尾

    }
    return S;
}

//写入文件
void write_txt(struct Node* S, char* txt_name[])
{
    
    FILE *fp = fopen(*txt_name,"w");
    for(int i=0;i<count-1;++i)
    {
        fputs(S->data,fp);//逐行输出
        S = S->next;
    }
    fclose(fp); //关闭文件
    return;
}

//写入标准输出流（键盘）
void write_stdin(struct Node* S)
{
    for(int i=0;i<count;++i)
    {
        puts(S->data);
        S = S->next;
    }  
}
int main(int argc, char *argv[])
{

    if (argc == 1)//无参数：键盘输入，屏幕输出
    {
        struct Node **S=NULL;
        S = read_stdin(S);//键盘读取
        write_stdin(S);//键盘输出
    }
    if (argc == 4)//四个参数：错误
    {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }
    if (argc == 3)//三个参数：文件输入，文件输出
    {        
        struct stat buf1,buf2;
        int fd1 = open(argv[1],O_RDONLY);
        int fd2 = open(argv[2],O_RDONLY);
        fstat(fd1,&buf1);
        fstat(fd2,&buf2);
        if(buf1.st_ino == buf2.st_ino)//读写文件相同：报错
        {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
        if(fopen(argv[1],"r")==NULL||fopen(argv[2],"r")==NULL)//文件打不开：报错
        {
            fprintf(stderr, "reverse: cannot open file '/no/such/file.txt'\n");
            exit(1);
        }
        struct Node **S=NULL;
        S = read_txt(S,&argv[1]);//文件读取
        write_txt(S,&argv[2]);//文件输出

    }
    if (argc == 2)//两个参数：文件输入，屏幕输出
    {
        if(fopen(argv[1],"r")==NULL)
        {
            fprintf(stderr, "reverse: cannot open file '/no/such/file.txt'\n");
            exit(1);
        }
        struct Node **S=NULL;
        S = read_txt(S,&argv[1]);//文件读取
        write_stdin(S);//屏幕输出
    }
}
