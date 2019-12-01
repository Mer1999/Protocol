#include"../common/common.h"

static int ena = 1;//收到链路层读信号则置1

static void read_network(int sig)
{
  ena = 1;
}


int main(int argc, char* argv[])
{
  writelog("RNL:start work\n");
  signal(SIG_RECV_NET_READ, read_network);
  sprintf(argv[0], "receiver_network");
  prctl(PR_SET_NAME, "receiver_network");
  char buffer[2][MAX_PKT + 1];    //一个缓冲区(存倒数第二个)一个接收区(最新的)
  memset(buffer[0], '\0', MAX_PKT + 1);
  memset(buffer[1], '\0', MAX_PKT + 1);
  int bufnum = 0;           //用buffer数组的哪一个来读新数据
  char filename[MAX_FILE_LEN];
  seq_nr filenum = 1;       //共享文件序号
  int realread = 0;         //fread返回值
  int lastbuf;              //存放最后一个包的数组位置
  int isend = 1;            //判断是否为最后一个包
  int i;                    //工具人
  int lastPKTsize = 0;      //最后一个包大小
  int fd = open("receive.txt", O_WRONLY | O_CREAT | O_APPEND, 0777);
  if (fd == -1)
  {
    writelog("RNL:open file fail,exit -1\n");
    exit(-1);
  }
  int firstbuf = 1;

  while (1)
  {
    //sleep(1);
    if (ena)//进行读操作
    {
      //ena=0;
      lastbuf = bufnum;
      bufnum = 1 - bufnum;
      sprintf(filename, "%s%04d", D_N_SHARE, filenum);//组合共享文件名
      inc(filenum);
      FILE* fin = fopen(filename, "rb");//打开共享文件
      if (fin == NULL)
      {
        writelog("RNL:open file fail,exit -1\n");
        exit(-1);
      }
      realread = fread(buffer[bufnum], 1, MAX_PKT, fin);
      fclose(fin);
      isend = 1;
      for (i = 0; i < MAX_PKT; i++)//判断是不是最后一个包
      {
        if (buffer[bufnum][i] != '\0')//若非全0包则不是最后一个
        {
          isend = 0;
          break;
        }
      }
      //printf("2");
      if (isend)//若是最后一个包
      {
        lastPKTsize = 0;
        for (i = MAX_PKT - 1; i >= 0; i--)
        {
          if (buffer[lastbuf][i] != '\0')
          {
            lastPKTsize = i + 1;//倒数第二个包非尾零部分大小
            break;
          }
        }
        write(fd, buffer[lastbuf], lastPKTsize);//将正文部分写入receive.txt
        writelog("RNL:work finish\n");
        break;//读文件完成
      }
      if (firstbuf)//若是第一个包则不进行写文件
        firstbuf = 0;
      else
        write(fd, buffer[lastbuf], MAX_PKT);
    }
    pause();//暂停等待RDL发送读文件信号
  }
  close(fd);
  return 0;
}
