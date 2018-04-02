TARGET = lwe_common
C_FLAGS += -Wall -Werror -std=c++14
C_INCLUDES += -I ../include -I ../../../glm

SOURCES = lwe_logging.cpp

$(TARGET).a: $(TARGET).o
		ar rcs $@ $^

$(TARGET).o: $(SOURCES)
		$(CXX) $(C_FLAGS) $(C_INCLUDES) -c -o $@ $<

clean:
		rm -f *.o *.a
