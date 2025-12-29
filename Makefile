## -*- GNUMakefile -*-
#===============================================================================
## @file Makefile
## @brief Build via CMake
## @author Tor Slettnes <tor@slett.net>
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
  CONFIG_PRESET      CMake Configuration Preset (e.g. toolchain selection)
  BUILD_PRESET       CMake Build Preset
  TEST_PRESET        CTest Preset
  PACKAGE_PRESET     CPack Preset
endef
export HELP_TEXT


MAKEFLAGS       += --no-print-directory

CONFIG_PRESET   ?= default
BUILD_PRESET    ?= default
TEST_PRESET     ?= default
PACKAGE_PRESET  ?= deb
THIS_DIR        ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR          = $(CURDIR)/out
BUILD_DIR        = $(strip $(shell $(call get_build_dir)))
INSTALL_DIR      = $(strip $(shell $(call get_cached,CMAKE_INSTALL_PREFIX)))
PACKAGE_DIR      = $(strip $(shell $(call get_package_dir)))
PYTHON          ?= /usr/bin/python3

### CMake customization
CMAKE_TAG = $(BUILD_DIR)/Makefile
CMAKE_CACHE = $(BUILD_DIR)/CMakeCache.txt

## Set CMake cache entries from variable overrides provided on command line.
## E.g. `make PRODUCT=myproduct` -> `cmake -DPRODUCT=myproduct`.
CONFIG_ARGS += $(foreach override,$(MAKEOVERRIDES),-D$(override))


### Utility functions
define get_build_dir
jq -r '.configurePresets.[]|select(.name == "$(CONFIG_PRESET)")|.binaryDir' \
    $(wildcard $(THIS_DIR)/CMake*Presets.json) \
    | sed -e 's|$${sourceDir}|$(CURDIR)|g'
endef

define get_package_dir
jq -r '.packagePresets.[]|select(.name == "$(PACKAGE_PRESET)")|.packageDirectory' \
    $(wildcard $(THIS_DIR)/CMake*Presets.json) \
    | sed -e 's|$${sourceDir}|$(CURDIR)|g'
endef

define list_cache
cmake -L -N --preset $(CONFIG_PRESET) 
endef

define get_cached
cmake -L -N --preset $(CONFIG_PRESET) | awk 'BEGIN {FS="="} /^$(1):/ {print $$2}'
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
cmake:
	@echo
	@echo "#############################################################"
	@echo "Generating CMake preset: $(CONFIG_PRESET)"
	@echo "#############################################################"
	@echo
	@cmake --preset "$(CONFIG_PRESET)" $(CONFIG_ARGS)

.PHONY: python_shell
python_shell: install
	@env PYTHONPATH=$(INSTALL_DIR)/$(call get_cached,PYTHON_INSTALL_DIR) $(PYTHON)

.PHONY: python_shell/%
python_shell/%: install
	@echo "Launching interactive Python prompt with module: $*"
	@env PYTHONPATH=$(INSTALL_DIR)/$(call get_cached,PYTHON_INSTALL_DIR) $(PYTHON) -i -m $*

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
	@$(call get_build_dir)

.PHONY: clean
clean: clean/cmake

.PHONY: clean/cmake cmake_clean
clean/cmake cmake_clean:
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
%:
	@[ -f "$(CMAKE_TAG)" ] || cmake --preset "$(CONFIG_PRESET)" $(CONFIG_ARGS)
	@cmake --build --preset "$(BUILD_PRESET)" --target $* $(CMAKE_BUILD_ARGS)
