@echo off

set wildcard=*.h *.cpp *.inl *.c

echo -------------------------
echo -------------------------

echo TODOS FOUND:
findstr -s -n -i -l "TODO" %wildcard%

echo -------------------------
echo -------------------------

