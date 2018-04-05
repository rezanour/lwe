TARGET = lwe_graphics
C_FLAGS += -Wall -Werror -std=c++14
C_INCLUDES += -I . -I ../include -I ../../lwe_common/include -I ../../../glm

SOURCES =\
	lwe_graphics.cpp \
	lwe_render_backend.cpp \
	gl/lwe_gl_graphics_device.cpp \
	gl/lwe_gl_graphics_system.cpp \
	gl/lwe_gl_render_backend.cpp \
	rendering/renderer.cpp

../../out/$(TARGET).a: $(SOURCES:cpp=o)
		ar rcs $@ $^
		rm -f */*.o *.o

lwe_graphics.o: lwe_graphics.cpp
		$(CXX) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

lwe_render_backend.o: lwe_render_backend.cpp
		$(CXX) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

gl/lwe_gl_graphics_device.o: gl/lwe_gl_graphics_device.cpp
		$(CXX) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

gl/lwe_gl_graphics_system.o: gl/lwe_gl_graphics_system.cpp
		$(CXX) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

gl/lwe_gl_render_backend.o: gl/lwe_gl_render_backend.cpp
		$(CXX) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

rendering/renderer.o: rendering/renderer.cpp
		$(CXX) $(C_FLAGS) $(C_INCLUDES) -c $< -o $@

clean:
		rm -f ../../out/$(TARGET).a
