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

test_cks_stdin () {
	local msg=`cat $2 | $1`
	local cks1=`echo $msg | awk '{print $1}'`
	local msg2=`grep $2 $3`
	local cks2=`echo $msg2 | awk '{print $1}'`
	if [ $cks1 != $cks2 ]; then
		echo "checksum mismatch for file $2: got: $cks1, expected: $cks2"
		exit 1;
	fi
}

check_cmd () {
	$1 > /dev/null
	local retval1=$?
	if [ $retval1 -ne 0 ]; then
	   echo "command ==$1== failed"
	   exit 1;
	fi
}

check_error_msg () {
	rm -f $tempnam
	$1 > $tempnam
	local retval1=$?
	if [ $retval1 -ne 1 ]; then
	   echo "command ==$1== did not fail"
	   exit 1;
	fi
	grep "$2" $tempnam > /dev/null
	local retval2=$?
	if [ $retval2 -ne 0 ]; then
		echo "error msg does not match ==$2=="
		exit 1;
	fi
	rm -f $tempnam
}

test_cks_file "../vh32sum test*" "output_32.txt"
test_cks_file "../vh128sum -l64 test*" "output_64.txt"
test_cks_file "../vh128sum test*" "output_128.txt"
test_cks_file "../vh128sum -l256 test*" "output_256.txt"
test_cks_file "../vh128sum --length 512 test*" "output_512.txt"
test_cks_file "../vh1024sum test*" "output_1024.txt"

test_cks_file "../vh32sum --tag test*" "BSD_output_32.txt"
test_cks_file "../vh128sum --length 64 --tag test*" "BSD_output_64.txt"
test_cks_file "../vh128sum --tag test*" "BSD_output_128.txt"
test_cks_file "../vh128sum --tag -l256 test*" "BSD_output_256.txt"
test_cks_file "../vh128sum --tag --length 512 -- test*" "BSD_output_512.txt"
test_cks_file "../vh1024sum --tag test*" "BSD_output_1024.txt"

test_cks_file "../vh128sum --zero test*" "output_zero_128.txt"
test_cks_file "../vh128sum --tag -z test*" "BSD_output_zero_128.txt"

test_cks_stdin "../vh32sum" "test0128" "output_32.txt"
test_cks_stdin "../vh32sum" "test0256" "output_32.txt"
test_cks_stdin "../vh64sum" "test0128" "output_64.txt"
test_cks_stdin "../vh64sum" "test0512" "output_64.txt"
test_cks_stdin "../vh128sum" "test0128" "output_128.txt"
test_cks_stdin "../vh128sum" "test1024" "output_128.txt"
test_cks_stdin "../vh256sum" "test0128" "output_256.txt"
test_cks_stdin "../vh256sum" "test1536" "output_256.txt"
test_cks_stdin "../vh512sum" "test0128" "output_512.txt"
test_cks_stdin "../vh512sum" "test2048" "output_512.txt"
test_cks_stdin "../vh1024sum" "test0128" "output_1024.txt"
test_cks_stdin "../vh1024sum" "test3072" "output_1024.txt"

check_cmd "../vh32sum -c output_32.txt"
check_cmd "../vh128sum -l64 -c output_64.txt"
check_cmd "../vh128sum -c output_128.txt"
check_cmd "../vh128sum -l256 -c output_256.txt"
check_cmd "../vh128sum --leng 512 --check output_512.txt"
check_cmd "../vh1024sum --check output_1024.txt"

check_cmd "../vh32sum -cQ BSD_output_32.txt"
check_cmd "../vh128sum -cQ -l64 BSD_output_64.txt"
check_cmd "../vh128sum -cq BSD_output_128.txt"
check_cmd "../vh128sum -l256 -c --quiet BSD_output_256.txt"
check_cmd "../vh128sum --length 512 -Q --check BSD_output_512.txt"
check_cmd "../vh1024sum --check --stat BSD_output_1024.txt"

check_cmd "../vh256sum -h"
check_cmd "../vh512sum --help"
check_cmd "../vh256sum -v"
check_cmd "../vh512sum --vers"
check_cmd "../vh512sum --verb test0128"

check_error_msg "../vh64sum --ver" "unrecognized option '--ver'"

check_error_msg "../vh64sum -i test0128" "the --ignore-missing option is meaningful only when verifying checksums"
check_error_msg "../vh64sum -Q test0128" "the --status option is meaningful only when verifying checksums"
check_error_msg "../vh64sum -q test0128" "the --quiet option is meaningful only when verifying checksums"
check_error_msg "../vh64sum -w test0128" "the --warn option is meaningful only when verifying checksums"
check_error_msg "../vh64sum -s test0128" "the --strict option is meaningful only when verifying checksums"
check_error_msg "../vh64sum --tag -c BSD_output_64.txt" "the --tag option is meaningless when verifying checksums"
check_error_msg "../vh64sum --tag -t test0128" ": --tag does not support --text mode"
check_error_msg "../vh64sum -ct output_64.txt" "the --binary and --text options are meaningless when verifying checksums"
check_error_msg "../vh64sum -cb output_64.txt" "the --binary and --text options are meaningless when verifying checksums"
check_error_msg "../vh128sum -cz output_zero_128.txt" "the --zero option is not supported when verifying checksums"

echo "===================================="
echo "$0: all tests succeeded"
echo "===================================="
