package com.example;

import java.io.*;
import java.net.Socket;

public class Worker implements Runnable {
    Socket socket;
    public Worker(Socket socket) {
        this.socket = socket;
    }

    @Override
    public void run() {
        try {
            System.out.println("客户端:"+socket.getInetAddress().getLocalHost()+"已连接到服务器");

            BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            //读取客户端发送来的消息
            String mess = br.readLine();
            System.out.println("客户端："+mess);
            BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
            bw.write(mess+"\n");
            bw.flush();
        } catch (IOException e) {

        }

    }
}
