.PHONY: all clean

PLATFORM              ?= PLATFORM_WEB

PROJECT_NAME          ?= Beans
PROJECT_VERSION       ?= 1.0
PROJECT_BUILD_PATH    ?= .

RAYLIB_PATH           ?= /Users/ludde/Library/raylib
RAYLIB_RELEASE_PATH   ?= $(RAYLIB_PATH)/src
RAYLIB_LIBTYPE        ?= STATIC

BUILD_MODE            ?= RELEASE

USE_WAYLAND_DISPLAY   ?= FALSE

# PLATFORM_WEB: Default properties
BUILD_WEB_ASYNCIFY    ?= FALSE
BUILD_WEB_SHELL       ?= $(RAYLIB_RELEASE_PATH)/minshell.html
BUILD_WEB_HEAP_SIZE   ?= 134217728
# BUILD_WEB_RESOURCES   ?= TRUE
# BUILD_WEB_RESOURCES_PATH  ?= resources

# Emscripten required variables
EMSDK_PATH         ?= /Users/ludde/Library/emsdk
EMSCRIPTEN_PATH    ?= $(EMSDK_PATH)/upstream/emscripten
CLANG_PATH          = $(EMSDK_PATH)/upstream/bin
PYTHON_PATH         = /usr/bin/python3
NODE_PATH           = $(EMSDK_PATH)/node/14.15.5_64bit/bin
export PATH         := $(EMSDK_PATH):$(EMSCRIPTEN_PATH):$(CLANG_PATH):$(NODE_PATH):$(PYTHON_PATH):$(PATH)

CC = $(EMSCRIPTEN_PATH)/emcc
MAKE ?= make
CFLAGS = -std=c99 -Wall -Wno-missing-braces -Wunused-result -D_DEFAULT_SOURCE

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -D_DEBUG
else
    ifeq ($(BUILD_WEB_ASYNCIFY),TRUE)
        CFLAGS += -O3
    else
        CFLAGS += -Os
    endif
endif


#------------------------------------------------------------------------------------------------
INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external -I$(RAYLIB_PATH)/src/extras

# Define library paths containing required libs: LDFLAGS
#------------------------------------------------------------------------------------------------
LDFLAGS = -L. -L$(RAYLIB_RELEASE_PATH) -L$(RAYLIB_PATH)/src

# -Os                        # size optimization
# -O2                        # optimization level 2, if used, also set --memory-init-file 0
# -s USE_GLFW=3              # Use glfw3 library (context/input management)
# -s ALLOW_MEMORY_GROWTH=1   # to allow memory resizing -> WARNING: Audio buffers could FAIL!
# -s TOTAL_MEMORY=16777216   # to specify heap memory size (default = 16MB) (67108864 = 64MB)
# -s USE_PTHREADS=1          # multithreading support
# -s WASM=0                  # disable Web Assembly, emitted by default
# -s ASYNCIFY                # lets synchronous C/C++ code interact with asynchronous JS
# -s FORCE_FILESYSTEM=1      # force filesystem to load/save files data
# -s ASSERTIONS=1            # enable runtime checks for common memory allocation errors (-O1 and above turn it off)
# --profiling                # include information for code profiling
# --memory-init-file 0       # to avoid an external memory initialization code file (.mem)
# --preload-file resources   # specify a resources folder for data compilation
# --source-map-base          # allow debugging in browser with source map
LDFLAGS += -s USE_GLFW=3 -s TOTAL_MEMORY=$(BUILD_WEB_HEAP_SIZE) -s FORCE_FILESYSTEM=1

# Build using asyncify
ifeq ($(BUILD_WEB_ASYNCIFY),TRUE)
    LDFLAGS += -s ASYNCIFY
endif

# # Add resources building if required
# ifeq ($(BUILD_WEB_RESOURCES),TRUE)
#     LDFLAGS += --preload-file $(BUILD_WEB_RESOURCES_PATH)
# endif

# Add debug mode flags if required
ifeq ($(BUILD_MODE),DEBUG)
    LDFLAGS += -s ASSERTIONS=1 --profiling
endif

# Define a custom shell .html and output extension
LDFLAGS += --shell-file $(BUILD_WEB_SHELL)
EXT = .html



# Libraries for web (HTML5) compiling
LDLIBS = $(RAYLIB_RELEASE_PATH)/libraylib.a -lwebsocket.js

# Define source code object files required
#------------------------------------------------------------------------------------------------
PROJECT_SOURCE_FILES ?= \
    main.c


# Define all object files from source files
OBJS = $(patsubst %.c, build/%.o, $(PROJECT_SOURCE_FILES))

MAKEFILE_PARAMS = $(PROJECT_NAME)

# Default target entry
all:
	$(MAKE) $(MAKEFILE_PARAMS)

# Project target defined by PROJECT_NAME
$(PROJECT_NAME): $(OBJS)
	$(CC) -o build/index.html $(OBJS) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)

# Compile source files
# NOTE: This pattern will compile every module defined on $(OBJS)
build/%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) -D$(PLATFORM)

server:
	gcc -I /usr/local/include/wsserver -L /usr/local/lib/ -l ws server.c -o build/server.out

# Clean everything
clean:
	rm build/*.o build/*.html build/*.js build/*.wasm build/*.out
	@echo Cleaning done

