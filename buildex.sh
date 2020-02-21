#!/bin/sh
set -x
protoc -I./proto --cpp_out protoFiles/ proto/*.proto
g++ -c ./protoFiles/*.pb.cc 
mv *.pb.o ./protoFiles -f
