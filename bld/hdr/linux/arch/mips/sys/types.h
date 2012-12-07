typedef unsigned long   dev_t;  /* Combination of node and device   */
#ifndef _GID_T_DEFINED_
 typedef long           gid_t;  /* Used for group IDs               */
 #define _GID_T_DEFINED_
#endif
typedef unsigned long   ino_t;  /* Used for file serial numbers     */
typedef unsigned long   mode_t; /* Used for some file attributes    */
typedef unsigned long   nlink_t;/* Used for link counts             */
typedef long            off_t;  /* Used for file sizes, offsets     */
typedef int             pid_t;  /* Used for process IDs & group IDs */
#ifndef _UID_T_DEFINED_
 typedef long           uid_t;  /* Used for user IDs                */
 #define _UID_T_DEFINED_
#endif
typedef long            id_t;   /* Generic ID, may be uid, gid, pid */
#if defined(_LINUX_SOURCE) || !defined(NO_EXT_KEYS)
#ifndef _TIMER_T_DEFINED_
 #define _TIMER_T_DEFINED_
typedef int             timer_t;/* Used for timer IDs               */
#endif
#ifndef _CLOCKID_T_DEFINED_
 #define _CLOCKID_T_DEFINED_
typedef int             clockid_t;/* Used for clock IDs             */
#endif
#endif

#define major(device)           ((int)(((device) >> 8) & 0xFF))
#define minor(device)           ((int)((device) & 0xff))
#define makedev(major,minor)    ((dev_t)(((major) << 8) | (minor)))

typedef char            *caddr_t;
typedef long            daddr_t;
