package com.example.lab;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;

public class LongPressButton extends android.support.v7.widget.AppCompatImageView {
    /**
     * 按下按鈕的時間
     */
    long actionDownTimestamp;
    /**
     * 區分單擊還是長按的時間閥值(毫秒)
     */
    public static final int LONG_PRESS_THRESHOLD = 1000;


    public LongPressButton(Context context) {
        this(context, null);
    }

    public LongPressButton(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public LongPressButton(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getAction();

        switch (action) {
            case MotionEvent.ACTION_DOWN:
                actionDownTimestamp = System.currentTimeMillis();
                return true;
            case MotionEvent.ACTION_UP:
                long now = System.currentTimeMillis();
                if (now - actionDownTimestamp > LONG_PRESS_THRESHOLD) {
                    SLog.info("is long press");
                } else {
                    SLog.info("is click");
                }
                return true;
            default:
                return super.onTouchEvent(event);
        }
    }
}
