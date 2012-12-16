/*
 *  scsiscan.h  SCSI scanner definitions
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

#ifndef _SCSISCAN_H_
#define _SCSISCAN_H_

#ifndef _ENABLE_AUTODEPEND
 #pragma read_only_file;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* SCSI request block flags */
#define SRB_FLAGS_DISABLE_SYNCH_TRANSFER    0x00000008L
#define SRB_FLAGS_DISABLE_AUTOSENSE         0x00000020L
#define SRB_FLAGS_DATA_IN                   0x00000040L
#define SRB_FLAGS_DATA_OUT                  0x00000080L
#define SRB_FLAGS_NO_DATA_TRANSFER          0x00000000L

/* SCSI request block status codes */
#define SRB_STATUS_PENDING                  0x00
#define SRB_STATUS_SUCCESS                  0x01
#define SRB_STATUS_ABORTED                  0x02
#define SRB_STATUS_ABORT_FAILED             0x03
#define SRB_STATUS_ERROR                    0x04
#define SRB_STATUS_BUSY                     0x05
#define SRB_STATUS_INVALID_REQUEST          0x06
#define SRB_STATUS_INVALID_PATH_ID          0x07
#define SRB_STATUS_NO_DEVICE                0x08
#define SRB_STATUS_TIMEOUT                  0x09
#define SRB_STATUS_SELECTION_TIMEOUT        0x0A
#define SRB_STATUS_COMMAND_TIMEOUT          0x0B
#define SRB_STATUS_MESSAGE_REJECTED         0x0D
#define SRB_STATUS_BUS_RESET                0x0E
#define SRB_STATUS_PARITY_ERROR             0x0F
#define SRB_STATUS_REQUEST_SENSE_FAILED     0x10
#define SRB_STATUS_NO_HBA                   0x11
#define SRB_STATUS_DATA_OVERRUN             0x12
#define SRB_STATUS_UNEXPECTED_BUS_FREE      0x13
#define SRB_STATUS_PHASE_SEQUENCE_FAILURE   0x14
#define SRB_STATUS_BAD_SRB_BLOCK_LENGTH     0x15
#define SRB_STATUS_REQUEST_FLUSHED          0x16
#define SRB_STATUS_INVALID_LUN              0x20
#define SRB_STATUS_INVALID_TARGET_ID        0x21
#define SRB_STATUS_BAD_FUNCTION             0x22
#define SRB_STATUS_ERROR_RECOVERY           0x23
#define SRB_STATUS_QUEUE_FROZEN             0x40
#define SRB_STATUS_AUTOSENSE_VALID          0x80
#define SRB_STATUS( x ) \
    ((x) & ~(SRB_STATUS_AUTOSENSE_VALID | SRB_STATUS_QUEUE_FROZEN))

/* Maximum string length */
#define MAX_STRING  128

/* SCSI scanner commands */
#define SCSISCAN_CMD_CODE       0x0004
#define SCSISCAN_LOCKDEVICE     0x0005
#define SCSISCAN_UNLOCKDEVICE   0x0006
#define SCSISCAN_SET_TIMEOUT    0x0007
#define SCSISCAN_GET_INFO       0x0008

/* SCSI scanner device I/O control codes */
#define IOCTL_SCSISCAN_CMD \
    CTL_CODE( FILE_DEVICE_SCANNER, SCSISCAN_CMD_CODE, METHOD_OUT_DIRECT, \
    FILE_ANY_ACCESS )
#define IOCTL_SCSISCAN_LOCKDEVICE \
    CTL_CODE( FILE_DEVICE_SCANNER, SCSISCAN_LOCKDEVICE, METHOD_OUT_DIRECT, \
    FILE_ANY_ACCESS )
#define IOCTL_SCSISCAN_UNLOCKDEVICE \
    CTL_CODE( FILE_DEVICE_SCANNER, SCSISCAN_UNLOCKDEVICE, METHOD_OUT_DIRECT, \
    FILE_ANY_ACCESS )
#define IOCTL_SCSISCAN_SET_TIMEOUT \
    CTL_CODE( FILE_DEVICE_SCANNER, SCSISCAN_SET_TIMEOUT, METHOD_BUFFERED, \
    FILE_ANY_ACCESS )
#define IOCTL_SCSISCAN_GET_INFO \
    CTL_CODE( FILE_DEVICE_SCANNER, SCSISCAN_GET_INFO, METHOD_OUT_DIRECT, \
    FILE_ANY_ACCESS )

/* SCSI scanner command */
typedef struct _SCSISCAN_CMD {
    ULONG   Reserved1;
    ULONG   Size;
    ULONG   SrbFlags;
    UCHAR   CdbLength;
    UCHAR   SenseLength;
    UCHAR   Reserved2;
    UCHAR   Reserved3;
    ULONG   TransferLength;
    UCHAR   Cdb[16];
    PUCHAR  pSrbStatus;
    PUCHAR  pSenseBuffer;
} SCSISCAN_CMD;
typedef SCSISCAN_CMD    *PSCSISCAN_CMD;

/* SCSI scanner information */
typedef struct _SCSISCAN_INFO {
    ULONG   Size;
    ULONG   Flags;
    UCHAR   PortNumber;
    UCHAR   PathId;
    UCHAR   TargetId;
    UCHAR   Lun;
    UCHAR   AdapterName[MAX_STRING];
    ULONG   Reserved;
} SCSISCAN_INFO;
typedef SCSISCAN_INFO   *PSCSISCAN_INFO;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _SCSISCAN_H_ */
