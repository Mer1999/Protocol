
# 源文件、目标文件和可执行文件
OBJS   := sender4_datalink.o sender4_network.o common.o receiver4_physical.o sender4_physical.o
TARGET := sender4_datalink sender4_network receiver4_physical sender4_physical

# link
$(TARGET):$(OBJS)
	gcc receiver4_physical.o common.o -o receiver4_physical
	gcc sender4_physical.o common.o -o  sender4_physical
	gcc sender4_datalink.o common.o -o sender4_datalink
	gcc sender4_network.o common.o -o sender4_network

# compile
common.o:
	gcc -c ../common/common.c -o common.o

# clean
clean:
	rm -fr *.o
	rm -fr $(TARGET)
	rm -fr network_datalink.share.*,datalink_network.share.*,datalink_physical.share.*,physical_datalink.share.*