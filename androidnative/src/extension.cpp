// Extension lib defines
#define EXTENSION_NAME androidnative
#define LIB_NAME "androidnative"
#define MODULE_NAME LIB_NAME


// Defold SDK
#define DLIB_LOG_DOMAIN LIB_NAME
#include <jni.h>
#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_ANDROID)

#include "testlib.h" // Multiply

// #include "glfw/lib/android/android_joystick.h"
#include <android_native_app_glue.h> //native will always work
#include <android/input.h>
#include <android/log.h> //native will always work
#include <android_native_app_glue.h>
#include <android/input.h>
#include <dmsdk/sdk.h>
#include <stdio.h>

#define LOGI(...) dmLogInfo(__VA_ARGS__)
#define LOGE(...) dmLogError(__VA_ARGS__)
#define DEAD_ZONE_THRESHOLD 0.01f


//#include "android_joystick.h" //What we need we cannot explose this
//#include <glfw/glfw_native.h>//Not possible to get
// #include "external/glfw/lib/android/android_joystick.h" //NO API 
const char* java_class = "ata.games.defold.android.NativeExample";

extern AInputEvent* g_lastInputEvent;

static JNIEnv* Attach()
{
    JNIEnv* env;
    JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
    vm->AttachCurrentThread(&env, NULL);
    return env;
}

static bool Detach(JNIEnv* env)
{
    bool exception = (bool) env->ExceptionCheck();
    env->ExceptionClear();
    JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
    vm->DetachCurrentThread();
    return !exception;
}

namespace {
struct AttachScope
{
    JNIEnv* m_Env;
    AttachScope() : m_Env(Attach())
    {
    }
    ~AttachScope()
    {
        Detach(m_Env);
    }
};
}

static jclass GetClass(JNIEnv* env, const char* classname)
{
    jclass activity_class = env->FindClass("android/app/NativeActivity");
    jmethodID get_class_loader = env->GetMethodID(activity_class,"getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject cls = env->CallObjectMethod(dmGraphics::GetNativeAndroidActivity(), get_class_loader);
    jclass class_loader = env->FindClass("java/lang/ClassLoader");
    jmethodID find_class = env->GetMethodID(class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jstring str_class_name = env->NewStringUTF(classname);
    jclass outcls = (jclass)env->CallObjectMethod(cls, find_class, str_class_name);
    env->DeleteLocalRef(str_class_name);
    return outcls;
}

// Function to expose key press data
int get_gamepad_key_input(AInputEvent* event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY && 
        AInputEvent_getSource(event) & AINPUT_SOURCE_GAMEPAD) {
        int32_t keycode = AKeyEvent_getKeyCode(event);
        int32_t action = AKeyEvent_getAction(event);
        LOGI("Gamepad key event: keycode=%d, action=%d", keycode, action);
        return keycode;  // Return the keycode
    }
    LOGE("Not a gamepad key event.");
    return -1; // No key input or not a gamepad event
}

// Function to expose joystick axis values
float get_joystick_axis_input(AInputEvent* event, int32_t axis) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION && 
        AInputEvent_getSource(event) & AINPUT_SOURCE_JOYSTICK) {
        float axis_value = AMotionEvent_getAxisValue(event, axis, 0);
        LOGI("Joystick axis input: axis=%d, value=%f", axis, axis_value);
        return axis_value; // Return the axis value
    }
    LOGE("Not a joystick axis event.");
    return 0.0f;
}


static int DoStuffJava(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    jclass cls = GetClass(env, "com.defold.androidnativeext.NativeExample");
    jmethodID method = env->GetStaticMethodID(cls, "DoStuff", "()Ljava/lang/String;");
    
    jstring return_value = (jstring)env->CallStaticObjectMethod(cls, method);
    lua_pushstring(L, env->GetStringUTFChars(return_value, 0));
    env->DeleteLocalRef(return_value);
    return 1;
}

static int DoStuffJar(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    // jclass cls = GetClass(env, "com.svenandersson.dummy.Dummy");
    jclass cls = GetClass(env, java_class);
    jmethodID method = env->GetStaticMethodID(cls, "DoStuff", "()Ljava/lang/String;");
    
    jstring return_value = (jstring)env->CallStaticObjectMethod(cls, method);
    lua_pushstring(L, env->GetStringUTFChars(return_value, 0));
    env->DeleteLocalRef(return_value);
    return 1;
}

