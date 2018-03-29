mkfile_path      := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir      := $(patsubst %/,%,$(dir $(mkfile_path)))
current_dir_name := $(notdir $(current_dir))

TARGET     := lwe_test
C_FLAGS    += -Wall -Werror -std=c++14
C_INCLUDES += -I $(current_dir)/../../src -I $(current_dir)/../../glm

SOURCES := \
	main.cpp \

LIBS := \
	$(current_dir)/../../src/liblwe.so \

all: $(TARGET)

$(TARGET):
	$(CXX) $(C_FLAGS) $(C_INCLUDES) -o $(TARGET) $(SOURCES) $(LIBS)
