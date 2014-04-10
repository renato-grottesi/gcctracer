#!/bin/bash

echo Plot allocated memory
cat gcctrace.dump | cut -d',' -f8 | gnuplot -e "set term dumb; plot '-';"

echo Plot delta memory allocation
cat gcctrace.dump | cut -d',' -f9 | gnuplot -e "set term dumb; plot '-';"
