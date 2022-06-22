# >> this makefile currently only builds for raspberry pi, not windows <<

rwildcard = $(foreach d,$(wildcard $(addsuffix *,$(1))),$(call rwildcard,$(d)/,$(2)) $(filter $(subst *,%,$(2)),$(d)))

mode ?= release

CROSS_PREFIX := arm-raspbian10-linux-gnueabihf-
CXX := g++
AR := ar

ifeq ($(OS),Windows_NT)
CXX := $(CROSS_PREFIX)$(CXX)
AR := $(CROSS_PREFIX)$(AR)
OS := windows
RM-R := del /s
CP := copy
DIR := \\ 
else #native
OS := native
RM-R := rm -r
CP := cp
DIR := /
endif

SRC_DIR := src
OBJ_DIR := obj
OUT_DIR := out
LIB_DIR := $(OUT_DIR)
HEADER_DIR := $(OUT_DIR)$(DIR)include

LNAME := cpptools
LIBSTATIC := $(LIB_DIR)/lib$(LNAME).a
LIBSHARED := $(LIB_DIR)/lib$(LNAME).so	#currently some code uses singletons so this should not be used

SRCS := $(call rwildcard,$(SRC_DIR)/,*.cpp *.c)
STATIC_OBJS := $(SRCS:$(SRC_DIR)/%=$(OBJ_DIR)/%.o)
SHARED_OBJS := $(SRCS:$(SRC_DIR)/%=$(OBJ_DIR)/%.o-s)

ifeq ($(OS),windows)
HEADERS := $(subst /,\,$(call rwildcard,$(SRC_DIR)/,*.h *.hpp *.inc))
else
HEADERS := $(call rwildcard,$(SRC_DIR)/,*.h *.hpp *.inc)
endif
COPY_HEADERS := $(HEADERS:$(SRC_DIR)%=$(HEADER_DIR)%)

CDEBUG := -g -Og -DDEBUG
CRELEASE := -O3 -DRELEASE

CPPFLAGS := -pthread -Iinclude -MMD -MP
CFLAGS := -Wall -fpermissive
LDFLAGS := -pthread -Wall -Llib
LDLIBS := -lm -lpigpio

ifeq ($(mode),release)
COPT := $(CRELEASE)
else
COPT := $(CDEBUG)
endif

.PHONY: static shared clean copy

static: $(LIBSTATIC) $(COPY_HEADERS)

shared: $(LIBSHARED) $(COPY_HEADERS)

copy: $(COPY_HEADERS)

clean:
	$(RM-R) $(OBJ_DIR)
	$(RM-R) $(OUT_DIR)


$(OBJ_DIR)/%.cpp.o : $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(COPT) -c -o $(OBJ_DIR)/$(@F) -std=c++17 $(CPPFLAGS) $(CFLAGS) $<

$(OBJ_DIR)/%.c.o : $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CXX) $(COPT) -c -o $(OBJ_DIR)/$(@F) -std=c++17 $(CPPFLAGS) $(CFLAGS) $<


$(OBJ_DIR)/%.cpp.o-s : $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(COPT) -c -fPIC -o $(OBJ_DIR)/$(@F) -std=c++17 $(CPPFLAGS) $(CFLAGS) $<

$(OBJ_DIR)/%.c.o-s : $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CXX) $(COPT) -c -fPIC -o $(OBJ_DIR)/$(@F) -std=c++17 $(CPPFLAGS) $(CFLAGS) $<


$(LIBSTATIC): $(STATIC_OBJS) | $(LIB_DIR)
	$(AR) rcs $@ $(foreach file,$(^F),$(OBJ_DIR)/$(file))

$(LIBSHARED): $(SHARED_OBJS) | $(LIB_DIR)
	$(CXX) -shared -o $@ $(foreach file,$(^F),$(OBJ_DIR)/$(file))

$(COPY_HEADERS): $(HEADER_DIR)% : $(SRC_DIR)% | $(HEADER_DIR)
	$(CP) $< $@

$(OBJ_DIR) $(OUT_DIR):
	mkdir $@

$(LIB_DIR) $(HEADER_DIR) : | $(OUT_DIR)
	mkdir $@

-include $(STATIC_OBJS:.o=.d)
-include $(SHARED_OBJS:.o-s=.d)