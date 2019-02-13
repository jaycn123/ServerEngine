#!/bin/sh
set -x
protoc -I./proto --cpp_out protoFiles/ proto/*.proto
g++ -c ./protoFiles/*.pb.cc 
mv *.pb.o ./protoFiles -f
rm -rf libso/*
g++ -g -c ./lib/*.cpp -std=c++17 
mv *.o libso
rm ./bin/* -rf
g++ -g -o bin/chatServer example/chatServer.cpp ./protoFiles/*.o ./libso/*.o  -lpthread  -lprotobuf -std=c++17 
g++ ./example/client.cpp ./protoFiles/*.o  -lprotobuf -lpthread -o ./bin/epoll_client -std=c++17
./bin/chatServer
