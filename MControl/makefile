rwildcard = $(foreach d,$(wildcard $(addsuffix *,$(1))),$(call rwildcard,$(d)/,$(2)) $(filter $(subst *,%,$(2)),$(d)))

CROSS_PREFIX := arm-raspbian10-linux-gnueabihf-
CXX := g++
STD := c++17

ifeq ($(OS),Windows_NT)
	CXX := $(CROSS_PREFIX)$(CXX)
	RM-R := del /s /Q
	CP := copy
else
	RM-R := rm -r
	CP := cp
endif

SRC_DIR := src
OBJ_DIR := obj
OUT_DIR := out

PROGRAM := $(OUT_DIR)/pi_program

SRCS := $(call rwildcard,$(SRC_DIR)/,*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%=$(OBJ_DIR)/%.o)

COPT := -O3
CPPFLAGS := -pthread -Isrc -MMD -MP
CFLAGS := -pthread -Wall -fpermissive
LDFLAGS := -Wall -Lsrc
LDLIBS := -lpigpio -lpthread -lrt

.PHONY: build clean

build: $(PROGRAM)

clean:
	$(RM-R) $(OBJ_DIR)
	$(RM-R) $(OUT_DIR)

$(OBJ_DIR)/%.cpp.o : $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(COPT) -c -o $(OBJ_DIR)/$(@F) -std=$(STD) $(CPPFLAGS) $(CFLAGS) $<

$(PROGRAM): $(OBJS) | $(OUT_DIR)
	$(CXX) -o $@ $(LDFLAGS) $(foreach file,$(^F),$(OBJ_DIR)/$(file)) $(LDLIBS)

$(OUT_DIR) $(OBJ_DIR):
	mkdir $@

-include $(OBJS:.o=.d)