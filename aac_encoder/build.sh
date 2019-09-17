#!/bin/bash
g++ main.c wavreader.c -o aac_encoder -I/usr/local/include -L/usr/local/lib -lfdk-aac

