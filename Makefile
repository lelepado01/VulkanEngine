CXX = g++
SHADER_CXX = glslc

CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread

H_FILES = $(shell find . -type f -name "*.h")

SRC_FILES = $(subst ./main.cpp,,$(shell find . -type f -name "*.cpp"))
OBJ_FILES = $(subst .cpp,.o,$(SRC_FILES))

SRC_SHADER = $(shell find . -type f -name "*.vert") $(shell find . -type f -name "*.frag") $(shell find . -type f -name "*.tese") $(shell find . -type f -name "*.tesc")
OBJ_SHADER = $(addsuffix .spv,$(SRC_SHADER))

NAME = VulkanEngine

%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

%.vert.spv: %.vert
	$(SHADER_CXX) $^ -o $@ 

%.frag.spv: %.frag
	$(SHADER_CXX) $^ -o $@ 

%.tese.spv: %.tese
	$(SHADER_CXX) $^ -o $@ 

%.tesc.spv: %.tesc
	$(SHADER_CXX) $^ -o $@ 

$(NAME): main.cpp $(OBJ_FILES) $(OBJ_SHADER) $(H_FILES)
	$(CXX) -o $(NAME) $(CFLAGS) $(OBJ_FILES) main.cpp $(LDFLAGS)

.PHONY: run clean

run: $(NAME)
	./$(NAME)

clean:  
	rm -f $(OBJ_SHADER)
	rm -f $(OBJ_FILES)