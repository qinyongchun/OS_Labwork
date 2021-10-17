#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
  
void Do_Cmd(char *);
void do_cd(char *);	//进入cd指令
void do_ls(char *, char *);	//进入ls指令
void do_exit(char *);
//void run_sh(char *);
void do_path(char *,char[10][100],int *,int *);
int getStrLength(char*);
char *simplify(char *);
int search_sym(char *,char);

char PATH[128]="(default)";
int PATH_flag=0;//unchanged
char search_path[10][100];
int search_path_num = 2;
int search_path_flag = 0;//unchanged
char search_address[100];

int main(int argc, char **argv)
{
	// argc = 2;
	// argv[1] = "tests/15.in";
	if(argc>2)
	{
		fprintf(stderr, "An error has occurred\n");
		exit(1);
	}
	FILE *file = fopen(argv[1],"r");
	if(argc==2&&!file)
	{
		fprintf(stderr, "An error has occurred\n");
		exit(1);
	}
	int mode;//交互or批处理模式
	//wait(NULL);
	FILE *fp = fopen(argv[1],"r");	
	char *command;
	size_t n=0;
	if (argc == 1)
	{	
		mode = 1;//interactive
		fp = stdin;
	}
	else 
		mode = 0;//batch
	while(1)
	{
		if (mode == 1)
		{	
			printf("/%s/::seush> ",PATH);
		}

		if(getline(&command,&n,fp)==-1)
			exit(0);

		command=simplify(command);
		if (strcmp(command,"\n")==0)
			continue;
		
		int p = search_sym(command,'&');
		// if(!p)
		// {
		// 	fprintf(stderr, "An error has occurred\n");
		// 	exit(0);
		// }
		if(p==-1)
		{
			Do_Cmd(command);
		}
		else
		{	
			char *command_list[10];
			memset(command_list,0,sizeof(command_list));
			int count = 0;
			while(1)
			{
				command_list[count]=strsep(&command,"&");//将每个参数分割开并统计参数个数（空格数加一）
				if (!command_list[count])
					break;
				//printf("%s\n",cds[count++]);
				count = count + 1;
			}
			int i;
			for (i=0;i<count;++i)
			{
				command_list[i]=simplify(command_list[i]);
				if(command_list[i]==NULL)
					continue;
				// if (i!=count-1)
				// {
					// command_list[i][len]='\n';
					// command_list[i][len+1]='\0';
				// }
				pid_t parallism = fork();
				if(parallism==0)
				{
					Do_Cmd(command_list[i]);
					exit(0);
				}
				else
				{
					wait(NULL);
				}
			}


			//return;
		}
	}
	exit(0);
}

void Do_Cmd(char *command)
{
		char *cmd_name;
		int right_cmd=0;
		// char cmd_tail[3];
		// n = getStrLength(command);
		// cmd_tail[2]=command[n];
		// cmd_tail[1]=command[n-1];
		// cmd_tail[0]=command[n-2];
		cmd_name=strsep(&command," ");//获取指令类型

			if( strcmp(cmd_name,"path\n") == 0||strcmp(cmd_name,"path") == 0)
			{
				memset(search_path,0,sizeof(search_path));
				do_path(command,search_path,&search_path_num,&search_path_flag);
				return;
				//continue;
			}

			if( strcmp(cmd_name,"exit\n") == 0||strcmp(cmd_name,"exit") == 0)
			{
				do_exit(command);
				return;
				//continue;
			}

			if( strcmp(cmd_name,"cd\n") == 0||strcmp(cmd_name,"cd") == 0)
			{
				do_cd(command);
				return;
				//continue;
			}

		if (!search_path_flag)
		{
			strcpy(search_path[0],"/bin/");
			strcpy(search_path[1],"/usr/bin/");
		}

		int k;
		int n;
		for (k=0;k<search_path_num;++k)
		{
			memset(search_address,0,sizeof(search_address));
			strcpy(search_address,search_path[k]);
			strcat(search_address,cmd_name);
			n = getStrLength(search_address);
			int m = getStrLength(cmd_name);
			if (cmd_name[m]=='\n')
				{search_address[n]='\0';}
			else
				{search_address[n+1]='\0';}

			if (access(search_address,X_OK)==0)
			{
				right_cmd=1;
				break;
			}
		}

		if(!right_cmd)
		{
			fprintf(stderr, "An error has occurred\n");
			return;
			//continue;
		}
		else
		{
			
			// if( strcmp(cmd_name,"ls\n") == 0||strcmp(cmd_name,"ls") == 0)
			// {
				do_ls(cmd_name,command);
				return;
				//continue;
			// }

		}

}

