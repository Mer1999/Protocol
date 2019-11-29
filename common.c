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
	while()
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


/*------------------------------------------舟------------------------------------------*/

