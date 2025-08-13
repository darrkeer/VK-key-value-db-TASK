CONF ?= debug

BUILD_DIR := build

.PHONY: all build test clean

all: build test

build:
	@mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=$(CONF) ..
	$(MAKE) -C $(BUILD_DIR)

test: build
	$(MAKE) -C $(BUILD_DIR) test

build-%:
	$(MAKE) build CONF=$*

test-%:
	$(MAKE) test CONF=$*

clean:
	rm -rf $(BUILD_DIR)