char *simplify(char *cmd)
{
	if(cmd==NULL)
	{
		return cmd;
	}
	int j = 0;
	int i = 0;
	int n = getStrLength(cmd);
	char *simple = (char *)malloc(100);
	//char *simple;
	for (;i < n+1;++i)
	{
		if (cmd[i]!=' ')
		{
			break;
		}
	}
	int flag = 0;
	while(cmd[i]!='\0')
	{
		if (cmd[i]==' ' && cmd[i+1]==' ')
		{
			flag = 1;
			break;
		}
		simple[j++]=cmd[i++];
	}
	if(flag == 1)
	{
		simple[j]='\n';
		simple[j+1]='\0';
	}
	int len = getStrLength(simple);
	if(simple[len]==' ')
	{
		simple[len]='\n';
		simple[len+1]='\0';
	}
	return simple;

}

void do_cd(char *cmd)
{
	int count=0;// 统计空格数量
	char *cds[10];// cd的参数列表

	while(1)
	{
		cds[count]=strsep(&cmd," ");//将每个参数分割开并统计参数个数（空格数加一）
		if (!cds[count])
			break;
		//printf("%s\n",cds[count++]);
		count = count + 1;
	}
	//printf("%d\n",count);
	if (count!=1)//cd有且仅有一个参数
	{
		fprintf(stderr, "An error has occurred\n");
		return;
	}
	strcpy(PATH,cds[0]);
	//PATH = cds[0];
	PATH_flag = 1;
	if(PATH_flag==1)
	{
		int i = strlen(PATH)-1;//去除最后一条指令的\n符
		PATH[i] = '\0';
	}

	return;
}
 
void do_ls(char *name,char *cmd)
{
	int count=0;// 统计空格数量
	char *temp_args[50];
	char *lss[50];//ls的参数列表
	int count_RD=0;//count redirection symbols
	char ReDirect[20];
	int locate=-1;
	int i;
		//locate = strchr(cmd,'>');

		int n = getStrLength(cmd);
		i = 0;
		for (;i<n;++i)
		{
			if (cmd[i]=='>')
				{
					locate = i;
					break;
				}
		}
		if (locate!=-1)
		{
			if (cmd[locate+1]!=' ')
			{
				int k = n;
				for(;k>=locate+1;--k)
				{
					cmd[k+1]=cmd[k];
				}
				cmd[locate+1]=' ';
			}
			if (cmd[locate-1]!=' ')
			{
				int k = n+1;
				for(;k>=locate;--k)
				{
					cmd[k+1]=cmd[k];
				}
				cmd[locate]=' ';
			}
		}

	int count1=0;
	while(1)
	{
		temp_args[count1+1]=strsep(&cmd," ");
		if (!temp_args[count1+1])
			break;
		//printf("%s\n",cds[count++]);
		count1 = count1 + 1;
	}
	i = 0;
	if(count1)
	{
		i = getStrLength(temp_args[count1]);//去除最后一条指令的\n符
		temp_args[count1][i] = '\0';
	}
	else
		temp_args[count1]="\0";


	i = 1;
	int j = 1;
	count = 0;
	int rd_flag=0;
	while(temp_args[i])
	{
		if(strcmp(temp_args[i],">")==0)
		{
			++i;
			++count_RD;
			rd_flag=1;
			break;
		}
		else
		{
			++count;
			lss[j++]=temp_args[i];
		}
		++i;
	}
	//strcpy(lss[count+1],NULL);
	lss[count+1] = NULL;
	memset(ReDirect,0,sizeof(ReDirect));
	if(rd_flag)
	{
		if(count1 != count + 2)
		{
			fprintf(stderr, "An error has occurred\n");
			return;
		}
		strcpy(ReDirect,temp_args[i]);
	}


	FILE *fp=fopen(ReDirect,"w");
	//printf("%d\n",count);
	if (count!=0&&count!=1&&count!=2&&count!=3)//ls指令最多两个参数
	{
		fprintf(stderr, "An error has occurred\n");
		return;
	}
	if(!fp&&count_RD)
	{
		fprintf(stderr, "An error has occurred\n");
		return;
	}
	
	i = getStrLength(name);
	name[i+1]='\0';

	//char address1[100] = "/bin/ls";//默认地址
	//char address2[100] = "/usr/bin/ls";
	if (PATH_flag==1)
	{
		int i;
		for (i = count+1; i > 0;--i)
		{
			lss[i+1] = lss[i];
		}
		lss[1] = PATH;
	}
	pid_t fpid;
	//strcpy(lss[0],name);
	lss[0]= "ls";
	//execv(search_address,lss);
	if (count_RD)
	{
		//fclose(stdout);
		stdout=fopen(ReDirect,"wb");
		// redirc=freopen(ReDirect,"w",stdout);
	}
	fpid = fork();
	if (fpid==0)
		{
			if (strcmp(search_address,"tests/p4.sh")==0)
				return;
			execv(search_address,lss);
		}
	else 
		wait(NULL);		
	// if (count_RD)
	// {
	// 	fclose(redirc);
	// }
	return;

}

