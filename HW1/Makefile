
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

MACRO = -D_PRINT_DEBUG_
OPT = -O3

TARGET = hw1

.phony: all

%.o: %.cpp
	@g++ $(MACRO) $(OPT) -c $<

$(TARGET): $(OBJS)
	@g++ $(OPT) -o $@ $(OBJS)

all: $(TARGET)

clean :
	@rm -f $(TARGET)
	@rm -f $(OBJS)

run: $(TARGET)
	@./$(TARGET)
