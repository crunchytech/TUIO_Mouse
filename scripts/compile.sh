#!/bin/sh
start_dir=$(pwd)
echo "compiling in "$(pwd)
javac -s bin -d bin -cp lib/libTUIO.jar src/com/crunchy/MouseDriver/TuioMouse.java
cd bin
jar xf ../lib/libTUIO.jar
rm -r META-INF
cd ../
echo "jaring in "$(pwd)
jar cvmf "$start_dir"/MANIFEST.MF "$start_dir"/out/TuioMouse.jar -C bin .

