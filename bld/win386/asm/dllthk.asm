;****************************************************************************
;***                                                                      ***
;*** DLLTHK.ASM - thunking layer to Windows 3.1 DLLs                      ***
;***              This set of functions makes sure that the proper dll    ***
;***              is loaded, and gets the real address of the function    ***
;***              to invoke, which is back-patched into the table.        ***
;***                                                                      ***
;*** By:  Craig Eisler                                                    ***
;***      December 1990-November 1992                                     ***
;*** By:  F.W.Crigger May 1993                                            ***
;***                                                                      ***
;****************************************************************************
extrn LOADLIBRARY : far
extrn FREELIBRARY : far
extrn GETPROCADDRESS : far
extrn DLLLoadFail_ : near
DGROUP group _DATA
_DATA segment word public 'DATA' use16

public DLLHandles
DLLHandles LABEL WORD
	dw    0
	dw    0
	dw    0
	dw    0
	dw    0
	dw    0
	dw    0
	dw    0
	dw    0
	dw    0
public DLLNames
DLLNames LABEL WORD
	dw    DGROUP:ddeml
	dw    DGROUP:commdlg
	dw    DGROUP:shell
	dw    DGROUP:penwin
	dw    DGROUP:ver
	dw    DGROUP:stress
	dw    DGROUP:lzexpand
	dw    DGROUP:mmsystem
	dw    DGROUP:toolhelp
	dw    DGROUP:odbc

ddeml	db    'ddeml.dll',0
commdlg	db    'commdlg.dll',0
shell	db    'shell.dll',0
penwin	db    'penwin.dll',0
ver	db    'ver.dll',0
stress	db    'stress.dll',0
lzexpand	db    'lzexpand.dll',0
mmsystem	db    'mmsystem.dll',0
toolhelp	db    'toolhelp.dll',0
odbc	db    'odbc.dll',0
_DATA ends


_TEXT segment word public 'CODE' use16
assume cs:_TEXT
assume ds:DGROUP
public __BackPatch_xxx
__BackPatch_xxx proc near
        push   di
        mov    dh,0
        mov    di,dx
        add    di,di
        mov    dx,ax
        mov    ax,DLLHandles[di]
        or     ax,ax
        jne    load1
          push dx
          mov  ax,DLLNames[di]
          push ds
          push ax
          call LOADLIBRARY
          mov  DLLHandles[di],ax
          pop  dx
load1:  cmp    ax,32
        jb     loadf1
        push   ax
        push   ds
        push   dx
        call   GETPROCADDRESS
        pop    di
        retf
loadf1: mov    ax,DLLNames[di]
        call   DLLLoadFail_
        sub    ax,ax
        sub    dx,dx
        pop    di
        retf
__BackPatch_xxx endp

public FiniDLLs_
FiniDLLs_ proc far
        push   si
        sub    si,si
next:   mov    ax,DLLHandles[si]
        cmp    ax,32
        jb     nextdll
        push   ax
        call   FREELIBRARY
nextdll:add    si,2
        cmp    si,20
        jne    next
        pop    si
        ret
FiniDLLs_ endp

public BackPatch_ddeml_
BackPatch_ddeml_ proc far
	mov    dl,0
	jmp    __BackPatch_xxx
BackPatch_ddeml_ endp

public BackPatch_commdlg_
BackPatch_commdlg_ proc far
	mov    dl,1
	jmp    __BackPatch_xxx
BackPatch_commdlg_ endp

public BackPatch_shell_
BackPatch_shell_ proc far
	mov    dl,2
	jmp    __BackPatch_xxx
BackPatch_shell_ endp

public BackPatch_penwin_
BackPatch_penwin_ proc far
	mov    dl,3
	jmp    __BackPatch_xxx
BackPatch_penwin_ endp

public BackPatch_ver_
BackPatch_ver_ proc far
	mov    dl,4
	jmp    __BackPatch_xxx
BackPatch_ver_ endp

public BackPatch_stress_
BackPatch_stress_ proc far
	mov    dl,5
	jmp    __BackPatch_xxx
BackPatch_stress_ endp

public BackPatch_lzexpand_
BackPatch_lzexpand_ proc far
	mov    dl,6
	jmp    __BackPatch_xxx
BackPatch_lzexpand_ endp

public BackPatch_mmsystem_
BackPatch_mmsystem_ proc far
	mov    dl,7
	jmp    __BackPatch_xxx
BackPatch_mmsystem_ endp

public BackPatch_toolhelp_
BackPatch_toolhelp_ proc far
	mov    dl,8
	jmp    __BackPatch_xxx
BackPatch_toolhelp_ endp

public BackPatch_odbc_
BackPatch_odbc_ proc far
	mov    dl,9
	jmp    __BackPatch_xxx
BackPatch_odbc_ endp

_TEXT ends
end
