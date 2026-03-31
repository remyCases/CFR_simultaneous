# Detect OS
ifeq ($(OS), Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
endif

# C99 specific default values
COMPILER ?= gcc
VERBOSE ?= OFF
CMAKE_GENERATOR ?= "Ninja"

# Cross compatibilty Linux-Windows
ifeq ($(DETECTED_OS), Linux)
	RM = rm -rf
	MD = mkdir -p
	CAT = cat
	CP = cp
	SANITIZE ?= OFF
	WHERE = which
	PYTHON = python3
	VENV=venv
	BIN=$(VENV)/bin
	PIP=$(BIN)/pip
	VENV_MYPY=$(BIN)/$(MYPY)
	VENV_PYTHON=$(BIN)/$(PYTHON)
	SHELL_EXEC = /bin/bash
	ECHO = echo
	NULL_DEVICE = /dev/null
	GET = 
else
	RM = powershell -Command "Remove-Item -Recurse"
	MD = powershell -Command "New-Item -Type Directory"
	CAT = powershell -Command "Get-Content -encoding UTF8"
	CP = powershell -Command "Copy-Item"
	WHERE = where.exe
	PYTHON = python
	VENV=.venv
	BIN=$(VENV)\Scripts
	PIP=$(BIN)\pip
	VENV_MYPY=$(BIN)\$(MYPY)
	VENV_PYTHON=$(BIN)\$(PYTHON)
	SHELL_EXEC = powershell
	ECHO = powershell -Command "Write-Host"
	NULL_DEVICE = NUL
	GET = powershell Get-Content
endif

CMAKE_BASE_CMD = cmake -Bbuild -G $(CMAKE_GENERATOR) \
    -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
    -DCMAKE_C_COMPILER=$(COMPILER) \
    -DCMAKE_VERBOSE_MAKEFILE=$(VERBOSE) \
	-Wall \
	-Wextra

ifeq ($(DETECTED_OS),Windows)
    CMAKE_CMD = $(CMAKE_BASE_CMD)
else
    CMAKE_CMD = $(CMAKE_BASE_CMD) -DENABLE_SANITIZER=$(SANITIZE)
endif

.DELETE_ON_ERROR:
build:
	$(CMAKE_CMD)
	cd build && ninja

run_rps:
	./build/bin/main.exe RPS

run_blt:
	./build/bin/main.exe Blotto

clean:
	@$(RM) ./build
