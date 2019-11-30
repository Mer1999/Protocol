include "../common/common.h"
  
int main()
{
  frame s;
  packet buffer;
  
  while(true){
    from_network_layer(&buffer);
    s.info=buffer;
    to_physical_layer(&s);
  }
  return 0;
}
