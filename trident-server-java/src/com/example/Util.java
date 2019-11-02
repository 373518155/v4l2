package com.example;

public class Util {
    /**
     * 从byte类型转换到unsigned char类型
     * @param b 取值范围 -128 ~ 127
     * @return
     */
    public static int byteToUint(byte b) {
        if (b >= 0) {
            return b;
        } else {
            return b + 256;
        }
    }

    /**
     * 从unsigned char 转换到byte类型
     * @param n 取值范围 0 ~ 255
     * @return
     */
    public static byte UintToByte(int n) {
        if (0 <= n && n <= 127) {
            return (byte) n;
        } else {
            return (byte) (n - 256);
        }
    }

    public static byte[] parseIPV4(String ip) {
        byte[] ipArr = new byte[4];

        String[] ipSeg = ip.split("\\.");

        ipArr[0] = UintToByte(Integer.valueOf(ipSeg[0]));
        ipArr[1] = UintToByte(Integer.valueOf(ipSeg[1]));
        ipArr[2] = UintToByte(Integer.valueOf(ipSeg[2]));
        ipArr[3] = UintToByte(Integer.valueOf(ipSeg[3]));

        return ipArr;
    }
}
