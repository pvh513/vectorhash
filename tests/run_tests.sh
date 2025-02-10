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

check_no_error_msg () {
	rm -f $tempnam
	$1 > $tempnam
	grep "$2" $tempnam > /dev/null
	local retval2=$?
	if [ $retval2 -eq 0 ]; then
		echo "error msg is matched ==$2=="
		exit 1;
	fi
	rm -f $tempnam
}

check_no_output () {
	rm -f $tempnam
	$1 > $tempnam
	if [ -s $tempnam ]; then
		echo "output was found in $tempnam"
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

test_cks_file "../vh32sum --tag --scalar test*" "BSD_output_32.txt"
test_cks_file "../vh128sum --length 64 --scalar --tag test*" "BSD_output_64.txt"
test_cks_file "../vh128sum --tag --scalar test*" "BSD_output_128.txt"
test_cks_file "../vh128sum --tag --scalar -l256 test*" "BSD_output_256.txt"
test_cks_file "../vh128sum --scalar --tag --length 512 -- test*" "BSD_output_512.txt"
test_cks_file "../vh1024sum --scalar --tag test*" "BSD_output_1024.txt"

test_cks_file "../vh128sum --zero test*" "output_zero_128.txt"
test_cks_file "../vh128sum --tag -z test*" "BSD_output_zero_128.txt"

test_cks_stdin "../vh32sum" "test0128" "output_32.txt"
test_cks_stdin "../vh32sum -" "test0256" "output_32.txt"
test_cks_stdin "../vh32sum --scalar" "test0256" "output_32.txt"
test_cks_stdin "../vh64sum" "test0128" "output_64.txt"
test_cks_stdin "../vh64sum -" "test0512" "output_64.txt"
test_cks_stdin "../vh64sum --scalar" "test0512" "output_64.txt"
test_cks_stdin "../vh128sum" "test0128" "output_128.txt"
test_cks_stdin "../vh128sum -" "test1024" "output_128.txt"
test_cks_stdin "../vh128sum --scalar" "test1024" "output_128.txt"
test_cks_stdin "../vh256sum" "test0128" "output_256.txt"
test_cks_stdin "../vh256sum -" "test1536" "output_256.txt"
test_cks_stdin "../vh256sum --scalar" "test1536" "output_256.txt"
test_cks_stdin "../vh512sum" "test0128" "output_512.txt"
test_cks_stdin "../vh512sum -" "test2048" "output_512.txt"
test_cks_stdin "../vh512sum --scalar" "test2048" "output_512.txt"
test_cks_stdin "../vh1024sum" "test0128" "output_1024.txt"
test_cks_stdin "../vh1024sum -" "test3072" "output_1024.txt"
test_cks_stdin "../vh1024sum --scalar" "test3072" "output_1024.txt"

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
check_cmd "../vh512sum --verbose --scalar test0128"

# make sure the code continues after a missing file
check_error_msg "../vh64sum tost0000 test0000" "tost0000: No such file or directory"
check_error_msg "../vh64sum tost0000 test0000" "73711a77d6031b6f  test0000"

check_error_msg "../vh64sum -G" "invalid option -- 'G'"
check_error_msg "../vh64sum --ver" "unrecognized option '--ver'"
check_error_msg "../vh64sum -l sum" "invalid length: 'sum'"
check_error_msg "../vh64sum -l 156" "invalid length: '156'"
check_error_msg "../vh64sum -l156" "length must be 32, 64, 128, 256, 512, or 1024"
check_error_msg "../vh64sum --len sum" "invalid length: 'sum'"
check_error_msg "../vh64sum --length 156" "invalid length: '156'"
check_error_msg "../vh64sum --length 156" "length must be 32, 64, 128, 256, 512, or 1024"
check_error_msg "../vh64sum tost0000" "tost0000: No such file or directory"

check_error_msg "../vh64sum -i test0128" "the --ignore-missing option is meaningful only when verifying checksums"
check_error_msg "../vh64sum -Q test0128" "the --status option is meaningful only when verifying checksums"
check_error_msg "../vh64sum -q test0128" "the --quiet option is meaningful only when verifying checksums"
check_error_msg "../vh64sum -w test0128" "the --warn option is meaningful only when verifying checksums"
check_error_msg "../vh64sum -s test0128" "the --strict option is meaningful only when verifying checksums"
check_error_msg "../vh64sum --tag -c BSD_output_64.txt" "the --tag option is meaningless when verifying checksums"
check_error_msg "../vh64sum --tag -t test0128" ": --tag does not support --text mode"
check_error_msg "../vh64sum -ct output_64.txt" "the --binary and --text options are meaningless when verifying checksums"
check_error_msg "../vh64sum -c --text output_64.txt" "the --binary and --text options are meaningless when verifying checksums"
check_error_msg "../vh64sum -cb output_64.txt" "the --binary and --text options are meaningless when verifying checksums"
check_error_msg "../vh64sum -c --binary output_64.txt" "the --binary and --text options are meaningless when verifying checksums"
check_error_msg "../vh128sum -cz output_zero_128.txt" "the --zero option is not supported when verifying checksums"
check_error_msg "../vh64sum -cQ --verbose output_64.txt" "the --verbose option conflicts with --status"

check_error_msg "../vh32sum --check --strict error0_32.txt" "WARNING: 1 line is improperly formatted"

check_error_msg "../vh128sum --check error1_128.txt" "test0768: OK"
check_error_msg "../vh128sum --check error1_128.txt" "test1024: FAILED"
check_error_msg "../vh128sum --check error1_128.txt" "tost3072: No such file or directory"
check_error_msg "../vh128sum --check error1_128.txt" "tost3072: FAILED open or read"
check_error_msg "../vh128sum --check error1_128.txt" "WARNING: 1 listed file could not be read"
check_error_msg "../vh128sum --check error1_128.txt" "WARNING: 1 computed checksum did NOT match"
check_error_msg "../vh128sum --check error1_128.txt" "WARNING: 1 line is improperly formatted"

check_error_msg "../vh128sum -cl256 error2_256.txt" "test0256: OK"
check_error_msg "../vh128sum -cl256 error2_256.txt" "test0128: FAILED"
check_error_msg "../vh128sum -cl256 error2_256.txt" "test2048: FAILED"
check_error_msg "../vh128sum -cl256 error2_256.txt" "tost0000: No such file or directory"
check_error_msg "../vh128sum -cl256 error2_256.txt" "tost0000: FAILED open or read"
check_error_msg "../vh128sum -cl256 error2_256.txt" "tost1536: No such file or directory"
check_error_msg "../vh128sum -cl256 error2_256.txt" "tost1536: FAILED open or read"
check_error_msg "../vh128sum -cl256 error2_256.txt" "WARNING: 2 listed files could not be read"
check_error_msg "../vh128sum -cl256 error2_256.txt" "WARNING: 2 computed checksums did NOT match"
check_error_msg "../vh128sum -cl256 error2_256.txt" "WARNING: 2 lines are improperly formatted"

check_error_msg "../vh128sum -cl64 error3_64.txt" "error3_64.txt: no properly formatted VH64 checksum lines found"

check_error_msg "../vh128sum -wcl512 BSD_error1_512.txt" "test0256: OK"
check_error_msg "../vh128sum -wcl512 BSD_error1_512.txt" "tost1536: No such file or directory"
check_error_msg "../vh128sum -wcl512 BSD_error1_512.txt" "tost1536: FAILED open or read"
check_error_msg "../vh128sum -wcl512 BSD_error1_512.txt" "test3072: FAILED"
check_error_msg "../vh128sum -wcl512 BSD_error1_512.txt" "WARNING: 1 listed file could not be read"
check_error_msg "../vh128sum -wcl512 BSD_error1_512.txt" "WARNING: 1 computed checksum did NOT match"
check_error_msg "../vh128sum -wcl512 BSD_error1_512.txt" "WARNING: 1 line is improperly formatted"

check_error_msg "../vh128sum --check --warn error1_128.txt" "error1_128.txt: 11: improperly formatted VH128 checksum line"
check_error_msg "../vh128sum -wcl256 error2_256.txt" "error2_256.txt: 12: improperly formatted VH256 checksum line"
check_error_msg "../vh128sum -wcl256 error2_256.txt" "error2_256.txt: 13: improperly formatted VH256 checksum line"
check_error_msg "../vh128sum -wcl512 BSD_error1_512.txt" "BSD_error1_512.txt: 7: improperly formatted VH512 checksum line"

check_no_error_msg "../vh128sum --check --ignore-missing error1_128.txt" "tost3072: No such file or directory"
check_no_error_msg "../vh128sum -ci error1_128.txt" "tost3072: FAILED open or read"

check_no_output "../vh128sum -Qcl256 error2_256.txt"
check_no_output "../vh128sum -cl64 --status error3_64.txt"
check_no_output "../vh128sum -cq output_128.txt"

echo "===================================="
echo "$0: all tests succeeded"
echo "===================================="
