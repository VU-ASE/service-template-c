# Makefile in accordance with the docs on git management (to use in combination with meta)
.PHONY: build start clean test

BUILD_DIR=bin/
BINARY_NAME=SERVICE_NAME

update:
	git submodule update --init --recursive
	git submodule update --recursive --remote

build: update
	@echo "building ${BINARY_NAME}"
	@mkdir -p $(BUILD_DIR)
	gcc -o $(BUILD_DIR)$(BINARY_NAME) \
	./src/*.c ./lib/src/*.c ./lib/src/rovercom/outputs/*.c ./lib/src/rovercom/tuning/*.c \
	-lcjson -lzmq -lprotobuf-c -lhashtable -llist -I/usr/include/cjson -I./lib/include -g

start: build
	@echo "starting ${BINARY_NAME}"
	./${BUILD_DIR}${BINARY_NAME} 

clean:
	@echo "Cleaning all targets for ${BINARY_NAME}"
	rm -rf $(BUILD_DIR)

test: 
	@echo "No tests configured"

