CC = g++
CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code\
         -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe\
         -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers\
         -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo\
         -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG\
         -D_EJUDGE_CLIENT_SIDE

BUILD_DIR = src\build

TRANSLATE_TARGET = translate.exe
VM_TARGET = vm.exe

COMMON_SRCS = $(shell for /r src\included %%i in (*.c) do @echo %%i) \
              $(shell for /r src\defines %%i in (*.c) do @echo %%i)

TRANSLATE_SRCS = $(shell for /r src\translator %%i in (*.c) do @echo %%i)
VM_SRCS        = $(shell for /r src\vm %%i in (*.c) do @echo %%i)

TRANSLATE_OBJS = $(COMMON_SRCS:src/%.c=$(BUILD_DIR)/%.o) $(TRANSLATE_SRCS:src/%.c=$(BUILD_DIR)/%.o)
VM_OBJS        = $(COMMON_SRCS:src/%.c=$(BUILD_DIR)/%.o) $(VM_SRCS:src/%.c=$(BUILD_DIR)/%.o)

all: $(TRANSLATE_TARGET) $(VM_TARGET)

$(TRANSLATE_TARGET): $(TRANSLATE_OBJS)
	$(CC) $(CFLAGS) -o $@ $(TRANSLATE_OBJS)

$(VM_TARGET): $(VM_OBJS)
	$(CC) $(CFLAGS) -o $@ $(VM_OBJS)

$(BUILD_DIR)/%.o: src/%.c
	@if not exist $(@D) mkdir $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	if exist $(TRANSLATE_TARGET) del $(TRANSLATE_TARGET)
	if exist $(VM_TARGET) del $(VM_TARGET)

rebuild: clean all

.PHONY: all clean rebuild