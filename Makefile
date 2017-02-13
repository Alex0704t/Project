# Program name.
PROG:=					Project

TOOLCHAIN:=				arm-none-eabi-
AR:=					${TOOLCHAIN}ar
CC:=					${TOOLCHAIN}gcc
OBJCOPY:=				${TOOLCHAIN}objcopy
SIZE:=					${TOOLCHAIN}size
STLINK:=				st-link_cli
#LDSCRIPT:=				stm32f407vg.ld
LDSCRIPT:=				standalone.ld
REMOVE:=				rm -f	
DEBUG:=					-g2
OPTIM:=					-O2
#OPTIM:=					-O3
STANDART:=				-std=gnu11
#STANDART:=				-std=gnu99
M_CPU:=					cortex-m4
M_FPU:=					fpv4-sp-d16
M_FLOAT:=				hard
#M_FLOAT:=				softfp

# Directories
#STM lib directory
STM_CORE_DIR:=			CMSIS/Include
STM_DIR:=				Device
#Headers & source directory
INC_DIRS:=				. $(STM_CORE_DIR) $(STM_DIR)
SRC_DIRS:=				. $(STM_DIR)
LIBS:=					libgcc libm libc 
LIB_DIRS:=

USER_DIR:=				User
include $(USER_DIR)/Makefile
USB_DIR:=				USB_lib
include $(USB_DIR)/Makefile
MP3_DIR:=				Audio
include $(MP3_DIR)/Makefile

SRCS:=					$(wildcard $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c)))
OBJS:=    				$(SRCS:.c=.o)
DEPS:=					$(wildcard $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.d)))
include $(DEPS)
#STARTUP:=				$(STM_DIR)/startup/startup_stm32f407xx.s
STARTUP:=				$(STM_DIR)/startup/startup_stm32f4xx.c
#SRCS+=					$(STARTUP)
#OBJS+=					$(STARTUP:.s=.o)

# Compiler flags.
CFLAGS:=				$(DEBUG) $(OPTIM) $(STANDART)
CFLAGS+=				-Wall
CFLAGS+=				-T$(LDSCRIPT)
CFLAGS+=				-mcpu=$(M_CPU)
CFLAGS+=				-mthumb
#CFLAGS+=				-mlittle-endian -mthumb -mthumb-interwork
CFLAGS+=				-mfloat-abi=$(M_FLOAT) -mfpu=$(M_FPU)
CFLAGS+=				-ffreestanding
CFLAGS+=				-ggdb
#CFLAGS+=				-MD

# Preprocessor flags.
CPPFLAGS:=				-DSTM32F407xx
#CPPFLAGS+=				-DUSE_STD_PERIPH_DRIVER
#CPPFLAGS+= 			-DUSE_HAL_DRIVER
CPPFLAGS+=				-D__GNUC__
CPPFLAGS+=				$(foreach dir, $(INC_DIRS), -I$(dir))
CPPFLAGS+=				-Dinline= -mthumb
CPPFLAGS+=				-D__FPU_USED


# Linker flags.
LDFLAGS:=				-nostartfiles
LDFLAGS+=				-Xlinker -o$(PROG).elf
LDFLAGS+=				-Xlinker -M
LDFLAGS+=				-Xlinker -Map=$(PROG).map
#LDFLAGS+=  			--specs=nosys.specs 
LDFLAGS+=				--specs=nano.specs
LDFLAGS+= 				$(foreach dir, $(LIB_DIRS), -L$(dir)) 
LDFLAGS+= 				$(patsubst lib%, -l%, $(LIBS))
LDFLAGS+=				-u _printf_float

#Targets
all: begin $(PROG).bin size finished end
$(PROG).bin: $(PROG).elf
	$(OBJCOPY) $(PROG).elf -O ihex $(PROG).hex
$(PROG).elf: $(OBJS) startup.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) startup.o $(LDFLAGS)
$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -MD -o $@
startup.o: $(STARTUP)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -O1 $(STARTUP) -o startup.o
size:
	$(SIZE) --format=berkeley $(PROG).elf
#Define Messages
#English
MSG_ERRORS_NONE= 		Errors: none
MSG_BEGIN= 				Begin ---------
MSG_END= 				End  --------
MSG_CLEANING= 			Cleaning the project ---
begin:
	@echo
	@echo $(MSG_BEGIN)
finished:
	@echo $(MSG_ERRORS_NONE)
end:
	@echo $(MSG_END)
	@echo
clean: begin clean_list end
clean_list :
	@echo $(MSG_CLEANING)
	$(REMOVE) $(OBJS)
	$(REMOVE) $(DEPS)
	$(REMOVE) $(PROG).hex
	$(REMOVE) $(PROG).bin
	$(REMOVE) $(PROG).elf
	$(REMOVE) $(PROG).map
	$(REMOVE) startup.o
rebuild: begin clean_list $(PROG).bin size finished end	
download: begin program end
rebuild_load: begin clean_list $(PROG).bin size finished program end
program: $(PROG).bin
	$(STLINK) -p $(PROG).hex -run
erase: begin full_erase end	
full_erase:
	$(STLINK) -me	
print:
	@echo INC_DIRS
	@echo $(INC_DIRS)
	@echo SRC_DIRS
	@echo $(SRC_DIRS)
	@echo LIBS:
	@echo $(LIBS)
	@echo SRCS:
	@echo $(notdir $(SRCS))
	@echo OBJS:
	@echo $(notdir $(wildcard $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.o))))
	@echo DEPS:
	@echo $(notdir $(wildcard $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.d))))
	@echo HEADERS:
	@echo $(notdir $(wildcard $(foreach dir, $(INC_DIRS), $(wildcard $(dir)/*.h))))
	@echo CFLAGS:
	@echo $(CFLAGS)
	@echo CPPFLAGS:
	@echo $(CPPFLAGS)
	@echo LDFLAGS:
	@echo $(LDFLAGS)

#=======================================================
specs_file := D:/Eclipse/.metadata/.plugins/.org.eclipse.cdt.make.core/specs.cpp
discovery_log = discovery.log
discovery:
	-@$(RM) $(discovery_log)
	@echo — discovery started… >> $(discovery_log)
	@echo $(CC) $(INCLUDES) $(CFLAGS) -E -P -v -dD ‘$(specs_file)’ >> $(discovery_log)
	$(CC) $(INCLUDES) $(CFLAGS) -E -P -v -dD ‘$(specs_file)’ >> $(discovery_log) 2>&1
	$(CC) $(INCLUDES) $(CFLAGS) -E -P -v -dD ‘$(specs_file)’
	@echo — discovery done! >> $(discovery_log)	