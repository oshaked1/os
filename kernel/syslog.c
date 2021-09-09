#include "syslog.h"
#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include "../libc/stdlib.h"

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

void log(char *subsys, int severity, char *mnemonic, char *message)
{
    if (severity < 0 || severity > 7)
        return;

    // print message on screen
    if (dst_screen_severity >= severity)
    {
        kputchar('%');
        kprint(subsys);
        kputchar('-');
        char temp[2];
        kprint(itoa(severity, temp, 10));
        kputchar('-');
        kprint(mnemonic);
        kprint(": ");
        kprint(message);
        kputchar('\n');
    }

    // send message to serial port
    if (dst_serial_severity >= severity)
    {
        if (!serial_initialized)
        {
            init_serial(syslog_serial_port);
            serial_initialized = TRUE;
        }
        serial_sends(syslog_serial_port, "%");
        serial_sends(syslog_serial_port, subsys);
        serial_sends(syslog_serial_port, "-");
        char temp[2];
        serial_sends(syslog_serial_port, itoa(severity, temp, 10));
        serial_sends(syslog_serial_port, "-");
        serial_sends(syslog_serial_port, mnemonic);
        serial_sends(syslog_serial_port, ": ");
        serial_sends(syslog_serial_port, message);
        serial_sends(syslog_serial_port, "\n");
    }
}
