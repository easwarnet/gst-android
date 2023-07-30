#include <string>
#include <iostream>
#include <memory>
#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <pthread.h>
#include "androidlogger.h"
#include "rtspserver.h"

GST_DEBUG_CATEGORY_STATIC (debug_category);
#define GST_CAT_DEFAULT debug_category

#define LOGD(tag, fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, fmt "\n--> %s\n----> %s:%d", ##__VA_ARGS__, __FILE__, __FUNCTION__, __LINE__)
#define LOGE(tag, fmt, ...) __android_log_print(ANDROID_LOG_ERROR, tag, fmt "\n--> %s\n----> %s:%d", ##__VA_ARGS__, __FILE__, __FUNCTION__, __LINE__)
/*
 * These macros provide a way to store the native pointer to CustomData, which might be 32 or 64 bits, into
 * a jlong, which is always 64 bits, without warnings.
 */
#if GLIB_SIZEOF_VOID_P == 8
# define GET_CUSTOM_DATA(thiz, fieldID) (CustomData *)(env)->GetLongField (thiz, fieldID)
# define SET_CUSTOM_DATA(thiz, fieldID, data) (env)->SetLongField (thiz, fieldID, (jlong)data)
#else
# define GET_CUSTOM_DATA(thiz, fieldID) (CustomData *)(jint)(env)->GetLongField (thiz, fieldID)
# define SET_CUSTOM_DATA(thiz, fieldID, data) (env)->SetLongField (thiz, fieldID, (jlong)(jint)data)
#endif

#define DEFAULT_RTSP_PORT "8554"

using namespace std;

static JavaVM *java_vm;
static pthread_t gst_app_thread;
static pthread_key_t current_jni_env;
static jfieldID custom_data_field_id;
static jmethodID set_message_method_id;
static jmethodID on_gstreamer_initialized_method_id;
static RtspServer *rtspServer = nullptr;

typedef struct _CustomData {
    jobject app;                  /* Application instance, used to call its methods. A global reference is kept. */
    //GstElement *pipeline;         /* The running pipeline */
    GMainContext *context;        /* GLib context used to run the main loop */
    GMainLoop *main_loop;         /* GLib main loop */
    gboolean initialized;         /* To avoid informing the UI multiple times about the initialization */
    //GstElement *video_sink;       /* The video sink element which receives XOverlay commands */
    //string uri;
    //bool credentials;
    //string username;
    //string password;
    //ANativeWindow *native_window; /* The Android native window where video will be rendered */
} CustomData;

static char *port = (char *) DEFAULT_RTSP_PORT;

static JNIEnv *attach_current_thread (void) {
    JNIEnv *env;
    JavaVMAttachArgs args;

    //GST_DEBUG ("Attaching thread %p", g_thread_self());
    LOGD(LOGTAG, "%p", g_thread_self());
    args.version = JNI_VERSION_1_4;
    args.name = NULL;
    args.group = NULL;

    if ((java_vm)->AttachCurrentThread(&env, &args) < 0) {
        //GST_ERROR ("Failed to attach current thread");
        LOGE(LOGTAG, "Failed to attach current thread");
        return NULL;
    }

    return env;
}

/* Unregister this thread from the VM */
static void detach_current_thread (void *env) {
    //GST_DEBUG ("Detaching thread %p", g_thread_self());
    LOGD(LOGTAG, "Detaching thread %p", g_thread_self());
    (java_vm)->DetachCurrentThread();
}

/* Retrieve the JNI environment for this thread */
static JNIEnv *get_jni_env (void) {
    JNIEnv *env;

    if ((env = (JNIEnv *) pthread_getspecific(current_jni_env)) == NULL) {
        env = attach_current_thread();
        pthread_setspecific(current_jni_env, env);
    }

    return env;
}
static void *app_function(void *userdata) {
    string pipeline = " ( videotestsrc ! videoconvert ! x264enc ! rtph264pay name=pay0 pt=96 ) ";
    rtspServer = new RtspServer(pipeline);
    rtspServer->RtspServerInit(true, "/test", port);
    rtspServer->RtspStart();
}


