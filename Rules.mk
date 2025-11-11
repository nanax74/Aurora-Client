#===============================================================================
# Directory
#===============================================================================
BUILD ?= Build
SOURCE ?= Source
INCLUDE ?= Include

#===============================================================================
# Function
#===============================================================================
define strip-root
$(strip $(foreach dir,$(SOURCE),$(if $(filter $(dir)/%,$(1)),$(patsubst $(dir)/%,%,$(1)))))
endef

define make-rule
$(BUILD)/$(call strip-root,$(1:.cpp=.o)): $(1)
	@mkdir -p $(BUILD)/$(dir $(call strip-root,$(1:.cpp=.o)))
	@echo "Compiling $$<..."
	$(CXX) $(CXXFLAGS) -c $$< -o $$@
endef

#===============================================================================
# Prepare
#===============================================================================
CXX := g++
CXXFLAGS += $(foreach dir,$(INCLUDE),-I$(dir))

SOURCES := $(foreach dir,$(SOURCE),$(shell find $(dir) -name '*.cpp'))
OBJECTS := $(foreach src,$(SOURCES),$(BUILD)/$(call strip-root,$(src:.cpp=.o)))

OUTPUT := $(DIST)/$(TARGET)

#===============================================================================
# Rules
#===============================================================================
$(OUTPUT): $(OBJECTS)
	@mkdir -p $(dir $@)
	@echo "Linking..."
	$(CXX) $^ -o $@ $(LDFLAGS)

$(foreach src,$(SOURCES),$(eval $(call make-rule,$(src))))