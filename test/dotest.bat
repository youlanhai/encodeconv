@echo off

::gbk to utf-8. same as: encodeconv -f test.txt -o test_utf8.txt -s gbk -d utf-8
encodeconv -f test.txt -o test_utf8.txt

::gbk to utf-8 no bom
encodeconv -f test.txt -o test_utf8_nb.txt -nb

::utf-8 to ucs2
encodeconv -f test_utf8.txt -o test_ucs2.txt -d ucs2

::utf-8 to ucs-2be
encodeconv -f test_utf8.txt -o test_ucs_2be.txt -d ucs-2be

::ucs2 to utf-8
encodeconv -f test_ucs2.txt -o test_ucs2_utf8.txt -d utf-8

::ucs-2be to utf-8
encodeconv -f test_ucs_2be.txt -o test_ucs_2be_utf8.txt -d utf-8

@pause
