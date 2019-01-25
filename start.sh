#!/bin/bash

MAX=10000
cd bin

for ((i=0; i < MAX; i++))
do
    ./epoll_client&
done
