#include"common.h"

/*------------------------------------------牟------------------------------------------*/
/*设置文件锁*/
int set_lock(int fd, int type)
{
	struct flock old_lock, lock;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_type = type;
	lock.l_pid = -1;
	if ((fcntl(fd, F_SETLKW, &lock)) < 0)
	{
		printf("Lock failed");
		return 1;
	}
	return 0;
}




int key_from_network_layer = 0;//信号阻塞
void key_from_network_layer_enable()//信号启用
{
	key_from_network_layer = 1;
}

/*发送方从网络层得到纯数据包*/
void from_network_layer(packet* p)
{
	while (key_from_network_layer == 0);//信号阻塞
	key_from_network_layer = 0;

	static int seq_num = 1;
	int fd;
	char share_filename[MAX_FILE_LEN];
	sprintf(share_filename, "%s%04d", N_D_SHARE, seq_num);
	inc(seq_num);
	fd = open(share_filename, O_RDONLY);
	set_lock(fd, F_RDLCK);
	read(fd, p->data, MAX_PKT);
	set_lock(fd, F_UNLCK);
	close(fd);
	int pid = FindPidByName("./sender_network");//获得网络层进程的pid
	kill(pid, 40);//发送40号信号
	printf("datalink receive from network successfully\n");
}

/*接收方向网络层发送纯数据包*/
void to_network_layer(packet* p)
{
	static int seq_num = 1;
	char share_filename[MAX_FILE_LEN];
	int d_nfd;
	sprintf(share_filename, "%s%04d", D_N_SHARE, seq_num);
	inc(seq_num);
	d_nfd = open(share_filename, O_WRONLY | O_CREAT, 0644);
	write(d_nfd, p->data, MAX_PKT);
	close(d_nfd);
}

/*接收方从物理层取得帧，调用本函数前已验证过校验和，若发生错误,则发送cksum_err事件
因此只有帧正确的情况下会调用本函数*/
int key_from_physical_layer = 0;//信号阻塞
void key_from_physical_layer_enable()//信号启用
{
	key_from_physical_layer = 1;
}

void from_physical_layer(frame* s)
{
	while (key_from_physical_layer == 0){};//信号阻塞
	key_from_physical_layer = 0;

	static int seq_num = 1;
	int fd;
	char share_filename[seq_num];
	sprintf(share_filename, "%s%04d", P_D_SHARE, seq_num);
	inc(seq_num);//读取文件序号+1
	fd = open(share_filename, O_RDONLY);//打开文件
	set_lock(fd, F_RDLCK);//设置文件锁
	read(fd, &(s->kind), 4);//帧类型
	read(fd, &(s->seq), 4);//发送序号
	read(fd, &(s->ack), 4);//接收序号
	if (s->kind == data)read(fd, s->info.data, MAX_PKT);//数据包
	set_lock(fd, F_UNLCK);//解开文件锁
	close(fd);
	printf("数据链路层已从物理层读入帧数据\n");
}

/*发送方向物理层发送帧*/
void to_physical_layer(frame *s)
{
	static int seq_num = 1;
	char share_filename[MAX_FILE_LEN];//写入文件名
	int fd;
	sprintf(share_filename, "%s%04d", D_P_SHARE, seq_num);//文件名
	inc(seq_num);//文件序号+1
	fd = open(share_filename, O_WRONLY | O_CREAT, 0644);//打开文件
	set_lock(fd, F_RDLCK);//设置文件锁
	write(fd, &(s->kind), 4);//帧类型
	write(fd, &(s->seq), 4);//发送序号
	write(fd, &(s->ack), 4);//接收序号
	if (s->kind == data)write(fd, s->info.data, MAX_PKT);//数据包写数据
	set_lock(fd, F_UNLCK);//解开文件锁
	close(fd);//关文件
	int pid = FindPidByName("./sender_physical");//获得网络层进程的pid
	kill(pid, 40);//发送信号让物理层读文件
	printf("数据链路层已向物理层写入文件\n");
}

/*物理层读取链路层*/
void from_datalink_layer(frame *s)
{
	static int seq_num = 1;
	char share_file_name[MAX_FILE_LEN];
	int fd = -1;
	sprintf(share_file_name, "%s%04d", D_P_SHARE, seq_num);
	inc(seq_num);
	while (fd == -1)
	{
		fd = open(share_file_name, O_RDONLY);
	}
	/*加锁，读取文件*/
	set_lock(fd, F_RDLCK);
	read(fd, s, sizeof(frame));
	set_lock(fd, F_UNLCK);//读完开锁

	close(fd);
}

