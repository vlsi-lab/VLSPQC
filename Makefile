SHELL := /bin/bash

.PHONY: help menu run

help:
	@echo "VLSPQC test launcher"
	@echo ""
	@echo "Targets:"
	@echo "  make menu                                      # full interactive menu"
	@echo "  make run                                       # same as menu"
	@echo "  make run CATEGORY=common                       # pick family + test"
	@echo "  make run CATEGORY=common FAMILY=ML-KEM         # pick test within family"
	@echo "  make run CATEGORY=common FAMILY=ML-KEM TEST=2  # fully non-interactive"
	@echo ""
	@echo "CATEGORY : common | profiled"
	@echo "FAMILY   : discovered dynamically from masked/, original/, other/, profiling/"
	@echo "           typical values: ASCON | CROSS | FALCON | HQC | LESS | ML-DSA | ML-KEM | SLH-DSA | tests"
	@echo "TEST     : 1-based variant number within the chosen family"

menu:
	@bash run_test.sh

run:
	@bash run_test.sh "$(CATEGORY)" "$(FAMILY)" "$(TEST)"
