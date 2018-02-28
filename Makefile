CXX = g++
CXXFLAGS = -pthread -Wall -g -O3 -std=c++11
LIBS  = -lGL -lglut -lGLU -lpthread
# LIBS  = -lGL -lGLU -lglut -lpthread
SRCS = $(wildcard *.cpp)
OBJS := ${SRCS:.cpp=.o}

TARGET = interpolate

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

clean:
	rm -f *.o *.d *~ *.bak $(TARGET)

# Generate a list of dependencies for each cpp file
%.d: $(SRCS)
	@ $(CXX) $(CPPFLAGS) -MM $*.cpp | sed -e 's@^\(.*\)\.o:@\1.d \1.o:@' > $@

# At the end of the makefile
# Include the list of dependancies generated for each object file
# unless make was called with target clean
ifneq "$(MAKECMDGOALS)" "clean"
-include ${SRCS:.cpp=.d}
endif
