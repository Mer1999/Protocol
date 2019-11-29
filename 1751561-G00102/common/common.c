#include"common.h"

/*------------------------------------------牟------------------------------------------*/
/*设置文件锁*/
int set_lock(int fd,int type)  
{  
    struct flock old_lock,lock;  
    lock.l_whence = SEEK_SET;  
    lock.l_start = 0;  
    lock.l_len = 0;  
    lock.l_type = type;  
    lock.l_pid = -1;  
    if ((fcntl(fd,F_SETLKW,&lock)) < 0)  
    {  
        printf("Lock failed");  
        return 1;  
    }  
    return 0;  
}  

/*使k在[1 ~ MAX_SEQ-1]间循环增长*/
int inc_seq_num(int &seq_num)
{
	if(seq_num<10000)
		seq_num++;
	else
		seq_num=0;
	return seq_num;
}

/*发送方从网络层得到纯数据包*/
void from_network_layer(packet* p)
{
	static int seq_num=1;
	int n_dfd;
	char share_filename[seq_num];
	sprintf(share_filename,"%s%04d",N_D_SHARE,seq_num);
	inc(seq_num);
	fd=open(share_filename,O_RDONLY);
	set_lock(fd,F_RDLCK);
	read(share_file,p->data,MAX_PKT);
	set_lock(fd,F_UNLCK);
	close(n_dfd);
	printf("datalink receive from network successfully\n");
}

/*接收方向网络层发送纯数据包*/
void to_network_layer(packet* p)
{
	static int seq_num=1;
	char share_filename[seq_num];
	int d_nfd;
	sprintf(share_filename,"%s%04d",D_N_SHARE,seq_num);
	inc(seq_num);
	fd=open(share_file_name,O_WRONLY|O_CREAT,0644);
	write(d_nfd,p->data,MAX_PKT);
	close(d_nfd);
}

void from_physical_layer(packet* p);	

void to_physical_layer(frame *s);			

/*------------------------------------------冕------------------------------------------*/
void enable_network_layer()
{
	char buf[128] = { 0 };
	FILE *fp = NULL;
	fp = popen("ps -ef|grep _network|awk '{print $2}'", "r");//找到网络层进程的pid号，发送信号使用
	fgets(buf, 127, fp);
	pid_t pid;
	pid = atoi(buf);
	kill(pid, 38);
}

void disable_network_layer()
{
	char buf[128] = { 0 };
	FILE *fp = NULL;
	fp = popen("ps -ef|grep _network|awk '{print $2}'", "r");//找到网络层进程的pid号，发送信号使用
	fgets(buf, 127, fp);
	pid_t pid;
	pid = atoi(buf);
	kill(pid, 39);
}

/*------------------------------------------舟------------------------------------------*/
//-----------------------------计时器-------------------------------
LinkList timeL;//使用的链表
int nowtime,nowusec;//现在是第几帧、第几毫秒
struct timeval tv;//用于获取当前时间

/*启动第k帧的定时器*/
void start_timer(seq_nr k)
{
	if (k<nowtime)return;
	Listadd(timeL,k-nowtime);
}
/*停止第k帧的定时器*/
void stop_timer(seq_nr k)
{
	int pos=List_find(timeL,k),e;
	Listdelete(timeL,pos,e);
}
/*启动确认包定时器*/
void start_ack_timer(void)
{

}
/*停止确认包定时器*/
void stop_ack_timer(void)
{

}

/*对定时器进行维护*/
void timer_keep(void)
{
    gettimeofday(&tv);//获取当前时间
	if (tv.tv_usec==nowusec)return;//触发毫秒时间中断
	int n;
	n=L->next->data;//获取当前第一个定时器的时间
	n--;//时间减一
	if (n==0)//计时器超时
	{
		Listdelete(timeL,1,e);//删除第一个计时器
		//发送timeout事件，等待填入
	}
}

/*定时器初始化，请在开始时使用*/
void timer_create(void)
{
	nowtime=0;//初始化当前帧
	
    gettimeofday(&tv);//获取当前时间
	nowusec=tv.tv_usec;//初始化当前时间
}

