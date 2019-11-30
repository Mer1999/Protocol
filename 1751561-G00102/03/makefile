
# 源文件、目标文件和可执行文件
OBJS   := receiver3_datalink.o sender3_datalink.o receiver3_network.o sender3_network.o common.o receiver3_physical.o sender3_physical.o
TARGET := receiver3_datalink sender3_datalink receiver3_network sender3_network receiver3_physical sender3_physical

# link
$(TARGET):$(OBJS)
	gcc receiver3_physical.o common.o -o receiver3_physical
	gcc sender3_physical.o common.o -o  sender3_physical
	gcc receiver3_datalink.o common.o -o receiver3_datalink 
	gcc sender3_datalink.o common.o -o sender3_datalink
	gcc receiver3_network.o common.o -o receiver3_network
	gcc sender3_network.o common.o -o sender3_network

# compile
common.o:
	gcc -c ../common/common.c -o common.o

# clean
clean:
	rm -fr *.o
	rm -fr $(TARGET)
	rm -fr network_datalink.share.*,datalink_network.share.*,datalink_physical.share.*,physical_datalink.share.*