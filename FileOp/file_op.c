/**
 * 文件操作
 * 比如：文件截取、文件拼接
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../common/common.h"

#define BUF_SIZE 8192

const char* usage = "使用方法:\n"
                    "文件截取\n"
                    "  %s truncate <file> <start> [end]\n"
                    "          file -- 要截取的文件\n"
                    "          start -- zero-based, 开始字节(包含)\n"
                    "          end -- zero-based, 结束字节(包含，可选值，如果不指定，表示到文件结束)\n"
                    "文件拼接\n"
                    "  %s concate <file1> <file2> ... <fileN>\n"
                    "          最少要有2个文件\n\n";

char programName[100];

unsigned char buffer[BUF_SIZE];

void printUsage() {
    printf(usage, programName, programName);
}

void truncateFile(int argc, char *argv[]) {
    char inFilename[80];
    char outFilename[80];

    strcpy(inFilename, argv[2]);
    int fileSize = getFileSize(inFilename);
    if (fileSize < 0) {
        printf("Error!file[%s] not exists\n", inFilename);
        return;
    }

    if (fileSize == 0) {
        printf("Error!file[%s] is empty\n", inFilename);
        return;
    }

    int start = atoi(argv[3]);
    int end = fileSize - 1;

    if (argc == 5) { // if end position is specified, use this value as end position
        end = atoi(argv[4]);
        if (end >= fileSize) { // end position cannot exceed the file size
            end = fileSize - 1;
        }
    }

    if (start < 0 || start > end) {
        printUsage();
        return;
    }

    int dataSize = end - start + 1;
    char timestamp[30];
    getTimestamp(timestamp, 3);


    sprintf(outFilename, "%s.out", timestamp);

    int offset = start;
    while (dataSize > 0) {
        int len = dataSize;
        if (len > BUF_SIZE) {
            len = BUF_SIZE;
        }
        int n = readdata(inFilename, offset, buffer, len);
        if (n < 0) {
            printf("Error!readdata failed");
            return;
        }

        offset += n;
        dataSize -= n;
        appenddata(outFilename, buffer, n);
    }

    printf("truncated file[%s]\n", outFilename);
}

void concateFile(int argc, char *argv[]) {
    char filename[120];
    char outFilename[80];
    int fileCount = argc - 2;
    int i;

    // check the file exists or not
    for (i = 2; i < argc; i++) {
        strcpy(filename, argv[i]);
        int fileSize = getFileSize(filename);
        if (fileSize < 0) {
            printf("Error!file[%s] not exist\n", filename);
            return;
        }
    }

    char timestamp[30];
    getTimestamp(timestamp, 3);
    sprintf(outFilename, "%s.out", timestamp);

    for (i = 2; i < argc; i++) {
        strcpy(filename, argv[i]);
        int fileSize = getFileSize(filename);
        int offset = 0;

        while (fileSize > 0) {
            int len = fileSize;
            if (len > BUF_SIZE) {
                len = BUF_SIZE;
            }
            int n = readdata(filename, offset, buffer, len);
            if (n < 0) {
                printf("Error!readdata failed");
                return;
            }

            offset += n;
            fileSize -= n;
            appenddata(outFilename, buffer, n);
        }
    }

    printf("concated file[%s]\n", outFilename);
}

int main(int argc, char *argv[]) {
    strcpy(programName, argv[0]);

    if (argc < 4) {
        printUsage(argv[0]);
        return -1;
    }

    const char *operation = argv[1];
    if (strcmp(operation, "truncate") == 0) {
        truncateFile(argc, argv);
    } else if (strcmp(operation, "concate") == 0) {
        concateFile(argc, argv);
    } else {
        printUsage(argv[0]);
        return -1;
    }


    return 0;
}