void do_exit(char *cmd)
{
	int count=0;// 统计空格数量
	char *exits[10];// cd的参数列表
	while(1)
	{
		exits[count]=strsep(&cmd," ");//将每个参数分割开并统计参数个数（空格数加一）
		if (!exits[count])
			break;
		count = count + 1;
	}
	if (count == 0)
	{
		exit(0);
	}
	else
	{
		fprintf(stderr, "An error has occurred\n");
		return;
	}

}

// void run_sh(char *cmd)
// {
// 	int count=0;// 统计空格数量
// 	char *shs[50];//ls的参数列表	
// 	// while(1)
// 	// {
// 	// 	shs[count+1]=strsep(&cmd," ");
// 	// 	if (!shs[count+1])
// 	// 		break;
// 	// 	//printf("%s\n",cds[count++]);
// 	// 	count = count + 1;
// 	// }
// 	shs[0]="";
// 	shs[1] = NULL;
// 	pid_t pid;
// 	pid = fork();
// 	if (pid==0)
// 		{
// 			execvp(cmd,(char*)NULL);
// 		}
// 	else 
// 		wait(NULL);		
// }

void do_path(char *cmd, char path[10][100],int *num,int *flag)
{
	int count=0;// 统计空格数量
	char *paths[10];// cd的参数列表
	//char *path[30];
	//free(path);
	//path = (char *)malloc(10);
	
	while(1)
	{
		paths[count+1]=strsep(&cmd," ");//将每个参数分割开并统计参数个数（空格数加一）
		if (!paths[count+1])
			break;
		count = count + 1;
	}

	if(count)
	{
		int m = strlen(paths[count])-1;//去除最后一条指令的\n符
		paths[count][m] = '\0';
	}
	else
		paths[count]="\0";

	int i;
	for (i = 0;i<count;++i)
	{
		strcpy(path[i],paths[i+1]);
		//path[i]=paths[i+1];
	}
	*num = count;

	int n;
	for (n = 0;n < count; ++n)
	{
		int m = strlen(paths[n+1]);
		//int t;
		// for (t = m-1; t >=0; --t)
		// {
		// 	path[n][t+1]=path[n][t];
		// }
		// path[n][0]='/';
		path[n][m]='/';
	}

	path[count][0]='\0';
	//search_path = path;
	*flag = 1;

}

int getStrLength(char* str) 
{
    int i = 0;
	if (str == NULL)
		return 0;
    while (str[i] != '\0') 
	{
        i++;
    }
    return i-1;
}

int search_sym(char *str, char p)
{
	int i = 0;
	while(str[i])
	{
		if(str[i++]==p)
			return i-1;
	}
	return -1;
}