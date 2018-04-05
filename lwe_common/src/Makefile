TARGET = lwe_common
C_FLAGS += -Wall -Werror -std=c++14
C_INCLUDES += -I ../include -I ../../../glm

SOURCES := lwe_logging.cpp lwe_system.cpp

../../out/$(TARGET).a: $(SOURCES:cpp=o)
		ar rcs $@ $^
		rm -f *.o

%.o: %.cpp
		$(CXX) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

clean:
		rm -f ../../out/$(TARGET).a
