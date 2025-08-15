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

# Пути к исходным файлам для форматирования
FORMAT_SRCS := $(shell find ./src -type f -name '*.h' -or -name '*.cpp' -or -name '*.hpp')

# Команда для форматирования
.PHONY: format
format:
	clang-format -i --style=file $(FORMAT_SRCS)