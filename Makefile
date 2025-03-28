# Makefile in accordance with the docs on git management (to use in combination with meta)
.PHONY: build start clean test check-lib

BUILD_DIR=bin/
BINARY_NAME=SERVICE_NAME


check-lib:
	@if [ ! -d "lib" ] || [ -z "$$(ls -A lib 2>/dev/null)" ]; then \
		echo "lib directory doesn't exist or is empty. Cloning repository..."; \
		git clone https://github.com/VU-ASE/service-template-c.git lib; \
	elif [ ! -d "lib/.git" ]; then \
		echo "lib exists but is not a git repository. Removing and cloning..."; \
		rm -rf lib; \
		git clone https://github.com/VU-ASE/service-template-c.git lib; \
	else \
		echo "lib directory exists and is a git repository."; \
	fi


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