static int Vibrate(lua_State* L)
{
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    int duration = luaL_checkint(L, 1);

    // jclass cls = GetClass(env, "com.defold.androidnativeext.NativeExample");
    jclass cls = GetClass(env, java_class);
    
    jmethodID vibrate_method = env->GetStaticMethodID(cls, "vibratePhone", "(Landroid/content/Context;I)V");
    jobject context = dmGraphics::GetNativeAndroidActivity();    
    env->CallStaticVoidMethod(cls, vibrate_method, context, duration);    
    return 0;
}

static int SetOrientation(lua_State* L)
{
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    const char* orientation = luaL_checkstring(L, 1);

    jclass cls = GetClass(env, java_class);
    jmethodID method = env->GetStaticMethodID(cls, "setOrientation", "(Landroid/app/Activity;Ljava/lang/String;)V");

    jstring jOrientation = env->NewStringUTF(orientation);
    jobject context = dmGraphics::GetNativeAndroidActivity();    
    env->CallStaticVoidMethod(cls, method, context, jOrientation);
    env->DeleteLocalRef(jOrientation);

    return 0;
}

static int ShowToast(lua_State* L)
{
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    const char* message = luaL_checkstring(L, 1);

    //ata.games.defold.android
    jclass cls = GetClass(env, java_class);
    jmethodID method = env->GetStaticMethodID(cls, "showToast", "(Landroid/content/Context;Ljava/lang/String;)V");

    jstring jMessage = env->NewStringUTF(message);
    jobject context = dmGraphics::GetNativeAndroidActivity();    
    env->CallStaticVoidMethod(cls, method, context, jMessage);
    env->DeleteLocalRef(jMessage);

    return 0;
}

// Function to access the latest input event from Lua
static int Lua_get_latest_input_event(lua_State* L) {
    AInputEvent* event = g_lastInputEvent;

    if (event == NULL) {
        // dmLogInfo("No input event available");
        lua_pushnil(L);
        return 1;
    }

    // Example: Get keycode and return it to Lua
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        int32_t keycode = AKeyEvent_getKeyCode(event);
        lua_pushinteger(L, keycode);
    } else {
        lua_pushnil(L);  // Return nil if no key event is available
    }

    return 1;
}

static int Lua_get_latest_key_event(lua_State* L) {
    AInputEvent* event = g_lastInputEvent;

    if (event == NULL) {
        // dmLogInfo("No input event available");
        lua_pushnil(L);
        return 1;
    }

    int32_t eventType = AInputEvent_getType(event);

    // Check for key events (button presses)
    if (eventType == AINPUT_EVENT_TYPE_KEY) {
        int32_t keycode = AKeyEvent_getKeyCode(event);
        int32_t action = AKeyEvent_getAction(event); // ACTION_DOWN or ACTION_UP

        // Push keycode and action to Lua
        lua_pushinteger(L, keycode);  // keycode of the pressed key
        lua_pushinteger(L, action);   // action (pressed or released)
        return 2;
    }

    lua_pushnil(L);  // If no key event, return nil
    return 1;
}

// Function to access the latest axis event from Lua
static int Lua_get_latest_axis_event(lua_State* L) {
    AInputEvent* event = g_lastInputEvent;

    if (event == NULL) {
        // dmLogInfo("No input event available");
        lua_pushnil(L);
        return 1;
    }

    int32_t eventType = AInputEvent_getType(event);
    int32_t source = AInputEvent_getSource(event);

    // Check for motion events (axes for joysticks/gamepads)
    if (eventType == AINPUT_EVENT_TYPE_MOTION) {
        if (source & AINPUT_SOURCE_JOYSTICK || source & AINPUT_SOURCE_GAMEPAD) {
            // Extract joystick or gamepad axis values
            float axis_x = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_X, 0);
            float axis_y = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Y, 0);
            float hat_x = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0);
            float hat_y = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0);

            // Apply dead zone threshold to filter out near-zero values
            if (fabs(axis_x) < DEAD_ZONE_THRESHOLD) axis_x = 0.0f;
            if (fabs(axis_y) < DEAD_ZONE_THRESHOLD) axis_y = 0.0f;
            if (fabs(hat_x) < DEAD_ZONE_THRESHOLD) hat_x = 0.0f;
            if (fabs(hat_y) < DEAD_ZONE_THRESHOLD) hat_y = 0.0f;

            // Push axis values to Lua if they are outside the dead zone
            lua_pushnumber(L, axis_x);  // X-axis value
            lua_pushnumber(L, axis_y);  // Y-axis value
            lua_pushnumber(L, hat_x);   // Hat switch X-axis value
            lua_pushnumber(L, hat_y);   // Hat switch Y-axis value
            return 4;  // Return 4 values (X, Y, HAT_X, HAT_Y)
        }
    }

    lua_pushnil(L);  // If no axis event, return nil
    return 1;
}

