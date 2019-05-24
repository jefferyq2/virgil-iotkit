/*
 *   Copyright (C) 2015-2019 Virgil Security Inc.
 *
 *   Logger library implementation
 *
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include <logger.h>
#include <logger_implement.h>

#define EOL "\r\n"
#define VS_ASSERT(...)

static vs_log_level_t _log_level = VS_LOGLEV_UNKNOWN;
static size_t _max_buf_size = 0;

/******************************************************************************/
bool
vs_logger_init(vs_log_level_t log_level, size_t max_buf_size) {
    vs_logger_set_loglev(log_level);

    _max_buf_size = max_buf_size;
    if (!max_buf_size) {
        return false;
    }

    return true;
}

/******************************************************************************/
vs_log_level_t
vs_logger_set_loglev(vs_log_level_t new_level) {
    vs_log_level_t prev_level = _log_level;

    _log_level = new_level;

    return prev_level;
}

/******************************************************************************/
vs_log_level_t
vs_logger_get_loglev(void) {
    return _log_level;
}

/******************************************************************************/
bool
vs_logger_is_loglev(vs_log_level_t level) {

    VS_ASSERT(_log_level != VS_LOGLEV_UNKNOWN);

    return level <= _log_level;
}

/******************************************************************************/
static const char *
_get_level_str(vs_log_level_t log_level) {

    switch (log_level) {
    case VS_LOGLEV_INFO:
        return "INFO";
    case VS_LOGLEV_FATAL:
        return "FATAL";
    case VS_LOGLEV_ALERT:
        return "ALERT";
    case VS_LOGLEV_CRITICAL:
        return "CRITICAL";
    case VS_LOGLEV_ERROR:
        return "ERROR";
    case VS_LOGLEV_WARNING:
        return "WARNING";
    case VS_LOGLEV_NOTICE:
        return "NOTICE";
    case VS_LOGLEV_TRACE:
        return "TRACE";
    case VS_LOGLEV_DEBUG:
        return "DEBUG";

    default:
        VS_ASSERT(false && "Unsupported logging level");
        return "";
    }
}

// Output current time
/*********************************************************/
static bool
_output_time(void) {
    return true;

#if 0
    time_t time_tmp;
    static const size_t TIME_STR_SIZE = 26; // "%Y-%m-%d %H:%M:%S:"
    char time_buf[TIME_STR_SIZE];

    time(&time_tmp);
    strftime(time_buf, TIME_STR_SIZE, "%Y-%m-%d %H:%M:%S:", localtime(&time_tmp));

    return vs_logger_implement(time_buf);
#endif
}

/******************************************************************************/
#if defined(__GNUC__) && VIRGIL_IOT_MCU_BUILD
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstack-usage="
#endif
static bool
_output_preface(vs_log_level_t level, const char *cur_filename, size_t line_num) {
    int str_size;
    const char *level_str = NULL;
    int snprintf_res;
    bool res = false;

    level_str = _get_level_str(level);

    // Output time string
    if (!_output_time()) {
        return false;
    }

    // Calculate preface string size
    // TODO : snprintf - since C99
    if (!cur_filename || !line_num) {
        str_size = snprintf(NULL, 0, " [%s] ", level_str) + 1;
    } else {
        str_size = snprintf(NULL, 0, " [%s] [%s:%d] ", level_str, cur_filename, (int)line_num) + 1;
    }

    VS_ASSERT(str_size > 0 && str_size <= _max_buf_size);

    // TODO : VAL, variable not at the function begin - since C99
    char stack_buf[str_size];

    // TODO : snprintf - since C99
    if (!cur_filename || !line_num) {
        snprintf_res = snprintf(stack_buf, str_size, " [%s] ", level_str);
    } else {
        snprintf_res = snprintf(stack_buf, str_size, " [%s] [%s:%d] ", level_str, cur_filename, (int)line_num);
    }

    if (snprintf_res < 0) {
        VS_ASSERT(false);
        return false;
    }

    // Output string
    res = vs_logger_implement(stack_buf);

    return res;
}

/******************************************************************************/
bool
vs_logger_message(vs_log_level_t level, const char *cur_filename, size_t line_num, const char *format, ...) {
    static const char *CUTTED_STR = "...";
    static const size_t CUTTED_STR_SIZE = 3;
    va_list args1;
    va_list args2;
    int str_size;
    int snprintf_res;
    bool res = true;
    bool cutted_str = false;

    if (!vs_logger_is_loglev(level)) {
        return true;
    }

    VS_ASSERT(cur_filename);
    VS_ASSERT(format);

    if (!_output_preface(level, cur_filename, line_num)) {
        return false;
    }

    // Calculate string size
    va_start(args1, format);
    va_copy(args2, args1);

    str_size = vsnprintf(NULL, 0, format, args1) /* format ... */ + 1;

    va_end(args1);

    VS_ASSERT(str_size > 0);

    if (str_size > _max_buf_size) {
        str_size = _max_buf_size;
    }

    // Allocate stack buffer

    // TODO : VAL, variable not at the function begin - since C99
    char stack_buf[str_size];

    // Make full string

    // TODO : vsnprintf - since C99
    snprintf_res = vsnprintf(stack_buf, str_size, format, args2);

    if (snprintf_res >= 0 && snprintf_res >= str_size) {
        strcpy(stack_buf + snprintf_res + str_size - (CUTTED_STR_SIZE + 1 /* '\0' */), CUTTED_STR);
        cutted_str = true;
    } else if (snprintf_res < 0) {
        res = false;
    }

    va_end(args2);

    // Output string
    if (res) {
        res &= vs_logger_implement(stack_buf);
    }

    // EOL
    if (res) {
        res &= vs_logger_implement(EOL);
    }

    return res && !cutted_str;
}
#if defined(__GNUC__) && VIRGIL_IOT_MCU_BUILD
#pragma GCC diagnostic pop
#endif
/******************************************************************************/
bool
vs_logger_message_hex(vs_log_level_t level,
                      const char *cur_filename,
                      size_t line_num,
                      const char *prefix,
                      const void *data_buf,
                      const size_t data_size) {
    char buf[3]; // "%02X"
    unsigned char *cur_byte;
    size_t pos;
    bool res;

    VS_ASSERT(prefix);
    VS_ASSERT(data_buf && data_size);

    if (!vs_logger_is_loglev(level)) {
        return true;
    }

    if (!_output_preface(level, cur_filename, line_num)) {
        return false;
    }

    res = vs_logger_implement(prefix);

    if (res) {
        cur_byte = (unsigned char *)data_buf;
        for (pos = 0; pos < data_size && res; ++pos, ++cur_byte) {
            sprintf(buf, "%02X", *cur_byte);
            res = vs_logger_implement(buf);
        }
    }

    if (res) {
        res = vs_logger_implement(EOL);
    }

    return res;
}
