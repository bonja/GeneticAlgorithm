
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

MACRO = -D_PRINT_DEBUG_ -D_RAND_TEST_ -D_FACTOR_TEST_ -D_SUBMIT
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
	@echo "=== test for unweighted_50 ==="
	@cp -f data/unweighted_50.txt maxcut.in
	@./$(TARGET)
	@cat maxcut.out
	@echo "=== test for unweighted_100 ==="
	@cp -f data/unweighted_100.txt maxcut.in
	@./$(TARGET)
	@cat maxcut.out
	@echo "=== test for unweighted_500 ==="
	@cp -f data/unweighted_500.txt maxcut.in
	@./$(TARGET)
	@cat maxcut.out
	@echo "=== test for weighted_500 ==="
	@cp -f data/weighted_500.txt maxcut.in
	@./$(TARGET)
	@cat maxcut.out
	@echo "=== test for weighted_chimera_297 ==="
	@cp -f data/weighted_chimera_297.txt maxcut.in
	@./$(TARGET)
	@cat maxcut.out
