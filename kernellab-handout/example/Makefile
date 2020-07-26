obj-m := debugfs_example.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
WARN_FLAGS += -Wall

.PHONY: default
defualt:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) M=$(PWD) clean ;
	sudo rmmod debugfs_example.ko
