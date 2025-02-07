## -*- Makefile -*-
#===============================================================================
## @file Makefile
## @brief Build via CMake
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

MAKEFLAGS    += --no-print-directory
SHARED_DIR   ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR      ?= $(CURDIR)/out
BUILD_DIR    ?= $(OUT_DIR)/build
INSTALL_DIR  ?= $(OUT_DIR)/install
PACKAGE_DIR  ?= $(OUT_DIR)/packages

ifdef TARGET
    export CMAKE_TOOLCHAIN_FILE ?= $(SHARED_DIR)cmake/toolchain-$(TARGET).cmake
	BUILD_DIR := $(BUILD_DIR)-$(TARGET)
	INSTALL_DIR := $(BUILD_DIR)-$(TARGET)
	PACKAGE_DIR := $(BUILD_DIR)-$(TARGET)
else
	TARGET := $(shell uname -s)-$(shell uname -m)

    ifneq ("$(wildcard $(SHARED_DIR)cmake/toolchain-$(TARGET).cmake)","")
        export CMAKE_TOOLCHAIN_FILE ?= $(SHARED_DIR)cmake/toolchain-$(TARGET).cmake
    endif
endif

ifdef BUILD_TYPE
	BUILD_DIR := $(BUILD_DIR)-$(BUILD_TYPE)
else
	BUILD_TYPE = Release
endif

export CMAKE_BUILD_TYPE ?= $(BUILD_TYPE)
ifeq ($(shell uname), Linux)
   export CMAKE_BUILD_PARALLEL_LEVEL = $(shell nproc)
endif

## Set CMake cache entries from variable overrides provided on command line.
## E.g. `make PRODUCT=myproduct` -> `cmake -DPRODUCT=myproduct`.
CMAKE_CONFIG_ARGS += $(foreach override,$(MAKEOVERRIDES),-D$(override))

## Create CMake cache only if this file is absent.
CMAKE_TAG = $(BUILD_DIR)/Makefile

### Check for a target-specific toolchain and use that if available

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

.PHONY: local/strip
local/strip: build
	@echo
	@echo "#############################################################"
	@echo "Installing and stripping in ${INSTALL_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR) --strip

.PHONY: uninstall
uninstall:
	@rm -rfv "$(INSTALL_DIR)"

.PHONY: get_common_version
get_common_vesion:
	@cmake -L -P local.cmake -P defaults.cmake 2>&1 | \
		sed -ne 's/^VERSION:STRING=\(.*\)$$/\1/ p'

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
	@cmake --build "$(BUILD_DIR)" --target test

.PHONY: test/report
test/report: build
	@echo
	@echo "#############################################################"
	@echo "Testing & reporting failures"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)" --target test ARGS=--output-on-failure

.PHONY: retest
retest: build
	@echo
	@echo "#############################################################"
	@echo "Rerunning failed tests"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)" --target test ARGS="--rerun-failed --output-on-failure"

.PHONY: build
build: cmake
	@echo
	@echo "#############################################################"
	@echo "Building in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)"

.PHONY: cmake
cmake: $(CMAKE_TAG)

ifneq ($(strip $(CMAKE_CONFIG_ARGS)),)
.PHONY: $(CMAKE_TAG)
endif

$(CMAKE_TAG):
	@echo
	@echo "#############################################################"
	@echo "Generating build files in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cmake -B "$(BUILD_DIR)" $(CMAKE_CONFIG_ARGS)

.PHONY: cmake_clean
cmake_clean:
	@if [ -f "$(CMAKE_TAG)" ]; \
	then \
		echo "Invoking CMake target 'clean'"; \
		cmake --build "$(BUILD_DIR)" --target clean || true; \
	fi

.PHONY: clean
clean: cmake_clean

.PHONY: pkg_clean package_clean
pkg_clean package_clean:
	@rm -rfv "$(PACKAGE_DIR)"

.PHONY: realclean
realclean: pkg_clean uninstall
	@rm -rfv "$(BUILD_DIR)"

.PHONY: cleanout clean_out
cleanout clean_out:
	@echo "Removing all build outputs: ${OUT_DIR}"
	@rm -rf "$(OUT_DIR)"

.PHONY: pristine distclean
pristine distclean: cleanout

$(BUILD_DIR):
	@mkdir -p "$(BUILD_DIR)"

### Delegate docker_ targets to its own Makefile
docker_%:
	@$(MAKE) -C $(SHARED_DIR)/scripts/docker $(MAKECMDGOALS) HOST_DIR=$(CURDIR)

### Delegate any other target to CMake
%:
	@[ -f "$(CMAKE_TAG)" ] || cmake -B "$(BUILD_DIR)" $(CMAKE_CONFIG_ARGS)
	@cmake --build "$(BUILD_DIR)" --target $(patsubst cmake_%,%,$@)

