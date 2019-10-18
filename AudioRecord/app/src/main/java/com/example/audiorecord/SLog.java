package com.example.audiorecord;

/**
 * Created by 37351 on 2018/6/28.
 */

/**
 * 在控制台中输出日志
 * 使用方法:
 *      SLog.info("n = %d", 666);
 * 输出类似如下内容:
 *      [2018-02-23 13:28:39.772][RxJavaDemo.java][00052]n = 666
 */

import android.util.Log;
import java.text.SimpleDateFormat;
import java.util.Date;

public class SLog {
    public static void info(String fromat, Object... args) {
        StackTraceElement[] traceArray = Thread.currentThread().getStackTrace();

        StackTraceElement trace = traceArray[3];  // 在Android中固定为3
        String content = String.format(fromat, args);


        // 生成时间戳
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS"); // 设置日期格式： 2017-08-29 21:17:22.387
        String timestamp = sdf.format(new Date());// new Date()为获取当前系统时间

        String fileName = trace.getFileName();
        String lineNumber = String.format("%05d", trace.getLineNumber());

        String logContent = String.format("[%s][%s][%s]%s", timestamp, fileName, lineNumber, content);
        Log.e("SLog", logContent);
    }
}
