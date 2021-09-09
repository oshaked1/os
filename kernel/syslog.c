#include "syslog.h"
#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include "../libc/stdlib.h"
#include "../libc/stdio.h"
#include "../libc/stdarg.h"

ushort dst_screen_severity  = LOG_NOTICE;
ushort dst_serial_severity  = LOG_DEBUG;
ushort dst_vty_severity     = LOG_CRIT;
ushort dst_logfile_severity = LOG_INFO;
ushort dst_syslog_severity  = LOG_INFO;

ushort syslog_serial_port = COM1; // COM1 is used by default
bool serial_initialized = FALSE;

int get_log_dst_severity(int dst)
{
    switch (dst)
    {
    case DST_SCREEN:
        return dst_screen_severity;
        break;
    
    case DST_SERIAL:
        return dst_serial_severity;
        break;
    
    case DST_VTY:
        return dst_vty_severity;
        break;
    
    case DST_LOGFILE:
        return dst_logfile_severity;
        break;
    
    case DST_SYSLOG:
        return dst_syslog_severity;
        break;
    
    default:
        return -1;
        break;
    }
}

void set_log_dst_severity(int dst, int severity)
{
    if (severity < 0 || severity > 7)
        return;

    switch (dst)
    {
    case DST_SCREEN:
        dst_screen_severity = severity;
        break;
    
    case DST_SERIAL:
        dst_serial_severity = severity;
        break;
    
    case DST_VTY:
        dst_vty_severity = severity;
        break;
    
    case DST_LOGFILE:
        dst_logfile_severity = severity;
        break;
    
    case DST_SYSLOG:
        dst_syslog_severity = severity;
        break;
    }
}

void set_syslog_serial_port(ushort com)
{
    if (!serial_initialized || (syslog_serial_port != com))
    {
        init_serial(com);
        serial_initialized = TRUE;
    }
    syslog_serial_port = com;
}

void log(const char *subsys, int severity, const char *mnemonic, const char *message, ...)
{
    char str[MAX_LENGTH];
    va_list args;
    va_start(args, message);
    vsnprintf(str, MAX_LENGTH, message, args);
    va_end(args);

    if (severity < 0 || severity > 7)
        return;

    char fullstr[MAX_LENGTH];
    snprintf(fullstr, MAX_LENGTH, "%%%s-%d-%s: %s\n", subsys, severity, mnemonic, str);
    // if the message was too long and was cut short, make sure it ends with a newline
    fullstr[MAX_LENGTH-2] = '\n';

    // print message on screen
    if (dst_screen_severity >= severity)
        kprint(fullstr);

    // send message to serial port
    if (dst_serial_severity >= severity)
    {
        if (!serial_initialized)
        {
            init_serial(syslog_serial_port);
            serial_initialized = TRUE;
        }
        serial_sends(syslog_serial_port, fullstr);
    }
}

void debug(const char *message, ...)
{
    char str[MAX_LENGTH];
    va_list args;
    va_start(args, message);
    vsnprintf(str, MAX_LENGTH, message, args);
    va_end(args);

    log("SYS", 7, "DEBUG", str);
}
