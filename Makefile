## -*- Makefile -*-
#===============================================================================
## @file Makefile
## @brief Build via CMake
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

SHARED_DIR   ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR      ?= $(CURDIR)/out
BUILD_TYPE   ?= Release

ifdef TARGET
    export CMAKE_TOOLCHAIN_FILE ?= $(SHARED_DIR)cmake/toolchain-$(TARGET).cmake
else
	TARGET := $(shell uname -s)-$(shell uname -m)

    ifneq ("$(wildcard $(SHARED_DIR)cmake/toolchain-$(TARGET).cmake)","")
        export CMAKE_TOOLCHAIN_FILE ?= $(SHARED_DIR)cmake/toolchain-$(TARGET).cmake
    endif
endif

BUILD_FLAVOR ?= $(TARGET)-$(BUILD_TYPE)
BUILD_DIR    ?= $(OUT_DIR)/build/$(BUILD_FLAVOR)
INSTALL_DIR  ?= $(OUT_DIR)/install/$(TARGET)
PACKAGE_DIR  ?= $(OUT_DIR)/packages

export CMAKE_INSTALL_PREFIX ?= ${INSTALL_DIR}
export CMAKE_BUILD_TYPE ?= $(BUILD_TYPE)
export CPACK_PACKAGE_DIRECTORY ?= $(PACKAGE_DIR)

ifeq ($(shell uname), Linux)
   export CMAKE_BUILD_PARALLEL_LEVEL ?= $(shell nproc)
endif

### Check for a target-specific toolchain and use that if available

all: test package

package: build
	@echo
	@echo "#############################################################"
	@echo "Creating packages in ${PACKAGE_DIR}"
	@echo "#############################################################"
	@echo
	@cpack --config "${BUILD_DIR}/CPackConfig.cmake" -B "${PACKAGE_DIR}"

install: build
	@echo
	@echo "#############################################################"
	@echo "Installing in ${INSTALL_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR)

install/strip: build
	@echo
	@echo "#############################################################"
	@echo "Installing and stripping in ${INSTALL_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR) --strip

uninstall:
	@rm -rfv "$(INSTALL_DIR)"

test: build
	@echo
	@echo "#############################################################"
	@echo "Testing in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cd "$(BUILD_DIR)" && ctest

build: cmake
	@echo
	@echo "#############################################################"
	@echo "Building in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)"

cmake: $(BUILD_DIR)
	@echo
	@echo "#############################################################"
	@echo "Generating build files in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cmake -B "$(BUILD_DIR)"

clean: uninstall cmake/clean

pkg_clean:
	@rm -rfv "$(PACKAGE_DIR)"

realclean:
	@rm -rfv "$(BUILD_DIR)" "$(INSTALL_DIR)"

realclean_all:
	@rm -rfv "$(OUT_DIR)"/build "$(OUT_DIR)"/install

pristine:
	@rm -rfv "$(OUT_DIR)"

cmake/clean:
	@[ -d "$(BUILD_DIR)" ] && $(MAKE) -C "$(BUILD_DIR)" clean || true


$(BUILD_DIR):
	@mkdir -p "$(BUILD_DIR)"

%:
	@$(MAKE) -C "$(BUILD_DIR)" $(MAKECMDGOALS)


.PHONY: all install install/test test build cmake clean realclean realclean_all pristine
