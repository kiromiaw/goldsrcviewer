CXX     := g++
TARGET  := build/app
SRC     := src/main.cpp

ifeq ($(OS), Windows_NT)
    LIBS    := -lSDL2 -lopengl32 -lglew32
    TARGET  := triangle.exe
    RM      := del /Q
else
    UNAME := $(shell uname -s)
    ifeq ($(UNAME), Darwin)
        # macOS SDL2 & GLEW via Homebrew
        LIBS    := $(shell sdl2-config --libs) -framework OpenGL -lGLEW
        CFLAGS  += $(shell sdl2-config --cflags)
    else
        # Linux
        LIBS    := -lSDL2 -lGL -lGLEW
    endif
    RM      := rm -f
endif

CFLAGS  += -std=c++17 -Wall -Wextra
RELFLAGS := -O2 -DNDEBUG
DBGFLAGS := -g -O0 -DDEBUG


.PHONY: all release debug clean run

all: release

release: $(SRC)
	$(CXX) $(CFLAGS) $(RELFLAGS) $^ $(LIBS) -o $(TARGET)
	@echo "Built $(TARGET) (release)"

debug: $(SRC)
	$(CXX) $(CFLAGS) $(DBGFLAGS) $^ $(LIBS) -o $(TARGET)
	@echo "Built $(TARGET) (debug)"

run: release
	./$(TARGET)

clean:
	$(RM) $(TARGET)