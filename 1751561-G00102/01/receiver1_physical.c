#include "../common/common.h"
//#include "common.h"

int main(int argc,char* argv[])
{
	sprintf(argv[0],"receiver_physical");//修改进程名
	frame s;

	/*建立套接字*/
	int sockfd_server, sockfd_client;
	struct sockaddr_in sock_addr;
	if ((sockfd_server = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("error from socket\n");
		return;
	}

	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	sock_addr.sin_port = htons(DEFAULT_PORT);

	unsigned int value = 1;
	if (setsockopt(sockfd_server, SOL_SOCKET, SO_REUSEADDR, (void*)&value, sizeof(value)) < 0) {
		perror("error from setsockopt\n");
		return;
	}

	if (bind(sockfd_server, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) {
		printf("error from bind\n");
		return;
	}

	if (listen(sockfd_server, 10) == -1) {
		printf("error from listen\n");
		return;
	}

	while ((sockfd_client = accept(sockfd_server, (struct sockaddr*)NULL, NULL)) == -1) {
		continue;
	}
	/************/

	char share_file_name[MAX_FILE_LEN];
	int fd, seq_num = 0; 
	int nlen = 0;
	while (true) {
		nlen = receive_from_phy(&s, sockfd_server, sockfd_client);//从另一端的物理层获取帧
		if (nlen <= 0)break;
		sprintf(share_file_name, "%s%04d", P_D_SHARE, seq_num);//编辑需要打开的文件名
		inc(seq_num);
		fd = open(share_file_name, O_WRONLY | O_CREAT, 0644);
		if (fd < 0)break;
		write(fd, &s, sizeof(frame));//向文件中写入帧
		int pid = FindPidByName("./receiver_datalink");//获得链路层进程的pid
		kill(pid, 42);//发送信号让链路层层读文件
		close(fd);
	}
	close(sockfd_server);
	close(sockfd_client);
}
