#!/usr/bin/env bash

# Exit immediately if any command below fails.
set -e

make


echo "Generating a test_files directory.."
mkdir -p test_files
rm -f test_files/*


echo "Generating test files.."
printf "Hello, World!\n" > test_files/ascii.input
printf "Hello, World!" > test_files/ascii2.input
printf "Hello,\x00World!\n" > test_files/data.input
printf "" > test_files/empty.input
### TODO: Generate more test files ###
##text encoding tests
printf "\xCA\x8A\n" > test_files/utf8_2byte_test.input
printf "\xE6\x9A\xB8\n" > test_files/utf8_3byte_test.input
printf "\xF3\x96\xAA\x9D\n" > test_files/utf8_4byte_test.input
printf "\xB0\xB6\n" > test_files/ISOtest.input
printf "\xC4\xF9\n" > test_files/ISOtest2.input
printf "\xE3\xE9\xB9\n" > test_files/ISOtest3.input
printf "\xF5\xAC\xB3\xFA\n" > test_files/ISOtest4.input
printf "Hello, World. \xB2\xBB\n" > test_files/ISOwithASCIItest.input
printf "\xCA\x8A\xE6\x9A\xB8\xF3\x96\xAA\x9D\x2E\n" > test_files/utf8_allbytetypes.input
printf "\xE6\x9A\xB8 Hello, World!\n" > test_files/utf8withASCIItest.input

##data tests
#most of these fail. real encoding type outside the scope of this assignment
printf "\xCA\x8A\x9E\n" > test_files/utf8andISOtest.input
printf "\xE6\x8A\xB0\xB6\n" > test_files/utf8andISOtest2.input
printf "\xF7\xBE\x96\xAB\xB9\n" > test_files/utf8andISOtest3.input
printf "\xC6\x97. Hello, World! \xB6\n" > test_files/AllEncodingstest.input
printf "\xE6\x9E\xAE. Hello, World! \xBE\n" > test_files/AllEncodingstest2.input
printf "\xF3\x96\xAA\x9F. Hello, World! \xA6\n" > test_files/AllEncodingstest3.input
printf "\x87\x94\x9F\n" > test_files/between126to160test.input
#This one works. seems 127 makes it data
printf "\x7F\x9B\x8E\n" > test_files/between126to160test2.input
printf "\x80\x8B\x93\n" > test_files/between126to160test3.input
#twice cause it wasn't happy with only 1 byte
printf "\x7F\x7F" > test_files/127test.input
printf "\xC6\x97. Hello, World! \xB6\x8F\n" > test_files/allencodingsand143test.input
printf "\xC6\x97. Hello, World! \xB6\x7F\n" > test_files/allencodingsand127test.input
echo "Running the tests.."
exitcode=0
for f in test_files/*.input
do
  echo ">>> Testing ${f}.."
  file    ${f} | sed -e 's/ASCII text.*/ASCII text/' \
                         -e 's/UTF-8 Unicode text.*/UTF-8 Unicode text/' \
                         -e 's/ISO-8859 text.*/ISO-8859 text/' \
                         -e 's/writable, regular file, no read permission/cannot determine (Permission denied)/' \
                         > "${f}.expected"
  ./file  "${f}" > "${f}.actual"

  if ! diff -u "${f}.expected" "${f}.actual"
  then
    echo ">>> Failed :-("
    exitcode=1
  else
    echo ">>> Success :-)"
  fi
done
exit $exitcode
