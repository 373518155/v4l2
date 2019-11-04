package com.example;

import java.io.*;
import java.net.Socket;

public class Worker implements Runnable {
    public static final int BUFFER_SIZE = 1024 * 1024;

    public static final int SOCKS5_STATUS_INIT = 0;  // 建立连接前
    public static final int SOCKS5_STATUS_CONNECTING = 1;  // 与目标服务器建立连接
    public static final int SOCKS5_STATUS_CONNECTED = 2;   // 与目标服务器成功建立连接

    int status = SOCKS5_STATUS_INIT;

    Socket socket; // 客户端与代理服务器的连接
    String ip;     // 代理服务器的IP
    int port;      // 代理服务器的端口

    Socket targetSocket; // 与目标服务器的socket连接
    InputStream targetInputStream;
    OutputStream targetOutputStream;

    /**
     * Constructor
     * @param socket 连接socket
     * @param ip 本机（即代理服务器）的ip地址
     * @param port 本机（即代理服务器）的端口
     */
    public Worker(Socket socket, String ip, int port) {
        this.socket = socket;
        this.ip = ip;
        this.port = port;
    }

    @Override
    public void run() {
        try {
            System.out.println("客户端:"+socket.getInetAddress().getLocalHost()+"已连接到服务器");

            byte[] inputBuffer = new byte[BUFFER_SIZE];
            byte[] outputBuffer = new byte[BUFFER_SIZE];
            byte[] targetInputBuffer = new byte[BUFFER_SIZE];

            InputStream inputStream = socket.getInputStream();
            OutputStream outputStream = socket.getOutputStream();

            while (true) {
                int n = inputStream.read(inputBuffer);
                if (n < 0) {
                    break;
                }

                if (n == 0) {
                    continue;
                }
                SLog.hex(inputBuffer, n);
                if (status == SOCKS5_STATUS_INIT) {
                    if (inputBuffer[0] == 0x05 && inputBuffer[1] == 0x01 && inputBuffer[2] == 0x00) { // 无需认证
                        status = SOCKS5_STATUS_CONNECTING;
                        SLog.info("建立socks5连接");
                        outputBuffer[0] = 0x05;
                        outputBuffer[1] = 0x00; // 无需认证
                        outputStream.write(outputBuffer, 0, 2);
                    }
                } else if (status == SOCKS5_STATUS_CONNECTING) {
                    SLog.info("处理命令");

                    // COMMAND 命令
                    if (inputBuffer[1] == 0x01) {
                        SLog.info("CONNECT 连接上游服务器");
                    } else if (inputBuffer[1] == 0x02) {
                        SLog.info("BIND 绑定");
                    } else if (inputBuffer[1] == 0x03) {
                        SLog.info("UDP 中继");
                    }

                    // ADDRESS_TYPE 目标服务器地址类型
                    if (inputBuffer[3] == 0x01) {
                        SLog.info("IP V4地址");

                        String ip = String.format("%d.%d.%d.%d",
                                Util.byteToUint(inputBuffer[4]), Util.byteToUint(inputBuffer[5]), Util.byteToUint(inputBuffer[6]), Util.byteToUint(inputBuffer[7]));
                        int port = 256 * Util.byteToUint(inputBuffer[8]) + Util.byteToUint(inputBuffer[9]);
                        SLog.info("目标服务器地址ip[%s], port[%d]", ip, port);

                        targetSocket = new Socket(ip,port); // 连接到目标服务器的socket

                        targetInputStream = targetSocket.getInputStream();
                        targetOutputStream = targetSocket.getOutputStream();

                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                while (true) {
                                    try {
                                        int n = targetInputStream.read(targetInputBuffer);
                                        if (n == 0) {
                                            continue;
                                        }
                                        if (n < 0) {
                                            break;
                                        }

                                        SLog.hex(targetInputBuffer, n);

                                        outputStream.write(targetInputBuffer, 0, n);
                                    } catch (Exception e) {
                                        break;
                                    }
                                }


                            }
                        }).start();


                        /*
                        代理服务器连接目标主机，并返回结果给客户端
                            0x05 SOCKS5协议版本
                            0x00 连接成功
                            0x00 RSV保留字段
                            0x01 地址类型为IPV4
                            0x7f 0x00 0x00 0x01 代理服务器连接目标服务器成功后的代理服务器IP, 127.0.0.1
                            0xaa 0xaa 代理服务器连接目标服务器成功后的代理服务器端口（代理服务器使用该端口与目标服务器通信），本例端口号为43690
                         */

                        outputBuffer[0] = 0x05;
                        outputBuffer[1] = 0x00;
                        outputBuffer[2] = 0x00;
                        outputBuffer[3] = 0x01;
                        // ip地址
                        byte[] ipArr = Util.parseIPV4(ip);

                        outputBuffer[4] = ipArr[0];
                        outputBuffer[5] = ipArr[1];
                        outputBuffer[6] = ipArr[2];
                        outputBuffer[7] = ipArr[3];
                        // 端口
                        outputBuffer[8] = Util.UintToByte(port / 256);
                        outputBuffer[9] = Util.UintToByte(port % 256);

                        SLog.hex(outputBuffer, 10);
                        outputStream.write(outputBuffer, 0, 10);

                        status = SOCKS5_STATUS_CONNECTED;
                    } else if (inputBuffer[3] == 0x03) {
                        SLog.info("域名地址"); // 域名地址的第1个字节为域名长度，剩下字节为域名名称字节数组
                    } else if (inputBuffer[3] == 0x04) {
                        SLog.info("IP V6地址");
                    }
                } else if (status == SOCKS5_STATUS_CONNECTED) {
                    targetOutputStream.write(inputBuffer, 0, n);

                }
            }

        } catch (Exception e) {
            SLog.info("Error!%s", e.getMessage());
        }
    }
}
