@echo off

set wildcard=*.h *.cpp *.inl *.c

echo -------------------------
echo -------------------------

echo STATICS FOUND:
findstr -s -n -i -l "static" %wildcard%

echo -------------------------
echo -------------------------

echo GLOBALS FOUND:
findstr -s -n -i -l "global_variable" %wildcard%
findstr -s -n -i -l "local_persist" %wildcard%


echo -------------------------
echo -------------------------