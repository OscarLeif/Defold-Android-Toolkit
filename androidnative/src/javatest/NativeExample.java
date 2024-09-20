package ata.games.defold.android;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.os.Vibrator;
import android.widget.Toast;
import android.app.Activity;

import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;

import android.os.Handler;
import android.os.Looper;


import android.util.Log;

import android.app.Activity;
import android.view.KeyEvent;
import android.util.Log;

import android.app.Activity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Toast;

import java.util.HashMap;
import java.util.Map;

class NativeExample {

    private static Activity mainActivity;
    private static final String TAG = "NativeExample";


    // Track button states
    private static boolean currentButtonAPressed = false;
    private static boolean currentButtonBPressed = false;
    private static boolean previousButtonAPressed = false;
    private static boolean previousButtonBPressed = false;

    // Map to track current and previous button states
    private static Map<Integer, Boolean> currentButtonStates = new HashMap<>();
    private static Map<Integer, Boolean> previousButtonStates = new HashMap<>();

    public static void setMainActivity(Activity activity) {
        mainActivity = activity;
        registerKeyListener();
    }

    public static void registerKeyListener() {
        if (mainActivity != null) {
            mainActivity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    // Ensure the DecorView is focusable and can receive key events
                    mainActivity.getWindow().getDecorView().setFocusableInTouchMode(true);
                    mainActivity.getWindow().getDecorView().requestFocus();
                    mainActivity.getWindow().getDecorView().setOnKeyListener(new View.OnKeyListener() {
                        @Override
                        public boolean onKey(View v, int keyCode, KeyEvent event) {
                            return handleKeyEvent(keyCode, event);
                        }
                    });
                }
            });
        }
    }

    public static final void vibratePhone(Context context, int vibrateMilliSeconds) {
        Vibrator vibrator = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
        vibrator.vibrate(vibrateMilliSeconds);
    }

    public static boolean handleKeyEvent(int keyCode, KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            currentButtonStates.put(keyCode, true);
            return true;
        } else if (event.getAction() == KeyEvent.ACTION_UP) {
            currentButtonStates.put(keyCode, false);
            return false;
        }
        return false;
    }

    public static void updateButtonStates() {
        previousButtonStates.clear();
        previousButtonStates.putAll(currentButtonStates);
    }

    public static boolean isButtonPressed(int keyCode) {
        return currentButtonStates.getOrDefault(keyCode, false);
    }

    public static boolean wasButtonPressedThisFrame(int keyCode) {
        return isButtonPressed(keyCode) && !previousButtonStates.getOrDefault(keyCode, false);
    }   

    public static String DoStuff() {
        return "Message From Java!";
    }

    //making a dependency will make to search on the project self package url.
    //in short it will crash !
    //we must change this to asset folder
    public static String GetRaw(Context context) {
        return "";
        // try {
        //     InputStream inputStream = context.getResources().openRawResource(R.raw.test);
        //     InputStreamReader inputreader = new InputStreamReader(inputStream);
        //     BufferedReader buffreader = new BufferedReader(inputreader);
        //     String line;
        //     StringBuilder text = new StringBuilder();
        //     while ((line = buffreader.readLine()) != null) {
        //         text.append(line);
        //         text.append('\n');
        //     }
        //     return text.toString();
        // } catch (Exception e) {
        //     return "io exception: " + e.getMessage();
        // }
    }

    public static void showToast(Context context, String message) 
    {
        Log.d(TAG, message);

        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(context, message, Toast.LENGTH_SHORT).show();
            }
        });
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
        case "portraitSensor":  // Add this case for sensor-based portrait orientation            
            activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT);
            showToast(activity, orientation);
            break;
        default:
            // Optional: set to unspecified if none matches
            activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);
            break;
    }
    }
}