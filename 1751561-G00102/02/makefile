
# 源文件、目标文件和可执行文件
OBJS   := receiver2_datalink.o sender2_datalink.o receiver2_network.o sender2_network.o common.o receiver2_physical.o sender2_physical.o
TARGET := receiver2_datalink sender2_datalink receiver2_network sender2_network receiver2_physical sender2_physical

# link
$(TARGET):$(OBJS)
	gcc receiver2_physical.o common.o -o receiver2_physical
	gcc sender2_physical.o common.o -o  sender2_physical
	gcc receiver2_datalink.o common.o -o receiver2_datalink 
	gcc sender2_datalink.o common.o -o sender2_datalink
	gcc receiver2_network.o common.o -o receiver2_network
	gcc sender2_network.o common.o -o sender2_network

# compile
common.o:
	gcc -c ../common/common.c -o common.o

# clean
clean:
	rm -fr *.o
	rm -fr $(TARGET)
	rm -fr network_datalink.share.*,datalink_network.share.*,datalink_physical.share.*,physical_datalink.share.*