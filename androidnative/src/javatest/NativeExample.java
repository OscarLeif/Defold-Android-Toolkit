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

import android.app.Activity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;

import java.util.HashMap;
import java.util.Map;

import android.hardware.input.InputManager;
import android.view.InputDevice;
import android.view.MotionEvent;

class NativeExample {

    private static Activity mainActivity;
    private static InputManager inputManager;

    private static final String TAG = "NativeExample";

    // Maps to track current and previous button states
    private static Map<Integer, Boolean> currentButtonStates = new HashMap<>();

    private static float m_axis_x;
    private static float m_axis_y;

    // Store the device ID of the gamepad for reference
    private static int gamepadDeviceId = -1;

    public static void setMainActivity(Activity activity) {
        mainActivity = activity;
        // Ensure this runs on the UI thread
        mainActivity.runOnUiThread(() -> {
            inputManager = (InputManager) mainActivity.getSystemService(Context.INPUT_SERVICE);
            Log.d(TAG, "Input Manager Register");

            if (inputManager == null) {
                Log.d(TAG, "Warning: Input Manager is NULL");
            } else {
                Log.d(TAG, "Input Manager should be OK");
            }

            // Set a listener for device changes (connect/disconnect gamepads)
            inputManager.registerInputDeviceListener(new InputManager.InputDeviceListener() {
                @Override
                public void onInputDeviceAdded(int deviceId) {
                    InputDevice device = inputManager.getInputDevice(deviceId);
                    if (isGamepad(device)) {
                        gamepadDeviceId = deviceId;
                        Log.d(TAG, "Gamepad connected: " + device.getName());
                    }
                    Log.d(TAG, "A device was added");
                }

                @Override
                public void onInputDeviceRemoved(int deviceId) {
                    if (deviceId == gamepadDeviceId) {
                        gamepadDeviceId = -1;
                        Log.d(TAG, "Gamepad disconnected");
                    }
                    Log.d(TAG, "A device was removed");
                }

                @Override
                public void onInputDeviceChanged(int deviceId) {
                    // Handle device changes (optional)
                }
            }, null);

            registerKeyListener();
        });
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

    // Method to poll axis data from the gamepad
    public static void pollGamepadAxes() {
        // Get all connected input device IDs
        int[] deviceIds = inputManager.getInputDeviceIds();
    
        // Check if there are any connected devices
        if (deviceIds.length > 0) {
            for (int deviceId : deviceIds) {
                InputDevice device = inputManager.getInputDevice(deviceId);
                if (device != null && 
                    ((device.getSources() & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK ||
                     (device.getSources() & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD)) 
                 {
                    Log.d(TAG, "Using gamepad or joystick: " + device.getName());
    
                    // Retrieve axis values from the gamepad/joystick
                    float axisX = getAxisValue(device, MotionEvent.AXIS_X);
                    float axisY = getAxisValue(device, MotionEvent.AXIS_Y);
                    float leftTrigger = getAxisValue(device, MotionEvent.AXIS_LTRIGGER);
                    float rightTrigger = getAxisValue(device, MotionEvent.AXIS_RTRIGGER);
    
                    // Logging the axis values
                    Log.d(TAG, "Axis X: " + axisX + ", Axis Y: " + axisY);
                    // Log.d(TAG, "Left Trigger: " + leftTrigger + ", Right Trigger: " + rightTrigger);
    
                    // Only handle the first gamepad/joystick found for now
                    break; // Exit after the first one is found and polled
                }
            }
        } else {
            Log.d(TAG, "No input devices connected.");
        }
    }

    // Helper method to retrieve the axis value
    private static float getAxisValue(InputDevice device, int axis) {
        InputDevice.MotionRange motionRange = device.getMotionRange(axis);
        if (motionRange != null) {
            return motionRange.getFlat(); // You might want to get the current value instead
        }
        return 0.0f; // Return 0 if there's no motion range found
    }

    // Check if the device is a gamepad
    private static boolean isGamepad(InputDevice device) {
        int sources = device.getSources();
        return ((sources & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD ||
                (sources & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK);
    }

    // Helper method to create or capture a MotionEvent for the device
    private static MotionEvent getMotionEventForDevice(InputDevice device) {
        // In practice, this method would capture the actual motion event (from system
        // input)
        // For illustration, you would capture MotionEvents from the event queue or
        // listeners
        // MotionEvent event = ...
        return null; // Replace with actual MotionEvent data
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

    public static float getAxisX() {
        return m_axis_x;
    }

    public static float getAxisY() {
        return m_axis_y;
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