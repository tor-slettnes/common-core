## -*- GNUMakefile -*-
#===============================================================================
## @file Makefile
## @brief Build via CMake
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

define HELP_TEXT
Available targets:
  help               Show this help message
  build-requirements Install Debian packages required to build
  local              Build, test and install locally
  release            Build, test and create release packages
  package            Create packages (currently only deb)
  debs, deb          Create Debian packages
  wheels             Create Python wheel packages
  install            Install built components
  install/strip      Install and strip debug symbols
  install/<comp>     Install specific component
  doc                Generate documentation
  ctest              Run tests
  ctest/report       Run tests and show detailed failure reports
  ctest/rerun        Rerun failed tests
  build              Build the project
  cmake-gui          Launch CMake GUI
  cmake              Generate build files
  python_shell       Launch Python shell with project modules
  python_shell/<m>   Launch Python shell with specific module
  list               List all available targets
  get_config         Show current CMake configuration
  get_version        Show project version
  get_product        Show product name
  clean              Clean build artifacts
  clean/cmake        Clean CMake build files
  clean/core         Remove core dumps
  clean/build        Remove build directory
  clean/install      Remove installed files
  clean/deb          Remove package files
  clean/cache        Remove CMake cache
  clean/python       Remove Python build artifacts
  clean/out          Remove all output directories
  realclean          Remove all build artifacts
  distclean          Remove all generated files

Variables:
  TARGET             Target platform (e.g., Linux-x86_64)
  BUILD_TYPE         Build type (Debug/Release)
  OUT_DIR            Output directory
  BUILD_DIR          Build directory
  INSTALL_DIR        Installation directory
  PACKAGE_DIR        Package output directory
  PARALLEL_JOBS      Parallel build jobs
endef
export HELP_TEXT

MAKEFLAGS     += --no-print-directory
THIS_DIR      ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR       ?= $(CURDIR)/out
BUILD_DIR     ?= $(OUT_DIR)/build
INSTALL_DIR   ?= $(OUT_DIR)/install
PACKAGE_DIR   ?= $(OUT_DIR)/packages
PYTHON        ?= /usr/bin/python3
TOOLCHAIN_FILE = $(THIS_DIR)build/cmake/toolchain-$(TARGET).cmake

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

ifneq ($(wildcard $(TOOLCHAIN_FILE)),)
  export CMAKE_TOOLCHAIN_FILE ?= $(TOOLCHAIN_FILE)
endif

ifeq ($(shell uname),Linux)
  PARALLEL_JOBS ?= $(shell nproc)
else
  PARALLEL_JOBS ?= 8
endif

CMAKE_BUILD_ARGS += --parallel $(PARALLEL_JOBS)

## Set CMake cache entries from variable overrides provided on command line.
## E.g. `make PRODUCT=myproduct` -> `cmake -DPRODUCT=myproduct`.
OVERRIDES = $(filter-out PARALLEL_JOBS=%,$(MAKEOVERRIDES))
CMAKE_CONFIG_ARGS += $(foreach override,$(OVERRIDES),-D$(override))

## Create CMake cache only if this file is absent.
CMAKE_TAG = $(BUILD_DIR)/Makefile
CMAKE_CACHE = $(BUILD_DIR)/CMakeCache.txt

### Utility functions
define list_cache_or_default
(\
  cat $(BUILD_DIR)/CMakeCache.txt || \
  cmake -L -P build/buildspec.cmake -P $(THIS_DIR)/build/buildspec.cmake\
) 2>/dev/null
endef

define get_cached_or_default
$(strip $(shell $(list_cache_or_default) | awk 'BEGIN {FS="="} /^$(1):/ {print $$2}'))
endef

define list_debian_requirements
(\
  cat $(CURDIR)/build/debian-requirements.txt || \
  cat $(THIS_DIR)/build/debian-requirements.txt\
) 2>/dev/null
endef


define remove
	if [ -e "${1}" ]; then \
		echo "Removing: ${1}"; \
		rm -rf "${1}"; \
	fi
endef


### Build targets
.PHONY: local
local: ctest install

.PHONY: release
release: ctest package

.PHONY: package
package: debs

.PHONY: debs deb
debs deb: build
	@echo
	@echo "#############################################################"
	@echo "Creating Debian release packages in ${PACKAGE_DIR}"
	@echo "#############################################################"
	@echo
	@cpack --config "${BUILD_DIR}/CPackConfig.cmake" -B "${PACKAGE_DIR}" -G DEB

.PHONY: wheels
wheels: cmake
	@echo
	@echo "#############################################################"
	@echo "Creating PIP-installable Python distributions ('.whl')"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)" --target python_wheels $(CMAKE_BUILD_ARGS)
	@echo
	@echo "Wheels are created in '$(OUT_DIR)/python/wheels'"

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

install/%:
	@echo
	@echo "#############################################################"
	@echo "Installing component '$*' in ${INSTALL_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR) --component $*

