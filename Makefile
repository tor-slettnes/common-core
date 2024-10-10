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
	BUILD_DIR += -$(TARGET)
	INSTALL_DIR += $(TARGET)
	PACKAGE_DIR += $(TARGET)
else
	TARGET := $(shell uname -s)-$(shell uname -m)

    ifneq ("$(wildcard $(SHARED_DIR)cmake/toolchain-$(TARGET).cmake)","")
        export CMAKE_TOOLCHAIN_FILE ?= $(SHARED_DIR)cmake/toolchain-$(TARGET).cmake
    endif
endif

ifdef BUILD_TYPE
	BUILD_DIR += -$(BUILD_TYPE)
else
	BUILD_TYPE = Release
endif

export CMAKE_INSTALL_PREFIX ?= ${INSTALL_DIR}
export CMAKE_BUILD_TYPE ?= $(BUILD_TYPE)
export CPACK_PACKAGE_DIRECTORY ?= $(PACKAGE_DIR)
export PYTHON_VENV=venv

ifeq ($(shell uname), Linux)
   export CMAKE_BUILD_PARALLEL_LEVEL ?= $(shell nproc)
endif

### Check for a target-specific toolchain and use that if available

local: test install

release: test package

package: deb

deb: build
	@echo
	@echo "#############################################################"
	@echo "Creating release packages in ${PACKAGE_DIR}"
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
	@cmake -B "$(BUILD_DIR)" \
		$(if $(PRODUCT),-D PRODUCT="$(PRODUCT)") \
		$(if $(VERSION),-D VERSION="$(VERSION)") \
		$(if $(BUILD_NUMBER),-D BUILD_NUMBER="$(BUILD_NUMBER)") \
		$(if $(PACKAGE_NAME),-D PACKAGE_NAME_PREFIX="$(PACKAGE_NAME)")

clean: pkg_clean uninstall cmake/clean

cmake/clean:
	@[ -d "$(BUILD_DIR)" ] && $(MAKE) -C "$(BUILD_DIR)" clean || true

pkg_clean:
	@rm -rfv "$(PACKAGE_DIR)"

realclean:
	@rm -rfv "$(BUILD_DIR)" "$(INSTALL_DIR)" "$(PACKAGE_DIR)"

realclean_all:
	@rm -rfv "$(OUT_DIR)"/build "$(OUT_DIR)"/install "$(PACKAGE_DIR)"

distclean: pristine venvclean

pristine:
	@rm -rfv "$(OUT_DIR)"

$(BUILD_DIR):
	@mkdir -p "$(BUILD_DIR)"

venv: cmake
	@$(MAKE) -C "$(BUILD_DIR)" python_venv

venvclean:
	@echo "Removing Python Virtual Environment (VENV)"
	@rm -rf "$(PYTHON_VENV)"

docker_%:
	@$(MAKE) -C $(SHARED_DIR)/scripts/docker $(MAKECMDGOALS) HOST_DIR=$(CURDIR)

%:
	@$(MAKE) -C "$(BUILD_DIR)" $(MAKECMDGOALS)


.PHONY: local release package deb install install/strip test build cmake cmake/clean clean realclean realclean_all distclean pristine
