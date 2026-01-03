PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

SRC_DIR := $(PWD)/src
BUILD_DIR := $(PWD)/build
BUILD_FILES += $(SRC_DIR)/*.ko
MODULE_NAME ?= hello_world

CLANG_FORMAT := clang-format
CLANG_FORMAT_FLAGS += -i
FORMAT_FILES := $(SRC_DIR)/*.c

$(shell mkdir -p $(BUILD_DIR))

kbuild:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
	mv $(BUILD_FILES) $(BUILD_DIR)/.

format:
	$(CLANG_FORMAT) $(CLANG_FORMAT_FLAGS) $(FORMAT_FILES)

check:
	python3 $(SRC_DIR)/checker/main.py $(MODULE_NAME)

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

insmod:
	insmod $(BUILD_DIR)/$(MODULE_NAME).ko

rmmod:
	rmmod $(MODULE_NAME)

.PHONY: kbuild clean format check insmod rmmod
