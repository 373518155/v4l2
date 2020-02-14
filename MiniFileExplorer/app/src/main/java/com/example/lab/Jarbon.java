package com.example.lab;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

public class Jarbon {
    /**
     * 時間戳(秒)
     */
    private int timestamp;

    /**
     * 時間戳(毫秒)
     */
    private long timestampMillis;


    private int year;
    private int month;
    private int day;
    private int hour;
    private int minute;
    private int second;
    private int milliSecond;

    /**
     * 星期几
     * 星期一  1
     * 星期二  2
     * 星期三  3
     * 星期四  4
     * 星期五  5
     * 星期六  6
     * 星期日  7
     */
    private int dayOfWeek;


    public Jarbon() {
        this(System.currentTimeMillis());
    }

    public Jarbon(long timestampMillis) {
        this.timestamp = (int) (timestampMillis / 1000);
        this.timestampMillis = timestampMillis;

        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(timestampMillis);

        year = calendar.get(Calendar.YEAR);
        month = calendar.get(Calendar.MONTH) + 1;
        day = calendar.get(Calendar.DAY_OF_MONTH);
        hour = calendar.get(Calendar.HOUR_OF_DAY);
        minute = calendar.get(Calendar.MINUTE);
        second = calendar.get(Calendar.SECOND);

        milliSecond = calendar.get(Calendar.MILLISECOND);


        /*
        Calender星期几的表示
         * 星期日  1
         * 星期一  2
         * 星期二  3
         * 星期三  4
         * 星期四  5
         * 星期五  6
         * 星期六  7
         */
        dayOfWeek = calendar.get(Calendar.DAY_OF_WEEK);
        if (dayOfWeek == Calendar.SUNDAY) {
            dayOfWeek = 7;
        } else {
            dayOfWeek = dayOfWeek - 1;
        }
    }


    public static Jarbon create(int year, int month, int day, int hour, int minute, int second, int milliSecond) {
        Calendar calendar = Calendar.getInstance();
        calendar.set(year, month - 1, day, hour, minute, second);
        calendar.set(Calendar.MILLISECOND, milliSecond);

        return new Jarbon(calendar.getTimeInMillis());
    }

    public static Jarbon create(int year, int month, int day, int hour, int minute, int second) {
        return create(year, month, day, hour, minute, second, 0);
    }

    public static Jarbon create(int year, int month, int day) {
        return create(year, month, day, 0, 0, 0);
    }

    /**
     * 解析字符串格式的日期时间
     * 支持如下格式:
     *      2019-07-26 08:15.20.083
     *      2019-07-26 08:15.20
     *      2019-07-26
     * @param datetime
     * @return
     */
    public static Jarbon parse(String datetime) {
        if (datetime == null) {
            return null;
        }

        int len = datetime.length();

        int Y = 0;
        int m = 0;
        int d = 0;
        int H = 0;
        int i = 0;
        int s = 0;
        int u = 0;
        if (len >= 10) { // 至少為這種格式  2019-07-26
            Y = Integer.valueOf(datetime.substring(0, 4));
            m = Integer.valueOf(datetime.substring(5, 7));
            d = Integer.valueOf(datetime.substring(8, 10));

            if (len >= 19) { // 至少為這種格式  2019-07-26 08:15.20
                H = Integer.valueOf(datetime.substring(11, 13));
                i = Integer.valueOf(datetime.substring(14, 16));
                s = Integer.valueOf(datetime.substring(17, 19));

                if (len >= 23) { // 這種格式 2019-07-26 08:15.20.083
                    u = Integer.valueOf(datetime.substring(20, 23));
                }
            }
        }

        return create(Y, m, d, H, i, s, u);
    }

    public Jarbon startOfDay() {
        return create(year, month, day);
    }


    /**
     * 相差的天数(自然天)
     * @param jarbon
     * @return 如果早于jarbon，返回正数  如果是同一天，返回0  如果晚于jarbon，返回负数
     */
    public int diffInDays(Jarbon jarbon) {
        Jarbon startOfDay1 = startOfDay();
        Jarbon startOfDay2 = jarbon.startOfDay();

        int timestamp1 = startOfDay1.getTimestamp();
        int timestamp2 = startOfDay2.getTimestamp();

        return (timestamp2 - timestamp1) / 86400;
    }


