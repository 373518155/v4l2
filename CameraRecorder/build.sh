#!/bin/bash
gcc queue.c CameraRecorder.c slog.c -o CameraRecorder -I/usr/local/include -L/usr/local/lib -lx264 -lpthread
