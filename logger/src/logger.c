//  Copyright (C) 2015-2019 Virgil Security, Inc.
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//      (1) Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//      (2) Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in
//      the documentation and/or other materials provided with the
//      distribution.
//
//      (3) Neither the name of the copyright holder nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
//  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
//  Lead Maintainer: Virgil Security Inc. <support@virgilsecurity.com>

#include <logger-config.h>

#if VS_IOT_LOGGER_USE_LIBRARY == 1

#include <stdlib-config.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <logger.h>
#include <logger_hal.h>
#include <utoa_fast_div.h>

static vs_log_level_t _log_level = VS_LOGLEV_UNKNOWN;
static bool _last_res = true;

// Output directly string without single '%' in the string
#define VS_IOT_LOGGER_OPTIMIZE_NONFORMAT_CALL 1

/******************************************************************************/
bool
vs_logger_last_result(void) {
    return _last_res;
}

/******************************************************************************/
void
vs_logger_init(vs_log_level_t log_level) {
    vs_logger_set_loglev(log_level);
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

    VS_IOT_ASSERT(_log_level != VS_LOGLEV_UNKNOWN);

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
        VS_IOT_ASSERT(0 && "Unsupported logging level");
        return "";
    }
}

/******************************************************************************/
#define VS_LOGGER_OUTPUT(STR)                                                                                          \
    do {                                                                                                               \
        if (!vs_logger_output_hal(STR))                                                                                \
            goto terminate;                                                                                            \
    } while (0)

#if defined(__GNUC__) && VIRGIL_IOT_MCU_BUILD
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstack-usage="
#endif // defined(__GNUC__) && VIRGIL_IOT_MCU_BUILD

/******************************************************************************/
static bool
_output_preface(vs_log_level_t level, const char *cur_filename, uint32_t line_num) {
    const char *level_str = NULL;
    bool res = true;
    char buf[11]; // for line number less than 9'000'000'000

    level_str = _get_level_str(level);

#if VS_IOT_LOGGER_OUTPUT_TIME == 1

    // Output time string
    if (!vs_logger_current_time_hal()) {
        return false;
    }

#endif // VS_IOT_LOGGER_OUTPUT_TIME == 1

    // Output level and file
    VS_LOGGER_OUTPUT(" [");
    VS_LOGGER_OUTPUT(level_str);
    VS_LOGGER_OUTPUT("] ");

    if (cur_filename && line_num) {
        VS_LOGGER_OUTPUT(" [");
        VS_LOGGER_OUTPUT(cur_filename);
        VS_LOGGER_OUTPUT(":");
        VS_LOGGER_OUTPUT(utoa_fast_div((uint32_t)line_num, buf));
        VS_LOGGER_OUTPUT("] ");
    }

terminate:

    _last_res = res;
    return res;
}

#if VS_IOT_LOGGER_OPTIMIZE_NONFORMAT_CALL == 1

/******************************************************************************/
static bool
_no_format(const char *format) {
    const char *cur_pos;

    for (cur_pos = format; *cur_pos != '\0'; ++cur_pos) {
        if (*cur_pos != '%') {
            continue;
        }

        if (cur_pos[1] == '\0') {
            break;
        }

        // "%"
        if (cur_pos[1] != '%') {
            return false;
        }

        ++cur_pos;
    }

    return true;
}

#endif // VS_IOT_LOGGER_OPTIMIZE_NONFORMAT_CALL == 1

/******************************************************************************/
void
vs_logger_message(vs_log_level_t level, const char *cur_filename, uint32_t line_num, const char *format, ...) {
    static const char *CUTTED_STR = "...";
    static const size_t CUTTED_STR_SIZE = 3;
    va_list args1;
    int snprintf_res;
    bool res = false;
#if VS_IOT_LOGGER_USE_STATIC_BUFFER == 1
    static
#endif // VS_IOT_LOGGER_USE_STATIC_BUFFER == 1
            char stack_buf[VS_IOT_LOGGER_MAX_BUFFER_SIZE];

    _last_res = true;

    if (!vs_logger_is_loglev(level)) {
        goto terminate;
    }

    VS_IOT_ASSERT(cur_filename);
    VS_IOT_ASSERT(format);

    if (!_output_preface(level, cur_filename, line_num)) {
        goto terminate;
    }

#if VS_IOT_LOGGER_OPTIMIZE_NONFORMAT_CALL == 1

    // Omit arguments if there are no single "%"
    if (_no_format(format)) {
        VS_LOGGER_OUTPUT(format);
        VS_LOGGER_OUTPUT(VS_IOT_LOGGER_EOL);
        res = true;
        goto terminate;
    }

#endif // VS_IOT_LOGGER_OPTIMIZE_NONFORMAT_CALL == 1

    va_start(args1, format);

    // TODO : vsnprintf - since C99
    snprintf_res = VS_IOT_VSNPRINTF(stack_buf, VS_IOT_LOGGER_MAX_BUFFER_SIZE, format, args1);

    va_end(args1);

    if (snprintf_res < 0) {
        goto terminate;
    } else if (snprintf_res >= VS_IOT_LOGGER_MAX_BUFFER_SIZE) {
        VS_IOT_STRCPY(stack_buf + VS_IOT_LOGGER_MAX_BUFFER_SIZE - (CUTTED_STR_SIZE + 1 /* '\0' */), CUTTED_STR);
    }

    // Output string
    VS_LOGGER_OUTPUT(stack_buf);

    // EOL
    VS_LOGGER_OUTPUT(VS_IOT_LOGGER_EOL);

    res = true;

terminate:

    _last_res = res;

    return;
}

#if defined(__GNUC__) && VIRGIL_IOT_MCU_BUILD
#pragma GCC diagnostic pop
#endif // defined(__GNUC__) && VIRGIL_IOT_MCU_BUILD

/******************************************************************************/
void
vs_logger_message_hex(vs_log_level_t level,
                      const char *cur_filename,
                      uint32_t line_num,
                      const char *prefix,
                      const void *data_buf,
                      const size_t data_size) {
    static const char *HEX_FORMAT = "%02X";
    char buf[3]; // HEX_FORMAT output
    unsigned char *cur_byte;
    size_t pos;
    bool res = false;

    _last_res = true;

    VS_IOT_ASSERT(prefix);
    VS_IOT_ASSERT(data_buf);
    VS_IOT_ASSERT(data_size);

    if (!vs_logger_is_loglev(level)) {
        _last_res = false;
        return;
    }

    if (!_output_preface(level, cur_filename, line_num)) {
        return;
    }

    VS_LOGGER_OUTPUT(prefix);

    cur_byte = (unsigned char *)data_buf;
    for (pos = 0; pos < data_size; ++pos, ++cur_byte) {
        if (VS_IOT_SPRINTF(buf, HEX_FORMAT, *cur_byte) < 0)

            VS_LOGGER_OUTPUT(buf);
    }

    VS_LOGGER_OUTPUT(VS_IOT_LOGGER_EOL);

    res = true;

terminate:

    _last_res = res;

    return;
}

#endif // VS_IOT_LOGGER_USE_LIBRARY == 1
