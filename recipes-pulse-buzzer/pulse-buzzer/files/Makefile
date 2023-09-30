obj-m += PulseBuzzer.o
SRC := $(shell pwd)


all:
	$(MAKE) -I*.h -C $(KERNEL_SRC) M=$(SRC) modules

modules_install:
	$(MAKE) -I*.h -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	$(MAKE) -I*.h -C $(KERNEL_SRC) M=$(SRC) clean
