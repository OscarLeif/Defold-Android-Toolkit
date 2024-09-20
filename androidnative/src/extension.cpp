// Extension lib defines
#define EXTENSION_NAME androidnative
#define LIB_NAME "androidnative"
#define MODULE_NAME LIB_NAME


// Defold SDK
#define DLIB_LOG_DOMAIN LIB_NAME
#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_ANDROID)

#include "testlib.h" // Multiply

const char* java_class = "ata.games.defold.android.NativeExample";

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

static int InitializeNativeExample(lua_State* L) {
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    jobject activity = dmGraphics::GetNativeAndroidActivity();    
    // jclass native_example_class = env->FindClass("ata/games/defold/android/NativeExample");
    jclass native_example_class = GetClass(env, java_class);

    jmethodID set_activity = env->GetStaticMethodID(native_example_class, "setMainActivity", "(Landroid/app/Activity;)V");
    env->CallStaticVoidMethod(native_example_class, set_activity, activity);

    return 0;
}

static int Lua_ResetPressedThisFrameStates(lua_State* L) {
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;
    jclass native_example_class = GetClass(env, java_class);
    jmethodID reset_pressed_this_frame_states = env->GetStaticMethodID(native_example_class, "updateButtonStates", "()V");
    env->CallStaticVoidMethod(native_example_class, reset_pressed_this_frame_states);
    return 0;
}

//Seems I dont need this.
static int Lua_UpdateButtonStates(lua_State* L) {
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;
    jclass native_example_class = GetClass(env, java_class);
    jmethodID update_button_states = env->GetStaticMethodID(native_example_class, "updateButtonStates", "()V");
    env->CallStaticVoidMethod(native_example_class, update_button_states);
    return 0;
}

static int Lua_IsButtonPressed(lua_State* L)
{    
    AttachScope attachscope;
    int keyCode = luaL_checkinteger(L,1);
    JNIEnv* env = attachscope.m_Env;
    jclass native_example_class = GetClass(env, java_class);
    jmethodID is_button_pressed = env->GetStaticMethodID(native_example_class, "isButtonPressed", "(I)Z");
    jboolean result = env->CallStaticBooleanMethod(native_example_class, is_button_pressed, keyCode);
    lua_pushboolean(L, result);
    return 1;
}

static int Lua_WasButtonPressedThisFrame(lua_State* L) {
    AttachScope attachscope;
    int keyCode = luaL_checkinteger(L, 1);
    JNIEnv* env = attachscope.m_Env;
    jclass native_example_class = GetClass(env, java_class);
    jmethodID was_button_pressed_this_frame = env->GetStaticMethodID(native_example_class, "wasButtonPressedThisFrame", "(I)Z");
    jboolean result = env->CallStaticBooleanMethod(native_example_class, was_button_pressed_this_frame, keyCode);
    lua_pushboolean(L, result);
    return 1;
}

static int Lua_WasButtonReleasedThisFrame(lua_State* L) {
    AttachScope attachscope;
    int keyCode = luaL_checkinteger(L, 1);
    JNIEnv* env = attachscope.m_Env;
    jclass native_example_class = GetClass(env, java_class);
    jmethodID was_button_released_this_frame = env->GetStaticMethodID(native_example_class, "wasButtonReleasedThisFrame", "(I)Z");
    jboolean result = env->CallStaticBooleanMethod(native_example_class, was_button_released_this_frame, keyCode);
    lua_pushboolean(L, result);
    return 1;
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
    {"was_button_pressed_this_frame", Lua_WasButtonPressedThisFrame},
    {"was_button_released_this_frame", Lua_WasButtonReleasedThisFrame},  // For button release tracking
    {"is_button_pressed", Lua_IsButtonPressed},                         // Check if the button is held
    {"reset_pressed_this_frame_states", Lua_ResetPressedThisFrameStates},  
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
