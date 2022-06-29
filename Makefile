VERSION = 0.0

CXXFLAGS = -O2 -pipe -std=c++17 '-DVER="${VERSION}"' -Wall -Iinclude
LDFLAGS = -pthread
CXX = c++

# ifneq (,$(shell uname | grep -i mingw))
ifeq ($(OS),Windows_NT)
LDFLAGS += -lole32
else
ifeq ($(shell uname -s),Linux)
LDFLAGS += -ldl
endif
endif

OBJ = config.o job.o process_platform.o report.o schedule.o util.o wobble.o\
      date/tz.o tiny-process-library/process.o loguru/loguru.o fmt/format.o\
      fmt/os.o

all: wobble

%.o: %.cpp
	${CXX} -c ${CXXFLAGS} $< -o $@

%.o: %.cc
	${CXX} -c ${CXXFLAGS} $< -o $@

wobble: ${OBJ}
	${CXX} -o $@ ${OBJ} ${LDFLAGS}

clean: clean-intermediate
	rm -f wobble

clean-intermediate:
	rm -f ${OBJ}

.PHONY: all clean clean-intermediate
