#pragma once
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/file.h>  
#include <sys/stat.h> 
#include <netinet/in.h>
#include <sys/errno.h>
#include <signal.h>
#include <sys/prctl.h>

#define MAX_PKT 1024 //每一帧最大容量
#define MAX_FILE_LEN 128 //最大共享文件名长度
#define MAX_SEQ 9999 //共享文件名数字
#define FRAMEHEAD 12 //nak或者ack帧的大小
#define N_D_SHARE "network_datalink.share."
#define D_N_SHARE "datalink_network.share."
#define D_P_SHARE "datalink_physical.share."
#define P_D_SHARE "physical_datalink.share."

#define SIG_ERR 		35	//chsum_err
#define SIG_FRAME_ARRIVAL 	36	//frame_arrival
#define SIG_NET_READY 		37	//network_layer_ready
#define SIG_ENABLE_NET 		38	//enable_network_layer
#define SIG_DISABLE_NET 	39	//disable_network_layer
#define SIG_SEND_PHY_READ 	40	//send数据链路层通知物理层读共享文件
#define SIG_SEND_LINK_READ 	41	//send网络层通知数据链路层读共享文件
#define SIG_RECV_LINK_READ 	42	//recv物理层通知数据链路层读共享文件
#define SIG_RECV_NET_READ 	43	//recv数据链路层通知网络层读共享文件

typedef enum {
	false,		//false=0
	true		//true=1
} boolean;

typedef unsigned int seq_nr;//发送序号

/*数据包，纯数据*/
typedef struct {
	unsigned char data[MAX_PKT];
} packet;

/*帧类型枚举量*/
typedef enum {
	data, //数据包 
	ack, //确认包 
	nak //否定确认包
} frame_kind;

/*帧结构*/
typedef struct {
	frame_kind kind;	    //帧类型 
	seq_nr seq;			    //发送序号 
	seq_nr ack;			    //接收序号 
	packet info;			//数据包
} frame;

/*事件类型枚举量*/
typedef enum {
	frame_arrival,			//帧到达
	cksum_err,				//检验和错
	timeout,				//发送超时
	network_layer_ready,	//网络层就绪 
	ack_timeout				//确认包超时
} event_type;

/*阻塞函数，等待事件发生*/
void wait_for_event(event_type* event);
/*发送方从网络层得到纯数据包*/
void from_network_layer(packet* p);
/*接收方向网络层发送纯数据包,去掉帧的类型、发送/确认序号等控制信息*/
void to_network_layer(packet* p);
/*接收方从物理层取得帧，帧头尾的FLAG字节、数据中的字节填充均已去掉
调用本函数前已验证过校验和，若发生错误,则发送cksum_err事件
因此只有帧正确的情况下会调用本函数*/
void from_physical_layer(packet* p);
/*发送方向物理层发送帧,帧头尾加FLAG字节、数据中进行字节填充,计算校验和放入帧尾*/
void to_physical_layer(frame* s);
/*物理层读取链路层*/
void from_datalink_layer(frame *s);
/*物理层间传输*/
void send_to_phy(frame *s, int sockfd);
/*启动第k帧的定时器*/
void start_timer(seq_nr k);
/*停止第k帧的定时器*/
void stop_timer(seq_nr k);
/*启动确认包定时器*/
void start_ack_timer(void);
/*停止确认包定时器*/
void stop_ack_timer(void);
/*对定时器进行维护*/
void timer_keep(void)
/*解除网络层阻塞,使可以产生新的network_layer_ready事件*/
void enable_network_layer(void);
/*使网络层阻塞,不再产生新的network_layer_ready事件*/
void disable_network_layer(void);
/*使k在[1 ~ MAX_SEQ-1]间循环增长,如果MAX_SEQ=1，则0/1互换*/
#define inc(k) if(k<MAX_SEQ) k=k+1; else k=1;
/*文件锁设置*/
int set_lock(int fd, int type)



