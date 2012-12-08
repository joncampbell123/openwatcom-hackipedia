#ifndef _PBT_H
#define _PBT_H
#if __GNUC__ >= 3
#pragma GCC system_header
#endif

/* Also in winuser.h  */
#define PBT_APMQUERYSUSPEND 0
#define PBT_APMQUERYSTANDBY 1
#define PBT_APMQUERYSUSPENDFAILED 2
#define PBT_APMQUERYSTANDBYFAILED 3
#define PBT_APMSUSPEND 4
#define PBT_APMSTANDBY 5
#define PBT_APMRESUMECRITICAL 6
#define PBT_APMRESUMESUSPEND 7
#define PBT_APMRESUMESTANDBY 8
#define PBT_APMBATTERYLOW 9
#define PBT_APMPOWERSTATUSCHANGE 10
#define PBT_APMOEMEVENT 11
#define PBT_APMRESUMEAUTOMATIC 18

#define PBTF_APMRESUMEFROMFAILURE 1

#endif
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif
