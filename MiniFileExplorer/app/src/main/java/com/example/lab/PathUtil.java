package com.example.lab;


/**
 * 文件操作相關的類
 * @author zwm
 */
public class PathUtil {

    /**
     *
     * 获取文件名
     * 例如  /storage/emulated/0/SnailPad/data.20181101.txt 会返回data.20181101.txt
     * @param path
     * @return
     */
    public static String getFilename(String path) {
        path = trimTailSlash(path);
        int index = path.lastIndexOf('/');
        if (index == -1) {
            return path;
        }

        return path.substring(index + 1);
    }


    /**
     * 获取所在目录(后面不带斜杠 / )
     * @param path
     * @return
     */
    public static String getDirectory(String path) {
        path = trimTailSlash(path);
        int index = path.lastIndexOf('/');
        if (index == -1) {
            return null;
        }

        String dir = path.substring(0, index);
        return trimTailSlash(dir);
    }


    /**
     * 返回不带扩展名的文件名
     * 例如  /storage/emulated/0/SnailPad/data.20181101.txt 会返回data.20181101
     * @param path
     * @return
     */
    public static String getPureFilename(String path) {
        String filename = getFilename(path);
        int index = filename.lastIndexOf('.');
        if (index == -1 || // 不带扩展名
                index == 0  // Unix系统中的隐藏文件
        ) {
            return filename;
        }

        return filename.substring(0, index);
    }

    /**
     * 获取扩展名
     * @param path
     * @return
     */
    public static String getExtension(String path) {
        return getExtension(path, true);
    }

    /**
     * 获取扩展名
     * @param path
     * @param lowerCase  是否强制转换为小写字母
     * @return
     */
    public static String getExtension(String path, boolean lowerCase) {
        String filename = getFilename(path);
        int index = filename.lastIndexOf('.');
        if (index == -1 || // 不带扩展名
                index == 0  // Unix系统中的隐藏文件
        ) {
            return null;
        }

        if (lowerCase) {
            return filename.substring(index + 1).toLowerCase();
        } else {
            return filename.substring(index + 1);
        }
    }



    /**
     * 去除路径最后的斜杠 / 【注意：如果只有一个字符且为斜杠，例如根目录，则保留】
     * @param path
     * @return
     */
    public static String trimTailSlash(String path) {
        while (true) {
            int len = path.length();
            if (len <= 1) {
                return path;
            }
            char ch = path.charAt(len - 1);
            if (ch == '/') {
                path = path.substring(0, len - 1);
            } else {
                return path;
            }
        }
    }
}



