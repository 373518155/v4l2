package com.example.lab;


import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.text.TextUtils;

import com.yanzhenjie.permission.Action;
import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.Permission;
import com.yanzhenjie.permission.Rationale;
import com.yanzhenjie.permission.RequestExecutor;
import com.yanzhenjie.permission.Setting;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by 37351 on 2018/3/27.
 */



/*
如果用户继续尝试使用需要某项权限的功能，但继续拒绝权限请求，则可能表明用户不理解应用为什么需要此权限才能提供相关功能。对于
这种情况，比较好的做法是显示解释。
为了帮助查找用户可能需要解释的情形，Android 提供了一个实用程序方法，即 shouldShowRequestPermissionRationale()。如
果应用之前请求过此权限但用户拒绝了请求，此方法将返回 true。

注：如果用户在过去拒绝了权限请求，并在权限请求系统对话框中选择了 Don't ask again 选项，此方法将返回 false。如果设备规
范禁止应用具有该权限，此方法也会返回 false。
 */
public class PermissionUtil {
    /**
     * 检查是否拥有某一权限
     * @param permission
     * @return  true -- 有权限  false -- 没权限
     */
    public static boolean hasPermission(String[] permission) {
        return AndPermission.hasPermissions(LabApplication.getInstance(), permission);
    }


    public static boolean hasStoragePermission() {
        boolean granted = hasPermission(Permission.Group.STORAGE);
        return granted;
    }

    public static boolean hasCameraPermission() {
        boolean granted = hasPermission(Permission.Group.CAMERA);
        return granted;
    }

