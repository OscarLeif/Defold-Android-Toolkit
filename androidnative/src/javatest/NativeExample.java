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

class NativeExample {

    private static final String TAG = "NativeExample";

    public static final void vibratePhone(Context context, int vibrateMilliSeconds) {
        Vibrator vibrator = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
        vibrator.vibrate(vibrateMilliSeconds);
    }

    public static String DoStuff() {
        return "Message From Java!";
    }

    //making a dependency will make to search on the project self package url.
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
                break;
            default:
                // Optional: set to unspecified if none matches
                activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);
                break;
        }
    }
}