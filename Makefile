## -*- GNUMakefile -*-
#===============================================================================
## @file Makefile
## @brief Build via CMake
## @author Tor Slettnes
#===============================================================================

define HELP_TEXT
Available targets:
  help               Show this help message
  install-build-requirements
                     Install Debian packages required to build
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

Variables that control the build behavior:
  TARGET             Target platform (e.g., Linux-x86_64)
  CONFIG_PRESET      CMake Configuration Preset (e.g. toolchain selection)
  BUILD_PRESET       CMake Build Preset
  TEST_PRESET        CTest Preset
  PACKAGE_PRESET     CPack Preset
endef
export HELP_TEXT


### Utility functions
define list_cache
cmake -L -N --preset $(CONFIG_PRESET)
endef

define get_cached
cmake -L -N --preset $(CONFIG_PRESET) | sed -n -e 's/^$(1):\w*=\(.*\)$$/\1/ p'
endef

define get_preset_var
cmake -N --preset $(CONFIG_PRESET) | sed -ne 's/.*$(1)=\"\(.*\)\"/\1/ p'
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


MAKEFLAGS          += --no-print-directory

CONFIG_PRESET      ?= default
BUILD_PRESET       ?= default
TEST_PRESET        ?= default
PACKAGE_PRESET     ?= debs
THIS_DIR           ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR             = $(strip $(shell $(call get_preset_var,OUT_DIR)))
BUILD_DIR           = $(strip $(shell $(call get_preset_var,BUILD_DIR)))
PACKAGE_DIR         = $(strip $(shell $(call get_preset_var,PACKAGE_DIR)))
INSTALL_DIR         = $(strip $(shell $(call get_cached,CMAKE_INSTALL_PREFIX)))
PYTHON_INSTALL_DIR  = $(strip $(shell $(call get_cached,PYTHON_INSTALL_DIR)))
PYTHON             ?= /usr/bin/python3
TARGET             ?= $(shell uname -s)-$(shell uname -m)
TOOLCHAIN_FILE      = $(THIS_DIR)build/cmake/toolchain-$(TARGET).cmake

ifneq ($(wildcard $(TOOLCHAIN_FILE)),)
  export CMAKE_TOOLCHAIN_FILE ?= $(TOOLCHAIN_FILE)
endif


### CMake customization
CMAKE_TAG           = $(BUILD_DIR)/Makefile
CMAKE_CACHE         = $(BUILD_DIR)/CMakeCache.txt

## Set CMake cache entries from variable overrides provided on command line.
## E.g. `make PRODUCT=myproduct` -> `cmake -DPRODUCT=myproduct`.
OVERRIDES = $(filter-out TARGET=% CONFIG_PRESET=% BUILD_PRESET=% TEST_PRESET=% PACKAGE_PRESET=%,$(MAKEOVERRIDES))
CMAKE_CONFIG_ARGS += $(foreach override,$(OVERRIDES),-D$(override))

### Build targets
.PHONY: local
local: ctest install

.PHONY: release
release: ctest package


.PHONY: package debs deb
package debs deb: build
	@echo
	@echo "#############################################################"
	@echo "Creating Debian release packages in $(PACKAGE_DIR)"
	@echo "#############################################################"
	@echo
	@cpack --preset "$(PACKAGE_PRESET)"

.PHONY: wheels
wheels: cmake
	@echo
	@echo "#############################################################"
	@echo "Creating PIP-installable Python distributions ('.whl')"
	@echo "#############################################################"
	@echo
	@cmake --build --preset $(BUILD_PRESET) --target python_wheels $(CMAKE_BUILD_ARGS)
	@echo
	@echo "Wheels are created in '$(OUT_DIR)/python/wheels'"

.PHONY: install
install: build
	@echo
	@echo "#############################################################"
	@echo "Installing in ${INSTALL_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --build --preset "$(BUILD_PRESET)" --target install $(CMAKE_BUILD_ARGS)

.PHONY: install/strip
install/strip: build
	@echo
	@echo "#############################################################"
	@echo "Installing and stripping in ${INSTALL_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --build --preset "$(BUILD_PRESET)" --target install/strip $(CMAKE_BUILD_ARGS)

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
	@cmake --build --preset $(BUILD_PRESET) --target doxygen $(CMAKE_BUILD_ARGS)

.PHONY: ctest
ctest: build
	@echo
	@echo "#############################################################"
	@echo "Testing"
	@echo "#############################################################"
	@echo
	@ctest --preset "$(TEST_PRESET)"

.PHONY: ctest/report
ctest/report: build
	@echo
	@echo "#############################################################"
	@echo "Testing & reporting failures"
	@echo "#############################################################"
	@echo
	@ctest --preset "$(TEST_PRESET)" --output-on-failure

