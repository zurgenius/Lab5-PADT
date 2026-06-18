BUILD_DIR = build
LINT_BUILD_DIR = build-lint
FORMAT_FILES = $(shell find include src tests -type f \( -name '*.h' -o -name '*.cpp' -o -name '*.tpp' \))

.PHONY: all configure configure-gui configure-tests demo-data program run tests lint format clean

all: program

demo-data:
	rm -f data/demo.sqlite3
	sqlite3 data/demo.sqlite3 < data/demo_seed.sql

configure:
	cmake -S . -B $(BUILD_DIR)

configure-gui:
	cmake -S . -B $(BUILD_DIR) -DWITH_RELALG_GUI=ON

configure-tests:
	cmake -S . -B $(BUILD_DIR) -DWITH_RELALG_GUI=OFF

program: demo-data configure-gui
	cmake --build $(BUILD_DIR) --target relalg_program
	cp $(BUILD_DIR)/program program

run: program
	./program

tests: configure-tests
	cmake --build $(BUILD_DIR) --target relalg_tests
	ctest --test-dir $(BUILD_DIR) --output-on-failure
	cp $(BUILD_DIR)/tests_runner tests_runner

lint:
	cmake -S . -B $(LINT_BUILD_DIR) -DRELALG_ENABLE_WERROR=ON -DWITH_RELALG_GUI=OFF
	cmake --build $(LINT_BUILD_DIR) --target relalg_program
	cmake --build $(LINT_BUILD_DIR) --target relalg_tests
	ctest --test-dir $(LINT_BUILD_DIR) --output-on-failure

format:
	@if command -v clang-format >/dev/null 2>&1; then \
		clang-format -i $(FORMAT_FILES); \
	else \
		echo "clang-format is not installed"; \
		exit 1; \
	fi

clean:
	rm -rf $(BUILD_DIR) $(LINT_BUILD_DIR) program tests_runner
