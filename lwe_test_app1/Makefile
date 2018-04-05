TARGET = lwe_test_app1
C_FLAGS += -Wall -Werror -std=c++14
C_INCLUDES += -I ../lwe_common/include -I ../lwe_graphics/include -I ../../glm
C_LIBS += ../out/lwe_common.a ../out/lwe_graphics.a -framework GLUT -framework OpenGL

SOURCES =\
	main.cpp \

../out/$(TARGET): $(SOURCES)
		$(CXX) $(C_FLAGS) $(C_INCLUDES) -o $@ $(C_LIBS) $<

clean:
		rm -f ../out/$(TARGET)
