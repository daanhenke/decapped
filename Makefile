PROJECT_DIR := .
BUILD_DIR := $(PROJECT_DIR)/dist/native
OBJECT_DIR := $(BUILD_DIR)/obj

CXX := clang++
LD := wasm-ld

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

RUNTIME_TARGET := $(BUILD_DIR)/runtime.wasm

RUNTIME_SOURCES := \
	runtime/source/main.cc \
	runtime/source/memory.cc

RUNTIME_INCLUDES := $(addprefix -I,$(RUNTIME_INCLUDE_DIRS))
RUNTIME_OBJECTS := $(addprefix $(OBJECT_DIR)/,$(addsuffix .o,$(RUNTIME_SOURCES)))

$(OBJECT_DIR)/%.cc.o: $(PROJECT_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(RUNTIME_INCLUDES) -c $< -o $@

$(RUNTIME_TARGET): $(RUNTIME_OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) $(LD_FLAGS) -o $@ $^

.PHONY: copy-to-public
copy-to-public: $(RUNTIME_TARGET)
	@mkdir -p $(PROJECT_DIR)/public/wasm/
	cp $(RUNTIME_TARGET) $(PROJECT_DIR)/public/wasm/
