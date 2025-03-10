## -*- GNUMakefile -*-
#===============================================================================
## @file Makefile
## @brief Build via CMake
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

MAKEFLAGS     += --no-print-directory
SHARED_DIR    ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR       ?= $(CURDIR)/out
BUILD_DIR     ?= $(OUT_DIR)/build
INSTALL_DIR   ?= $(OUT_DIR)/install
PACKAGE_DIR   ?= $(OUT_DIR)/packages
TOOLCHAIN_FILE = $(SHARED_DIR)build/cmake/toolchain-$(TARGET).cmake

ifdef TARGET
	BUILD_DIR := $(BUILD_DIR)-$(TARGET)
	INSTALL_DIR := $(INSTALL_DIR)-$(TARGET)
	PACKAGE_DIR := $(PACKAGE_DIR)-$(TARGET)
else
	TARGET := $(shell uname -s)-$(shell uname -m)
endif

ifdef BUILD_TYPE
	BUILD_DIR := $(BUILD_DIR)-$(BUILD_TYPE)
else
	BUILD_TYPE = Release
endif

### CMake customization
export CMAKE_BUILD_TYPE ?= $(BUILD_TYPE)

ifeq ($(shell uname),Linux)
  export CMAKE_BUILD_PARALLEL_LEVEL = $(shell nproc)
endif

ifneq ($(wildcard $(TOOLCHAIN_FILE)),)
  export CMAKE_TOOLCHAIN_FILE ?= $(TOOLCHAIN_FILE)
endif

## Set CMake cache entries from variable overrides provided on command line.
## E.g. `make PRODUCT=myproduct` -> `cmake -DPRODUCT=myproduct`.
CMAKE_CONFIG_ARGS += $(foreach override,$(MAKEOVERRIDES),-D$(override))

## Create CMake cache only if this file is absent.
CMAKE_TAG = $(BUILD_DIR)/Makefile


### Utility functions
define remove
	if [ -e "$(1)" ]; then \
		echo "Removing: $(1)"; \
		rm -rf "$(1)"; \
	fi
endef

### Build targets
.PHONY: local
local: test install

.PHONY: release
release: test package

.PHONY: package
package: deb

.PHONY: deb
deb: build
	@echo
	@echo "#############################################################"
	@echo "Creating release packages in ${PACKAGE_DIR}"
	@echo "#############################################################"
	@echo
	@cpack --config "${BUILD_DIR}/CPackConfig.cmake" -B "${PACKAGE_DIR}" -G DEB

.PHONY: install
install: build
	@echo
	@echo "#############################################################"
	@echo "Installing in ${INSTALL_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR)

.PHONY: install/strip
install/strip: build
	@echo
	@echo "#############################################################"
	@echo "Installing and stripping in ${INSTALL_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR) --strip

.PHONY: doc
doc: cmake
	@echo
	@echo "#############################################################"
	@echo "Generating documentation"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)" --target doxygen

.PHONY: test
test: build
	@echo
	@echo "#############################################################"
	@echo "Testing"
	@echo "#############################################################"
	@echo
	@cd "$(BUILD_DIR)" && ctest

.PHONY: test/report
test/report: build
	@echo
	@echo "#############################################################"
	@echo "Testing & reporting failures"
	@echo "#############################################################"
	@echo
	@cd "$(BUILD_DIR)" && ctest --output-on_failure

.PHONY: retest
retest: build
	@echo
	@echo "#############################################################"
	@echo "Rerunning failed tests"
	@echo "#############################################################"
	@echo
	@cd "$(BUILD_DIR)" && ctest --rerun-failed --output-on_failure

.PHONY: build
build: cmake
	@echo
	@echo "#############################################################"
	@echo "Building in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)"

.PHONY: cmake-gui
cmake-gui: cmake
	@cmake-gui $(BUILD_DIR)

.PHONY: cmake
cmake: $(CMAKE_TAG)

### If we have defined custom arguments to CMake (see above), we force
### regeneration of the CMake cache by declaring any previous result as phony.
ifneq ($(or $(CMAKE_FORCE_REGENERATE),$(CMAKE_CONFIG_ARGS)),)
.PHONY: $(CMAKE_TAG)
endif

$(CMAKE_TAG):
	@echo
	@echo "#############################################################"
	@echo "Generating build files in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@echo -n "Invoking: "
	@cmake -B "$(BUILD_DIR)" $(CMAKE_CONFIG_ARGS)

$(BUILD_DIR):
	@mkdir -p "$(BUILD_DIR)"

.PHONY: list
list: cmake
	@echo "Default target:"
	@sed -n -e 's/^\([[:alnum:]][^:]*\):.*$$/    \1/p' $(MAKEFILE_LIST) | head -1
	@echo
	@echo "Makefile targets:"
	@sed -n -e 's/^\([[:alnum:]][^:]*\):.*$$/... \1/p' $(MAKEFILE_LIST) | sort -u
	@echo
	@echo "Additional targets from CMake:"
	@cmake --build "$(BUILD_DIR)" --target help | tail +3

.PHONY: get_config
get_config:
	@(cat $(BUILD_DIR)/CMakeCache.txt || \
      cmake -L -P build/buildspec.cmake -P $(SHARED_DIR)/build/buildspec.cmake) 2>/dev/null | \
      awk 'BEGIN{FS="="}                    \
		/:INTERNAL=/ {next;}                \
		/^[A-Z0-9_]*:[A-Z]*=/ {             \
			sub(":.*","",$$1);              \
			printf("%-40s = %s\n",$$1,$$2); \
		}'

.PHONY: get_version
get_version:
	@$(MAKE) get_config | awk '/^VERSION / { print $$3; }'

.PHONY: clean/cmake cmake_clean
clean/cmake cmake_clean:
	@if [ -f "$(CMAKE_TAG)" ]; \
	then \
		echo "Invoking CMake target 'clean'"; \
		cmake --build "$(BUILD_DIR)" --target clean || true; \
	fi

.PHONY: clean clean/core
clean: clean/cmake

.PHONY: clean/core
clean/core:
	@$(call remove,core)

.PHONY: clean/build
clean/build:
	@$(call remove,$(BUILD_DIR))

.PHONY: clean/install uninstall
clean/install uninstall:
	@$(call remove,$(INSTALL_DIR))

.PHONY: clean/package pkg_clean
clean/package pkg_clean:
	@$(call remove,$(PACKAGE_DIR))

.PHONY: clean/out cleanout
clean/out cleanout:
	@$(call remove,$(OUT_DIR))

.PHONY: distclean pristine
distclean pristine: clean/install clean/package clean/build clean/out

### Delegate docker_ targets to its own Makefile
docker_%:
	@$(MAKE) -C $(SHARED_DIR)/build/docker $(MAKECMDGOALS) HOST_DIR=$(CURDIR)

### Delegate any other target to CMake
%:
	@[ -f "$(CMAKE_TAG)" ] || cmake -B "$(BUILD_DIR)" $(CMAKE_CONFIG_ARGS)
	@cmake --build "$(BUILD_DIR)" --target $(patsubst cmake_%,%,$@)
