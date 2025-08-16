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

test-%: build-%
	cd $(BUILD_DIR) && GTEST_COLOR=1 \
		&& ./tests --gtest_output="xml:test_detail.xml" \
		--gtest_color=yes \
		--gtest_print_time=1 \
		--gtest_break_on_failure \
		--gtest_print_utf8=1 \
		|| (echo "Tests failed"; exit 1)


clean:
	rm -rf $(BUILD_DIR)

# Пути к исходным файлам для форматирования
FORMAT_SRCS := $(shell find ./src -type f -name '*.h' -or -name '*.cpp' -or -name '*.hpp')

# Команда для форматирования
.PHONY: format
format:
	clang-format -i --style=file $(FORMAT_SRCS)