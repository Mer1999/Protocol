
# 源文件、目标文件和可执行文件
OBJS   := sender6_datalink.o sender6_network.o common.o receiver6_physical.o sender6_physical.o
TARGET := sender6_datalink sender6_network receiver6_physical sender6_physical

# link
$(TARGET):$(OBJS)
	gcc receiver6_physical.o common.o -o receiver6_physical
	gcc sender6_physical.o common.o -o  sender6_physical
	gcc sender6_datalink.o common.o -o sender6_datalink
	gcc sender6_network.o common.o -o sender6_network

# compile
common.o:
	gcc -c ../common/common.c -o common.o

# clean
clean:
	rm -fr *.o
	rm -fr $(TARGET)
	rm -fr network_datalink.share.*,datalink_network.share.*,datalink_physical.share.*,physical_datalink.share.*