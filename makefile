TARGET = lineFollower
EXTRAINCDIR = lib/inc
CSTANDARD = gnu99
CPPSTANDARD = gnu++98
BUILDDIRBASE = build
MCU = atmega8
F_CPU = 8000000
BUILDDIR = $(BUILDDIRBASE)/release
RM = rm -rf
MKDIR = mkdir -p

GCC = avr-gcc
SIZE = avr-size
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRDUDE = avrdude

SRC = $(TARGET).c
SRC += lib/pid.c
SRC += lib/asuro.c lib/adc.c lib/adc_low.c lib/encoder.c lib/encoder_low.c lib/globals.c \
       lib/i2c.c lib/lcd.c lib/leds.c lib/motor.c lib/motor_low.c lib/my_motor_low.c \
       lib/print.c lib/printf.c lib/rc5.c lib/sound.c lib/switches.c lib/time.c lib/uart.c \
       lib/ultrasonic.c lib/version.c

CARGS = -I$(EXTRAINCDIR)
CARGS += -Os
CARGS += -std=$(CSTANDARD)
CARGS += -mmcu=$(MCU)
CARGS += -Wall -Wextra -pedantic
#CARGS += -Werror
CARGS += -funsigned-char
CARGS += -funsigned-bitfields
CARGS += -fpack-struct
CARGS += -fshort-enums
CARGS += -ffunction-sections
CARGS += -fdata-sections
CARGS += -Wshadow
CARGS += -Wpointer-arith -Wcast-qual -Wstrict-prototypes
CARGS += -Wno-main -Wno-write-strings -Wno-unused-parameter
CARGS += -DF_CPU=$(F_CPU)
LINKER = -Wl,--relax,-Map,$(@:.elf=.map),--gc-sections,--cref
LINKER += -lm

ifeq ($(DEBUG),1)
	CARGS += -DDEBUG
	CARGS += -g
endif

PROGRAMMER = arduino
ISPPORT = /dev/tty.usbserial-A100OZQ1
ISPBAUD = 57600

# -----------------------------------------------------------------------------

OBJ += $(addprefix $(BUILDDIR)/, $(SRC:.c=.o))
OBJ += $(addprefix $(BUILDDIR)/, $(CPPSRC:.cpp=.o))

.PHONY: all
all: $(BUILDDIR)/$(TARGET).hex $(BUILDDIR)/$(TARGET).bin
	@cp $(BUILDDIR)/$(TARGET).hex $(TARGET)_release.hex

.PHONY: debug
debug:
	$(MAKE) BUILDDIR=build/debug DEBUG=1 all
	@cp $(BUILDDIR)/$(TARGET).hex $(TARGET)_debug.hex

.PHONY: debugProgram
debugProgram:
	$(MAKE) BUILDDIR=build/debug DEBUG=1 program

.PHONY: program
program: $(BUILDDIR)/$(TARGET).hex
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -b $(ISPBAUD) -P $(ISPPORT) -e -D -U $(BUILDDIR)/$(TARGET).hex

.PHONY: clean
clean:
	$(RM) $(BUILDDIRBASE)
	$(RM) $(TARGET)_debug.hex
	$(RM) $(TARGET)_release.hex

-include $(OBJ:.o=.d)

$(BUILDDIR)/%.o: %.c
	@$(MKDIR) $(BUILDDIR)/$(dir $<)
	$(GCC) $(CARGS) -c $< -o $@
	@$(GCC) $(CARGS) -MM $*.c > $(BUILDDIR)/$*.d
	@mv -f $(BUILDDIR)/$*.d $(BUILDDIR)/$*.d.tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.o:|' < $(BUILDDIR)/$*.d.tmp > $(BUILDDIR)/$*.d
	@sed -e 's/.*://' < $(BUILDDIR)/$*.d.tmp | sed -e 's/\\$$//' | \
		fmt -1 | sed -e 's/^ *//' | sed -e 's/$$/:/' >> $(BUILDDIR)/$*.d
	@rm -f $(BUILDDIR)/$*.d.tmp

$(BUILDDIR)/%.o: %.cpp
	@$(MKDIR) $(BUILDDIR)/$(dir $<)
	$(GPP) $(CPPARGS) -c $< -o $@
	@$(GPP) $(CPPARGS) -MM $*.cpp > $(BUILDDIR)/$*.d
	@mv -f $(BUILDDIR)/$*.d $(BUILDDIR)/$*.d.tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.o:|' < $(BUILDDIR)/$*.d.tmp > $(BUILDDIR)/$*.d
	@sed -e 's/.*://' < $(BUILDDIR)/$*.d.tmp | sed -e 's/\\$$//' | \
		fmt -1 | sed -e 's/^ *//' | sed -e 's/$$/:/' >> $(BUILDDIR)/$*.d
	@rm -f $(BUILDDIR)/$*.d.tmp

$(BUILDDIR)/$(TARGET).elf: $(OBJ)
	$(GCC) $(CARGS) $(OBJ) $(LINKER) --output $@
	$(SIZE) $@

$(BUILDDIR)/$(TARGET).hex: $(BUILDDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@
	$(OBJDUMP) -h -S $< > $(@:.hex=.lss)

$(BUILDDIR)/$(TARGET).bin: $(BUILDDIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

