#!/bin/bash

tempnam='vhtest.tmp.R6sq9'

test_cks_file () {
	rm -f $tempnam
	$1 > $tempnam
	local retval1=$?
	if [ $retval1 -ne 0 ]; then
	   echo "command ==$1== failed"
	   exit 1;
	fi
	diff -q $tempnam $2
	local retval2=$?
	if [ $retval2 -ne 0 ]; then
	   echo "verification of file $2 failed"
	   exit 1;
	fi
	rm -f $tempnam
}

verify_cks_file () {
	$1 > /dev/null
	local retval1=$?
	if [ $retval1 -ne 0 ]; then
	   echo "command ==$1== failed"
	   exit 1;
	fi
}

test_cks_file "../vh128sum -l64 test*" "output_64.txt"
test_cks_file "../vh128sum test*" "output_128.txt"
test_cks_file "../vh128sum -l256 test*" "output_256.txt"
test_cks_file "../vh128sum --length 512 test*" "output_512.txt"
test_cks_file "../vh1024sum test*" "output_1024.txt"

test_cks_file "../vh128sum --length 64 --tag test*" "BSD_output_64.txt"
test_cks_file "../vh128sum --tag test*" "BSD_output_128.txt"
test_cks_file "../vh128sum --tag -l256 test*" "BSD_output_256.txt"
test_cks_file "../vh128sum --tag --length 512 test*" "BSD_output_512.txt"
test_cks_file "../vh1024sum --tag test*" "BSD_output_1024.txt"

test_cks_file "../vh128sum --zero test*" "output_zero_128.txt"
test_cks_file "../vh128sum --tag -z test*" "BSD_output_zero_128.txt"

verify_cks_file "../vh128sum -l64 -c output_64.txt"
verify_cks_file "../vh128sum -c output_128.txt"
verify_cks_file "../vh128sum -l256 -c output_256.txt"
verify_cks_file "../vh128sum --length 512 --check output_512.txt"
verify_cks_file "../vh1024sum --check output_1024.txt"

verify_cks_file "../vh128sum -cQ -l64 BSD_output_64.txt"
verify_cks_file "../vh128sum -cq BSD_output_128.txt"
verify_cks_file "../vh128sum -l256 -c --quiet BSD_output_256.txt"
verify_cks_file "../vh128sum --length 512 -Q --check BSD_output_512.txt"
verify_cks_file "../vh1024sum --check --status BSD_output_1024.txt"

echo "===================================="
echo "$0: all tests succeeded"
echo "===================================="
