.DEFAULT_GOAL := build

SHELL := /bin/bash

test_lower4: test_lower4.c
	gcc -O0 -g -o $@ $<

run_test_lower: test_lower4
	./test_lower4

test_topmost: test_topmost.c
	gcc -O0 -g -o $@ $<

run_test_topmost: test_topmost
	./test_topmost