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
    // 線程等待隊列大小
    private static final int THREAD_QUEUE_SIZE = 1024;
    // 線程池大小
    private static final int THREAD_POOL_SIZE = 12;
    // 線程池
    private static ExecutorService executorService;

    public static void main(String[] args) {

        try {

            File cfgFile = new File("config.txt");
            byte[] cfgContent = Files.readAllBytes(cfgFile.toPath());
            String config = new String(cfgContent);
            String[] configArr = config.split(":");
            String ip = configArr[0];
            int port = Integer.valueOf(configArr[1]);

            SLog.info("server address[%s:%d]", ip, port);

            // 初始化線程池
            executorService =
                    new ThreadPoolExecutor(THREAD_POOL_SIZE, THREAD_POOL_SIZE, 0L, TimeUnit.MILLISECONDS,
                            new LinkedBlockingQueue<Runnable>(THREAD_QUEUE_SIZE));

            ServerSocket ss = new ServerSocket(port);
            System.out.println("启动服务器....");

            while (true) {
                Socket s = ss.accept();
                executorService.execute(new Worker(s, ip, port));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

