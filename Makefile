# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread

# Folders
CLIENT_DIR = client
SERVER_DIR = server

# Source files
CLIENT_SRCS = $(wildcard $(CLIENT_DIR)/*.cpp)
SERVER_SRCS = $(wildcard $(SERVER_DIR)/*.cpp)

# Output executables
CLIENT_BIN = speedtest_client
SERVER_BIN = speedtest_server

# Build rules
all: $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SERVER_BIN): $(SERVER_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean rule
clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)
