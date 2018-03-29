mkfile_path      := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir      := $(patsubst %/,%,$(dir $(mkfile_path)))
current_dir_name := $(notdir $(current_dir))

TARGET     := liblwe
C_FLAGS    += -shared -Wall -Werror -std=c++14
C_INCLUDES += -I $(current_dir) -I $(current_dir)/../glm

SOURCES := \
	common/logging.cpp \
	rendering/renderer.cpp \
	system/graphics_system.cpp \

all: $(TARGET)

$(TARGET):
	$(CXX) $(C_FLAGS) $(C_INCLUDES) -o $(TARGET).so $(SOURCES)
