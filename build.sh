#!/bin/sh
set -x
protoc -I./proto --cpp_out protoFiles/ proto/*.proto
g++ -c ./protoFiles/*.pb.cc 
mv *.pb.o ./protoFiles -f
rm -rf libso/*
g++ -c ./lib/*.cpp -std=c++11 
mv *.o libso
rm ./bin/* -rf

g++ -o bin/chatServer example/chatServer.cpp ./protoFiles/*.o ./libso/*.o  -lpthread  -lprotobuf 
g++ ./example/client.cpp -o ./bin/epoll_client
./bin/chatServer
