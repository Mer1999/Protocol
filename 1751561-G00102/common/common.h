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

#define SIG_CHSUM_ERR      	35	//chsum_err
#define SIG_FRAME_ARRIVAL 	36	//frame_arrival
#define SIG_NET_READY 		37	//network_layer_ready
#define SIG_ENABLE_NET 		38	//enable_network_layer
#define SIG_DISABLE_NET 	39	//disable_network_layer
#define SIG_SEND_PHY_READ 	40	//send数据链路层通知物理层读共享文件
#define SIG_SEND_LINK_READ 	41	//send网络层通知数据链路层读共享文件
#define SIG_RECV_LINK_READ 	42	//recv物理层通知数据链路层读共享文件
#define SIG_RECV_NET_READ 	43	//recv数据链路层通知网络层读共享文件

#define DEFAULT_PORT 4000
#define RIP "192.168.80.233"
#define SIP "192.168.80.230"

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
void key_from_network_layer_enable();//信号启用
/*发送方从网络层得到纯数据包*/
void from_network_layer(packet* p);
/*接收方向网络层发送纯数据包,去掉帧的类型、发送/确认序号等控制信息*/
void to_network_layer(packet* p);
/*接收方从物理层取得帧，帧头尾的FLAG字节、数据中的字节填充均已去掉
调用本函数前已验证过校验和，若发生错误,则发送cksum_err事件
因此只有帧正确的情况下会调用本函数*/
void key_from_physical_layer_enable();//信号启用
void from_physical_layer(frame* s);
/*发送方向物理层发送帧,帧头尾加FLAG字节、数据中进行字节填充,计算校验和放入帧尾*/
void to_physical_layer(frame* s);
/*物理层读取链路层*/
void from_datalink_layer(frame *s);
/*物理层间传输*/
void send_to_phy(frame *s, int sockfd);
/*物理层间接收*/
int receive_from_phy(frame *s, int sockfd_server,int sockfd_client);
/*启动第k帧的定时器*/
void start_timer(seq_nr k);
/*停止第k帧的定时器*/
void stop_timer(seq_nr k);
/*启动确认包定时器*/
void start_ack_timer(void);
/*停止确认包定时器*/
void stop_ack_timer(void);
/*对定时器进行维护*/
void timer_keep(void);
/*定时器初始化，请在开始时使用*/
void mytimer_create(void);
/*解除网络层阻塞,使可以产生新的network_layer_ready事件*/
void enable_network_layer(void);
/*使网络层阻塞,不再产生新的network_layer_ready事件*/
void disable_network_layer(void);
/*使k在[1 ~ MAX_SEQ-1]间循环增长,如果MAX_SEQ=1，则0/1互换*/
#define inc(k) if(k<MAX_SEQ) k=k+1; else k=1;
/*文件锁设置*/
int set_lock(int fd, int type);

/*读取pid*/
int FindPidByName(const char *pName);

//链表
#define OK           1
#define ERROR        0
#define INFEASIBLE  -1
#define OVERFLOW	  -1

typedef int Status;

typedef int ElemType;	//可根据需要修改元素的类型

typedef struct LNode {
	ElemType      data;	//定时器超时时间
	struct LNode *next;	//存放直接后继的指针
} LNode, *LinkList;
