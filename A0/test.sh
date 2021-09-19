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
printf "\xF3\x96\xAA\x9D" > test_files/utf8_4byte_test.input
printf "\xB0\xB6\n" > test_files/ISOtest.input
printf "\xC4\xF9" > test_files/ISOtest2.input
printf "Hello, World. \xB2\xBB\n" > test_files/ISOwithASCIItest.input
printf "\xCA\x8A\xE6\x9A\xB8\xF3\x96\xAA\x9D\x2E\n" > test_files/utf8_allbytetypes.input
##data tests
#fails. real encoding type outside our scope
printf "\xCA\x8A\xB0\xB6\n" > test_files/utf8andISOtest.input

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
