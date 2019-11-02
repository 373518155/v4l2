package com.example;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.file.Files;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class Main {
    public static final String IP = "127.0.0.1";
    public static final int PORT = 6677;


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

            ServerSocket ss = new ServerSocket(PORT);
            System.out.println("启动服务器....");

            while (true) {
                Socket s = ss.accept();
                executorService.execute(new Worker(s, IP, PORT));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

