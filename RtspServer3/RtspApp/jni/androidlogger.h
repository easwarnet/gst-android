//
// Created by easwar on 25/07/2023.
//

#ifndef RTSPSERVER3_ANDROIDLOGGER_H
#define RTSPSERVER3_ANDROIDLOGGER_H
#include <android/log.h>

#define LOGD(tag, fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, fmt "\n--> %s\n----> %s:%d", ##__VA_ARGS__, __FILE__, __FUNCTION__, __LINE__)
#define LOGE(tag, fmt, ...) __android_log_print(ANDROID_LOG_ERROR, tag, fmt "\n--> %s\n----> %s:%d", ##__VA_ARGS__, __FILE__, __FUNCTION__, __LINE__)
#define LOGV(tag, fmt, ...) __android_log_print(ANDROID_LOG_VERBOSE, tag, fmt "\n--> %s\n----> %s:%d", ##__VA_ARGS__, __FILE__, __FUNCTION__, __LINE__)

#define LOGTAG "com.tarjet.rtspapp"

#endif //RTSPSERVER3_ANDROIDLOGGER_H
