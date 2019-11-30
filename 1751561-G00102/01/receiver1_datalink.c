#include "../common/common.h"
  
int main()
{
  frame r;
  event_type event;
  //物理层通知读文件的信号
  signal(SIG_RECV_LINK_READ, key_from_physical_layer_enable);
  
  //死循环从物理层读文件，往网络层写文件
  while(true){
    wait_for_event(&event);
    from_physical_layer(&r);
    to_network_layer(&r.info);
  }
  return 0;
}
