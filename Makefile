VERSION = 0.0

CXXFLAGS = -O2 -pipe -std=c++17 '-DVER="${VERSION}"' -Wall -I.
LDFLAGS = -pthread
CXX = c++

SRC = config.cpp job.cpp report.cpp schedule.cpp wobble.cpp util.cpp process.cpp process_platform.cpp
OBJ = ${SRC:.cpp=.o}

all: wobble

.cpp.o:
	${CXX} -c ${CXXFLAGS} $<

wobble: ${OBJ}
	${CXX} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f wobble ${OBJ}

.PHONY: all clean
