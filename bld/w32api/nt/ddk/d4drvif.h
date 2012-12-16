/*
 *  d4drvif.h   DOT4 driver interface
 *
 * =========================================================================
 *
 *                          Open Watcom Project
 *
 * Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
 *
 *    This file is automatically generated. Do not edit directly.
 *
 * =========================================================================
 */

#ifndef _DOT4DRVIF_H
#define _DOT4DRVIF_H

#ifndef _ENABLE_AUTODEPEND
 #pragma read_only_file;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum service length */
#define MAX_SERVICE_LENGTH  40

/* DOT4 device I/O control codes */
#define FILE_DEVICE_DOT4        0x3A
#define IOCTL_DOT4_USER_BASE    2049
#define IOCTL_DOT4_LAST         (IOCTL_DOT4_USER_BASE + 9)
#define IOCTL_DOT4_CREATE_SOCKET \
    CTL_CODE( FILE_DEVICE_DOT4, IOCTL_DOT4_USER_BASE + 7, METHOD_OUT_DIRECT, \
    FILE_ANY_ACCESS )
#define IOCTL_DOT4_DESTROY_SOCKET \
    CTL_CODE( FILE_DEVICE_DOT4, IOCTL_DOT4_USER_BASE + 9, METHOD_OUT_DIRECT, \
    FILE_ANY_ACCESS )
#define IOCTL_DOT4_WAIT_FOR_CHANNEL \
    CTL_CODE( FILE_DEVICE_DOT4, IOCTL_DOT4_USER_BASE + 8, METHOD_OUT_DIRECT, \
    FILE_ANY_ACCESS )
#define IOCTL_DOT4_OPEN_CHANNEL \
    CTL_CODE( FILE_DEVICE_DOT4, IOCTL_DOT4_USER_BASE + 0, METHOD_OUT_DIRECT, \
    FILE_ANY_ACCESS )
#define IOCTL_DOT4_CLOSE_CHANNEL \
    CTL_CODE( FILE_DEVICE_DOT4, IOCTL_DOT4_USER_BASE + 1, METHOD_BUFFERED, \
    FILE_ANY_ACCESS )
#define IOCTL_DOT4_READ \
    CTL_CODE( FILE_DEVICE_DOT4, IOCTL_DOT4_USER_BASE + 2, METHOD_OUT_DIRECT, \
    FILE_ANY_ACCESS )
#define IOCTL_DOT4_WRITE \
    CTL_CODE( FILE_DEVICE_DOT4, IOCTL_DOT4_USER_BASE + 3, METHOD_IN_DIRECT, \
    FILE_ANY_ACCESS )
#define IOCTL_DOT4_ADD_ACTIVITY_BROADCAST \
    CTL_CODE( FILE_DEVICE_DOT4, IOCTL_DOT4_USER_BASE + 4, METHOD_BUFFERED, \
    FILE_ANY_ACCESS )
#define IOCTL_DOT4_REMOVE_ACTIVITY_BROADCAST \
    CTL_CODE( FILE_DEVICE_DOT4, IOCTL_DOT4_USER_BASE + 5, METHOD_BUFFERED, \
    FILE_ANY_ACCESS )
#define IOCTL_DOT4_WAIT_ACTIVITY_BROADCAST \
    CTL_CODE( FILE_DEVICE_DOT4, IOCTL_DOT4_USER_BASE + 6, METHOD_OUT_DIRECT, \
    FILE_ANY_ACCESS )

/* DOT4 driver command */
typedef struct _DOT4_DRIVER_CMD {
    CHANNEL_HANDLE  hChannelHandle;
    ULONG           ulSize;
    ULONG           ulOffset;
    ULONG           ulTimeout;
} DOT4_DRIVER_CMD;
typedef DOT4_DRIVER_CMD *PDOT4_DRIVER_CMD;

/* DOT4 DC open data */
typedef struct _DOT4_DC_OPEN_DATA {
    unsigned char   bHsid;
    unsigned char   fAddActivity;
    CHANNEL_HANDLE  hChannelHandle;
} DOT4_DC_OPEN_DATA;
typedef DOT4_DC_OPEN_DATA   *PDOT4_DC_OPEN_DATA;

/* DOT4 DC create data */
typedef struct _DOT4_DC_CREATE_DATA {
    unsigned char   bPsid;
    CHAR            pServiceName[MAX_SERVICE_LENGTH + 1];
    unsigned char   bType;
    ULONG           ulBufferSize;
    USHORT          usMaxHtoPPacketSize;
    USHORT          usMaxPtoHPacketSize;
    unsigned char   bHsid;
} DOT4_DC_CREATE_DATA;
typedef DOT4_DC_CREATE_DATA *PDOT4_DC_CREATE_DATA;

/* DOT4 DC destroy data */
typedef struct _DOT4_DC_DESTROY_DATA {
    unsigned char   bHsid;
} DOT4_DC_DESTROY_DATA;
typedef DOT4_DC_DESTROY_DATA    *PDOT4_DC_DESTROY_DATA;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _DOT4DRVIF_H */
