#include "../common/common.h"
//#include "common.h"

static int r_en = 0;//为可读信号，链路层写完文件发送信号告知物理层可读取，此时r_en由0变1，读完再变回0
static void EnPhy(int signo)
{
	r_en = 1;
}



int main()
{
	prctl(PR_SET_NAME, "sender_physical");
	/*注册信号处理函数*/
	signal(SIG_PHY_READ, EnPhy);//可读信号到来

	 frame s;

	 /*建立套接字*/
	 int sockfd;
	 int n;
	 struct sockaddr_in sock_addr;
	 if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		 printf("error from socket\n");
		 return;
	 }
	 memset(&sock_addr, 0, sizeof(sock_addr));
	 sock_addr.sin_family = AF_INET;
	 serv_addr.sin_addr.s_addr = inet_addr(RIP);
	 serv_addr.sin_port = htons(DEFAULT_PORT);

	 while (connect(*sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0) {
		 continue;
	 }

	 /*传输文件*/
	 while (true) {
		 if (r_en == 0)continue;

		 from_datalink_layer(&s);
		 send_to_phy(&s, sockfd);

		 r_en = 0;
	 }

	 return 0;
}
