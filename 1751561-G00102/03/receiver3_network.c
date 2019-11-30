#include"../common/common.h"

static int ena = 0;//收到链路层读信号则置1

static void read_network(int sig)
{
    signal(SIG_RECV_NET_READ, read_network);
    ena = 1;
}


int main()
{
	prctl(PR_SET_NAME, "receiver_network");
	char buffer[2][MAX_PKT+1];//一个缓冲区(存倒数第二个)一个接收区(最新的)
    memset(buffer[0], '\0', MAX_PKT + 1);
    memset(buffer[1], '\0', MAX_PKT + 1);
    int bufnum=0;//用buffer数组的哪一个来读新数据
    char filename[MAX_FILE_LEN];
    seq_nr filenum = 1;           //共享文件序号
    int realread=0;//fread返回值
    int lastbuf;//存放最后一个包的数组位置
    int isend=1;//判断是否为最后一个包
    int i;//工具人
    int lastPKTsize=0;//最后一个包大小
    int fd=open("receive.txt",O_WRONLY|O_CREAT,0777);
    int firstbuf=1;
    if(fd==-1)
    {
    	perror("open file");
    	exit(-1);
    }

    while(1)
    {
    	if(ena)//进行读操作
    	{
    		lastbuf=bufnum;
    		bufnum=1-bufnum;
    		sprintf(filename, "%s%04d", D_N_SHARE, filenum);//组合共享文件名
    		inc(filenum);
    		FILE*fin=fopen(filename,"rb");
    		if(fin==NULL)
    		{
    			perror("open file");
    			exit(-1);
    		}
    		realread=fread(buffer[bufnum],1,MAX_PKT,fin);
    		fclose(fin);
    		isend=1;
    		for(i=0;i<MAX_PKT;i++)
    		{
    			if(buffer[bufnum][i]!='\0')
    			{
    				isend=0;
    				break;
    			}
    		}
    		if(isend)
    		{
    			lastPKTsize=0;
    			for(i=MAX_PKT-1;i>=0;i--)
    			{
    				if(buffer[lastbuf][i]!='\0')
    				{
    					lastPKTsize=i+1;
    					break;
    				}
    			}
    			write(fd,buffer[lastbuf],lastPKTsize);
    			break;//读文件完成
    		}
    		if(firstbuf)
    			firstbuf=0;
    		else
    			write(fd,buffer[lastbuf],MAX_PKT);
    	}
    	pause();
    }
    close(fd);
    return 0;
}
