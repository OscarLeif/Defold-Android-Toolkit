package ata.games.defold.android;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class CustomActivity extends Activity {

    private final String TAG = "CustomActivity";
    private final String defoldActivityClassName = "com.dynamo.android.DefoldActivity";

    @Override
    public void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);

        // // Processing intents
        Log.d(TAG, "Hello Does this makes sense?");
        Class<?> gameActivityClass = null;
        try {
            gameActivityClass = Class.forName(defoldActivityClassName);
        } catch(ClassNotFoundException ex) {
        }

        if (gameActivityClass != null)
        {
            Intent newIntent = new Intent(this, gameActivityClass);
            // startActivity(newIntent);
        }

        // finish();
    }    

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        Log.d(TAG, "Key Down: " + KeyEvent.keyCodeToString(keyCode));
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        Log.d(TAG, "Key Up: " + KeyEvent.keyCodeToString(keyCode));
        return super.onKeyUp(keyCode, event);
    }

    @Override
    public boolean onGenericMotionEvent(MotionEvent event) {
        // Log joystick axes and trigger movements
        Log.d(TAG, "Motion Event: " + event.toString());

        // Handle motion events like joystick movements here
        float axisX = event.getAxisValue(MotionEvent.AXIS_X);
        float axisY = event.getAxisValue(MotionEvent.AXIS_Y);
        float axisZ = event.getAxisValue(MotionEvent.AXIS_Z);
        
        Log.d(TAG, "Axis X: " + axisX + " | Axis Y: " + axisY + " | Axis Z: " + axisZ);

        return super.onGenericMotionEvent(event);
    }
}