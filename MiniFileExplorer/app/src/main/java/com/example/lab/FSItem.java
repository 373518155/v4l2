package com.example.lab;

/**
 * 文件系统的item
 * @author zwm 2020-01-08 18:09
 */
public class FSItem {
    public static final int TYPE_FOLDER = 1;
    public static final int TYPE_FILE = 2;

    /**
     * 类型
     * 文件夹还是文件
     */
    public int type;

    /**
     * 绝对路径
     */
    public String absolutePath;

    /**
     * 文件大小
     */
    public long length;

    /**
     * 最近一次修改时间
     */
    public long mtime;
}