    public static void requestStoragePermission(final Context context) {
        AndPermission.with(context)
                .runtime()
                .permission(Permission.Group.STORAGE)
                .rationale(new Rationale<List<String>>() {
                    @Override
                    public void showRationale(Context context, List<String> permissions, final RequestExecutor executor) {
                        List<String> permissionNames = transformText(context, permissions);
                        String message = "使用本App需要授予" + TextUtils.join(",", permissionNames) + "的权限";

                        new AlertDialog.Builder(context)
                                .setCancelable(false)
                                .setTitle("温馨提示")
                                .setMessage(message)
                                .setPositiveButton("继续", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        executor.execute();
                                    }
                                })
                                .setNegativeButton("取消", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        executor.cancel();
                                    }
                                })
                                .show();
                    }
                })
                .onGranted(new Action<List<String>>() {
                    @Override
                    public void onAction(List<String> data) {
                        SLog.info("授权外存储权限");
                    }
                })
                .onDenied(new Action<List<String>>() {
                    @Override
                    public void onAction(List<String> data) {
                        SLog.info("拒绝外存储权限");
                        if (AndPermission.hasAlwaysDeniedPermission(context, Permission.Group.STORAGE)) {
                            // 这里使用一个Dialog展示没有这些权限应用程序无法继续运行，询问用户是否去设置中授权。
                            AndPermission.with(context)
                                    .runtime()
                                    .setting()
                                    .onComeback(new Setting.Action() {
                                        @Override
                                        public void onAction() {
                                            // 用户从设置回来了。
                                            SLog.info("用户从设置回来了");
                                        }
                                    })
                                    .start();
                        }
                    }
                })
                .start();

    }

    public static void requestCameraPermission(final Context context) {
        AndPermission.with(context)
                .runtime()
                .permission(Permission.Group.CAMERA)
                .rationale(new Rationale<List<String>>() {
                    @Override
                    public void showRationale(Context context, List<String> permissions, final RequestExecutor executor) {
                        List<String> permissionNames = transformText(context, permissions);
                        String message = "使用本App需要授予" + TextUtils.join(",", permissionNames) + "的权限";

                        new AlertDialog.Builder(context)
                                .setCancelable(false)
                                .setTitle("温馨提示")
                                .setMessage(message)
                                .setPositiveButton("继续", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        executor.execute();
                                    }
                                })
                                .setNegativeButton("取消", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        executor.cancel();
                                    }
                                })
                                .show();
                    }
                })
                .onGranted(new Action<List<String>>() {
                    @Override
                    public void onAction(List<String> data) {
                        SLog.info("授权相机权限");
                    }
                })
                .onDenied(new Action<List<String>>() {
                    @Override
                    public void onAction(List<String> data) {
                        SLog.info("拒绝相机权限");
                        if (AndPermission.hasAlwaysDeniedPermission(context, Permission.Group.CAMERA)) {
                            // 这里使用一个Dialog展示没有这些权限应用程序无法继续运行，询问用户是否去设置中授权。
                            AndPermission.with(context)
                                    .runtime()
                                    .setting()
                                    .onComeback(new Setting.Action() {
                                        @Override
                                        public void onAction() {
                                            // 用户从设置回来了。
                                            SLog.info("用户从设置回来了");
                                        }
                                    })
                                    .start();
                        }
                    }
                })
                .start();

    }


    /**
     * 采取某个操作前先检查权限，根据【授权】或【拒绝授权】采取不同的操作
     * @param permissionGroup   权限组，比如 com.yanzhenjie.permission.Permission.Group.CAMERA
     * @param rationaleHint  rationale对话框的提示前缀    比如 "拍摄照片/视频需要授予"
     * @param handler  (根据不同的授权结果的)操作处理器
     * @param alwaysRequest  拒绝授权时，是否总是弹出请求对话框
     */
    public static void actionWithPermission(final Context context, final String[] permissionGroup, final String rationaleHint, final CommonCallback handler, final boolean alwaysRequest) {
        if (PermissionUtil.hasPermission(permissionGroup)) {
            handler.onSuccess(null);
        } else {  // 请求权限
            AndPermission.with(context)
                    .runtime()
                    .permission(permissionGroup)
                    .rationale(new Rationale<List<String>>() {
                        @Override
                        public void showRationale(Context context, List<String> permissions, final RequestExecutor executor) {
                            List<String> permissionNames = PermissionUtil.transformText(context, permissions);
                            String message = rationaleHint + TextUtils.join(",", permissionNames) + "的权限";

                            new AlertDialog.Builder(context)
                                    .setCancelable(false)
                                    .setTitle("温馨提示")
                                    .setMessage(message)
                                    .setPositiveButton("继续", new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which) {
                                            executor.execute();
                                        }
                                    })
                                    .setNegativeButton("取消", new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which) {
                                            executor.cancel();
                                        }
                                    })
                                    .show();
                        }
                    })
                    .onGranted(new Action<List<String>>() {
                        @Override
                        public void onAction(List<String> data) {
                            SLog.info("授予权限");
                            handler.onSuccess(null);
                        }
                    })
                    .onDenied(new Action<List<String>>() {
                        @Override
                        public void onAction(List<String> data) {
                            SLog.info("拒绝权限");
                            handler.onFailure(null);
                            if (alwaysRequest && AndPermission.hasAlwaysDeniedPermission(context, permissionGroup)) {
                                // 这里使用一个Dialog展示没有这些权限应用程序无法继续运行，询问用户是否去设置中授权。
                                AndPermission.with(context)
                                        .runtime()
                                        .setting()
                                        .onComeback(new Setting.Action() {
                                            @Override
                                            public void onAction() {
                                                // 用户从设置回来了。
                                                SLog.info("用户从设置回来了");
                                            }
                                        })
                                        .start();
                            }
                        }
                    })
                    .start();
        }
    }

    /**
     *
     * @param context
     * @param permissionGroup
     * @param rationaleHint
     * @param handler
     */
    public static void actionWithPermission(final Context context, String[] permissionGroup, String rationaleHint, CommonCallback handler) {
        actionWithPermission(context, permissionGroup, rationaleHint, handler, true);
    }


    /**
     * Turn permissions into text.
     * 调用com.yanzhenjie.permission.Permission.transformText()会产生java.lang.NoClassDefFoundError错误，
     * 所以定义了这个方法
     */
    public static List<String> transformText(Context context, List<String> permissions) {
        List<String> textList = new ArrayList<>();
        for (String permission : permissions) {
            switch (permission) {
                case Permission.READ_CALENDAR:
                case Permission.WRITE_CALENDAR: {
                    String message = "日历";
                    if (!textList.contains(message)) {
                        textList.add(message);
                    }
                    break;
                }

                case Permission.CAMERA: {
                    String message = "相机";
                    if (!textList.contains(message)) {
                        textList.add(message);
                    }
                    break;
                }
                case Permission.READ_CONTACTS:
                case Permission.WRITE_CONTACTS:
                case Permission.GET_ACCOUNTS: {
                    String message = "通讯录";
                    if (!textList.contains(message)) {
                        textList.add(message);
                    }
                    break;
                }
                case Permission.ACCESS_FINE_LOCATION:
                case Permission.ACCESS_COARSE_LOCATION: {
                    String message = "位置信息";
                    if (!textList.contains(message)) {
                        textList.add(message);
                    }
                    break;
                }
                case Permission.RECORD_AUDIO: {
                    String message = "麦克风";
                    if (!textList.contains(message)) {
                        textList.add(message);
                    }
                    break;
                }
                case Permission.READ_PHONE_STATE:
                case Permission.CALL_PHONE:
                case Permission.READ_CALL_LOG:
                case Permission.WRITE_CALL_LOG:
                case Permission.USE_SIP:
                case Permission.PROCESS_OUTGOING_CALLS: {
                    String message = "电话";
                    if (!textList.contains(message)) {
                        textList.add(message);
                    }
                    break;
                }
                case Permission.BODY_SENSORS: {
                    String message = "身体传感器";
                    if (!textList.contains(message)) {
                        textList.add(message);
                    }
                    break;
                }
                case Permission.SEND_SMS:
                case Permission.RECEIVE_SMS:
                case Permission.READ_SMS:
                case Permission.RECEIVE_WAP_PUSH:
                case Permission.RECEIVE_MMS: {
                    String message = "短信";
                    if (!textList.contains(message)) {
                        textList.add(message);
                    }
                    break;
                }
                case Permission.READ_EXTERNAL_STORAGE:
                case Permission.WRITE_EXTERNAL_STORAGE: {
                    String message = "存储空间";
                    if (!textList.contains(message)) {
                        textList.add(message);
                    }
                    break;
                }
            }
        }
        return textList;
    }
}
