#!/bin/sh

GCC=/c/TDM-GCC-64/bin/gcc.exe
JAVA_HOME="/c/Program Files/Java/jdk1.7.0_25"

${GCC} -m64 -shared \
  -o ../../plugins/net.sf.egtkwave.ui.win32.win32.x86_64/egtkwave.dll \
  -I"${JAVA_HOME}/include" -I"${JAVA_HOME}/include/win32" \
  egtkwave_win32_jni.c

