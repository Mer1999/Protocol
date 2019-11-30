SUBDIRS = $(shell pwd)

.PHONY:all
all:
	@for subdir in $(SUBDIRS)/**/;do	$(MAKE) -C $$subdir;	cp $$subdir/*[^.c][^.o][^Makefile] $(SUBDIRS);	done

.PHONY:clean
clean:
	@for subdir in $(SUBDIRS)/**/;do	$(MAKE) -C $$subdir clean;	done
	@rm sender1_network sender1_datalink sender1_physical receiver1_network receiver1_datalink receiver1_physical \
	sender2_network sender2_datalink sender2_physical receiver2_network receiver2_datalink receiver2_physical \
	sender3_network sender3_datalink sender3_physical receiver3_network receiver3_datalink receiver3_physical \
	protocol4_network protocol4_datalink protocol4_physical_c protocol4_physical_s \
	protocol5_network protocol5_datalink protocol5_physical_c protocol5_physical_s \
	protocol6_network protocol6_datalink protocol6_physical_c protocol6_physical_s