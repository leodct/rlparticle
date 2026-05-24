.PHONY: default clean raylib particles

default: particles

clean:
	rm -f particles libraylib.a

raylib:
	cd raylib && \
	rm -rf build && \
	mkdir build && \
	cd build && \
	cmake .. -DCMAKE_BUILD_TYPE=Release && \
	make -j$$(nproc) && \
	cp raylib/libraylib.a ../../libraylib.a

particles: raylib
	g++ -o particles src/main.cpp src/particles.cpp -I./include ./libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11