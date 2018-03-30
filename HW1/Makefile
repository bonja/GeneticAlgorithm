
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

TARGET = hw1

.phony: all

%.o: %.cpp
	@g++ -c $<

$(TARGET): $(OBJS)
	@g++ -o $@ $(OBJS)

all: $(TARGET)

clean :
	@rm -f $(TARGET)
	@rm -f $(OBJS)

run: $(TARGET)
	@./$(TARGET)
