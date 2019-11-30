#include "../common/common.h"
int ena = 1;                  //enable|disable

void enable_network(int sig)
{
   ena=1;
}
void disable_network(int sig)
{
   ena=0;
}

int main()
{
   prctl(PR_SET_NAME, sender_network);
   (void) signal(38, enable_network);
   (void) signal(39, disable_network);
   char buffer[MAX_PKT + 1] = { 0 };//存储文件中读取的数据
   FILE* fp = fopen("test.txt", "r");//打开文件进行读
   if (fp == NULL)//文件打开失败
   {
      perror("open file");
      exit(-1);
   }
   int realread = 0;             //fread返回值，实际读取到的字符数
   int keepread = 1;             //控制循环进行
   int i = 0;                    //贡具人
   int fd;                       //共享文件句柄
   
   char filename[MAX_FILE_LEN];  //共享文件名
   seq_nr filenum = 1;           //共享文件序号

   while (keepread)
   {
      
      if (ena)
      {
         ena=0;
         memset(buffer, '\0', MAX_PKT + 1);
         realread = fread(buffer, 1, MAX_PKT, fp);//读取MAX_PKT个字符并存储到buffer中
         printf("realread = %d\n", realread);
         printf("%s\n", buffer);
         //if(realread==1024)
         // printf("读到1024个字符\n");
         if (realread < MAX_PKT)//文件内容全部读完，空余全部填尾0
         {
            for (i = realread; i < MAX_PKT; i++)
               buffer[i] = '\0';
         }
         if (!realread)//最后一次循环，将keepread置零
            keepread = 0;
         sprintf(filename, "%s%04d", N_D_SHARE, filenum);//组合共享文件名
         printf("filename = %s\n", filename);
         inc(filenum);//filenum++
         fd = open(filename, O_WRONLY | O_CREAT, 0777);
         write(fd, buffer, MAX_PKT);//将buffer内容写入共享文件
         close(fd);
         int pid=FindPidByName("./sender_datalink");
         kill(pid,41);//通知链路层读共享文件
      }
   }
   fclose(fp);
   return(0);
}
