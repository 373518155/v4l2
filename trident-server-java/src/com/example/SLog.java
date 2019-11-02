package com.example;

/**
 * 在控制台中输出日志
 * 使用方法:
 *      SLog.info("n = %d", 666);
 * 输出类似如下内容:
 *      [2018-02-23 13:28:39.772][RxJavaDemo.java][00052]n = 666
 */

import java.text.SimpleDateFormat;
import java.util.Date;

public class SLog {
    public static final String table_head = "| OFFSET | 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  | 0123456789ABCDEF |\n";
    public static final String table_border = "-------------------------------------------------------------------------------\n";

    public static void info(String fromat, Object... args) {
        StackTraceElement[] traceArray = Thread.currentThread().getStackTrace();

        StackTraceElement trace = traceArray[2];  // 在java中固定为2
        String content = String.format(fromat, args);


        // 生成时间戳
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS"); // 设置日期格式： 2017-08-29 21:17:22.387
        String timestamp = sdf.format(new Date());// new Date()为获取当前系统时间

        String fileName = trace.getFileName();
        String lineNumber = String.format("%05d", trace.getLineNumber());

        System.out.printf("[%s][%s][%s]%s\n", timestamp, fileName, lineNumber, content);
    }


    public static void hex(byte[] data, int len) {
        StackTraceElement[] traceArray = Thread.currentThread().getStackTrace();

        StackTraceElement trace = traceArray[2];  // 在java中固定为2


        // 生成时间戳
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS"); // 设置日期格式： 2017-08-29 21:17:22.387
        String timestamp = sdf.format(new Date());// new Date()为获取当前系统时间

        String fileName = trace.getFileName();
        String lineNumber = String.format("%05d", trace.getLineNumber());

        String dataLengthStr;
        if (data == null) {
            dataLengthStr = "null";
        } else {
            if (data.length < len) {
                len = data.length;
            }
            dataLengthStr = String.valueOf(len);
        }

        String header = String.format("[%s][%s][%s]data-length: %s Byte(s)\n", timestamp, fileName, lineNumber, dataLengthStr);
        System.out.printf(header);


        if (data == null || len == 0) {
            return;
        }

        System.out.printf(table_border);
        System.out.printf(table_head);
        System.out.printf(table_border);

        int offset = 0;
        while (offset < len) {
            System.out.printf("| %06d | ", offset);
            /* 打印hex部分 */
            for (int i = 0; i < 16; ++i) {
                if (offset + i < len) {
                    System.out.printf("%02X ", data[offset + i]);
                } else {
                    System.out.printf("   "); /* 超出真实数据长度用空白代替 */
                }
            }
            System.out.printf("| ");

            /* 打印ascii部分 */
            for (int i = 0; i < 16; ++i) {
                if (offset + i < len) {
                    char ch = byteToChar(data[offset + i]);
                    if (isPrint(ch)) {
                        System.out.printf("%c", ch);
                    } else {
                        System.out.printf(".");
                    }
                } else {
                    System.out.printf(" "); /* 超出真实数据长度用空白代替 */
                }
            }
            System.out.printf(" |\n");
            offset += 16;
        }
        System.out.printf(table_border);
    }

    /**
     * 判断字符是否可以打印
     * @param ch
     * @return
     */
    public static boolean isPrint(char ch) {
        if((0 <= ch && ch <=31) || ch >= 127) { // 不可打印字符
            return false;
        }
        return true;
    }


    /**
     * 将有符号的byte(-128~127)转换为无符号的char(0~255)
     * java中byte的表示范围是 -128 ~ 127，需要转换
     * 参考:
     * 详解java中的byte类型  https://www.cnblogs.com/zl181015/p/9435035.html
     * @param b
     * @return
     */
    public static char byteToChar(byte b) {
        short val = b;
        char ch;
        if (val >= 0) {
            ch = (char) val;
        } else {
            ch = (char) (val + 256);
        }

        return ch;
    }
}