    public int diffInMinutes(Jarbon jarbon) {
        /*
        必须用毫秒比较，比如下面的这个时间，如果用秒比较，会认为是相差1分钟，实际上是相差 59.999 秒，不足1分钟
        2019-07-27 20:05:17.466
        2019-07-27 20:06:17.465
         */
        long timestampMillis1 = timestampMillis;
        long timestampMillis2 = jarbon.timestampMillis;

        return (int) ((timestampMillis2 - timestampMillis1) / 60000);
    }

    /**
     * 返回日期部分 格式 2019-07-26
     * @return
     */
    public String toDateString() {
        return toString().substring(0, 10);
    }

    /**
     * 返回时间部分 格式 14:15:16
     * @return
     */
    public String toTimeString() {
        return toString().substring(11, 19);
    }

    /**
     * 返回日期时间（不包括毫秒） 格式 2019-07-26 20:13:30
     * @return
     */
    public String toDateTimeString() {
        return toString().substring(0, 19);
    }


    @Override
    public String toString() {
        // return String.format("%04d-%02d-%02d %02d:%02d:%02d.%03d", year, month, day, hour, minute, second, milliSecond);
        return format("Y-m-d H:i:s.u");
    }

    public String format(String format) {
        StringBuilder sb = new StringBuilder();
        List<Object> paramList = new ArrayList<>();
        int len = format.length();
        for (int i = 0; i < len; i++) {
            char ch = format.charAt(i);

            switch (ch) {
                case 'Y':
                    sb.append("%04d");
                    paramList.add(year);
                    break;
                case 'm':
                    sb.append("%02d");
                    paramList.add(month);
                    break;
                case 'd':
                    sb.append("%02d");
                    paramList.add(day);
                    break;
                case 'H':
                    sb.append("%02d");
                    paramList.add(hour);
                    break;
                case 'i':
                    sb.append("%02d");
                    paramList.add(minute);
                    break;
                case 's':
                    sb.append("%02d");
                    paramList.add(second);
                    break;
                case 'u':
                    sb.append("%03d");
                    paramList.add(milliSecond);
                    break;
                case '%':
                    sb.append("%%");
                    break;
                case 'y':
                    // 兩位數的年(帶前導0)
                    sb.append("%02d");
                    paramList.add(year % 100);
                    break;
                case 'n':
                    // 月份(不帶前導0)
                    sb.append("%d");
                    paramList.add(month);
                    break;
                case 'j':
                    // 日(不帶前導0）
                    sb.append("%d");
                    paramList.add(day);
                    break;
                case '\\':
                    if (i + 1 < len) {
                        ch = format.charAt(i + 1);
                        if (ch == '%') {
                            sb.append("%%");
                        } else {
                            sb.append(ch);
                        }
                    }
                    break;
                default:
                    sb.append(ch);
                    break;

            }
        }

        // SLog.info("sb[%s]", sb.toString());


        Object[] params = new Object[paramList.size()];
        for (int i = 0; i < paramList.size(); i++) {
            params[i] = paramList.get(i);
        }

        return String.format(sb.toString(), params);
    }

    /**
     * 返回详细信息的字符串
     * @return
     */
    public String dump() {
        return String.format("datetime[%04d-%02d-%02d %02d:%02d:%02d.%03d],timestamp[%d],timestampMillis[%s],dayOfWeek[%d]",
                year, month, day, hour, minute, second, milliSecond, timestamp, timestampMillis, dayOfWeek);
    }


    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (!(obj instanceof Jarbon)) {
            return false;
        }

        Jarbon jarbon = (Jarbon) obj;
        return timestampMillis == jarbon.timestampMillis;
    }

    /**
     * 获取时区ID 例如 Asia/Shanghai
     * @return
     */
    public static String getTimeZoneID() {
        Calendar calendar = Calendar.getInstance();
        return calendar.getTimeZone().getID();
    }

    public int getTimestamp() {
        return timestamp;
    }

    public long getTimestampMillis() {
        return timestampMillis;
    }

    public int getYear() {
        return year;
    }

    public int getMonth() {
        return month;
    }

    public int getDay() {
        return day;
    }

    public int getHour() {
        return hour;
    }

    public int getMinute() {
        return minute;
    }

    public int getSecond() {
        return second;
    }

    public int getMilliSecond() {
        return milliSecond;
    }

    public int getDayOfWeek() {
        return dayOfWeek;
    }
}
