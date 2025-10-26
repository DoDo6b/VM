CC = g++
CFLAGS = -DNDEBUG -o3 -g -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual \
         -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
         -Wformat-signedness -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self \
         -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default \
         -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing \
         -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow \
         -fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla -Wno-deprecated

BUILD_DIR = src/build

TARGETS = translate vm agen

COMMON_SRCS = $(shell find src/included src/defines -name '*.c')

TRANSLATE_SRCS = $(shell find src/translator src/structures -name '*.c')
VM_SRCS = $(shell find src/vm src/structures -name '*.c')
AGEN_SRCS = $(shell find src/animagen -name '*.c')

TRANSLATE_OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(TRANSLATE_SRCS) $(COMMON_SRCS))
VM_OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(VM_SRCS) $(COMMON_SRCS))
AGEN_OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(AGEN_SRCS) $(COMMON_SRCS))

all: $(TARGETS)

translate: $(TRANSLATE_OBJS)
	@$(CC) $(CFLAGS) -DTARGET_TRNSLT -o $@ $(TRANSLATE_OBJS)

vm: $(VM_OBJS)
	@$(CC) $(CFLAGS) -o $@ $(VM_OBJS)

agen: $(AGEN_OBJS)
	@$(CC) $(CFLAGS) -o $@ $(AGEN_OBJS)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR) $(TARGETS)

rebuild: clean all

.PHONY: all clean rebuild