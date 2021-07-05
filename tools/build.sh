#!/bin/bash

qmake -o Makefile ESerialPort.pro 
make && ./release/ESerialPort

