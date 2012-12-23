/*
 *  hidclass.h  Human Interface Device (HID) class definitions
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

#include <basetyps.h>

#ifndef __HIDCLASS_H__
#define __HIDCLASS_H__

#ifndef _ENABLE_AUTODEPEND
 #pragma read_only_file;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* GUID class input string */
#define GUID_CLASS_INPUT_STR    "4D1E55B2-F16F-11CF-88CB-001111000030"

/* HID revision number */
#define HID_REVISION    0x00000001L

/* Macros to make HID control codes */
#define HID_CTL_CODE( x ) \
    CTL_CODE( FILE_DEVICE_KEYBOARD, x, METHOD_NEITHER, FILE_ANY_ACCESS )
#define HID_BUFFER_CTL_CODE( x ) \
    CTL_CODE( FILE_DEVICE_KEYBOARD, x, METHOD_BUFFERED, FILE_ANY_ACCESS )
#define HID_IN_CTL_CODE( x ) \
    CTL_CODE( FILE_DEVICE_KEYBOARD, x, METHOD_IN_DIRECT, FILE_ANY_ACCESS )
#define HID_OUT_CTL_CODE( x ) \
    CTL_CODE( FILE_DEVICE_KEYBOARD, x, METHOD_OUT_DIRECT, FILE_ANY_ACCESS )

/* HID device I/O control codes */
#define IOCTL_HID_GET_DRIVER_CONFIG             HID_BUFFER_CTL_CODE( 100 )
#define IOCTL_HID_SET_DRIVER_CONFIG             HID_BUFFER_CTL_CODE( 101 )
#define IOCTL_HID_GET_POLL_FREQUENCY_MSEC       HID_BUFFER_CTL_CODE( 102 )
#define IOCTL_HID_SET_POLL_FREQUENCY_MSEC       HID_BUFFER_CTL_CODE( 103 )
#define IOCTL_GET_NUM_DEVICE_INPUT_BUFFERS      HID_BUFFER_CTL_CODE( 104 )
#define IOCTL_SET_NUM_DEVICE_INPUT_BUFFERS      HID_BUFFER_CTL_CODE( 105 )
#define IOCTL_HID_GET_COLLECTION_INFORMATION    HID_BUFFER_CTL_CODE( 106 )
#define IOCTL_HID_GET_COLLECTION_DESCRIPTOR     HID_CTL_CODE( 100 )
#define IOCTL_HID_FLUSH_QUEUE                   HID_CTL_CODE( 101 )
#define IOCTL_HID_SET_FEATURE                   HID_IN_CTL_CODE( 100 )
#if (NTDDI_VERSION >= 0x05010000)
    #define IOCTL_HID_SET_OUTPUT_REPORT         HID_IN_CTL_CODE( 101 )
#endif
#define IOCTL_HID_GET_FEATURE                   HID_OUT_CTL_CODE( 100 )
#define IOCTL_GET_PHYSICAL_DESCRIPTOR           HID_OUT_CTL_CODE( 102 )
#define IOCTL_HID_GET_HARDWARE_ID               HID_OUT_CTL_CODE( 103 )
#if (NTDDI_VERSION >= 0x05010000)
    #define IOCTL_HID_GET_INPUT_REPORT          HID_OUT_CTL_CODE( 104 )
#endif
#define IOCTL_HID_GET_MANUFACTURER_STRING       HID_OUT_CTL_CODE( 110 )
#define IOCTL_HID_GET_PRODUCT_STRING            HID_OUT_CTL_CODE( 111 )
#define IOCTL_HID_GET_SERIALNUMBER_STRING       HID_OUT_CTL_CODE( 112 )
#define IOCTL_HID_GET_INDEXED_STRING            HID_OUT_CTL_CODE( 120 )
#if (NTDDI_VERSION >= 0x05010000)
    #define IOCTL_HID_GET_MS_GENRE_DESCRIPTOR   HID_OUT_CTL_CODE( 121 )
    #define IOCTL_HID_ENABLE_SECURE_READ        HID_CTL_CODE( 130 )
    #define IOCTL_HID_DISABLE_SECURE_READ       HID_CTL_CODE( 131 )
#endif

/* HID transfer packet */
typedef struct _HID_XFER_PACKET {
    PUCHAR  reportBuffer;
    ULONG   reportBufferLen;
    UCHAR   reportId;
} HID_XFER_PACKET;
typedef HID_XFER_PACKET *PHID_XFER_PACKET;

/* Device object states */
#ifdef NT_INCLUDED
enum DeviceObjectState {
    DeviceObjectStarted = 0,
    DeviceObjectStopped = 1,
    DeviceObjectRemoved = 2
};
#endif

/* HID status change callback */
#ifdef NT_INCLUDED
typedef VOID (NTAPI *PHID_STATUS_CHANGE)( PVOID, enum DeviceObjectState );
#endif

/* HID Plug and Play notify interface */
#ifdef NT_INCLUDED
typedef struct _HID_INTERFACE_NOTIFY_PNP {
#ifndef __cplusplus
    INTERFACE;
#else
    INTERFACE           i;
#endif
    PHID_STATUS_CHANGE  StatusChangeFn;
    PVOID               CallbackContext;
} HID_INTERFACE_NOTIFY_PNP;
typedef HID_INTERFACE_NOTIFY_PNP    *PHID_INTERFACE_NOTIFY_PNP;
#endif

/* HID parse callback */
#ifdef NT_INCLUDED
#ifdef __HIDPI_H__
typedef NTSTATUS (__stdcall *PHIDP_GETCAPS)( PHIDP_PREPARSED_DATA, PHIDP_CAPS );
#endif
#endif

/* HID parse interface */
#ifdef NT_INCLUDED
#ifdef __HIDPI_H__
typedef struct _HID_INTERFACE_HIDPARSE {
#ifndef __cplusplus
    INTERFACE;
#else
    INTERFACE       i;
#endif
    PHIDP_GETCAPS   HidpGetCaps;
} HID_INTERFACE_HIDPARSE;
typedef HID_INTERFACE_HIDPARSE  *PHID_INTERFACE_HIDPARSE;
#endif
#endif

/* HID collection information */
typedef struct _HID_COLLECTION_INFORMATION {
    ULONG   DescriptorSize;
    BOOLEAN Polled;
    UCHAR   Reserved1[1];
    USHORT  VendorID;
    USHORT  ProductID;
    USHORT  VersionNumber;
} HID_COLLECTION_INFORMATION;
typedef HID_COLLECTION_INFORMATION  *PHID_COLLECTION_INFORMATION;

/* HID driver configuration */
typedef struct _HID_DRIVER_CONFIG {
    ULONG   Size;
    ULONG   RingBufferSize;
} HID_DRIVER_CONFIG;
typedef HID_DRIVER_CONFIG   *PHID_DRIVER_CONFIG;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __HIDCLASS_H__ */