static void gst_native_init(JNIEnv *env, jobject thiz, jboolean credentials,
                            jstring username, jstring password) {
    CustomData *data = g_new0 (CustomData, 1);
    SET_CUSTOM_DATA (thiz, custom_data_field_id, data);
    GST_DEBUG_CATEGORY_INIT (debug_category, "rtspserver", 0, "Android tutorial 3");
    gst_debug_set_threshold_for_name("rtspserver", GST_LEVEL_DEBUG);
    GST_DEBUG ("Created CustomData at %p", data);
    LOGD(LOGTAG, "Created CustomData at %p", data);
    data->app = (env)->NewGlobalRef(thiz);
    //string pipeline = " ( ahcsrc ! videoscale ! "
    //                  "capsfilter caps=video/x-raw ! videoconvert ! " //clockoverlay ! "
    //                  "queue ! x264enc ! rtph264pay name=pay0 pt=96 ) ";
    //string pipeline = " ( videotestsrc ! videoconvert ! x264enc ! rtph264pay name=pay0 pt=96 ) ";
    //rtspServer = new RtspServer(pipeline);
    //rtspServer->RtspServerInit(true, "/test", port);

    if(credentials == true) {
        const char *char_username = env->GetStringUTFChars(username, NULL);
        const char *char_password = env->GetStringUTFChars(password, NULL);
        GST_DEBUG ("Username Received %s", char_username);
        GST_DEBUG ("Password Received %s", char_password);
        //rtspServer->RtspServerAddUser("admin", "password", true, true);
        env->ReleaseStringUTFChars(username, char_username);
        env->ReleaseStringUTFChars(password, char_password);
    }

    //GST_DEBUG ("Created GlobalRef for app object at %p", data->app);
    //LOGD(LOGTAG, "Created GlobalRef for app object at %p", data->app);
    pthread_create(&gst_app_thread, NULL, &app_function, data);
    __android_log_print(ANDROID_LOG_VERBOSE, LOGTAG, "RtspInit");

}

static void gst_native_start (JNIEnv * env, jobject thiz) {
    //GST_DEBUG ("Native Start");
    CustomData *data = GET_CUSTOM_DATA (thiz, custom_data_field_id);
    if (!data)
        return;
    pthread_create(&gst_app_thread, NULL, &app_function, data);
}

static void gst_native_finalize (JNIEnv * env, jobject thiz) {
    CustomData *data = GET_CUSTOM_DATA (thiz, custom_data_field_id);
    if (!data)
        return;
    GST_DEBUG ("Quitting main loop...");
    __android_log_print(ANDROID_LOG_VERBOSE, "com.tarjet.rtspapp", LOGTAG, "Quitting main loop...");
    rtspServer->RtspStop();
    //g_main_loop_quit(data->main_loop);
    GST_DEBUG ("Waiting for thread to finish...");
    LOGD(LOGTAG, "Waiting for thread to finish...");
    pthread_join(gst_app_thread, NULL);
    GST_DEBUG ("Deleting GlobalRef for app object at %p", data->app);
    LOGD(LOGTAG, "Deleting GlobalRef for app object at %p", data->app);
    (env)->DeleteGlobalRef(data->app);
    GST_DEBUG ("Freeing CustomData at %p", data);
    LOGD(LOGTAG, "Freeing CustomData at %p", data);
    g_free(data);
    SET_CUSTOM_DATA (thiz, custom_data_field_id, NULL);
    GST_DEBUG ("Done finalizing");
    LOGD(LOGTAG, "Done finalizing");
    delete rtspServer;
}

static jboolean gst_native_class_init (JNIEnv * env, jclass klass) {
    custom_data_field_id = (env)->GetFieldID(klass, "native_custom_data", "J");
    set_message_method_id = (env)->GetMethodID(klass, "setMessage", "(Ljava/lang/String;)V");
    on_gstreamer_initialized_method_id = (env)->GetMethodID(klass, "onGStreamerInitialized", "()V");
    __android_log_print(ANDROID_LOG_VERBOSE, LOGTAG, "gst_native_class_init");

    if (!custom_data_field_id || !set_message_method_id || !on_gstreamer_initialized_method_id) {
        /* We emit this message through the Android log instead of the GStreamer log because the later
         * has not been initialized yet.
         */
        LOGD(LOGTAG, "rtspclient: calling class does not implement all necessary interface methods");
             //__android_log_print(ANDROID_LOG_ERROR, "rtspclient",
             //               "The calling class does not implement all necessary interface methods");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static JNINativeMethod native_methods[] = {
        {"nativeInit", "(ZLjava/lang/String;Ljava/lang/String;)V", (void *) gst_native_init},
        {"nativeFinalize", "()V", (void *) gst_native_finalize},
        {"nativeStart", "()V", (void *) gst_native_start},
        {"nativeClassInit", "()Z", (void *) gst_native_class_init}
};

/* Library initializer */
jint JNI_OnLoad (JavaVM * vm, void *reserved) {
    JNIEnv *env = NULL;
    java_vm = vm;

    if ((vm)->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        //__android_log_print(ANDROID_LOG_ERROR, "rtspclient", "Could not retrieve JNIEnv");
        LOGD(LOGTAG, "rtspclient: Could not retrieve JNIEnv");
        return 0;
    }
    jclass klass = (env)->FindClass("com/tarjet/rtspapp/RtspServer");
    (env)->RegisterNatives(klass, native_methods, G_N_ELEMENTS (native_methods));

    pthread_key_create(&current_jni_env, detach_current_thread);

    return JNI_VERSION_1_4;
}