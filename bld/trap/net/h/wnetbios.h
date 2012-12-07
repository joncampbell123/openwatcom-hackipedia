/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  NetBIOS structures and definitions.
*
****************************************************************************/


enum commands {
    NET_CALL                    = 0x10,
    NET_LISTEN                  = 0x11,
    NET_HANG_UP                 = 0x12,
    NET_SEND                    = 0x14,
    NET_RECEIVE                 = 0x15,
    NET_RECEIVE_ANY             = 0x16,
    NET_CHAIN_SEND              = 0x17,
    NET_SEND_DATAGRAM           = 0x20,
    NET_RECEIVE_DATAGRAM        = 0x21,
    NET_SEND_BROAD_DATAGRAM     = 0x22,
    NET_RECEIVE_BROAD_DATAGRAM  = 0x23,
    NET_ADD_NAME                = 0x30,
    NET_DELETE_NAME             = 0x31,
    NET_RESET                   = 0x32,
    NET_ADAPTER_STATUS          = 0x33,
    NET_SESSION_STATUS          = 0x34,
    NET_CANCEL                  = 0x35,
    NET_ADD_GROUP_NAME          = 0x36,
    NET_ENUM                    = 0x37,
    NET_UNLINK                  = 0x70,
    NET_SEND_NOACK              = 0x71,
    NET_CHAIN_SEND_NOACK        = 0x72,
    NET_INVALID_CMD             = 0x7f,
    NET_NOWAIT                  = 0x80
};

enum nbrc {
    NET_RC_OK                   = 0x00,
    NET_RC_BUFLEN               = 0x01,
    NET_RC_ILLCMD               = 0x03,
    NET_RC_CMDTMO               = 0x05,
    NET_RC_INCOMP               = 0x06,
    NET_RC_BADDR                = 0x07,
    NET_RC_SNUMOUT              = 0x08,
    NET_RC_NORES                = 0x09,
    NET_RC_SCLOSED              = 0x0A,
    NET_RC_CMDCAN               = 0x0B,
    NET_RC_DUPNAME              = 0x0D,
    NET_RC_INUSE                = 0x16,
    NET_RC_SYSTEM               = 0x40,
    NET_RC_PENDING              = 0xFF
};

#define ILLEGAL_NAME_NUM    0
#define ILLEGAL_LSN         0
#define MAX_NAME_LEN        16
#define MAX_LANA            254

typedef struct {
    unsigned char           cmd;
    unsigned char           ret_code;
    unsigned char           lsn;
    unsigned char           name_num;
    void                    far *buff;
    unsigned short          length;
    char                    call_name[ MAX_NAME_LEN ];
    char                    name[ MAX_NAME_LEN ];
    unsigned char           r_timeout;
    unsigned char           s_timeout;
    void                    (far *post)();
    unsigned char           lana_num;
    volatile unsigned char  cmd_cmplt;
    char                    reserved[14];
} NCB;

typedef struct {
    unsigned char           length;
    unsigned char           lana[ MAX_LANA + 1 ];
} LANA_ENUM;
