#include "../common/common.h"
  
int main()
{
      prctl(PR_SET_NAME, "sender_datalink");
  frame s;
  packet buffer;
  //网络层通知读文件的信号
  signal(SIG_SEND_LINK_READ, key_from_network_layer_enable);
  
  //死循环从网络层读文件，往物理层写文件
  while(true){
    from_network_layer(&buffer);
    s.info=buffer;
    to_physical_layer(&s);
  }
  return 0;
}
