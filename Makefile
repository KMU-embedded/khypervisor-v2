include config.mk

export ROOT_PATH	= $(CURDIR)
export CONFIG_PATH	= $(ROOT_PATH)/config.mk

ARCH_DIRS		= $(ROOT)/$(ARCH) $(ROOT)/$(ARCH)/$(VERSION)
CORE_DIRS		= $(ROOT)/$(CORE) $(ROOT)/$(CORE)/sched $(ROOT)/$(CORE)/vdev
DRIVERS_DIRS		= $(ROOT)/$(DRV)
LIB_DIRS		= $(ROOT)/$(LIB)/c/src $(ROOT)/$(LIB)/c/src/arch-arm $(ROOT)/$(LIB)/c/src/sys-baremetal	\
			  $(ROOT)/$(LIB)/c/src/sys-baremetal/arch-arm
TESTS_DIRS		= $(ROOT)/$(TESTS) $(ROOT)/$(TESTS)/libs

BIN			= $(BUILD_DIR)/$(TARGET).bin
LD_SCRIPT		= $(PROJECT).lds.S
OUTPUT			= $(TARGET).axf
MAP			= $(PROJECT).map

CC			= $(CROSS_COMPILE)gcc
LD			= $(CROSS_COMPILE)ld
NM			= $(CROSS_COMPILE)nm
OBJCOPY			= $(CROSS_COMPILE)objcopy

INCLUDES		= -I $(INC)
INCLUDES		+= -I $(INC)/$(LIB)/c 
INCLUDES		+= -I $(INC)/$(LIB)/bsd
INCLUDES		+= -I $(INC)/$(LIB)
INCLUDES		+= -I $(INC)/$(CORE)
INCLUDES		+= -I $(INC)/$(DRV)

INCLUDES		+= -I $(ARCH)/$(ARM)

INCLUDES		+= -I $(CORE)/libhw
INCLUDES		+= -I $(CORE)
INCLUDES		+= -I $(CORE)/$(INC)

INCLUDES		+= -I $(LIB)

INCLUDES		+= -I $(TESTS)
INCLUDES		+= -I $(TESTS)/libs

CPPFLAGS		= $(CONFIG_FLAG) $(INCLUDES) -ffreestanding -nostdlib -nodefaultlibs -nostartfiles $(DEBUG_FLAG)
CPPFLAGS		+= -Wall


all:	ARCH CORE DRIVERS LIB TESTS \
	$(OUTPUT) $(MAP)

ARCH:
	@for dir in $(ARCH_DIRS); do \
	$(MAKE) -C $$dir all; \
	if [ $$? != 0 ]; then exit 1; fi; \
	done

CORE:
	@for dir in $(CORE_DIRS); do \
	$(MAKE) -C $$dir all; \
	if [ $$? != 0 ]; then exit 1; fi; \
	done

DRIVERS:
	@for dir in $(DRIVERS_DIRS); do \
	$(MAKE) -C $$dir all; \
	if [ $$? != 0 ]; then exit 1; fi; \
	done

LIB:
	@for dir in $(LIB_DIRS); do \
	$(MAKE) -C $$dir all; \
	if [ $$? != 0 ]; then exit 1; fi; \
	done

TESTS:
	@for dir in $(TESTS_DIRS); do \
	$(MAKE) -C $$dir all; \
	if [ $$? != 0 ]; then exit 1; fi; \
	done


$(MAP): $(OUTPUT)
	$(NM) $(BUILD)/$< > $(BUILD)/$@

$(OUTPUT): $(MACHINE).lds $(OBJS)
	$(CC) $(CPPFLAGS) -e __start -T $(BUILD)/$(MACHINE).lds -o $(BUILD)/$@ \
		$(BUILD)/$(ARCH)/*.o	\
		$(BUILD)/$(ARCH)/$(VERSION)/*.o	\
		$(BUILD)/$(CORE)/*.o	\
		$(BUILD)/$(CORE)/sched/*.o	\
		$(BUILD)/$(CORE)/vdev/*.o	\
		$(BUILD)/$(TESTS)/*.o		\
		$(BUILD)/$(TESTS)/libs/*.o	\
		$(BUILD)/$(DRV)/*.o		\
		$(BUILD)/$(LIB)/c/src/*.o	\
		$(BUILD)/$(LIB)/c/src/arch-arm/*.o	\
		$(BUILD)/$(LIB)/c/src/sys-baremetal/arch-arm/*.o	\
		$(BUILD)/$(LIB)/c/src/sys-baremetal/*.o

$(BIN): $(OUTPUT)
	$(OBJCOPY) -O binary $(OUTPUT) $(BIN)

$(MACHINE).lds: $(LD_SCRIPT) Makefile
	$(CC) $(CPPFLAGS) -E -P -C -o $(BUILD)/$@ $<

%: force
	$(MAKE) -C $(KERNEL_SRC) $@

force: ;

Makefile: ;

clean:
	rm -rf $(ROOT)/$(BUILD) 

.PHONY: all clean config.mk
