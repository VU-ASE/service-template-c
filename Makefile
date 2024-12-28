# Makefile in accordance with the docs on git management (to use in combination with meta)
.PHONY: build start clean test

BUILD_DIR=bin/
BINARY_NAME=SERVICE_NAME

lint:
	@echo "Lint check..."
	@echo "No linting configured"

build: lint
	@echo "building ${BINARY_NAME}"
	@mkdir -p $(BUILD_DIR)
	@gcc -I/usr/include/cjson -o $(BUILD_DIR)$(BINARY_NAME) src/*.c -lroverlib-c

start: build
	@echo "starting ${BINARY_NAME}"
	./${BUILD_DIR}${BINARY_NAME} 

clean:
	@echo "Cleaning all targets for ${BINARY_NAME}"
	rm -rf $(BUILD_DIR)

test: lint
	@echo "No tests configured"

