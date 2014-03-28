@echo off

::gbk to utf-8. same as: encodeconv -f test.txt -o test_utf8.txt -s gbk -d utf-8
encodeconv -f test.txt -s gbk -o test_utf8.txt

::gbk to utf-8 no bom
encodeconv -f test.txt -s gbk -o test_utf8_nb.txt -nb

::utf-8 to ucs2
encodeconv -f test_utf8.txt -s gbk  -d ucs2 -o test_ucs2.txt

::utf-8 to ucs-2be
encodeconv -f test_utf8.txt -s gbk -d ucs-2be -o test_ucs_2be.txt

::ucs2 to utf-8
encodeconv -f test_ucs2.txt -s gbk -d utf-8 -o test_ucs2_utf8.txt

::ucs-2be to utf-8
encodeconv -f test_ucs_2be.txt -s gbk -d utf-8 -o test_ucs_2be_utf8.txt

@pause