static int InitializeNativeExample(lua_State* L) {
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    jobject activity = dmGraphics::GetNativeAndroidActivity();    
    jclass native_example_class = GetClass(env, java_class);

    jmethodID set_activity = env->GetStaticMethodID(native_example_class, "setMainActivity", "(Landroid/app/Activity;)V");
    env->CallStaticVoidMethod(native_example_class, set_activity, activity);

    return 0;
}

static int GetRaw(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    // jclass cls = GetClass(env, "com.defold.androidnativeext.NativeExample");
    jclass cls = GetClass(env, java_class);

    jmethodID method = env->GetStaticMethodID(cls, "GetRaw", "(Landroid/content/Context;)Ljava/lang/String;");
    jstring return_value = (jstring)env->CallStaticObjectMethod(cls, method, dmGraphics::GetNativeAndroidActivity());
    lua_pushstring(L, env->GetStringUTFChars(return_value, 0));
    env->DeleteLocalRef(return_value);

    return 1;
}

static int Multiply(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);

    lua_Number a = luaL_checknumber(L, 1);
    lua_Number b = luaL_checknumber(L, 2);

    lua_pushnumber(L, a * b);
    return 1;
}

/*
*/
static int IsPlayingOnTV(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    jclass cls = GetClass(env, "android/content/pm/PackageManager");
    jmethodID hasSystemFeature = env->GetMethodID(cls, "hasSystemFeature", "(Ljava/lang/String;)Z");

    jobject packageManager = env->CallObjectMethod(dmGraphics::GetNativeAndroidActivity(), env->GetMethodID(env->GetObjectClass(dmGraphics::GetNativeAndroidActivity()), "getPackageManager", "()Landroid/content/pm/PackageManager;"));
    jstring leanbackFeature = env->NewStringUTF("android.software.leanback");

    jboolean isTV = env->CallBooleanMethod(packageManager, hasSystemFeature, leanbackFeature);
    lua_pushboolean(L, isTV);

    env->DeleteLocalRef(leanbackFeature);
    env->DeleteLocalRef(packageManager);

    return 1;
}

/*
Example
local hasLeanback = androidnative.check_system_feature("android.software.leanback")

 */
static int CheckSystemFeature(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    const char* feature = luaL_checkstring(L, 1);  // Get the system feature string from the Lua script

    jclass cls = GetClass(env, "android/content/pm/PackageManager");
    jmethodID hasSystemFeature = env->GetMethodID(cls, "hasSystemFeature", "(Ljava/lang/String;)Z");

    jobject packageManager = env->CallObjectMethod(dmGraphics::GetNativeAndroidActivity(), env->GetMethodID(env->GetObjectClass(dmGraphics::GetNativeAndroidActivity()), "getPackageManager", "()Landroid/content/pm/PackageManager;"));
    jstring jFeature = env->NewStringUTF(feature);

    jboolean result = env->CallBooleanMethod(packageManager, hasSystemFeature, jFeature);
    lua_pushboolean(L, result);

    env->DeleteLocalRef(jFeature);
    env->DeleteLocalRef(packageManager);

    return 1;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"dostuff_java", DoStuffJava},
    {"dostuff_jar", DoStuffJar},
    {"vibrate", Vibrate},
    {"getraw", GetRaw},
    {"set_orientation", SetOrientation},
    {"show_toast", ShowToast},  
    {"multiply", Multiply},
    {"is_playing_on_tv", IsPlayingOnTV}, 
    {"check_system_feature", CheckSystemFeature},
    {"initialize", InitializeNativeExample},
    {"get_latest_input_event", Lua_get_latest_input_event},
    {"get_latest_key_event", Lua_get_latest_key_event},
    {"get_latest_axis_event", Lua_get_latest_axis_event},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result AppInitializeExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeExtension(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    printf("Registered %s Extension\n", MODULE_NAME);
    
    // Automatically call the initialize() method to set up the key listener
    lua_getglobal(params->m_L, MODULE_NAME);
    lua_getfield(params->m_L, -1, "initialize");
    if (lua_pcall(params->m_L, 0, 0, 0) != 0) {
        dmLogError("Error calling initialize: %s", lua_tostring(params->m_L, -1));
        lua_pop(params->m_L, 1); // Remove the error from the stack
    }
    lua_pop(params->m_L, 1); // Remove the module from the stack

    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

#else

static dmExtension::Result AppInitializeExtension(dmExtension::AppParams* params)
{
    dmLogWarning("Registered %s (null) Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

#endif

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeExtension, AppFinalizeExtension, InitializeExtension, 0, 0, FinalizeExtension)
