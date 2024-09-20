package ata.games.defold.android;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.os.Vibrator;
import android.widget.Toast;
import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;

import java.util.HashMap;
import java.util.Map;

class NativeExample {

    private static Activity mainActivity;
    private static final String TAG = "NativeExample";

    // Maps to track current and previous button states
    private static Map<Integer, Boolean> currentButtonStates = new HashMap<>();

    public static void setMainActivity(Activity activity) {
        mainActivity = activity;
        registerKeyListener();
    }

    public static void registerKeyListener() {
        if (mainActivity != null) {
            mainActivity.runOnUiThread(() -> {
                View decorView = mainActivity.getWindow().getDecorView();
                decorView.setFocusableInTouchMode(true);
                decorView.requestFocus();
                decorView.setOnKeyListener((v, keyCode, event) -> handleKeyEvent(keyCode, event));
            });
        }
    }

    public static boolean handleKeyEvent(int keyCode, KeyEvent event) {
        // Log.d(TAG, "Key event: " + keyCode + ", Action: " + event.getAction());
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            if (!currentButtonStates.getOrDefault(keyCode, false)) {
                currentButtonStates.put(keyCode, true);                
            }
            return true;
        } else if (event.getAction() == KeyEvent.ACTION_UP) {
            currentButtonStates.put(keyCode, false);
            return true;
        }
        return false;
    }

    public static void printButtonStates(Map<Integer, Boolean> buttonStates) {
        for (Map.Entry<Integer, Boolean> entry : buttonStates.entrySet()) {
            System.out.println("Key: " + entry.getKey() + ", Value: " + entry.getValue());
        }
    }    

    // Returns true if the button is currently pressed
    public static boolean isButtonPressed(int keyCode) {
        return currentButtonStates.getOrDefault(keyCode, false);
    }

    public static void vibratePhone(Context context, int vibrateMilliSeconds) {
        Vibrator vibrator = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
        if (vibrator != null && vibrator.hasVibrator()) {
            vibrator.vibrate(vibrateMilliSeconds);
        }
    }

    public static String doStuff() {
        return "Message From Java!";
    }

    public static String getRaw(Context context) {
        return ""; // Commented-out functionality
    }

    public static void showToast(Context context, String message) {
        Log.d(TAG, message);
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(() -> Toast.makeText(context, message, Toast.LENGTH_SHORT).show());
    }

    // Method to set the orientation of the device
    public static void setOrientation(Activity activity, String orientation) {
        switch (orientation) {
            case "portrait":
                activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
                break;
            case "reversePortrait":
                activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT);
                break;
            case "landscape":
                activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                break;
            case "landscapeSensor":
                activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
                showToast(activity, orientation);
                break;
            case "portraitSensor":
                activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT);
                showToast(activity, orientation);
                break;
            default:
                activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);
                break;
        }
    }
}