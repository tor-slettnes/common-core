## -*- Makefile -*-
#===============================================================================
## @file Makefile
## @brief Build via CMake
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

SHARED_DIR  ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR     ?= out
BUILD_TYPE  ?= Release

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

export CMAKE_INSTALL_PREFIX ?= ${INSTALL_DIR}
export CMAKE_BUILD_TYPE ?= $(BUILD_TYPE)

ifeq ($(shell uname), Linux)
   export CMAKE_BUILD_PARALLEL_LEVEL ?= $(shell nproc)
endif

### Check for a target-specific toolchain and use that if available

install: build
	@echo
	@echo "#############################################################"
	@echo "Installing in ${INSTALL_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR)

install/strip: build
	@cmake --install $(BUILD_DIR) --prefix $(INSTALL_DIR) --strip

uninstall:
	@rm -rfv "$(INSTALL_DIR)"

build: cmake
	@echo
	@echo "#############################################################"
	@echo "Building in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cmake --build "$(BUILD_DIR)"

cmake:
	@echo
	@echo "#############################################################"
	@echo "Generating build files in ${BUILD_DIR}"
	@echo "#############################################################"
	@echo
	@cmake -B "$(BUILD_DIR)"

clean: uninstall cleanbuild

distclean:
	@rm -rfv "$(OUT_DIR)"

cmake/clean:
	@[ -d "$(BUILD_DIR)" ] && $(MAKE) -C "$(BUILD_DIR)" clean || true


$(BUILD_DIR):
	@mkdir -p "$(BUILD_DIR)"

%:
	@$(MAKE) -C "$(BUILD_DIR)" $(MAKECMDGOALS)


.PHONY: install build cmake clean