.PHONY: doc
doc: cmake
	@echo
	@echo "#############################################################"
	@echo "Generating documentation"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)" --target doxygen $(CMAKE_BUILD_ARGS)

.PHONY: ctest
ctest: build
	@echo
	@echo "#############################################################"
	@echo "Testing"
	@echo "#############################################################"
	@echo
	@cd "$(BUILD_DIR)" && ctest

.PHONY: ctest/report
ctest/report: build
	@echo
	@echo "#############################################################"
	@echo "Testing & reporting failures"
	@echo "#############################################################"
	@echo
	@cd "$(BUILD_DIR)" && ctest --output-on_failure

.PHONY: ctest/rerun
ctest/rerun: build
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
	@cmake --build "$(BUILD_DIR)" $(CMAKE_BUILD_ARGS)

.PHONY: cmake-gui
cmake-gui: cmake
	@cmake-gui $(BUILD_DIR)

.PHONY: cmake
cmake: $(CMAKE_TAG) $(CMAKE_CACHE)

### If we have defined custom arguments to CMake (see above), we force
### regeneration of the CMake cache by declaring any previous result as phony.
ifneq ($(or $(CMAKE_FORCE_REGENERATE),$(CMAKE_CONFIG_ARGS)),)
.PHONY: $(CMAKE_TAG)
endif

$(CMAKE_TAG) $(CMAKE_CACHE):
	@echo
	@echo "#############################################################"
	@echo "Generating build files in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cmake -B "$(BUILD_DIR)" $(CMAKE_CONFIG_ARGS)

$(BUILD_DIR):
	@mkdir -p "$(BUILD_DIR)"

.PHONY: python_shell
python_shell: install
	@env PYTHONPATH=$(INSTALL_DIR)/$(call get_cached_or_default,PYTHON_INSTALL_DIR) $(PYTHON)

.PHONY: python_shell/%
python_shell/%: install
	@echo "Launching interactive Python prompt with module: $*"
	@env PYTHONPATH=$(INSTALL_DIR)/$(call get_cached_or_default,PYTHON_INSTALL_DIR) $(PYTHON) -i -m $*

.PHONY: list_make_vars
list_make_vars:
	@echo "$(.VARIABLES)" | tr ' ' '\n' | sort

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
	@$(list_cache_or_default) | \
      awk 'BEGIN{FS="="}                    \
		/:INTERNAL=/ {next;}                \
		/^[A-Z0-9_]*:[A-Z]*=/ {             \
			sub(":.*","",$$1);              \
			printf("%-40s = %s\n",$$1,$$2); \
		}'

.PHONY: get_version
get_version:
	@echo $(call get_cached_or_default,VERSION)

.PHONY: get_product
get_product:
	@echo $(call get_cached_or_default,PRODUCT)

.PHONY: clean
clean: clean/cmake

.PHONY: clean/cmake cmake_clean
clean/cmake cmake_clean:
	@if [ -f "$(CMAKE_CACHE)" ]; \
	then \
		echo "Invoking CMake target 'clean'"; \
		cmake --build "$(BUILD_DIR)" --target clean $(CMAKE_BUILD_ARGS) || true; \
	fi

.PHONY: clean/core
clean/core:
	@$(call remove,core)

.PHONY: clean/build
clean/build:
	@$(call remove,$(BUILD_DIR))

.PHONY: clean/install uninstall
clean/install uninstall:
	@$(call remove,$(INSTALL_DIR))

.PHONY: clean/deb clean/package pkg_clean
clean/deb clean/package pkg_clean:
	@$(call remove,$(PACKAGE_DIR))

.PHONY: clean/cache clean/config
clean/cache clean/config:
	@$(call remove,$(CMAKE_CACHE))

.PHONY: clean/python
clean/python:
	@$(call remove,$(OUT_DIR)/python)

.PHONY: clean/out cleanout
clean/out cleanout:
	@$(call remove,$(OUT_DIR))

.PHONY: realclean
realclean: clean/core clean/cmake clean/package clean/install clean/build

.PHONY: distclean pristine
distclean pristine: clean/core clean/out

.PHONY: help
help:
	@echo "$${HELP_TEXT}"

.PHONY: build-requirements prepare_linux
build-requirements prepare_linux:
	@echo "Installing required Debian build dependencies..."
	@$(call list_debian_requirements) | sudo xargs apt install -y

### Delegate docker_ targets to its own Makefile
docker_%:
	@$(MAKE) -C $(THIS_DIR)/build/docker $(MAKECMDGOALS) HOST_DIR=$(CURDIR)

### Delegate any other target to CMake
%:
	@[ -f "$(CMAKE_TAG)" ] || cmake -B "$(BUILD_DIR)" $(CMAKE_CONFIG_ARGS)
	@cmake --build "$(BUILD_DIR)" --target $*
