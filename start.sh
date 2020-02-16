#!/bin/bash

MAX=50
cd bin

for ((i=0; i < MAX; i++))
do
    ./epoll_client&
done
