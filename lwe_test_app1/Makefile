TARGET = lwe_test_app1
C_FLAGS += -Wall -Werror -std=c++14
C_INCLUDES += -I ../lwe_common/include -I ../lwe_graphics/include -I ../../glm
C_LIBS += ../lwe_common/src/lwe_common.a ../lwe_graphics/src/lwe_graphics.a

SOURCES =\
	main.cpp \

$(TARGET): $(SOURCES)
		$(CXX) $(C_FLAGS) $(C_INCLUDES) -o $@ $< $(C_LIBS)

clean:
		rm -f *.o $(TARGET)
