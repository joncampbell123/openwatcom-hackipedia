/*
 *  videoagp.h  Video AGP definitions
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

#ifndef __VIDEOAGP_H__
#define __VIDEOAGP_H__

#ifndef _ENABLE_AUTODEPEND
 #pragma read_only_file;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Video AGP rates */
#define VIDEO_AGP_RATE_1X   1
#define VIDEO_AGP_RATE_2X   2
#define VIDEO_AGP_RATE_4X   4
#define VIDEO_AGP_RATE_8X   8

/* Video port cache types */
typedef enum {
    VpNonCached     = 0,
    VpWriteCombined = 1,
    VpCached        = 2
} VIDEO_PORT_CACHE_TYPE;

/* AGP callbacks */
typedef PHYSICAL_ADDRESS (STDAPICALLTYPE *PAGP_RESERVE_PHYSICAL)( PVOID, ULONG, VIDEO_PORT_CACHE_TYPE, PVOID * );
typedef VOID (STDAPICALLTYPE *PAGP_RELEASE_PHYSICAL)( PVOID, PVOID );
typedef BOOLEAN (STDAPICALLTYPE *PAGP_COMMIT_PHYSICAL)( PVOID, PVOID, ULONG, ULONG );
typedef VOID (STDAPICALLTYPE *PAGP_FREE_PHYSICAL)( PVOID, PVOID, ULONG, ULONG );
typedef PVOID (STDAPICALLTYPE *PAGP_RESERVE_VIRTUAL)( PVOID, HANDLE, PVOID, PVOID * );
typedef VOID (STDAPICALLTYPE *PAGP_RELEASE_VIRTUAL)( PVOID, PVOID );
typedef PVOID (STDAPICALLTYPE *PAGP_COMMIT_VIRTUAL)( PVOID, PVOID, ULONG, ULONG );
typedef VOID (STDAPICALLTYPE *PAGP_FREE_VIRTUAL)( PVOID, PVOID, ULONG, ULONG );
typedef BOOLEAN (STDAPICALLTYPE *PAGP_SET_RATE)( PVOID, ULONG );

/* Video port AGP services */
typedef struct _VIDEO_PORT_AGP_SERVICES {
    PAGP_RESERVE_PHYSICAL   AgpReservePhysical;
    PAGP_RELEASE_PHYSICAL   AgpReleasePhyiscal;
    PAGP_COMMIT_PHYSICAL    AgpCommitPhysical;
    PAGP_FREE_PHYSICAL      AgpFreePhyiscal;
    PAGP_RESERVE_VIRTUAL    AgpReserveVirtual;
    PAGP_RELEASE_VIRTUAL    AgpReleaseVirtual;
    PAGP_COMMIT_VIRTUAL     AgpCommitVirtual;
    PAGP_FREE_VIRTUAL       AgpFreeVirtual;
    ULONGLONG               AllocationLimit;
} VIDEO_PORT_AGP_SERVICES;
typedef VIDEO_PORT_AGP_SERVICES *PVIDEO_PORT_AGP_SERVICES;

/* Functions in VIDEOPRT.SYS */
BOOLEAN STDAPICALLTYPE  VideoPortGetAgpServices( PVOID, PVIDEO_PORT_AGP_SERVICES );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __VIDEOAGP_H__ */
