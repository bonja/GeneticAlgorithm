
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

MACRO = -D_PRINT_DEBUG

TARGET = hw1

.phony: all

%.o: %.cpp
	@g++ $(MACRO) -c $<

$(TARGET): $(OBJS)
	@g++ -o $@ $(OBJS)

all: $(TARGET)

clean :
	@rm -f $(TARGET)
	@rm -f $(OBJS)

run: $(TARGET)
	@./$(TARGET)
