PROJECT_DIR := .
BUILD_DIR := $(PROJECT_DIR)/dist/native
OBJECT_DIR := $(BUILD_DIR)/obj

CXX := clang++
ifndef LD
LD := wasm-ld
endif

CXX_FLAGS := \
	--target=wasm32 \
	-Os \
	-flto \
	-nostdlib \
	-fvisibility=hidden \
	-std=c++20 \
	-ffunction-sections \
	-fdata-sections

LD_FLAGS := \
	--no-entry \
	--strip-all \
	--export-dynamic \
	--import-memory \
	--import-undefined \
	--gc-sections \
	-shared-memory \
	--initial-memory=655360 \
	--features=atomics,bulk-memory

SHARED_INCLUDE_DIRS := \
	$(PROJECT_DIR)/shared/include

RUNTIME_INCLUDE_DIRS := \
	$(SHARED_INCLUDE_DIRS) \
	$(PROJECT_DIR)/runtime/include

INTERPRETER_INCLUDE_DIRS := \
	$(SHARED_INCLUDE_DIRS) \
	$(PROJECT_DIR)/interpreter/include

RUNTIME_TARGET := $(BUILD_DIR)/runtime.wasm
INTERPRETER_TARGET := $(BUILD_DIR)/interpreter.wasm

RUNTIME_SOURCES := \
	runtime/source/main.cc \
	runtime/source/memory.cc \
	runtime/source/guest_memory.cc \
	runtime/source/cpu.cc \
	runtime/source/decoder.cc \
	runtime/source/disassembler.cc

INTERPRETER_SOURCES := \
	interpreter/source/main.cc

RUNTIME_INCLUDES := $(addprefix -I,$(RUNTIME_INCLUDE_DIRS))
INTERPRETER_INCLUDES := $(addprefix -I,$(INTERPRETER_INCLUDE_DIRS))

RUNTIME_OBJECTS := $(addprefix $(OBJECT_DIR)/,$(addsuffix .o,$(RUNTIME_SOURCES)))
INTERPRETER_OBJECTS := $(addprefix $(OBJECT_DIR)/,$(addsuffix .o,$(INTERPRETER_SOURCES)))

$(OBJECT_DIR)/runtime/%.cc.o: $(PROJECT_DIR)/runtime/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(RUNTIME_INCLUDES) -c $< -o $@

$(OBJECT_DIR)/interpreter/%.cc.o: $(PROJECT_DIR)/interpreter/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(INTERPRETER_INCLUDES) -c $< -o $@

$(RUNTIME_TARGET): $(RUNTIME_OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) $(LD_FLAGS) -o $@ $^

$(INTERPRETER_TARGET): $(INTERPRETER_OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) $(LD_FLAGS) --global-base=4096 -o $@ $^

.PHONY: copy-to-public
copy-to-public: $(RUNTIME_TARGET) $(INTERPRETER_TARGET)
	@mkdir -p $(PROJECT_DIR)/public/wasm/
	cp $(RUNTIME_TARGET) $(PROJECT_DIR)/public/wasm/
	cp $(INTERPRETER_TARGET) $(PROJECT_DIR)/public/wasm/
