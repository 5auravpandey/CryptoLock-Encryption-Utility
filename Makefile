CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -Wextra -Wpedantic -O2
INCLUDES  := -Iinclude

SRCDIR    := src
OBJDIR    := obj
TARGET    := cryptolock

SOURCES   := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS   := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
	@echo ""
	@echo "  Build complete -> ./$(TARGET)"
	@echo ""

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

.PHONY: all clean rebuild run help

clean:
	rm -rf $(OBJDIR) $(TARGET)
	@echo "  Cleaned."

rebuild: clean all

run: all
	./$(TARGET) -h

help:
	@echo ""
	@echo "  make          - Build the project"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make rebuild  - Clean + Build"
	@echo "  make run      - Build and show help banner"
	@echo ""
