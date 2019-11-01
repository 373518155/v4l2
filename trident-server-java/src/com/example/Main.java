package com.example;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class Main {
    // 線程等待隊列大小
    private static final int THREAD_QUEUE_SIZE = 1024;
    // 線程池大小
    private static final int THREAD_POOL_SIZE = 12;
    // 線程池
    private static ExecutorService executorService;

    public static void main(String[] args) {
        try {
            // 初始化線程池
            executorService =
                    new ThreadPoolExecutor(THREAD_POOL_SIZE, THREAD_POOL_SIZE, 0L, TimeUnit.MILLISECONDS,
                            new LinkedBlockingQueue<Runnable>(THREAD_QUEUE_SIZE));

            ServerSocket ss = new ServerSocket(6677);
            System.out.println("启动服务器....");

            while (true) {
                Socket s = ss.accept();
                executorService.execute(new Worker(s));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

