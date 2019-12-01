
# 源文件、目标文件和可执行文件
OBJS   := receiver1_datalink.o sender1_datalink.o receiver1_network.o sender1_network.o common.o receiver1_physical.o sender1_physical.o
TARGET := receiver1_datalink sender1_datalink receiver1_network sender1_network receiver1_physical sender1_physical

# link
$(TARGET):$(OBJS)
	gcc receiver1_physical.o common.o -o receiver1_physical
	gcc sender1_physical.o common.o -o  sender1_physical
	gcc receiver1_datalink.o common.o -o receiver1_datalink 
	gcc sender1_datalink.o common.o -o sender1_datalink
	gcc receiver1_network.o common.o -o receiver1_network
	gcc sender1_network.o common.o -o sender1_network

# compile
common.o:
	gcc -c ../common/common.c -o common.o

# clean
clean:
	rm -fr *.o
	rm -fr $(TARGET)
	rm -fr network_datalink.share.*
	rm -fr datalink_network.share.*
	rm -fr datalink_physical.share.*
	rm -fr physical_datalink.share.*
	rm -fr protocol.log