//----------------------------------工具函数-------------------------
void sysLocalTime()
{
	time_t             timesec;
	struct tm         *p;
	time(×ec);
	p = localtime(×ec);
	printf("%d%d%d%d%d%d\n", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

}

void sysUsecTime()
{
	struct timeval    tv;
	struct timezone tz;
	struct tm         *p;
	gettimeofday(&tv, &tz);
	printf("tv_sec:%ld\n", tv.tv_sec);
	printf("tv_usec:%ld\n", tv.tv_usec);
	printf("tz_minuteswest:%d\n", tz.tz_minuteswest);
	printf("tz_dsttime:%d\n", tz.tz_dsttime);

	p = localtime(&tv.tv_sec);
	printf("time_now:%d%d%d%d%d%d.%ld\n", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec);
}

//读取pid 
int FindPidByName(const char *pName)
{
	int szPid = -1;

	char szProQuery[256];
	sprintf(szProQuery, "ps -ef|grep '%s'|grep -v 'grep'|awk '{print $2}'", pName);  // 打开管道,执行shell命令  

	FILE *fp = popen(szProQuery, "r");
	char szBuff[10];

	while (fgets(szBuff, 10, fp) != NULL) // 逐行读取执行结果
	{
		szPid = atoi(szBuff);
		break;
	}

	pclose(fp); // 关闭管道指针,不是fclose()很容易混淆  
	return szPid;
}

//-----------------------------链表实现---------------------------
#define OK           1
#define ERROR        0
#define INFEASIBLE  -1
#define OVERFLOW	  -1

typedef int Status;

typedef int ElemType;	//可根据需要修改元素的类型

typedef struct LNode {
	ElemType      time;	//定时器超时时间
	struct LNode *next;	//存放直接后继的指针
} LNode, *LinkList;



//获取链表长度
int ListLength(LinkList L) {
	LinkList p = L->next;
	int len = 0;
	if (p == NULL)return 0;
	while (p != L) {
		p = p->next;
		len++;
	}
	return len;
}


//在i处插入节点e，从1开始计数
Status ListInsert(LinkList &L, int i, ElemType e) {
	LinkList s, p = L;
	int pos = 0;
	while (p&&pos < i - 1) {
		p = p->next;
		pos++;
	}
	if (p == NULL || pos > i - 1)return ERROR;
	s = (LinkList)malloc(sizeof(LNode));
	if (s == NULL)return OVERFLOW;
	s->data = e;
	s->next = p->next;
	p->next = s;
	return OK;
}

//在尾处插入节点e
Status Listadd(LinkList &L, ElemType e) {
	LinkList s, p = L;
	while (p->next) {
		p = p->next;
		pos++;
	}
	if (p == NULL)return ERROR;
	s = (LinkList)malloc(sizeof(LNode));
	if (s == NULL)return OVERFLOW;
	s->data = e;
	s->next = p->next;
	p->next = s;
	return OK;
}

//在插入节点e,按从小到大排列
Status ListInsert_order(LinkList &L, ElemType e) {
	LinkList s, p = L;
	int pos = 0;
	while (p->next) {
		if (e <= p->next->data)break;
		p = p->next;
	}
	s = (LinkList)malloc(sizeof(LNode));
	if (s == NULL)return OVERFLOW;
	s->data = e;
	s->next = p->next;
	p->next = s;
	return OK;
}


//创建一个新链表（带头结点）
int create_L(LinkList &L)
{
	LinkList p;
	int i, n = 1, m;
	L = (LNode *)malloc(sizeof(LNode));
	if (L == NULL)exit(OVERFLOW);
	(L)->next = NULL;
	return OK;
}

//销毁链表
int destroy_L(LinkList &L)
{
	LinkList q, p = L;
	//从头结点开始一次释放
	while (p) {
		q = p->next;
		free(p);
		p = q;
	}
	L = NULL;
	return OK;
}


//在i处删除节点e，从1开始计数
Status Listdelete(LinkList L, int i, ElemType &e) {
	LinkList q, p = L;
	int pos = 0;
	while (p->next&&pos < i - 1) {
		p = p->next;
		pos++;
	}
	if (p->next == NULL || pos > i - 1)return ERROR;
	q = p->next;
	e = q->data;
	p->next = q->next;
	free(q);
	return OK;
}

//寻找值e的位置i，从1开始计数
Status List_find(LinkList L, int &i, ElemType e) {
	LinkList q, p = L->next;
	int pos = 0, len = ListLength(L);
	while (p) {
		if (p->data == e)break;
		p = p->next;
		pos++;
	}
	if (p == NULL)return ERROR;
	i = pos + 1;
	return OK;
}

