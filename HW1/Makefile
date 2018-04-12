
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

test: $(TARGET)
	$(info test - unweighted_50)
	@cp -f data/unweighted_50.txt maxcur.in
	@./$(TARGET)
	$(info test - unweighted_100)
	@cp -f data/unweighted_100.txt maxcur.in
	@./$(TARGET)
	$(info test - unweighted_500)
	@cp -f data/unweighted_500.txt maxcur.in
	@./$(TARGET)
	$(info test - weighted_500)
	@cp -f data/weighted_500.txt maxcur.in
	@./$(TARGET)
	$(info test - weighted_chimera_297)
	@cp -f data/weighted_chimera_297.txt maxcur.in
	@./$(TARGET)
