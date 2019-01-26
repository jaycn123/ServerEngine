#!/bin/bash

MAX=4999
cd bin

for ((i=0; i < MAX; i++))
do
    ./epoll_client&
done
