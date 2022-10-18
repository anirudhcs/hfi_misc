.DEFAULT_GOAL := build

SHELL := /bin/bash

ROOT_PATH=$(shell realpath ../)
WABT_SRC_ROOT=$(ROOT_PATH)/hfi_wasm2c_sandbox_compiler
WASM2C_RUNTIME_PATH=$(WABT_SRC_ROOT)/wasm2c/
WASM2C_RUNTIME_FILES=$(addprefix $(WASM2C_RUNTIME_PATH), wasm-rt-impl.c wasm-rt-os-unix.c wasm-rt-wasi.c)
HFIPATH=$(ROOT_PATH)/hw_isol_gem5/tests/test-progs/hfi/
FLAGS=-DWASM_NO_UVWASI -I$(WASM2C_RUNTIME_PATH) -I$(HFIPATH)
GUARDPAGES_FLAGS=-DWASM_USE_GUARD_PAGES
HFI_FLAGS=-DWASM_USE_HFI -DHFI_EMULATION3
BENCHMARK_FLAGS=-O3

ifeq ($(BENCH_OUTPUTFOLDER),)
BENCH_OUTPUTFOLDER=./
endif

test_%: %.c
	gcc -O0 -g -o $@ $<

benchmark_mprotect_guardpages: benchmark_mprotect.c $(WASM2C_RUNTIME_FILE)
	gcc $(BENCHMARK_FLAGS) -o $@ $< $(WASM2C_RUNTIME_FILES) $(FLAGS) $(GUARDPAGES_FLAGS)

benchmark_mprotect_hfi: benchmark_mprotect.c $(WASM2C_RUNTIME_FILE)
	gcc $(BENCHMARK_FLAGS) -o $@ $< $(WASM2C_RUNTIME_FILES) $(FLAGS) $(HFI_FLAGS)

run_%: ./%
	./$*

benchmark_mprotect: benchmark_mprotect_guardpages benchmark_mprotect_hfi
	hyperfine -N --warmup 10 --export-json "$(BENCH_OUTPUTFOLDER)/benchmark_mprotect_guardpages.json" ./benchmark_mprotect_guardpages && \
		cat "$(BENCH_OUTPUTFOLDER)/benchmark_mprotect_guardpages.json" | jq '.results[0].mean * 1000' > "$(BENCH_OUTPUTFOLDER)/benchmark_mprotect_guardpages.txt"
	hyperfine -N --warmup 10 --export-json "$(BENCH_OUTPUTFOLDER)/benchmark_mprotect_hfi.json" ./benchmark_mprotect_hfi && \
		cat "$(BENCH_OUTPUTFOLDER)/benchmark_mprotect_hfi.json" | jq '.results[0].mean * 1000' > "$(BENCH_OUTPUTFOLDER)/benchmark_mprotect_hfi.txt"

build: test_lower4 test_topmost benchmark_mprotect_guardpages benchmark_mprotect_hfi