.PHONY: ctest/rerun
ctest/rerun: build
	@echo
	@echo "#############################################################"
	@echo "Rerunning failed tests"
	@echo "#############################################################"
	@echo
	@ctest --preset "$(TEST_PRESET)" --rerun-failed --output-on-failure

.PHONY: build
build: cmake
	@echo
	@echo "#############################################################"
	@echo "Building in $(BUILD_DIR)"
	@echo "#############################################################"
	@echo
	@cmake --build --preset "$(BUILD_PRESET)" $(CMAKE_BUILD_ARGS)

.PHONY: cmake-gui
cmake-gui: cmake
	@cmake-gui --preset "$(CONFIG_PRESET)"

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
	@echo "Generating CMake preset: $(CONFIG_PRESET)"
	@echo "#############################################################"
	@echo
	@cmake --preset "$(CONFIG_PRESET)" $(CMAKE_CONFIG_ARGS)

.PHONY: python_shell
python_shell:
	@env PYTHONPATH=$(INSTALL_DIR)/$(PYTHON_INSTALL_DIR) $(PYTHON)

.PHONY: python_shell/%
python_shell/%:
	@echo "Launching interactive Python prompt with module: $*"
	@env PYTHONPATH=$(INSTALL_DIR)/$(PYTHON_INSTALL_DIR) $(PYTHON) -i -m $*

.PHONY: list_make_vars
list_make_vars:
	@echo "$(.VARIABLES)" | tr ' ' '\n' | sort

.PHONY: list
list: cmake
	@echo "Default target:"
	@echo "    $(.DEFAULT_GOAL)"
	@echo
	@echo "Makefile targets:"
	@sed -n -e 's/^\([[:alnum:]][^:]*\):.*$$/... \1/p' $(MAKEFILE_LIST) | sort -u
	@echo
	@echo "Additional targets from CMake:"
	@cmake --build --preset "$(BUILD_PRESET)" --target help $(CMAKE_BUILD_ARGS) | tail +3

.PHONY: get_config
get_config: cmake
	@$(list_cache) | \
      awk 'BEGIN{FS="="}                    \
		/:INTERNAL=/ {next;}                \
		/^[A-Z0-9_]*:[A-Z]*=/ {             \
			sub(":.*","",$$1);              \
			printf("%-40s = %s\n",$$1,$$2); \
		}'

.PHONY: get_build_dir
get_build_dir:
	@$(call get_preset_var,BUILD_DIR)

.PHONY: get_install_dir
get_install_dir:
	@$(call get_preset_var,INSTALL_DIR)

.PHONY: get_package_dir
get_package_dir:
	@$(call get_preset_var,PACKAGE_DIR)

.PHONY: clean clean/cmake cmake_clean
clean clean/cmake cmake_clean:
	@if [ -f "$(CMAKE_CACHE)" ]; \
	then \
		echo "Invoking CMake target 'clean'"; \
		cmake --build --preset "$(BUILD_PRESET)" --target clean $(CMAKE_BUILD_ARGS) || true; \
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

.PHONY: clean/package clean/deb pkg_clean
clean/package clean/deb pkg_clean:
	@$(call remove,$(PACKAGE_DIR))

.PHONY: clean/cache clean/config
clean/cache clean/config:
	@$(call remove,$(CMAKE_CACHE))

.PHONY: clean/python
clean/python: clean_python

.PHONY: clean/out cleanout
clean/out cleanout:
	@$(call remove,$(OUT_DIR))

.PHONY: realclean
realclean: clean/core clean/python clean/cmake clean/package clean/install clean/build

.PHONY: distclean pristine
distclean pristine: clean/core clean/out

.PHONY: help
help:
	@echo "$${HELP_TEXT}"

.PHONY: install-build-requirements prepare_linux
install-build-requirements prepare_linux:
	@echo "Installing required Debian build dependencies..."
	@$(call list_debian_requirements) | sudo xargs apt install -y

### Delegate docker_ targets to its own Makefile
docker-% docker_%:
	@$(MAKE) -C $(THIS_DIR)/build/docker docker_$* HOST_DIR=$(CURDIR)


### Delegate any other target to CMake
clean_%:
	@if [ -f "$(CMAKE_TAG)" ]; then \
		cmake --build --preset "$(BUILD_PRESET)" --target $* $(CMAKE_BUILD_ARGS); \
	fi

%:
	@[ -f "$(CMAKE_TAG)" ] || cmake --preset "$(CONFIG_PRESET)" $(CMAKE_CONFIG_ARGS)
	@cmake --build --preset "$(BUILD_PRESET)" --target $* $(CMAKE_BUILD_ARGS)
