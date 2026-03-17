CXX = clang++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra

TARGET = main
SRC = gain.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)