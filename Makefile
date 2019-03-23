bin/unet: src/*.cc include/*.h
	mkdir -p bin
	g++ -std=c++17 -I./include -Wall -Werror -o $@ src/*.cc

dev:
	docker build -t unet .
	docker run -it -v ${CURDIR}:/unet unet sh
