.DEFAULT_GOAL := build

SHELL := /bin/bash

test_lower4: test_lower4.c
	gcc -O0 -g -o $@ $<

run: test_lower4
	./test_lower4