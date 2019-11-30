
# 源文件、目标文件和可执行文件
OBJS   := sender5_datalink.o sender5_network.o common.o receiver5_physical.o sender5_physical.o
TARGET := sender5_datalink sender5_network receiver5_physical sender5_physical

# link
$(TARGET):$(OBJS)
	gcc receiver5_physical.o common.o -o receiver5_physical
	gcc sender5_physical.o common.o -o  sender5_physical
	gcc sender5_datalink.o common.o -o sender5_datalink
	gcc sender5_network.o common.o -o sender5_network

# compile
common.o:
	gcc -c ../common/common.c -o common.o

# clean
clean:
	rm -fr *.o
	rm -fr $(TARGET)
	rm -fr network_datalink.share.*,datalink_network.share.*,datalink_physical.share.*,physical_datalink.share.*