/*------------------------------------------冕------------------------------------------*/
/*物理层之间传输*/
void send_to_phy(frame *s,int sockfd)
{
	if (s->kind == nak || s->kind == ack)//ack或者nak帧传12字节
		write(sockfd, s, FRAMEHEAD);
	else//数据帧传1036字节
		write(sockfd, s, sizeof(frame));
}

static event_type e;

static void TIMEOUT()
{
	e = timeout;
}

static void CksumErr()
{
	e = cksum_err;
}

static void FrameArrival()
{
	e = frame_arrival;
}

static void NLReady()
{
	e = network_layer_ready;
}

void wait_for_event(event_type* event)
{
	signal(SIGALRM, TIMEOUT);
	signal(35, CksumErr);
	signal(36, FrameArrival);
	signal(37, NLReady);
	pause();
	*event = e;
	return;
}

void enable_network_layer()
{
	char buf[128] = { 0 };
	FILE *fp = NULL;
	int pid;
	if (getpid() == FindPidByName("./sender_datalink")) {//如果是sender方的信号，则发给sender方
		pid = FindPidByName("./sender_network");
	}
	else if (getpid() == FindPidByName("./receiver_datalink")) {//如果是receiver方的信号，则发给receiver方
		pid = FindPidByName("./receiver_network");
	}
	kill(pid, 38);
}

void disable_network_layer()
{
	char buf[128] = { 0 };
	FILE *fp = NULL;
	int pid;
	if (getpid() == FindPidByName("./sender_datalink")) {//如果是sender方的信号，则发给sender方
		pid = FindPidByName("./sender_network");
	}
	else if (getpid() == FindPidByName("./receiver_datalink")) {//如果是receiver方的信号，则发给receiver方
		pid = FindPidByName("./receiver_network");
	}
	kill(pid, 39);
}







//----------------------------------工具函数-------------------------

void sysUsecTime()
{
	struct timeval    tv;//tv.tv_usec就是毫秒
	gettimeofday(&tv);
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
Status ListInsert(LinkList *L, int i, ElemType e) {
	LinkList s, p = *L;
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
Status Listadd(LinkList *L, ElemType e) {
	LinkList s, p = *L;
	while (p->next) {
		p = p->next;
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
Status ListInsert_order(LinkList *L, ElemType e) {
	LinkList s, p = *L;
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
int create_L(LinkList *L)
{
	LinkList p;
	int i, n = 1, m;
	*L = (LNode *)malloc(sizeof(LNode));
	if (*L == NULL)exit(OVERFLOW);
	(*L)->next = NULL;
	return OK;
}

//销毁链表
int destroy_L(LinkList *L)
{
	LinkList q, p = *L;
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
Status Listdelete(LinkList *L, int i, ElemType *e) {
	LinkList q, p = *L;
	int pos = 0;
	while (p->next&&pos < i - 1) {
		p = p->next;
		pos++;
	}
	if (p->next == NULL || pos > i - 1)return ERROR;
	q = p->next;
	*e = q->data;
	p->next = q->next;
	free(q);
	return OK;
}

//寻找值e的位置i，从1开始计数
Status List_find(LinkList *L, int *i, ElemType e) {
	LinkList q, p = (*L)->next;
	int pos = 0, len = ListLength(*L);
	while (p) {
		if (p->data == e)break;
		p = p->next;
		pos++;
	}
	if (p == NULL)return ERROR;
	*i = pos + 1;
	return OK;
}

/*------------------------------------------舟------------------------------------------*/
//-----------------------------计时器-------------------------------
LinkList timeL;//使用的链表
int nowtime, nowusec;//现在是第几帧、第几毫秒
struct timeval tv;//用于获取当前时间

/*启动第k帧的定时器*/
void start_timer(seq_nr k)
{
	if (k < nowtime)return;
	Listadd(&timeL, k - nowtime);
}
/*停止第k帧的定时器*/
void stop_timer(seq_nr k)
{
	int pos ;
	//List_find(&timeL, k, e);
	//Listdelete(&timeL, pos, e);
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
	if (tv.tv_usec == nowusec)return;//触发毫秒时间中断
	int n;
	n = timeL->next->data;//获取当前第一个定时器的时间
	n--;//时间减一
	if (n == 0)//计时器超时
	{
		int e;
		Listdelete(&timeL, 1, &e);//删除第一个计时器
		//发送timeout事件，等待填入
	}
}

/*定时器初始化，请在开始时使用*/
void mytimer_create(void)
{
	nowtime = 0;//初始化当前帧

	gettimeofday(&tv);//获取当前时间
	nowusec = tv.tv_usec;//初始化当前时间
}
