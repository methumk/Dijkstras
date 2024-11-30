CC = g++ -std=c++17
CM = g++ -std=c++17 -U__STRICT_ANSI__ 
CFLAGS = -c
INCLUDES = -Isrc/include -Iimgui
LIB = -Lsrc/lib
LINKS = -lsfml-graphics -lsfml-window -lsfml-system -lopengl32

MAIN_OBJECT = main.o
SRC_DIR = Dijkstras
MAIN_FILE = $(SRC_DIR)/main.cpp
MAIN_DEPENDENCIES = $(SRC_DIR)/algo.hpp $(SRC_DIR)/graph.hpp $(SRC_DIR)/gui.hpp $(SRC_DIR)/links.hpp $(SRC_DIR)/node.hpp $(SRC_DIR)/IAnimImpl.hpp ${SRC_DIR}/DFSImpl.hpp ${SRC_DIR}/BFSImpl.hpp  ${SRC_DIR}/DijkImpl.hpp 

IMGUI_OBJECTS = imgui.o imgui-SFML.o imgui_draw.o imgui_widgets.o imgui_tables.o
all: main

imgui.o: imgui/imgui.cpp 
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@

imgui-SFML.o: imgui/imgui-SFML.cpp 
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@

imgui_draw.o: imgui/imgui_draw.cpp
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@

imgui_widgets.o: imgui/imgui_widgets.cpp
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@

imgui_tables.o: imgui/imgui_tables.cpp
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@

$(MAIN_OBJECT): $(MAIN_FILE) $(MAIN_DEPENDENCIES)
	$(CM) $(INCLUDES) $(CFLAGS) $< -o $@

main: $(MAIN_OBJECT) $(IMGUI_OBJECTS)
	$(CC) $(MAIN_OBJECT) $(IMGUI_OBJECTS) -o dijk $(LIB) $(LINKS)


.PHONY: clean
clean:
	rm -rf dijk.exe *.o

