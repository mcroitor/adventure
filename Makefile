CC = g++
CXXFLAGS = -O2 -std=c++20
LDFLAGS =

BINDIR = bin
SRCDIR = src
OBJDIR = obj

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))

APP = adventure

all: prepare $(APP)

prepare:
	mkdir -p $(OBJDIR) $(BINDIR)

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/*

help:
	@echo Usage:
	@echo make - builds the application
	@echo make clean - removes build artifacts
	@echo make help - displays help

$(APP): $(OBJ)
	$(CC) $(OBJ) -o $(BINDIR)/$(APP) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c $< $(CXXFLAGS) -o $@