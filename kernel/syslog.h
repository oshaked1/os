#include "../lib/types.h"

#ifndef _LOG_SEVERITY
#define _LOG_SEVERITY
enum LOG_SEVERITY{LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR,
                    LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG};
#endif

#ifndef _LOG_DST
#define _LOG_DST
enum LOG_DST{DST_SCREEN, DST_SERIAL, DST_VTY, DST_LOGFILE, DST_SYSLOG};
#endif

#define MAX_LENGTH 512

int get_log_dst_severity(int dst);
void set_log_dst_severity(int dst, int severity);
void set_syslog_serial_port(ushort com);
void log(const char *subsys, int severity, const char *mnemonic, const char *message, ...);
void debug(const char *message, ...);