BUILD_TYPE := Debug

all: configure
	cmake --build build

configure:
	cmake -B build -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

clean:
	rm -rf build

.PHONY: all configure clean
