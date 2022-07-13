BUILD_TYPE := Debug

all: configure
	cmake --build build

configure:
	cmake -B build -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

clean:
	rm -rf build

lc:
	cloc --exclude-dir=argagg,build,date,loguru,.vscode --exclude-lang=JSON,Expect .

.PHONY: all configure clean lc
