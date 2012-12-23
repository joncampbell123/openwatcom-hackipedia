;****************************************************************************
;***                                                                      ***
;*** WINGLUE.ASM - windows glue functions                                 ***
;***               This set of functions encompasses all possible types   ***
;***               of calls.  Each API call has a little                  ***
;***               stub which generates the appropriate call into these   ***
;***               functions.                                             ***
;***                                                                      ***
;*** By:  Craig Eisler                                                    ***
;***      December 1990-November 1992                                     ***
;*** By:  F.W.Crigger May 1993                                            ***
;***                                                                      ***
;****************************************************************************
.386p

include winglue.inc
DGROUP group _DATA



;*
;*** 16-bit segment declarations
;*
_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
_DATA ends

_DATA segment use16
_FunctionTable LABEL DWORD
PUBLIC _FunctionTable
  dd AbortDoc
  dd AccessResource
  dd AddAtom
  dd AddFontResource
  dd AllocDStoCSAlias
  dd AllocResource
  dd AllocSelector
  dd AnsiLower
  dd AnsiLowerBuff
  dd __AnsiNext
  dd __AnsiPrev
  dd AnsiUpper
  dd AnsiUpperBuff
  dd AnyPopup
  dd __AppendMenu
  dd Arc
  dd ArrangeIconicWindows
  dd BeginDeferWindowPos
  dd BeginPaint
  dd BitBlt
  dd BringWindowToTop
  dd BuildCommDCB
  dd CallMsgFilter
  dd CallNextHookEx
  dd CallWindowProc
  dd Catch
  dd ChangeClipboardChain
  dd ChangeMenu
  dd CheckMenuItem
  dd ChildWindowFromPoint
  dd Chord
  dd ClearCommBreak
  dd CloseClipboard
  dd CloseComm
  dd CloseDriver
  dd CloseMetaFile
  dd CombineRgn
  dd ConvertOutlineFontFile
  dd CopyCursor
  dd CopyIcon
  dd CopyMetaFile
  dd CountClipboardFormats
  dd CountVoiceNotes
  dd __CreateBitmap
  dd __CreateBitmapIndirect
  dd CreateBrushIndirect
  dd CreateCompatibleBitmap
  dd CreateCompatibleDC
  dd CreateCursor
  dd CreateDC
  dd CreateDialog
  dd CreateDialogIndirect
  dd CreateDialogIndirectParam
  dd CreateDialogParam
  dd __CreateDIBitmap
  dd CreateDIBPatternBrush
  dd CreateDiscardableBitmap
  dd CreateEllipticRgn
  dd CreateEllipticRgnIndirect
  dd CreateFont
  dd CreateFontIndirect
  dd CreateHatchBrush
  dd CreateIC
  dd CreateIcon
  dd CreateMenu
  dd CreateMetaFile
  dd CreatePalette
  dd CreatePatternBrush
  dd CreatePen
  dd CreatePenIndirect
  dd CreatePolygonRgn
  dd CreatePolyPolygonRgn
  dd CreatePopupMenu
  dd CreateRectRgn
  dd CreateRectRgnIndirect
  dd CreateRoundRectRgn
  dd CreateScalableFontResource
  dd CreateSolidBrush
  dd CreateWindow
  dd CreateWindowEx
  dd DefDlgProc
  dd DefDriverProc
  dd DeferWindowPos
  dd DefFrameProc
  dd DefHookProc
  dd DefMDIChildProc
  dd DefWindowProc
  dd DeleteAtom
  dd DeleteDC
  dd DeleteMenu
  dd DeleteMetaFile
  dd DeleteObject
  dd DestroyCursor
  dd DestroyIcon
  dd DestroyMenu
  dd DestroyWindow
  dd DialogBox
  dd DialogBoxIndirect
  dd DialogBoxIndirectParam
  dd DialogBoxParam
  dd DispatchMessage
  dd DlgDirList
  dd DlgDirListComboBox
  dd DlgDirSelect
  dd DlgDirSelectComboBox
  dd DlgDirSelectComboBoxEx
  dd DlgDirSelectEx
  dd DPtoLP
  dd DrawIcon
  dd DrawText
  dd Ellipse
  dd EmptyClipboard
  dd EnableCommNotification
  dd EnableHardwareInput
  dd EnableMenuItem
  dd EnableScrollBar
  dd EnableWindow
  dd EndDeferWindowPos
  dd EndDoc
  dd EndPage
  dd EngineMakeFontDir
  dd EnumChildWindows
  dd EnumClipboardFormats
  dd EnumFontFamilies
  dd EnumFonts
  dd EnumMetaFile
  dd EnumObjects
  dd EnumProps
  dd EnumTaskWindows
  dd EnumWindows
  dd EqualRect
  dd EqualRgn
  dd __Escape
  dd EscapeCommFunction
  dd ExcludeClipRect
  dd ExcludeUpdateRgn
  dd ExitWindows
  dd ExitWindowsExec
  dd ExtFloodFill
  dd ExtTextOut
  dd FillRect
  dd FillRgn
  dd FindAtom
  dd FindResource
  dd FindWindow
  dd FlashWindow
  dd FloodFill
  dd FlushComm
  dd FrameRect
  dd FrameRgn
  dd FreeModule
  dd FreeResource
  dd FreeSelector
  dd GetActiveWindow
  dd GetAspectRatioFilter
  dd GetAspectRatioFilterEx
  dd GetAsyncKeyState
  dd GetAtomHandle
  dd GetAtomName
  dd __GetBitmapBits
  dd GetBitmapDimension
  dd GetBitmapDimensionEx
  dd GetBkColor
  dd GetBkMode
  dd GetBoundsRect
  dd GetBrushOrg
  dd GetBrushOrgEx
  dd GetCapture
  dd GetCaretBlinkTime
  dd GetCharABCWidths
  dd GetCharWidth
  dd GetClassInfo
  dd GetClassLong
  dd GetClassName
  dd GetClassWord
  dd GetClipboardData
  dd GetClipboardFormatName
  dd GetClipboardOwner
  dd GetClipboardViewer
  dd GetClipBox
  dd GetCodeHandle
  dd GetCommError
  dd GetCommEventMask
  dd GetCommState
  dd GetCurrentPDB
  dd GetCurrentPosition
  dd GetCurrentPositionEx
  dd GetCurrentTask
  dd GetCurrentTime
  dd GetCursor
  dd GetDC
  dd GetDCEx
  dd GetDCOrg
  dd GetDesktopWindow
  dd GetDeviceCaps
  dd GetDialogBaseUnits
  dd __GetDIBits
  dd GetDlgCtrlID
  dd GetDlgItem
  dd GetDlgItemInt
  dd GetDlgItemText
  dd GetDOSEnvironment
  dd GetDoubleClickTime
  dd GetDriverInfo
  dd GetDriverModuleHandle
  dd GetDriveType
  dd GetEnvironment
  dd GetFocus
  dd GetFontData
  dd GetFreeSpace
  dd GetFreeSystemResources
  dd GetGlyphOutline
  dd GetInputState
  dd __GetInstanceData
  dd GetKBCodePage
  dd GetKeyboardType
  dd GetKeyNameText
  dd GetKeyState
  dd GetLastActivePopup
  dd GetMapMode
  dd GetMenu
  dd GetMenuCheckMarkDimensions
  dd GetMenuItemCount
  dd GetMenuItemID
  dd GetMenuState
  dd GetMenuString
  dd __GetMessage
  dd GetMessageExtraInfo
  dd GetMessagePos
  dd GetMessageTime
  dd GetMetaFile
  dd GetMetaFileBits
  dd GetModuleFileName
  dd GetModuleHandle
  dd GetModuleUsage
  dd GetNearestColor
  dd GetNearestPaletteIndex
  dd GetNextDlgGroupItem
  dd GetNextDlgTabItem
  dd GetNextDriver
  dd GetNextWindow
  dd GetNumTasks
  dd GetObject
  dd GetOpenClipboardWindow
  dd GetOutlineTextMetrics
  dd GetPaletteEntries
  dd GetParent
  dd GetPixel
  dd GetPolyFillMode
  dd GetPriorityClipboardFormat
  dd GetPrivateProfileInt
  dd GetPrivateProfileString
  dd GetProcAddress
  dd GetProfileInt
  dd GetProfileString
  dd GetProp
  dd GetQueueStatus
  dd GetRasterizerCaps
  dd GetRgnBox
  dd GetROP2
  dd GetScrollPos
  dd GetSelectorBase
  dd GetSelectorLimit
  dd GetStockObject
  dd GetStretchBltMode
  dd GetSubMenu
  dd GetSysColor
  dd GetSysModalWindow
  dd GetSystemDebugState
  dd GetSystemDirectory
  dd GetSystemMenu
  dd GetSystemMetrics
  dd GetSystemPaletteEntries
  dd GetSystemPaletteUse
  dd GetTabbedTextExtent
  dd GetTempDrive
  dd GetTempFileName
  dd GetTextAlign
  dd GetTextCharacterExtra
  dd GetTextColor
  dd GetTextExtent
  dd GetTextExtentPoint
  dd GetTextFace
  dd GetTextMetrics
  dd GetThresholdEvent
  dd GetThresholdStatus
  dd GetTickCount
  dd GetTimerResolution
  dd GetTopWindow
  dd GetUpdateRect
  dd GetUpdateRgn
  dd GetVersion
  dd GetViewportExt
  dd GetViewportExtEx
  dd GetViewportOrg
  dd GetViewportOrgEx
  dd GetWindow
  dd GetWindowDC
  dd GetWindowExt
  dd GetWindowExtEx
  dd GetWindowLong
  dd GetWindowOrg
  dd GetWindowOrgEx
  dd GetWindowPlacement
  dd GetWindowsDirectory
  dd GetWindowTask
  dd GetWindowText
  dd GetWindowTextLength
  dd GetWindowWord
  dd GetWinFlags
  dd GlobalAddAtom
  dd GlobalAlloc
  dd GlobalCompact
  dd GlobalDeleteAtom
  dd GlobalDosAlloc
  dd GlobalDosFree
  dd GlobalFindAtom
  dd GlobalFlags
  dd GlobalFree
  dd GlobalGetAtomName
  dd GlobalHandle
  dd GlobalLock
  dd GlobalLRUNewest
  dd GlobalLRUOldest
  dd GlobalPageLock
  dd GlobalPageUnlock
  dd GlobalReAlloc
  dd GlobalSize
  dd GlobalUnfix
  dd GlobalUnlock
  dd GlobalUnWire
  dd GlobalWire
  dd GrayString
  dd HiliteMenuItem
  dd InitAtomTable
  dd InSendMessage
  dd __InsertMenu
  dd IntersectClipRect
  dd IntersectRect
  dd InvertRgn
  dd IsBadCodePtr
  dd IsBadHugeReadPtr
  dd IsBadHugeWritePtr
  dd IsBadReadPtr
  dd IsBadStringPtr
  dd IsBadWritePtr
  dd IsCharAlpha
  dd IsCharAlphaNumeric
  dd IsCharLower
  dd IsCharUpper
  dd IsChild
  dd IsClipboardFormatAvailable
  dd IsDBCSLeadByte
  dd IsDialogMessage
  dd IsDlgButtonChecked
  dd IsGDIObject
  dd IsIconic
  dd IsMenu
  dd IsRectEmpty
  dd IsTask
  dd IsWindow
  dd IsWindowEnabled
  dd IsWindowVisible
  dd IsZoomed
  dd KillTimer
  dd LineTo
  dd LoadAccelerators
  dd LoadBitmap
  dd LoadCursor
  dd LoadIcon
  dd LoadLibrary
  dd LoadMenu
  dd LoadMenuIndirect
  dd LoadModule
  dd LoadResource
  dd LoadString
  dd LocalAlloc
  dd LocalCompact
  dd LocalFlags
  dd LocalFree
  dd LocalHandle
  dd LocalInit
  dd LocalLock
  dd LocalReAlloc
  dd LocalShrink
  dd LocalSize
  dd LocalUnlock
  dd LockInput
  dd LockResource
  dd LockSegment
  dd LockWindowUpdate
  dd LPtoDP
  dd lstrcat
  dd lstrcmp
  dd lstrcmpi
  dd lstrcpy
  dd lstrlen
  dd MakeProcInstance
  dd MapVirtualKey
  dd MessageBox
  dd __ModifyMenu
  dd MoveTo
  dd MoveToEx
  dd MoveWindow
  dd MulDiv
  dd OemKeyScan
  dd OffsetClipRgn
  dd OffsetRgn
  dd OffsetViewportOrg
  dd OffsetViewportOrgEx
  dd OffsetWindowOrg
  dd OffsetWindowOrgEx
  dd OpenClipboard
  dd OpenComm
  dd OpenDriver
  dd OpenFile
  dd OpenIcon
  dd OpenSound
  dd PaintRgn
  dd PatBlt
  dd __PeekMessage
  dd Pie
  dd PlayMetaFile
  dd Polygon
  dd Polyline
  dd PolyPolygon
  dd PostAppMessage
  dd PostMessage
  dd ProfInsChk
  dd PtInRect
  dd PtInRegion
  dd PtVisible
  dd QueryAbort
  dd QueryJob
  dd QuerySendMessage
  dd ReadComm
  dd RealizePalette
  dd Rectangle
  dd RectInRegion
  dd RectVisible
  dd RedrawWindow
  dd RegisterClass
  dd RegisterClipboardFormat
  dd RegisterWindowMessage
  dd ReleaseDC
  dd RemoveFontResource
  dd RemoveMenu
  dd RemoveProp
  dd ResizePalette
  dd RestoreDC
  dd RoundRect
  dd SaveDC
  dd ScaleViewportExt
  dd ScaleViewportExtEx
  dd ScaleWindowExt
  dd ScaleWindowExtEx
  dd ScrollDC
  dd ScrollWindowEx
  dd SelectClipRgn
  dd SelectObject
  dd SelectPalette
  dd SendDlgItemMessage
  dd SendDriverMessage
  dd SendMessage
  dd SetAbortProc
  dd SetActiveWindow
  dd __SetBitmapBits
  dd SetBitmapDimension
  dd SetBitmapDimensionEx
  dd SetBkColor
  dd SetBkMode
  dd SetBoundsRect
  dd SetBrushOrg
  dd SetCapture
  dd SetClassLong
  dd SetClassWord
  dd SetClipboardData
  dd SetClipboardViewer
  dd SetCommBreak
  dd SetCommEventMask
  dd SetCommState
  dd SetCursor
  dd __SetDIBits
  dd __SetDIBitsToDevice
  dd SetEnvironment
  dd SetErrorMode
  dd SetFocus
  dd SetHandleCount
  dd SetMapMode
  dd SetMapperFlags
  dd SetMenu
  dd SetMenuItemBitmaps
  dd SetMessageQueue
  dd SetMetaFileBits
  dd SetMetaFileBitsBetter
  dd SetPaletteEntries
  dd SetParent
  dd SetPixel
  dd SetPolyFillMode
  dd SetProp
  dd SetResourceHandler
  dd SetROP2
  dd SetScrollPos
  dd SetSelectorBase
  dd SetSelectorLimit
  dd SetSoundNoise
  dd SetStretchBltMode
  dd SetSwapAreaSize
  dd SetSysModalWindow
  dd SetSystemPaletteUse
  dd SetTextAlign
  dd SetTextCharacterExtra
  dd SetTextColor
  dd SetTextJustification
  dd SetTimer
  dd SetViewportExt
  dd SetViewportExtEx
  dd SetViewportOrg
  dd SetViewportOrgEx
  dd SetVoiceAccent
  dd SetVoiceEnvelope
  dd SetVoiceNote
  dd SetVoiceQueueSize
  dd SetVoiceSound
  dd SetVoiceThreshold
  dd SetWindowExt
  dd SetWindowExtEx
  dd SetWindowLong
  dd SetWindowOrg
  dd SetWindowOrgEx
  dd SetWindowPlacement
  dd SetWindowPos
  dd SetWindowsHook
  dd SetWindowsHookEx
  dd SetWindowWord
  dd ShowCursor
  dd ShowWindow
  dd SizeofResource
  dd SpoolFile
  dd __StartDoc
  dd StartPage
  dd StartSound
  dd StopSound
  dd StretchBlt
  dd __StretchDIBits
  dd SubtractRect
  dd SwapMouseButton
  dd SyncAllVoices
  dd SystemParametersInfo
  dd TabbedTextOut
  dd TextOut
  dd ToAscii
  dd TrackPopupMenu
  dd TranslateAccelerator
  dd TranslateMDISysAccel
  dd TranslateMessage
  dd TransmitCommChar
  dd UngetCommChar
  dd UnhookWindowsHook
  dd UnhookWindowsHookEx
  dd UnionRect
  dd UnlockSegment
  dd UnrealizeObject
  dd UnregisterClass
  dd UpdateColors
  dd VkKeyScan
  dd WaitSoundState
  dd WindowFromPoint
  dd WinExec
  dd __WinHelp
  dd WNetAddConnection
  dd WNetCancelConnection
  dd WNetGetConnection
  dd WriteComm
  dd WritePrivateProfileString
  dd WriteProfileString
  dd wvsprintf
  dd _lclose
  dd _lcreat
  dd _llseek
  dd _lopen
  dd _lread
  dd _lwrite
  dd GetKerningPairs
  dd CreateBitmap
  dd CreateBitmapIndirect
  dd CreateDIBitmap
  dd Escape
  dd GetBitmapBits
  dd GetDIBits
  dd GetMessage
  dd PeekMessage
  dd SetBitmapBits
  dd SetDIBits
  dd SetDIBitsToDevice
  dd StartDoc
  dd StretchDIBits
  dd WinHelp
  dd __DPMIFreeHugeAlias
  dd __DPMIGetAlias
  dd __DPMIGetHugeAlias
  dd __DPMIAlloc
  dd __DPMIFree
  dd _clib_bios_disk
  dd _clib_bios_equiplist
  dd _clib_bios_keybrd
  dd _clib_bios_memsize
  dd _clib_bios_printer
  dd _clib_bios_serialcom
  dd _clib_bios_timeofday
  dd _clib_clock
  dd _clib_dos_findfirst
  dd _clib_dos_findnext
  dd _clib_errno
  dd _clib_int86
  dd _clib_int86x
  dd _clib_intdos
  dd _clib_intdosx
  dd __DLLPatch ; ddemlDdeAbandonTransaction
  dd __DLLPatch ; ddemlDdeAccessData
  dd __DLLPatch ; ddemlDdeAddData
  dd __DLLPatch ; ddemlDdeClientTransaction
  dd __DLLPatch ; ddemlDdeCmpStringHandles
  dd __DLLPatch ; ddemlDdeConnect
  dd __DLLPatch ; ddemlDdeConnectList
  dd __DLLPatch ; ddemlDdeCreateDataHandle
  dd __DLLPatch ; ddemlDdeCreateStringHandle
  dd __DLLPatch ; ddemlDdeDisconnect
  dd __DLLPatch ; ddemlDdeDisconnectList
  dd __DLLPatch ; ddemlDdeEnableCallback
  dd __DLLPatch ; ddemlDdeFreeDataHandle
  dd __DLLPatch ; ddemlDdeFreeStringHandle
  dd __DLLPatch ; ddemlDdeGetData
  dd __DLLPatch ; ddemlDdeGetLastError
  dd __DLLPatch ; ddemlDdeInitialize
  dd __DLLPatch ; ddemlDdeKeepStringHandle
  dd __DLLPatch ; ddemlDdeNameService
  dd __DLLPatch ; ddemlDdePostAdvise
  dd __DLLPatch ; ddemlDdeQueryConvInfo
  dd __DLLPatch ; ddemlDdeQueryNextServer
  dd __DLLPatch ; ddemlDdeQueryString
  dd __DLLPatch ; ddemlDdeReconnect
  dd __DLLPatch ; ddemlDdeSetUserHandle
  dd __DLLPatch ; ddemlDdeUnaccessData
  dd __DLLPatch ; ddemlDdeUninitialize
  dd __DLLPatch ; commdlgChooseColor
  dd __DLLPatch ; commdlgChooseFont
  dd __DLLPatch ; commdlgFindText
  dd __DLLPatch ; commdlgGetFileTitle
  dd __DLLPatch ; commdlgGetOpenFileName
  dd __DLLPatch ; commdlgGetSaveFileName
  dd __DLLPatch ; commdlgPrintDlg
  dd __DLLPatch ; commdlgReplaceText
  dd __DLLPatch ; commdlgCommDlgExtendedError
  dd __DLLPatch ; shellRegOpenKey
  dd __DLLPatch ; shellRegCreateKey
  dd __DLLPatch ; shellRegCloseKey
  dd __DLLPatch ; shellRegDeleteKey
  dd __DLLPatch ; shellRegSetValue
  dd __DLLPatch ; shellRegQueryValue
  dd __DLLPatch ; shellRegEnumKey
  dd __DLLPatch ; shellDragQueryFile
  dd __DLLPatch ; shellDragQueryPoint
  dd __DLLPatch ; shellExtractIcon
  dd __DLLPatch ; shellShellExecute
  dd __DLLPatch ; shellFindExecutable
  dd __DLLPatch ; penwinAddPointsPenData
  dd __DLLPatch ; penwinBeginEnumStrokes
  dd __DLLPatch ; penwinCharacterToSymbol
  dd __DLLPatch ; penwinCompactPenData
  dd __DLLPatch ; penwinCorrectWriting
  dd __DLLPatch ; penwinCreatePenData
  dd __DLLPatch ; penwinDictionarySearch
  dd __DLLPatch ; penwinDPtoTP
  dd __DLLPatch ; penwinDuplicatePenData
  dd __DLLPatch ; penwinEndPenCollection
  dd __DLLPatch ; penwinEnumSymbols
  dd __DLLPatch ; penwinExecuteGesture
  dd __DLLPatch ; penwinGetGlobalRC
  dd __DLLPatch ; penwinGetPenAsyncState
  dd __DLLPatch ; penwinGetPenDataInfo
  dd __DLLPatch ; penwinGetPenDataStroke
  dd __DLLPatch ; penwinGetPenHwData
  dd __DLLPatch ; penwinGetPenHwEventData
  dd __DLLPatch ; penwinGetPointsFromPenData
  dd __DLLPatch ; penwinGetSymbolCount
  dd __DLLPatch ; penwinGetSymbolMaxLength
  dd __DLLPatch ; penwinGetVersionPenWin
  dd __DLLPatch ; penwinInstallRecognizer
  dd __DLLPatch ; penwinIsPenAware
  dd __DLLPatch ; penwinIsPenEvent
  dd __DLLPatch ; penwinMetricScalePenData
  dd __DLLPatch ; penwinOffsetPenData
  dd __DLLPatch ; penwinProcessWriting
  dd __DLLPatch ; penwinRecognize
  dd __DLLPatch ; penwinRecognizeData
  dd __DLLPatch ; penwinRedisplayPenData
  dd __DLLPatch ; penwinResizePenData
  dd __DLLPatch ; penwinSetGlobalRC
  dd __DLLPatch ; penwinSetPenHook
  dd __DLLPatch ; penwinSetRecogHook
  dd __DLLPatch ; penwinShowKeyboard
  dd __DLLPatch ; penwinSymbolToCharacter
  dd __DLLPatch ; penwinTPtoDP
  dd __DLLPatch ; penwinTrainContext
  dd __DLLPatch ; penwinTrainInk
  dd __DLLPatch ; verGetFileResource
  dd __DLLPatch ; verGetFileResourceSize
  dd __DLLPatch ; verGetFileVersionInfo
  dd __DLLPatch ; verGetFileVersionInfoSize
  dd __DLLPatch ; verGetSystemDir
  dd __DLLPatch ; verGetWindowsDir
  dd __DLLPatch ; verVerFindFile
  dd __DLLPatch ; verVerInstallFile
  dd __DLLPatch ; verVerLanguageName
  dd __DLLPatch ; verVerQueryValue
  dd __DLLPatch ; stressAllocDiskSpace
  dd __DLLPatch ; stressAllocFileHandles
  dd __DLLPatch ; stressAllocGDIMem
  dd __DLLPatch ; stressAllocMem
  dd __DLLPatch ; stressAllocUserMem
  dd __DLLPatch ; stressGetFreeFileHandles
  dd __DLLPatch ; lzexpandCopyLZFile
  dd __DLLPatch ; lzexpandGetExpandedName
  dd __DLLPatch ; lzexpandLZCopy
  dd __DLLPatch ; lzexpandLZInit
  dd __DLLPatch ; lzexpandLZOpenFile
  dd __DLLPatch ; lzexpandLZRead
  dd __DLLPatch ; lzexpandLZSeek
  dd __DLLPatch ; lzexpandLZStart
  dd __DLLPatch ; mmsystemauxGetDevCaps
  dd __DLLPatch ; mmsystemauxGetNumDevs
  dd __DLLPatch ; mmsystemauxGetVolume
  dd __DLLPatch ; mmsystemauxOutMessage
  dd __DLLPatch ; mmsystemauxSetVolume
  dd __DLLPatch ; mmsystemjoyGetDevCaps
  dd __DLLPatch ; mmsystemjoyGetNumDevs
  dd __DLLPatch ; mmsystemjoyGetPos
  dd __DLLPatch ; mmsystemjoyGetThreshold
  dd __DLLPatch ; mmsystemjoyReleaseCapture
  dd __DLLPatch ; mmsystemjoySetCapture
  dd __DLLPatch ; mmsystemjoySetThreshold
  dd __DLLPatch ; mmsystemmciGetCreatorTask
  dd __DLLPatch ; mmsystemmciGetDeviceID
  dd __DLLPatch ; mmsystemmciGetDeviceIDFromElementID
  dd __DLLPatch ; mmsystemmciGetErrorString
  dd __DLLPatch ; mmsystemmciGetYieldProc
  dd __DLLPatch ; mmsystemmciSendCommand
  dd __DLLPatch ; mmsystemmciSendString
  dd __DLLPatch ; mmsystemmciSetYieldProc
  dd __DLLPatch ; mmsystemmidiInAddBuffer
  dd __DLLPatch ; mmsystemmidiInClose
  dd __DLLPatch ; mmsystemmidiInGetDevCaps
  dd __DLLPatch ; mmsystemmidiInGetErrorText
  dd __DLLPatch ; mmsystemmidiInGetID
  dd __DLLPatch ; mmsystemmidiInGetNumDevs
  dd __DLLPatch ; mmsystemmidiInMessage
  dd __DLLPatch ; mmsystemmidiInOpen
  dd __DLLPatch ; mmsystemmidiInPrepareHeader
  dd __DLLPatch ; mmsystemmidiInReset
  dd __DLLPatch ; mmsystemmidiInStart
  dd __DLLPatch ; mmsystemmidiInStop
  dd __DLLPatch ; mmsystemmidiInUnprepareHeader
  dd __DLLPatch ; mmsystemmidiOutCacheDrumPatches
  dd __DLLPatch ; mmsystemmidiOutCachePatches
  dd __DLLPatch ; mmsystemmidiOutClose
  dd __DLLPatch ; mmsystemmidiOutGetDevCaps
  dd __DLLPatch ; mmsystemmidiOutGetErrorText
  dd __DLLPatch ; mmsystemmidiOutGetID
  dd __DLLPatch ; mmsystemmidiOutGetNumDevs
  dd __DLLPatch ; mmsystemmidiOutGetVolume
  dd __DLLPatch ; mmsystemmidiOutLongMsg
  dd __DLLPatch ; mmsystemmidiOutMessage
  dd __DLLPatch ; mmsystemmidiOutOpen
  dd __DLLPatch ; mmsystemmidiOutPrepareHeader
  dd __DLLPatch ; mmsystemmidiOutReset
  dd __DLLPatch ; mmsystemmidiOutSetVolume
  dd __DLLPatch ; mmsystemmidiOutShortMsg
  dd __DLLPatch ; mmsystemmidiOutUnprepareHeader
  dd __DLLPatch ; mmsystemmmioAdvance
  dd __DLLPatch ; mmsystemmmioAscend
  dd __DLLPatch ; mmsystemmmioClose
  dd __DLLPatch ; mmsystemmmioCreateChunk
  dd __DLLPatch ; mmsystemmmioDescend
  dd __DLLPatch ; mmsystemmmioFlush
  dd __DLLPatch ; mmsystemmmioGetInfo
  dd __DLLPatch ; mmsystemmmioInstallIOProc
  dd __DLLPatch ; mmsystemmmioOpen
  dd __DLLPatch ; mmsystemmmioRead
  dd __DLLPatch ; mmsystemmmioRename
  dd __DLLPatch ; mmsystemmmioSeek
  dd __DLLPatch ; mmsystemmmioSendMessage
  dd __DLLPatch ; mmsystemmmioSetBuffer
  dd __DLLPatch ; mmsystemmmioSetInfo
  dd __DLLPatch ; mmsystemmmioStringToFOURCC
  dd __DLLPatch ; mmsystemmmioWrite
  dd __DLLPatch ; mmsystemmmsystemGetVersion
  dd __DLLPatch ; mmsystemsndPlaySound
  dd __DLLPatch ; mmsystemtimeBeginPeriod
  dd __DLLPatch ; mmsystemtimeEndPeriod
  dd __DLLPatch ; mmsystemtimeGetDevCaps
  dd __DLLPatch ; mmsystemtimeGetSystemTime
  dd __DLLPatch ; mmsystemtimeGetTime
  dd __DLLPatch ; mmsystemtimeKillEvent
  dd __DLLPatch ; mmsystemtimeSetEvent
  dd __DLLPatch ; mmsystemwaveInAddBuffer
  dd __DLLPatch ; mmsystemwaveInClose
  dd __DLLPatch ; mmsystemwaveInGetDevCaps
  dd __DLLPatch ; mmsystemwaveInGetErrorText
  dd __DLLPatch ; mmsystemwaveInGetID
  dd __DLLPatch ; mmsystemwaveInGetNumDevs
  dd __DLLPatch ; mmsystemwaveInGetPosition
  dd __DLLPatch ; mmsystemwaveInMessage
  dd __DLLPatch ; mmsystemwaveInOpen
  dd __DLLPatch ; mmsystemwaveInPrepareHeader
  dd __DLLPatch ; mmsystemwaveInReset
  dd __DLLPatch ; mmsystemwaveInStart
  dd __DLLPatch ; mmsystemwaveInStop
  dd __DLLPatch ; mmsystemwaveInUnprepareHeader
  dd __DLLPatch ; mmsystemwaveOutBreakLoop
  dd __DLLPatch ; mmsystemwaveOutClose
  dd __DLLPatch ; mmsystemwaveOutGetDevCaps
  dd __DLLPatch ; mmsystemwaveOutGetErrorText
  dd __DLLPatch ; mmsystemwaveOutGetID
  dd __DLLPatch ; mmsystemwaveOutGetNumDevs
  dd __DLLPatch ; mmsystemwaveOutGetPitch
  dd __DLLPatch ; mmsystemwaveOutGetPlaybackRate
  dd __DLLPatch ; mmsystemwaveOutGetPosition
  dd __DLLPatch ; mmsystemwaveOutGetVolume
  dd __DLLPatch ; mmsystemwaveOutMessage
  dd __DLLPatch ; mmsystemwaveOutOpen
  dd __DLLPatch ; mmsystemwaveOutPause
  dd __DLLPatch ; mmsystemwaveOutPrepareHeader
  dd __DLLPatch ; mmsystemwaveOutReset
  dd __DLLPatch ; mmsystemwaveOutRestart
  dd __DLLPatch ; mmsystemwaveOutSetPitch
  dd __DLLPatch ; mmsystemwaveOutSetPlaybackRate
  dd __DLLPatch ; mmsystemwaveOutSetVolume
  dd __DLLPatch ; mmsystemwaveOutUnprepareHeader
  dd __DLLPatch ; mmsystemwaveOutWrite
  dd __DLLPatch ; toolhelpClassFirst
  dd __DLLPatch ; toolhelpClassNext
  dd __DLLPatch ; toolhelpGlobalEntryHandle
  dd __DLLPatch ; toolhelpGlobalEntryModule
  dd __DLLPatch ; toolhelpGlobalFirst
  dd __DLLPatch ; toolhelpGlobalHandleToSel
  dd __DLLPatch ; toolhelpGlobalInfo
  dd __DLLPatch ; toolhelpGlobalNext
  dd __DLLPatch ; toolhelpLocalFirst
  dd __DLLPatch ; toolhelpLocalInfo
  dd __DLLPatch ; toolhelpLocalNext
  dd __DLLPatch ; toolhelpMemManInfo
  dd __MemoryRead
  dd __MemoryWrite
  dd __DLLPatch ; toolhelpModuleFindHandle
  dd __DLLPatch ; toolhelpModuleFindName
  dd __DLLPatch ; toolhelpModuleFirst
  dd __DLLPatch ; toolhelpModuleNext
  dd __DLLPatch ; toolhelpNotifyRegister
  dd __DLLPatch ; toolhelpNotifyUnRegister
  dd __DLLPatch ; toolhelpStackTraceCSIPFirst
  dd __DLLPatch ; toolhelpStackTraceFirst
  dd __DLLPatch ; toolhelpStackTraceNext
  dd __DLLPatch ; toolhelpSystemHeapInfo
  dd __DLLPatch ; toolhelpTaskFindHandle
  dd __DLLPatch ; toolhelpTaskFirst
  dd __DLLPatch ; toolhelpTaskGetCSIP
  dd __DLLPatch ; toolhelpTaskNext
  dd __DLLPatch ; toolhelpTaskSetCSIP
  dd __DLLPatch ; toolhelpTaskSwitch
  dd __DLLPatch ; toolhelpTimerCount
  dd __DLLPatch ; toolhelpMemoryRead
  dd __DLLPatch ; toolhelpMemoryWrite
  dd __DLLPatch ; odbcSQLAllocConnect
  dd __DLLPatch ; odbcSQLAllocEnv
  dd __DLLPatch ; odbcSQLAllocStmt
  dd __DLLPatch ; odbcSQLBindCol
  dd __DLLPatch ; odbcSQLBrowseConnect
  dd __DLLPatch ; odbcSQLCancel
  dd __DLLPatch ; odbcSQLColAttributes
  dd __DLLPatch ; odbcSQLColumnPrivileges
  dd __DLLPatch ; odbcSQLColumns
  dd __DLLPatch ; odbcSQLConnect
  dd __DLLPatch ; odbcSQLDataSources
  dd __DLLPatch ; odbcSQLDescribeCol
  dd __DLLPatch ; odbcSQLDescribeParam
  dd __DLLPatch ; odbcSQLDisconnect
  dd __DLLPatch ; odbcSQLDriverConnect
  dd __DLLPatch ; odbcSQLError
  dd __DLLPatch ; odbcSQLExecDirect
  dd __DLLPatch ; odbcSQLExecute
  dd __DLLPatch ; odbcSQLExtendedFetch
  dd __DLLPatch ; odbcSQLFetch
  dd __DLLPatch ; odbcSQLForeignKeys
  dd __DLLPatch ; odbcSQLFreeConnect
  dd __DLLPatch ; odbcSQLFreeEnv
  dd __DLLPatch ; odbcSQLFreeStmt
  dd __DLLPatch ; odbcSQLGetConnectOption
  dd __DLLPatch ; odbcSQLGetCursorName
  dd __DLLPatch ; odbcSQLGetData
  dd __DLLPatch ; odbcSQLGetFunctions
  dd __DLLPatch ; odbcSQLGetInfo
  dd __DLLPatch ; odbcSQLGetStmtOption
  dd __DLLPatch ; odbcSQLGetTypeInfo
  dd __DLLPatch ; odbcSQLMoreResults
  dd __DLLPatch ; odbcSQLNativeSql
  dd __DLLPatch ; odbcSQLNumParams
  dd __DLLPatch ; odbcSQLNumResultCols
  dd __DLLPatch ; odbcSQLParamData
  dd __DLLPatch ; odbcSQLParamOptions
  dd __DLLPatch ; odbcSQLPrepare
  dd __DLLPatch ; odbcSQLPrimaryKeys
  dd __DLLPatch ; odbcSQLProcedureColumns
  dd __DLLPatch ; odbcSQLProcedures
  dd __DLLPatch ; odbcSQLPutData
  dd __DLLPatch ; odbcSQLRowCount
  dd __DLLPatch ; odbcSQLSetConnectOption
  dd __DLLPatch ; odbcSQLSetCursorName
  dd __DLLPatch ; odbcSQLSetParam
  dd __DLLPatch ; odbcSQLSetPos
  dd __DLLPatch ; odbcSQLSetScrollOptions
  dd __DLLPatch ; odbcSQLSetStmtOption
  dd __DLLPatch ; odbcSQLSpecialColumns
  dd __DLLPatch ; odbcSQLStatistics
  dd __DLLPatch ; odbcSQLTablePrivileges
  dd __DLLPatch ; odbcSQLTables
  dd __DLLPatch ; odbcSQLTransact
  dd ResetDC
  dd AdjustWindowRect
  dd AdjustWindowRectEx
  dd AnimatePalette
  dd AnsiToOem
  dd AnsiToOemBuff
  dd CheckDlgButton
  dd CheckRadioButton
  dd ClientToScreen
  dd ClipCursor
  dd CloseSound
  dd CloseWindow
  dd CopyRect
  dd CreateCaret
  dd DebugBreak
  dd DestroyCaret
  dd DirectedYield
  dd DrawFocusRect
  dd DrawMenuBar
  dd EndDialog
  dd EndPaint
  dd FatalAppExit
  dd FatalExit
  dd FreeLibrary
  dd GetCaretPos
  dd GetClientRect
  dd GetClipCursor
  dd GetCodeInfo
  dd GetCursorPos
  dd GetKeyboardState
  dd GetScrollRange
  dd GetWindowRect
  dd GlobalFix
  dd GlobalNotify
  dd HideCaret
  dd InflateRect
  dd InvalidateRect
  dd InvalidateRgn
  dd InvertRect
  dd LimitEmsPages
  dd LineDDA
  dd LogError
  dd LogParamError
  dd MapDialogRect
  dd MapWindowPoints
  dd MessageBeep
  dd OemToAnsi
  dd OemToAnsiBuff
  dd OffsetRect
  dd OutputDebugString
  dd PlayMetaFileRecord
  dd PostQuitMessage
  dd ProfClear
  dd ProfFinish
  dd ProfFlush
  dd ProfSampRate
  dd ProfSetup
  dd ProfStart
  dd ProfStop
  dd ReleaseCapture
  dd ReplyMessage
  dd ScreenToClient
  dd ScrollWindow
  dd SetCaretBlinkTime
  dd SetCaretPos
  dd SetCursorPos
  dd SetDlgItemInt
  dd SetDlgItemText
  dd SetDoubleClickTime
  dd SetKeyboardState
  dd SetRect
  dd SetRectEmpty
  dd SetRectRgn
  dd SetScrollRange
  dd SetSysColors
  dd SetWindowText
  dd ShowCaret
  dd ShowOwnedPopups
  dd ShowScrollBar
  dd SwapRecording
  dd SwitchStackBack
  dd SwitchStackTo
  dd Throw
  dd UpdateWindow
  dd ValidateCodeSegments
  dd ValidateFreeSpaces
  dd ValidateRect
  dd ValidateRgn
  dd WaitMessage
  dd Yield
  dd FreeProcInstance
  dd __DPMIFreeAlias
  dd _clib_delay
  dd _clib_intr
  dd __DLLPatch ; shellDragFinish
  dd __DLLPatch ; shellDragAcceptFiles
  dd __DLLPatch ; penwinAtomicVirtualEvent
  dd __DLLPatch ; penwinBoundingRectFromPoints
  dd __DLLPatch ; penwinDrawPenData
  dd __DLLPatch ; penwinEmulatePen
  dd __DLLPatch ; penwinFirstSymbolFromGraph
  dd __DLLPatch ; penwinInitRC
  dd __DLLPatch ; penwinPenPacket
  dd __DLLPatch ; penwinPostVirtualKeyEvent
  dd __DLLPatch ; penwinPostVirtualMouseEvent
  dd __DLLPatch ; penwinRegisterPenApp
  dd __DLLPatch ; penwinUninstallRecognizer
  dd __DLLPatch ; penwinUpdatePenInfo
  dd __DLLPatch ; stressFreeAllGDIMem
  dd __DLLPatch ; stressFreeAllMem
  dd __DLLPatch ; stressFreeAllUserMem
  dd __DLLPatch ; stressUnAllocDiskSpace
  dd __DLLPatch ; stressUnAllocFileHandles
  dd __DLLPatch ; lzexpandLZClose
  dd __DLLPatch ; lzexpandLZDone
  dd __DLLPatch ; toolhelpTerminateApp
_DATA ends

_TEXT segment use16
assume cs:_TEXT
assume ds:dgroup
__ThunkTable LABEL WORD
public __ThunkTable
  dw  __Thunk0
  dw  __Thunk1
  dw  __Thunk2
  dw  __Thunk3
  dw  __Thunk4
  dw  __Thunk5
  dw  __Thunk6
  dw  __Thunk7
  dw  __Thunk8
  dw  __Thunk9
  dw  __Thunk10
  dw  __Thunk11
  dw  __Thunk12
  dw  __Thunk13
  dw  __Thunk14
  dw  __Thunk15
  dw  __Thunk16
  dw  __Thunk17
  dw  __Thunk18
  dw  __Thunk19
  dw  __Thunk20
  dw  __Thunk21
  dw  __Thunk22
  dw  __Thunk23
  dw  __Thunk24
  dw  __Thunk25
  dw  __Thunk26
  dw  __Thunk27
  dw  __Thunk28
  dw  __Thunk29
  dw  __Thunk30
  dw  __Thunk31
  dw  __Thunk32
  dw  __Thunk33
  dw  __Thunk34
  dw  __Thunk35
  dw  __Thunk36
  dw  __Thunk37
  dw  __Thunk38
  dw  __Thunk39
  dw  __Thunk40
  dw  __Thunk41
  dw  __Thunk42
  dw  __Thunk43
  dw  __Thunk44
  dw  __Thunk45
  dw  __Thunk46
  dw  __Thunk47
  dw  __Thunk48
  dw  __Thunk49
  dw  __Thunk50
  dw  __Thunk51
  dw  __Thunk52
  dw  __Thunk53
  dw  __Thunk54
  dw  __Thunk55
  dw  __Thunk56
  dw  __Thunk57
  dw  __Thunk58
  dw  __Thunk59
  dw  __Thunk60
  dw  __Thunk61
  dw  __Thunk62
  dw  __Thunk63
  dw  __Thunk64
  dw  __Thunk65
  dw  __Thunk66
  dw  __Thunk67
  dw  __Thunk68
  dw  __Thunk69
  dw  __Thunk70
  dw  __Thunk71
  dw  __Thunk72
  dw  __Thunk73
  dw  __Thunk74
  dw  __Thunk75
  dw  __Thunk76
  dw  __Thunk77
  dw  __Thunk78
  dw  __Thunk79
  dw  __Thunk80
  dw  __Thunk81
  dw  __Thunk82
  dw  __Thunk83
  dw  __Thunk84
  dw  __Thunk85
  dw  __Thunk86
  dw  __Thunk87
  dw  __Thunk88
  dw  __Thunk89
  dw  __Thunk90
  dw  __Thunk91
  dw  __Thunk92
  dw  __Thunk93
  dw  __Thunk94
  dw  __Thunk95
  dw  __Thunk96
  dw  __Thunk97
  dw  __Thunk98
  dw  __Thunk99
  dw  __Thunk100
  dw  __Thunk101
  dw  __Thunk102
  dw  __Thunk103
  dw  __Thunk104
  dw  __Thunk105
  dw  __Thunk106
  dw  __Thunk107
  dw  __Thunk108
  dw  __Thunk109
  dw  __Thunk110
  dw  __Thunk111
  dw  __Thunk112
  dw  __Thunk113
  dw  __Thunk114
  dw  __Thunk115
  dw  __Thunk116
  dw  __Thunk117
  dw  __Thunk118
  dw  __Thunk119
  dw  __Thunk120
  dw  __Thunk121
  dw  __Thunk122
  dw  __Thunk123
  dw  __Thunk124
  dw  __Thunk125
  dw  __Thunk126
  dw  __Thunk127
  dw  __Thunk128
  dw  __Thunk129
  dw  __Thunk130
  dw  __Thunk131
  dw  __Thunk132
  dw  __Thunk133
  dw  __Thunk134
  dw  __Thunk135
  dw  __Thunk136
  dw  __Thunk137
  dw  __Thunk138
  dw  __Thunk139
  dw  __Thunk140
  dw  __Thunk141
  dw  __Thunk142
  dw  __Thunk143
  dw  __Thunk144
  dw  __Thunk145
  dw  __Thunk146
  dw  __Thunk147
  dw  __Thunk148
  dw  __Thunk149
  dw  __Thunk150
  dw  __Thunk151
  dw  __Thunk152
  dw  __Thunk153
  dw  __Thunk154
  dw  __Thunk155
  dw  __Thunk156
  dw  __Thunk157
  dw  __Thunk158
  dw  __Thunk159
  dw  __Thunk160
  dw  __Thunk161
  dw  __Thunk162
  dw  __Thunk163
  dw  __Thunk164
  dw  __Thunk165
  dw  __Thunk166
  dw  __Thunk167
  dw  __Thunk168
  dw  __Thunk169
  dw  __Thunk170
  dw  __Thunk171
  dw  __Thunk172
  dw  __Thunk173
  dw  __Thunk174
  dw  __Thunk175
  dw  __Thunk176
  dw  __Thunk177
  dw  __Thunk178
  dw  __Thunk179
  dw  __Thunk180
  dw  __Thunk181
  dw  __Thunk182
  dw  __Thunk183
  dw  __Thunk184
  dw  __Thunk185
  dw  __Thunk186
  dw  __Thunk187
  dw  __Thunk188
  dw  __Thunk189
  dw  __Thunk190
  dw  __Thunk191
  dw  __Thunk192
  dw  __Thunk193
  dw  __Thunk194

PUBLIC  __Thunk0
__Thunk0 proc near
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk0 endp

PUBLIC  __Thunk1
__Thunk1 proc near
	push	dx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk1 endp

PUBLIC  __Thunk2
__Thunk2 proc near
	mov	eax,es:[edi+16]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	jmp	Free16Alias
__Thunk2 endp

PUBLIC  __Thunk3
__Thunk3 proc near
	push	cx
	push	dx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk3 endp

PUBLIC  __Thunk4
__Thunk4 proc near
	mov	eax,es:[edi+20]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	word ptr es:[edi+16]	; Parm2
	jmp	Free16Alias
__Thunk4 endp

PUBLIC  __Thunk5
__Thunk5 proc near
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk5 endp

PUBLIC  __Thunk6
__Thunk6 proc near
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk6 endp

PUBLIC  __Thunk7
__Thunk7 proc near
	call	dword ptr ds:[bx]
	ret
__Thunk7 endp

PUBLIC  __Thunk8
__Thunk8 proc near
	push	si
	push	cx
	push	dx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk8 endp

PUBLIC  __Thunk9
__Thunk9 proc near
	push	word ptr es:[edi+48]	; Parm1
	push	word ptr es:[edi+44]	; Parm2
	push	word ptr es:[edi+40]	; Parm3
	push	word ptr es:[edi+36]	; Parm4
	push	word ptr es:[edi+32]	; Parm5
	push	word ptr es:[edi+28]	; Parm6
	push	word ptr es:[edi+24]	; Parm7
	push	word ptr es:[edi+20]	; Parm8
	push	word ptr es:[edi+16]	; Parm9
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk9 endp

PUBLIC  __Thunk10
__Thunk10 proc near
	mov	eax,es:[edi+16]		; Parm2
	call	GetFirst16Alias
	push	word ptr es:[edi+20]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	jmp	Free16Alias
__Thunk10 endp

PUBLIC  __Thunk11
__Thunk11 proc near
	push	word ptr es:[edi+48]	; Parm1
	push	word ptr es:[edi+44]	; Parm2
	push	word ptr es:[edi+40]	; Parm3
	push	word ptr es:[edi+36]	; Parm4
	push	word ptr es:[edi+32]	; Parm5
	push	word ptr es:[edi+28]	; Parm6
	push	word ptr es:[edi+24]	; Parm7
	push	word ptr es:[edi+20]	; Parm8
	push	dword ptr es:[edi+16]	; Parm9
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk11 endp

PUBLIC  __Thunk12
__Thunk12 proc near
	mov	eax,es:[edi+16]		; Parm2
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-16]	; Parm1Alias
	push	dword ptr [bp-8]	; Parm2Alias
	jmp	Free16Alias
__Thunk12 endp

PUBLIC  __Thunk13
__Thunk13 proc near
	push	esi
	push	cx
	push	dx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk13 endp

PUBLIC  __Thunk14
__Thunk14 proc near
	push	edi
	push	si
	push	cx
	push	dx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk14 endp

PUBLIC  __Thunk15
__Thunk15 proc near
	mov	eax,es:[edi+24]		; Parm3
	call	GetFirst16Alias
	push	word ptr es:[edi+32]	; Parm1
	push	word ptr es:[edi+28]	; Parm2
	push	dword ptr [bp-8]	; Parm3Alias
	push	word ptr es:[edi+20]	; Parm4
	push	word ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk15 endp

PUBLIC  __Thunk16
__Thunk16 proc near
	push	cx
	push	dx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk16 endp

PUBLIC  __Thunk17
__Thunk17 proc near
	push	dx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk17 endp

PUBLIC  __Thunk18
__Thunk18 proc near
	push	cx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk18 endp

PUBLIC  __Thunk19
__Thunk19 proc near
	push	si
	push	cx
	push	dx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk19 endp

PUBLIC  __Thunk20
__Thunk20 proc near
	mov	eax,es:[edi+16]		; Parm3
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+24]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-24]	; Parm1Alias
	push	dword ptr [bp-16]	; Parm2Alias
	push	dword ptr [bp-8]	; Parm3Alias
	jmp	Free16Alias
__Thunk20 endp

PUBLIC  __Thunk21
__Thunk21 proc near
	push	di
	push	si
	push	cx
	push	dx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk21 endp

PUBLIC  __Thunk22
__Thunk22 proc near
	mov	eax,es:[edi+16]		; Parm7
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm6
	call	Get16Alias
	push	word ptr es:[edi+40]	; Parm1
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	word ptr es:[edi+28]	; Parm4
	push	word ptr es:[edi+24]	; Parm5
	push	dword ptr [bp-16]	; Parm6Alias
	push	dword ptr [bp-8]	; Parm7Alias
	jmp	Free16Alias
__Thunk22 endp

PUBLIC  __Thunk23
__Thunk23 proc near
	mov	eax,es:[edi+16]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+24]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-32]	; Parm1Alias
	push	dword ptr [bp-24]	; Parm2Alias
	push	dword ptr [bp-16]	; Parm3Alias
	push	dword ptr [bp-8]	; Parm4Alias
	jmp	Free16Alias
__Thunk23 endp

PUBLIC  __Thunk24
__Thunk24 proc near
	mov	eax,es:[edi+24]		; Parm2
	call	GetFirst16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	word ptr es:[edi+20]	; Parm3
	push	dword ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk24 endp

PUBLIC  __Thunk25
__Thunk25 proc near
	mov	eax,es:[edi+28]		; Parm2
	call	GetFirst16Alias
	push	word ptr es:[edi+32]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	word ptr es:[edi+24]	; Parm3
	push	dword ptr es:[edi+20]	; Parm4
	push	dword ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk25 endp

PUBLIC  __Thunk26
__Thunk26 proc near
	mov	eax,es:[edi+20]		; Parm5
	call	GetFirst16Alias
	mov	eax,es:[edi+32]		; Parm2
	call	Get16Alias
	push	word ptr es:[edi+36]	; Parm1
	push	dword ptr [bp-16]	; Parm2Alias
	push	dword ptr es:[edi+28]	; Parm3
	push	dword ptr es:[edi+24]	; Parm4
	push	dword ptr [bp-8]	; Parm5Alias
	push	word ptr es:[edi+16]	; Parm6
	jmp	Free16Alias
__Thunk26 endp

PUBLIC  __Thunk27
__Thunk27 proc near
	mov	eax,es:[edi+16]		; Parm14
	call	GetFirst16Alias
	push	word ptr es:[edi+68]	; Parm1
	push	word ptr es:[edi+64]	; Parm2
	push	word ptr es:[edi+60]	; Parm3
	push	word ptr es:[edi+56]	; Parm4
	push	word ptr es:[edi+52]	; Parm5
	push	word ptr es:[edi+48]	; Parm6
	push	word ptr es:[edi+44]	; Parm7
	push	word ptr es:[edi+40]	; Parm8
	push	word ptr es:[edi+36]	; Parm9
	push	word ptr es:[edi+32]	; Parm10
	push	word ptr es:[edi+28]	; Parm11
	push	word ptr es:[edi+24]	; Parm12
	push	word ptr es:[edi+20]	; Parm13
	push	dword ptr [bp-8]	; Parm14Alias
	jmp	Free16Alias
__Thunk27 endp

PUBLIC  __Thunk28
__Thunk28 proc near
	mov	eax,es:[edi+24]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	word ptr es:[edi+20]	; Parm2
	push	word ptr es:[edi+16]	; Parm3
	jmp	Free16Alias
__Thunk28 endp

PUBLIC  __Thunk29
__Thunk29 proc near
	mov	eax,es:[edi+24]		; Parm2
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-16]	; Parm1Alias
	push	dword ptr [bp-8]	; Parm2Alias
	push	word ptr es:[edi+20]	; Parm3
	push	word ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk29 endp

PUBLIC  __Thunk30
__Thunk30 proc near
	push	word ptr es:[edi+36]	; Parm1
	push	word ptr es:[edi+32]	; Parm2
	push	word ptr es:[edi+28]	; Parm3
	push	word ptr es:[edi+24]	; Parm4
	push	word ptr es:[edi+20]	; Parm5
	push	word ptr es:[edi+16]	; Parm6
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk30 endp

PUBLIC  __Thunk31
__Thunk31 proc near
	mov	eax,es:[edi+16]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+24]		; Parm2
	call	Get16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	dword ptr [bp-24]	; Parm2Alias
	push	dword ptr [bp-16]	; Parm3Alias
	push	dword ptr [bp-8]	; Parm4Alias
	jmp	Free16Alias
__Thunk31 endp

PUBLIC  __Thunk32
__Thunk32 proc near
	mov	eax,es:[edi+16]		; Parm11
	call	GetFirst16Alias
	mov	eax,es:[edi+52]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+56]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-24]	; Parm1Alias
	push	dword ptr [bp-16]	; Parm2Alias
	push	dword ptr es:[edi+48]	; Parm3
	push	word ptr es:[edi+44]	; Parm4
	push	word ptr es:[edi+40]	; Parm5
	push	word ptr es:[edi+36]	; Parm6
	push	word ptr es:[edi+32]	; Parm7
	push	word ptr es:[edi+28]	; Parm8
	push	word ptr es:[edi+24]	; Parm9
	push	word ptr es:[edi+20]	; Parm10
	push	dword ptr [bp-8]	; Parm11Alias
	jmp	Free16Alias
__Thunk32 endp

PUBLIC  __Thunk33
__Thunk33 proc near
	mov	eax,es:[edi+16]		; Parm12
	call	GetFirst16Alias
	mov	eax,es:[edi+52]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+56]		; Parm2
	call	Get16Alias
	push	dword ptr es:[edi+60]	; Parm1
	push	dword ptr [bp-24]	; Parm2Alias
	push	dword ptr [bp-16]	; Parm3Alias
	push	dword ptr es:[edi+48]	; Parm4
	push	word ptr es:[edi+44]	; Parm5
	push	word ptr es:[edi+40]	; Parm6
	push	word ptr es:[edi+36]	; Parm7
	push	word ptr es:[edi+32]	; Parm8
	push	word ptr es:[edi+28]	; Parm9
	push	word ptr es:[edi+24]	; Parm10
	push	word ptr es:[edi+20]	; Parm11
	push	dword ptr [bp-8]	; Parm12Alias
	jmp	Free16Alias
__Thunk33 endp

PUBLIC  __Thunk34
__Thunk34 proc near
	push	edi
	push	si
	push	cx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk34 endp

PUBLIC  __Thunk35
__Thunk35 proc near
	push	word ptr es:[edi+44]	; Parm1
	push	word ptr es:[edi+40]	; Parm2
	push	word ptr es:[edi+36]	; Parm3
	push	word ptr es:[edi+32]	; Parm4
	push	word ptr es:[edi+28]	; Parm5
	push	word ptr es:[edi+24]	; Parm6
	push	word ptr es:[edi+20]	; Parm7
	push	word ptr es:[edi+16]	; Parm8
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk35 endp

PUBLIC  __Thunk36
__Thunk36 proc near
	mov	eax,es:[edi+16]		; Parm4
	call	GetFirst16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	word ptr es:[edi+24]	; Parm2
	push	dword ptr es:[edi+20]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	jmp	Free16Alias
__Thunk36 endp

PUBLIC  __Thunk37
__Thunk37 proc near
	push	di
	push	si
	push	cx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk37 endp

PUBLIC  __Thunk38
__Thunk38 proc near
	mov	eax,es:[edi+28]		; Parm2
	call	GetFirst16Alias
	push	word ptr es:[edi+32]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	word ptr es:[edi+24]	; Parm3
	push	word ptr es:[edi+20]	; Parm4
	push	word ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk38 endp

PUBLIC  __Thunk39
__Thunk39 proc near
	mov	eax,es:[edi+20]		; Parm2
	call	GetFirst16Alias
	push	word ptr es:[edi+24]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	word ptr es:[edi+16]	; Parm3
	jmp	Free16Alias
__Thunk39 endp

PUBLIC  __Thunk40
__Thunk40 proc near
	mov	eax,es:[edi+24]		; Parm2
	call	GetFirst16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	word ptr es:[edi+20]	; Parm3
	push	word ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk40 endp

PUBLIC  __Thunk41
__Thunk41 proc near
	mov	eax,es:[edi+20]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm2
	call	Get16Alias
	push	word ptr es:[edi+32]	; Parm1
	push	dword ptr [bp-16]	; Parm2Alias
	push	word ptr es:[edi+24]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	push	word ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk41 endp

PUBLIC  __Thunk42
__Thunk42 proc near
	push	di
	push	si
	push	cx
	push	dx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk42 endp

PUBLIC  __Thunk43
__Thunk43 proc near
	mov	eax,es:[edi+16]		; Parm3
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm2
	call	Get16Alias
	push	word ptr es:[edi+24]	; Parm1
	push	dword ptr [bp-16]	; Parm2Alias
	push	dword ptr [bp-8]	; Parm3Alias
	jmp	Free16Alias
__Thunk43 endp

PUBLIC  __Thunk44
__Thunk44 proc near
	mov	eax,es:[edi+24]		; Parm2
	call	GetFirst16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	dword ptr es:[edi+20]	; Parm3
	push	dword ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk44 endp

PUBLIC  __Thunk45
__Thunk45 proc near
	push	si
	push	cx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk45 endp

PUBLIC  __Thunk46
__Thunk46 proc near
	mov	eax,es:[edi+16]		; Parm5
	call	GetFirst16Alias
	push	word ptr es:[edi+32]	; Parm1
	push	word ptr es:[edi+28]	; Parm2
	push	word ptr es:[edi+24]	; Parm3
	push	dword ptr es:[edi+20]	; Parm4
	push	dword ptr [bp-8]	; Parm5Alias
	jmp	Free16Alias
__Thunk46 endp

PUBLIC  __Thunk47
__Thunk47 proc near
	push	edx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk47 endp

PUBLIC  __Thunk48
__Thunk48 proc near
	push	di
	push	si
	push	cx
	push	edx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk48 endp

PUBLIC  __Thunk49
__Thunk49 proc near
	mov	eax,es:[edi+16]		; Parm8
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm6
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm5
	call	Get16Alias
	push	word ptr es:[edi+44]	; Parm1
	push	word ptr es:[edi+40]	; Parm2
	push	word ptr es:[edi+36]	; Parm3
	push	word ptr es:[edi+32]	; Parm4
	push	dword ptr [bp-24]	; Parm5Alias
	push	dword ptr [bp-16]	; Parm6Alias
	push	word ptr es:[edi+20]	; Parm7
	push	dword ptr [bp-8]	; Parm8Alias
	jmp	Free16Alias
__Thunk49 endp

PUBLIC  __Thunk50
__Thunk50 proc near
	mov	eax,es:[edi+16]		; Parm4
	call	GetFirst16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	word ptr es:[edi+24]	; Parm2
	push	word ptr es:[edi+20]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	jmp	Free16Alias
__Thunk50 endp

PUBLIC  __Thunk51
__Thunk51 proc near
	mov	eax,es:[edi+20]		; Parm6
	call	GetFirst16Alias
	push	word ptr es:[edi+40]	; Parm1
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	word ptr es:[edi+28]	; Parm4
	push	dword ptr es:[edi+24]	; Parm5
	push	dword ptr [bp-8]	; Parm6Alias
	push	word ptr es:[edi+16]	; Parm7
	jmp	Free16Alias
__Thunk51 endp

PUBLIC  __Thunk52
__Thunk52 proc near
	mov	eax,es:[edi+20]		; Parm3
	call	GetFirst16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	word ptr es:[edi+24]	; Parm2
	push	dword ptr [bp-8]	; Parm3Alias
	push	word ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk52 endp

PUBLIC  __Thunk53
__Thunk53 proc near
	mov	eax,es:[edi+20]		; Parm2
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-16]	; Parm1Alias
	push	dword ptr [bp-8]	; Parm2Alias
	push	word ptr es:[edi+16]	; Parm3
	jmp	Free16Alias
__Thunk53 endp

PUBLIC  __Thunk54
__Thunk54 proc near
	mov	eax,es:[edi+20]		; Parm4
	call	GetFirst16Alias
	push	word ptr es:[edi+32]	; Parm1
	push	dword ptr es:[edi+28]	; Parm2
	push	dword ptr es:[edi+24]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	push	dword ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk54 endp

PUBLIC  __Thunk55
__Thunk55 proc near
	mov	eax,es:[edi+16]		; Parm7
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm6
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm4
	call	Get16Alias
	push	word ptr es:[edi+40]	; Parm1
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	dword ptr [bp-24]	; Parm4Alias
	push	dword ptr es:[edi+24]	; Parm5
	push	dword ptr [bp-16]	; Parm6Alias
	push	dword ptr [bp-8]	; Parm7Alias
	jmp	Free16Alias
__Thunk55 endp

PUBLIC  __Thunk56
__Thunk56 proc near
	push	cx
	push	edx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk56 endp

PUBLIC  __Thunk57
__Thunk57 proc near
	mov	eax,es:[edi+20]		; Parm2
	call	GetFirst16Alias
	push	dword ptr es:[edi+24]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	word ptr es:[edi+16]	; Parm3
	jmp	Free16Alias
__Thunk57 endp

PUBLIC  __Thunk58
__Thunk58 proc near
	mov	eax,es:[edi+28]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	word ptr es:[edi+24]	; Parm2
	push	word ptr es:[edi+20]	; Parm3
	push	word ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk58 endp

PUBLIC  __Thunk59
__Thunk59 proc near
	mov	eax,es:[edi+16]		; Parm3
	call	GetFirst16Alias
	push	word ptr es:[edi+24]	; Parm1
	push	word ptr es:[edi+20]	; Parm2
	push	dword ptr [bp-8]	; Parm3Alias
	jmp	Free16Alias
__Thunk59 endp

PUBLIC  __Thunk60
__Thunk60 proc near
	mov	eax,es:[edi+16]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-24]	; Parm1Alias
	push	dword ptr [bp-16]	; Parm2Alias
	push	word ptr es:[edi+20]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	jmp	Free16Alias
__Thunk60 endp

PUBLIC  __Thunk61
__Thunk61 proc near
	mov	eax,es:[edi+16]		; Parm6
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-40]	; Parm1Alias
	push	dword ptr [bp-32]	; Parm2Alias
	push	dword ptr [bp-24]	; Parm3Alias
	push	dword ptr [bp-16]	; Parm4Alias
	push	word ptr es:[edi+20]	; Parm5
	push	dword ptr [bp-8]	; Parm6Alias
	jmp	Free16Alias
__Thunk61 endp

PUBLIC  __Thunk62
__Thunk62 proc near
	mov	eax,es:[edi+20]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-32]	; Parm1Alias
	push	dword ptr [bp-24]	; Parm2Alias
	push	dword ptr [bp-16]	; Parm3Alias
	push	dword ptr [bp-8]	; Parm4Alias
	push	word ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk62 endp

PUBLIC  __Thunk63
__Thunk63 proc near
	mov	eax,es:[edi+16]		; Parm5
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm2
	call	Get16Alias
	push	word ptr es:[edi+32]	; Parm1
	push	dword ptr [bp-16]	; Parm2Alias
	push	word ptr es:[edi+24]	; Parm3
	push	word ptr es:[edi+20]	; Parm4
	push	dword ptr [bp-8]	; Parm5Alias
	jmp	Free16Alias
__Thunk63 endp

PUBLIC  __Thunk64
__Thunk64 proc near
	mov	eax,es:[edi+16]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm2
	call	Get16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	dword ptr [bp-16]	; Parm2Alias
	push	word ptr es:[edi+20]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	jmp	Free16Alias
__Thunk64 endp

PUBLIC  __Thunk65
__Thunk65 proc near
	mov	eax,es:[edi+36]		; Parm4
	call	GetFirst16Alias
	push	word ptr es:[edi+48]	; Parm1
	push	word ptr es:[edi+44]	; Parm2
	push	dword ptr es:[edi+40]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	push	word ptr es:[edi+32]	; Parm5
	push	word ptr es:[edi+28]	; Parm6
	push	word ptr es:[edi+24]	; Parm7
	push	word ptr es:[edi+20]	; Parm8
	push	word ptr es:[edi+16]	; Parm9
	jmp	Free16Alias
__Thunk65 endp

PUBLIC  __Thunk66
__Thunk66 proc near
	mov	eax,es:[edi+20]		; Parm3
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm2
	call	Get16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	dword ptr [bp-16]	; Parm2Alias
	push	dword ptr [bp-8]	; Parm3Alias
	push	word ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk66 endp

PUBLIC  __Thunk67
__Thunk67 proc near
	mov	eax,es:[edi+20]		; Parm2
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-16]	; Parm1Alias
	push	dword ptr [bp-8]	; Parm2Alias
	push	dword ptr es:[edi+16]	; Parm3
	jmp	Free16Alias
__Thunk67 endp

PUBLIC  __Thunk68
__Thunk68 proc near
	push	word ptr es:[edi+36]	; Parm1
	push	word ptr es:[edi+32]	; Parm2
	push	word ptr es:[edi+28]	; Parm3
	push	word ptr es:[edi+24]	; Parm4
	push	word ptr es:[edi+20]	; Parm5
	push	dword ptr es:[edi+16]	; Parm6
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk68 endp

PUBLIC  __Thunk69
__Thunk69 proc near
	mov	eax,es:[edi+32]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	word ptr es:[edi+28]	; Parm2
	push	word ptr es:[edi+24]	; Parm3
	push	word ptr es:[edi+20]	; Parm4
	push	word ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk69 endp

PUBLIC  __Thunk70
__Thunk70 proc near
	mov	eax,es:[edi+20]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	dword ptr es:[edi+16]	; Parm2
	jmp	Free16Alias
__Thunk70 endp

PUBLIC  __Thunk71
__Thunk71 proc near
	push	word ptr es:[edi+40]	; Parm1
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	word ptr es:[edi+28]	; Parm4
	push	word ptr es:[edi+24]	; Parm5
	push	word ptr es:[edi+20]	; Parm6
	push	word ptr es:[edi+16]	; Parm7
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk71 endp

PUBLIC  __Thunk72
__Thunk72 proc near
	mov	eax,es:[edi+16]		; Parm6
	call	GetFirst16Alias
	push	word ptr es:[edi+36]	; Parm1
	push	word ptr es:[edi+32]	; Parm2
	push	word ptr es:[edi+28]	; Parm3
	push	word ptr es:[edi+24]	; Parm4
	push	word ptr es:[edi+20]	; Parm5
	push	dword ptr [bp-8]	; Parm6Alias
	jmp	Free16Alias
__Thunk72 endp

PUBLIC  __Thunk73
__Thunk73 proc near
	mov	eax,es:[edi+16]		; Parm7
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm5
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm4
	call	Get16Alias
	push	word ptr es:[edi+40]	; Parm1
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	dword ptr [bp-24]	; Parm4Alias
	push	dword ptr [bp-16]	; Parm5Alias
	push	word ptr es:[edi+20]	; Parm6
	push	dword ptr [bp-8]	; Parm7Alias
	jmp	Free16Alias
__Thunk73 endp

PUBLIC  __Thunk74
__Thunk74 proc near
	mov	eax,es:[edi+20]		; Parm7
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm5
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm4
	call	Get16Alias
	push	word ptr es:[edi+44]	; Parm1
	push	word ptr es:[edi+40]	; Parm2
	push	word ptr es:[edi+36]	; Parm3
	push	dword ptr [bp-24]	; Parm4Alias
	push	dword ptr [bp-16]	; Parm5Alias
	push	word ptr es:[edi+24]	; Parm6
	push	dword ptr [bp-8]	; Parm7Alias
	push	word ptr es:[edi+16]	; Parm8
	jmp	Free16Alias
__Thunk74 endp

PUBLIC  __Thunk75
__Thunk75 proc near
	mov	eax,es:[edi+20]		; Parm11
	call	GetFirst16Alias
	push	word ptr es:[edi+60]	; Parm1
	push	word ptr es:[edi+56]	; Parm2
	push	word ptr es:[edi+52]	; Parm3
	push	word ptr es:[edi+48]	; Parm4
	push	word ptr es:[edi+44]	; Parm5
	push	word ptr es:[edi+40]	; Parm6
	push	word ptr es:[edi+36]	; Parm7
	push	word ptr es:[edi+32]	; Parm8
	push	word ptr es:[edi+28]	; Parm9
	push	dword ptr es:[edi+24]	; Parm10
	push	dword ptr [bp-8]	; Parm11Alias
	push	word ptr es:[edi+16]	; Parm12
	jmp	Free16Alias
__Thunk75 endp

PUBLIC  __Thunk76
__Thunk76 proc near
	mov	eax,es:[edi+20]		; Parm2
	call	GetFirst16Alias
	push	word ptr es:[edi+24]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	dword ptr es:[edi+16]	; Parm3
	jmp	Free16Alias
__Thunk76 endp

PUBLIC  __Thunk77
__Thunk77 proc near
	push	si
	push	ecx
	push	dx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk77 endp

PUBLIC  __Thunk78
__Thunk78 proc near
	push	word ptr es:[edi+56]	; Parm1
	push	word ptr es:[edi+52]	; Parm2
	push	word ptr es:[edi+48]	; Parm3
	push	word ptr es:[edi+44]	; Parm4
	push	word ptr es:[edi+40]	; Parm5
	push	word ptr es:[edi+36]	; Parm6
	push	word ptr es:[edi+32]	; Parm7
	push	word ptr es:[edi+28]	; Parm8
	push	word ptr es:[edi+24]	; Parm9
	push	word ptr es:[edi+20]	; Parm10
	push	dword ptr es:[edi+16]	; Parm11
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk78 endp

PUBLIC  __Thunk79
__Thunk79 proc near
	mov	eax,es:[edi+24]		; Parm11
	call	GetFirst16Alias
	push	word ptr es:[edi+64]	; Parm1
	push	word ptr es:[edi+60]	; Parm2
	push	word ptr es:[edi+56]	; Parm3
	push	word ptr es:[edi+52]	; Parm4
	push	word ptr es:[edi+48]	; Parm5
	push	word ptr es:[edi+44]	; Parm6
	push	word ptr es:[edi+40]	; Parm7
	push	word ptr es:[edi+36]	; Parm8
	push	word ptr es:[edi+32]	; Parm9
	push	dword ptr es:[edi+28]	; Parm10
	push	dword ptr [bp-8]	; Parm11Alias
	push	word ptr es:[edi+20]	; Parm12
	push	dword ptr es:[edi+16]	; Parm13
	jmp	Free16Alias
__Thunk79 endp

PUBLIC  __Thunk80
__Thunk80 proc near
	mov	eax,es:[edi+20]		; Parm7
	call	GetFirst16Alias
	mov	eax,es:[edi+32]		; Parm4
	call	Get16Alias
	push	word ptr es:[edi+44]	; Parm1
	push	word ptr es:[edi+40]	; Parm2
	push	word ptr es:[edi+36]	; Parm3
	push	dword ptr [bp-16]	; Parm4Alias
	push	word ptr es:[edi+28]	; Parm5
	push	word ptr es:[edi+24]	; Parm6
	push	dword ptr [bp-8]	; Parm7Alias
	push	word ptr es:[edi+16]	; Parm8
	jmp	Free16Alias
__Thunk80 endp

PUBLIC  __Thunk81
__Thunk81 proc near
	mov	eax,es:[edi+20]		; Parm4
	call	GetFirst16Alias
	push	word ptr es:[edi+32]	; Parm1
	push	word ptr es:[edi+28]	; Parm2
	push	word ptr es:[edi+24]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	push	word ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk81 endp

PUBLIC  __Thunk82
__Thunk82 proc near
	mov	eax,es:[edi+20]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm3
	call	Get16Alias
	push	word ptr es:[edi+32]	; Parm1
	push	word ptr es:[edi+28]	; Parm2
	push	dword ptr [bp-16]	; Parm3Alias
	push	dword ptr [bp-8]	; Parm4Alias
	push	word ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk82 endp

PUBLIC  __Thunk83
__Thunk83 proc near
	mov	eax,es:[edi+16]		; Parm7
	call	GetFirst16Alias
	push	word ptr es:[edi+40]	; Parm1
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	word ptr es:[edi+28]	; Parm4
	push	word ptr es:[edi+24]	; Parm5
	push	word ptr es:[edi+20]	; Parm6
	push	dword ptr [bp-8]	; Parm7Alias
	jmp	Free16Alias
__Thunk83 endp

PUBLIC  __Thunk84
__Thunk84 proc near
	push	di
	push	si
	push	ecx
	push	dx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk84 endp

PUBLIC  __Thunk85
__Thunk85 proc near
	push	ecx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk85 endp

PUBLIC  __Thunk86
__Thunk86 proc near
	push	word ptr es:[edi+40]	; Parm1
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	word ptr es:[edi+28]	; Parm4
	push	word ptr es:[edi+24]	; Parm5
	push	dword ptr es:[edi+20]	; Parm6
	push	dword ptr es:[edi+16]	; Parm7
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk86 endp

PUBLIC  __Thunk87
__Thunk87 proc near
	push	esi
	push	ecx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk87 endp

PUBLIC  __Thunk88
__Thunk88 proc near
	push	si
	push	ecx
	push	dx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk88 endp

PUBLIC  __Thunk89
__Thunk89 proc near
	push	di
	push	esi
	push	cx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk89 endp

PUBLIC  __Thunk90
__Thunk90 proc near
	push	word ptr es:[edi+36]	; Parm1
	push	dword ptr es:[edi+32]	; Parm2
	push	dword ptr es:[edi+28]	; Parm3
	push	dword ptr es:[edi+24]	; Parm4
	push	dword ptr es:[edi+20]	; Parm5
	push	word ptr es:[edi+16]	; Parm6
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk90 endp

PUBLIC  __Thunk91
__Thunk91 proc near
	push	word ptr es:[edi+68]	; Parm1
	push	word ptr es:[edi+64]	; Parm2
	push	word ptr es:[edi+60]	; Parm3
	push	word ptr es:[edi+56]	; Parm4
	push	word ptr es:[edi+52]	; Parm5
	push	word ptr es:[edi+48]	; Parm6
	push	word ptr es:[edi+44]	; Parm7
	push	word ptr es:[edi+40]	; Parm8
	push	word ptr es:[edi+36]	; Parm9
	push	word ptr es:[edi+32]	; Parm10
	push	word ptr es:[edi+28]	; Parm11
	push	word ptr es:[edi+24]	; Parm12
	push	word ptr es:[edi+20]	; Parm13
	push	dword ptr es:[edi+16]	; Parm14
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk91 endp

PUBLIC  __Thunk92
__Thunk92 proc near
	push	ecx
	push	dx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk92 endp

PUBLIC  __Thunk93
__Thunk93 proc near
	push	esi
	push	ecx
	push	dx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk93 endp

PUBLIC  __Thunk94
__Thunk94 proc near
	push	si
	push	ecx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk94 endp

PUBLIC  __Thunk95
__Thunk95 proc near
	push	dword ptr es:[edi+56]	; Parm1
	push	dword ptr es:[edi+52]	; Parm2
	push	dword ptr es:[edi+48]	; Parm3
	push	word ptr es:[edi+44]	; Parm4
	push	word ptr es:[edi+40]	; Parm5
	push	word ptr es:[edi+36]	; Parm6
	push	word ptr es:[edi+32]	; Parm7
	push	word ptr es:[edi+28]	; Parm8
	push	word ptr es:[edi+24]	; Parm9
	push	word ptr es:[edi+20]	; Parm10
	push	dword ptr es:[edi+16]	; Parm11
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk95 endp

PUBLIC  __Thunk96
__Thunk96 proc near
	push	dword ptr es:[edi+60]	; Parm1
	push	dword ptr es:[edi+56]	; Parm2
	push	dword ptr es:[edi+52]	; Parm3
	push	dword ptr es:[edi+48]	; Parm4
	push	word ptr es:[edi+44]	; Parm5
	push	word ptr es:[edi+40]	; Parm6
	push	word ptr es:[edi+36]	; Parm7
	push	word ptr es:[edi+32]	; Parm8
	push	word ptr es:[edi+28]	; Parm9
	push	word ptr es:[edi+24]	; Parm10
	push	word ptr es:[edi+20]	; Parm11
	push	dword ptr es:[edi+16]	; Parm12
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk96 endp

PUBLIC  __Thunk97
__Thunk97 proc near
	push	di
	push	esi
	push	cx
	push	dx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk97 endp

PUBLIC  __Thunk98
__Thunk98 proc near
	push	di
	push	esi
	push	cx
	push	edx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk98 endp

PUBLIC  __Thunk99
__Thunk99 proc near
	push	word ptr es:[edi+44]	; Parm1
	push	word ptr es:[edi+40]	; Parm2
	push	word ptr es:[edi+36]	; Parm3
	push	word ptr es:[edi+32]	; Parm4
	push	dword ptr es:[edi+28]	; Parm5
	push	dword ptr es:[edi+24]	; Parm6
	push	word ptr es:[edi+20]	; Parm7
	push	dword ptr es:[edi+16]	; Parm8
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk99 endp

PUBLIC  __Thunk100
__Thunk100 proc near
	push	word ptr es:[edi+40]	; Parm1
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	word ptr es:[edi+28]	; Parm4
	push	dword ptr es:[edi+24]	; Parm5
	push	dword ptr es:[edi+20]	; Parm6
	push	word ptr es:[edi+16]	; Parm7
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk100 endp

PUBLIC  __Thunk101
__Thunk101 proc near
	push	si
	push	cx
	push	edx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk101 endp

PUBLIC  __Thunk102
__Thunk102 proc near
	push	ecx
	push	edx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk102 endp

PUBLIC  __Thunk103
__Thunk103 proc near
	push	di
	push	esi
	push	ecx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk103 endp

PUBLIC  __Thunk104
__Thunk104 proc near
	push	word ptr es:[edi+40]	; Parm1
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	dword ptr es:[edi+28]	; Parm4
	push	dword ptr es:[edi+24]	; Parm5
	push	dword ptr es:[edi+20]	; Parm6
	push	dword ptr es:[edi+16]	; Parm7
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk104 endp

PUBLIC  __Thunk105
__Thunk105 proc near
	push	esi
	push	cx
	push	dx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk105 endp

PUBLIC  __Thunk106
__Thunk106 proc near
	push	esi
	push	ecx
	push	dx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk106 endp

PUBLIC  __Thunk107
__Thunk107 proc near
	push	dword ptr es:[edi+36]	; Parm1
	push	dword ptr es:[edi+32]	; Parm2
	push	dword ptr es:[edi+28]	; Parm3
	push	dword ptr es:[edi+24]	; Parm4
	push	word ptr es:[edi+20]	; Parm5
	push	dword ptr es:[edi+16]	; Parm6
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk107 endp

PUBLIC  __Thunk108
__Thunk108 proc near
	push	edi
	push	esi
	push	ecx
	push	edx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk108 endp

PUBLIC  __Thunk109
__Thunk109 proc near
	push	di
	push	esi
	push	cx
	push	dx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk109 endp

PUBLIC  __Thunk110
__Thunk110 proc near
	push	word ptr es:[edi+48]	; Parm1
	push	word ptr es:[edi+44]	; Parm2
	push	dword ptr es:[edi+40]	; Parm3
	push	dword ptr es:[edi+36]	; Parm4
	push	word ptr es:[edi+32]	; Parm5
	push	word ptr es:[edi+28]	; Parm6
	push	word ptr es:[edi+24]	; Parm7
	push	word ptr es:[edi+20]	; Parm8
	push	word ptr es:[edi+16]	; Parm9
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk110 endp

PUBLIC  __Thunk111
__Thunk111 proc near
	push	si
	push	ecx
	push	edx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk111 endp

PUBLIC  __Thunk112
__Thunk112 proc near
	push	edi
	push	si
	push	cx
	push	dx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk112 endp

PUBLIC  __Thunk113
__Thunk113 proc near
	push	word ptr es:[edi+40]	; Parm1
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	dword ptr es:[edi+28]	; Parm4
	push	dword ptr es:[edi+24]	; Parm5
	push	word ptr es:[edi+20]	; Parm6
	push	dword ptr es:[edi+16]	; Parm7
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk113 endp

PUBLIC  __Thunk114
__Thunk114 proc near
	push	word ptr es:[edi+44]	; Parm1
	push	word ptr es:[edi+40]	; Parm2
	push	word ptr es:[edi+36]	; Parm3
	push	dword ptr es:[edi+32]	; Parm4
	push	dword ptr es:[edi+28]	; Parm5
	push	word ptr es:[edi+24]	; Parm6
	push	dword ptr es:[edi+20]	; Parm7
	push	word ptr es:[edi+16]	; Parm8
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk114 endp

PUBLIC  __Thunk115
__Thunk115 proc near
	push	word ptr es:[edi+60]	; Parm1
	push	word ptr es:[edi+56]	; Parm2
	push	word ptr es:[edi+52]	; Parm3
	push	word ptr es:[edi+48]	; Parm4
	push	word ptr es:[edi+44]	; Parm5
	push	word ptr es:[edi+40]	; Parm6
	push	word ptr es:[edi+36]	; Parm7
	push	word ptr es:[edi+32]	; Parm8
	push	word ptr es:[edi+28]	; Parm9
	push	dword ptr es:[edi+24]	; Parm10
	push	dword ptr es:[edi+20]	; Parm11
	push	word ptr es:[edi+16]	; Parm12
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk115 endp

PUBLIC  __Thunk116
__Thunk116 proc near
	push	word ptr es:[edi+64]	; Parm1
	push	word ptr es:[edi+60]	; Parm2
	push	word ptr es:[edi+56]	; Parm3
	push	word ptr es:[edi+52]	; Parm4
	push	word ptr es:[edi+48]	; Parm5
	push	word ptr es:[edi+44]	; Parm6
	push	word ptr es:[edi+40]	; Parm7
	push	word ptr es:[edi+36]	; Parm8
	push	word ptr es:[edi+32]	; Parm9
	push	dword ptr es:[edi+28]	; Parm10
	push	dword ptr es:[edi+24]	; Parm11
	push	word ptr es:[edi+20]	; Parm12
	push	dword ptr es:[edi+16]	; Parm13
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk116 endp

PUBLIC  __Thunk117
__Thunk117 proc near
	push	word ptr es:[edi+44]	; Parm1
	push	word ptr es:[edi+40]	; Parm2
	push	word ptr es:[edi+36]	; Parm3
	push	dword ptr es:[edi+32]	; Parm4
	push	word ptr es:[edi+28]	; Parm5
	push	word ptr es:[edi+24]	; Parm6
	push	dword ptr es:[edi+20]	; Parm7
	push	word ptr es:[edi+16]	; Parm8
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk117 endp

PUBLIC  __Thunk118
__Thunk118 proc near
	push	di
	push	si
	push	ecx
	push	edx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk118 endp

PUBLIC  __Thunk119
__Thunk119 proc near
	push	word ptr es:[edi+40]	; Parm1
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	word ptr es:[edi+28]	; Parm4
	push	word ptr es:[edi+24]	; Parm5
	push	word ptr es:[edi+20]	; Parm6
	push	dword ptr es:[edi+16]	; Parm7
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk119 endp

PUBLIC  __Thunk120
__Thunk120 proc near
	mov	eax,es:[edi+16]		; Parm2
	call	GetFirst16Alias
	push	dword ptr es:[edi+20]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	jmp	Free16Alias
__Thunk120 endp

PUBLIC  __Thunk121
__Thunk121 proc near
	mov	eax,es:[edi+20]		; Parm2
	call	GetFirst16Alias
	push	dword ptr es:[edi+24]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	dword ptr es:[edi+16]	; Parm3
	jmp	Free16Alias
__Thunk121 endp

PUBLIC  __Thunk122
__Thunk122 proc near
	mov	eax,es:[edi+16]		; Parm3
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-16]	; Parm1Alias
	push	word ptr es:[edi+20]	; Parm2
	push	dword ptr [bp-8]	; Parm3Alias
	jmp	Free16Alias
__Thunk122 endp

PUBLIC  __Thunk123
__Thunk123 proc near
	mov	eax,es:[edi+24]		; Parm2
	call	GetFirst16Alias
	push	dword ptr es:[edi+28]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	dword ptr es:[edi+20]	; Parm3
	push	dword ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk123 endp

PUBLIC  __Thunk124
__Thunk124 proc near
	mov	eax,es:[edi+16]		; Parm8
	call	GetFirst16Alias
	mov	eax,es:[edi+44]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-16]	; Parm1Alias
	push	dword ptr es:[edi+40]	; Parm2
	push	dword ptr es:[edi+36]	; Parm3
	push	dword ptr es:[edi+32]	; Parm4
	push	word ptr es:[edi+28]	; Parm5
	push	word ptr es:[edi+24]	; Parm6
	push	dword ptr es:[edi+20]	; Parm7
	push	dword ptr [bp-8]	; Parm8Alias
	jmp	Free16Alias
__Thunk124 endp

PUBLIC  __Thunk125
__Thunk125 proc near
	mov	eax,es:[edi+16]		; Parm4
	call	GetFirst16Alias
	push	dword ptr es:[edi+28]	; Parm1
	push	dword ptr es:[edi+24]	; Parm2
	push	dword ptr es:[edi+20]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	jmp	Free16Alias
__Thunk125 endp

PUBLIC  __Thunk126
__Thunk126 proc near
	mov	eax,es:[edi+16]		; Parm5
	call	GetFirst16Alias
	push	dword ptr es:[edi+32]	; Parm1
	push	dword ptr es:[edi+28]	; Parm2
	push	dword ptr es:[edi+24]	; Parm3
	push	dword ptr es:[edi+20]	; Parm4
	push	dword ptr [bp-8]	; Parm5Alias
	jmp	Free16Alias
__Thunk126 endp

PUBLIC  __Thunk127
__Thunk127 proc near
	mov	eax,es:[edi+36]		; Parm2
	call	GetFirst16Alias
	push	dword ptr es:[edi+40]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	dword ptr es:[edi+32]	; Parm3
	push	dword ptr es:[edi+28]	; Parm4
	push	dword ptr es:[edi+24]	; Parm5
	push	word ptr es:[edi+20]	; Parm6
	push	word ptr es:[edi+16]	; Parm7
	jmp	Free16Alias
__Thunk127 endp

PUBLIC  __Thunk128
__Thunk128 proc near
	mov	eax,es:[edi+28]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	dword ptr es:[edi+24]	; Parm2
	push	dword ptr es:[edi+20]	; Parm3
	push	dword ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk128 endp

PUBLIC  __Thunk129
__Thunk129 proc near
	push	esi
	push	ecx
	push	edx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk129 endp

PUBLIC  __Thunk130
__Thunk130 proc near
	mov	eax,es:[edi+16]		; Parm3
	call	GetFirst16Alias
	push	dword ptr es:[edi+24]	; Parm1
	push	dword ptr es:[edi+20]	; Parm2
	push	dword ptr [bp-8]	; Parm3Alias
	jmp	Free16Alias
__Thunk130 endp

PUBLIC  __Thunk131
__Thunk131 proc near
	mov	eax,es:[edi+24]		; Parm3
	call	GetFirst16Alias
	push	dword ptr es:[edi+32]	; Parm1
	push	dword ptr es:[edi+28]	; Parm2
	push	dword ptr [bp-8]	; Parm3Alias
	push	dword ptr es:[edi+20]	; Parm4
	push	word ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk131 endp

PUBLIC  __Thunk132
__Thunk132 proc near
	push	dword ptr es:[edi+44]	; Parm1
	push	dword ptr es:[edi+40]	; Parm2
	push	dword ptr es:[edi+36]	; Parm3
	push	dword ptr es:[edi+32]	; Parm4
	push	word ptr es:[edi+28]	; Parm5
	push	word ptr es:[edi+24]	; Parm6
	push	dword ptr es:[edi+20]	; Parm7
	push	dword ptr es:[edi+16]	; Parm8
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk132 endp

PUBLIC  __Thunk133
__Thunk133 proc near
	push	edi
	push	esi
	push	ecx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk133 endp

PUBLIC  __Thunk134
__Thunk134 proc near
	push	dword ptr es:[edi+40]	; Parm1
	push	dword ptr es:[edi+36]	; Parm2
	push	dword ptr es:[edi+32]	; Parm3
	push	dword ptr es:[edi+28]	; Parm4
	push	dword ptr es:[edi+24]	; Parm5
	push	word ptr es:[edi+20]	; Parm6
	push	word ptr es:[edi+16]	; Parm7
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk134 endp

PUBLIC  __Thunk135
__Thunk135 proc near
	mov	eax,es:[edi+16]		; Parm3
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm2
	call	Get16Alias
	push	dword ptr es:[edi+24]	; Parm1
	push	dword ptr [bp-16]	; Parm2Alias
	push	dword ptr [bp-8]	; Parm3Alias
	jmp	Free16Alias
__Thunk135 endp

PUBLIC  __Thunk136
__Thunk136 proc near
	mov	eax,es:[edi+20]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm2
	call	Get16Alias
	push	dword ptr es:[edi+32]	; Parm1
	push	dword ptr [bp-16]	; Parm2Alias
	push	dword ptr es:[edi+24]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	push	dword ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk136 endp

PUBLIC  __Thunk137
__Thunk137 proc near
	mov	eax,es:[edi+16]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+24]		; Parm2
	call	Get16Alias
	push	dword ptr es:[edi+28]	; Parm1
	push	dword ptr [bp-24]	; Parm2Alias
	push	dword ptr [bp-16]	; Parm3Alias
	push	dword ptr [bp-8]	; Parm4Alias
	jmp	Free16Alias
__Thunk137 endp

PUBLIC  __Thunk138
__Thunk138 proc near
	mov	eax,es:[edi+20]		; Parm3
	call	GetFirst16Alias
	push	dword ptr es:[edi+28]	; Parm1
	push	dword ptr es:[edi+24]	; Parm2
	push	dword ptr [bp-8]	; Parm3Alias
	push	dword ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk138 endp

PUBLIC  __Thunk139
__Thunk139 proc near
	mov	eax,es:[edi+20]		; Parm5
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm2
	call	Get16Alias
	push	word ptr es:[edi+36]	; Parm1
	push	dword ptr [bp-32]	; Parm2Alias
	push	dword ptr [bp-24]	; Parm3Alias
	push	dword ptr [bp-16]	; Parm4Alias
	push	dword ptr [bp-8]	; Parm5Alias
	push	word ptr es:[edi+16]	; Parm6
	jmp	Free16Alias
__Thunk139 endp

PUBLIC  __Thunk140
__Thunk140 proc near
	mov	eax,es:[edi+24]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+32]		; Parm2
	call	Get16Alias
	push	word ptr es:[edi+36]	; Parm1
	push	dword ptr [bp-16]	; Parm2Alias
	push	word ptr es:[edi+28]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	push	dword ptr es:[edi+20]	; Parm5
	push	dword ptr es:[edi+16]	; Parm6
	jmp	Free16Alias
__Thunk140 endp

PUBLIC  __Thunk141
__Thunk141 proc near
	mov	eax,es:[edi+20]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-24]	; Parm1Alias
	push	dword ptr [bp-16]	; Parm2Alias
	push	word ptr es:[edi+24]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	push	word ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk141 endp

PUBLIC  __Thunk142
__Thunk142 proc near
	mov	eax,es:[edi+28]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	word ptr es:[edi+24]	; Parm2
	push	dword ptr es:[edi+20]	; Parm3
	push	dword ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk142 endp

PUBLIC  __Thunk143
__Thunk143 proc near
	mov	eax,es:[edi+16]		; Parm3
	call	GetFirst16Alias
	push	word ptr es:[edi+24]	; Parm1
	push	dword ptr es:[edi+20]	; Parm2
	push	dword ptr [bp-8]	; Parm3Alias
	jmp	Free16Alias
__Thunk143 endp

PUBLIC  __Thunk144
__Thunk144 proc near
	mov	eax,es:[edi+20]		; Parm3
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-24]	; Parm1Alias
	push	dword ptr [bp-16]	; Parm2Alias
	push	dword ptr [bp-8]	; Parm3Alias
	push	word ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk144 endp

PUBLIC  __Thunk145
__Thunk145 proc near
	mov	eax,es:[edi+20]		; Parm3
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm2
	call	Get16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	dword ptr [bp-16]	; Parm2Alias
	push	dword ptr [bp-8]	; Parm3Alias
	push	dword ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk145 endp

PUBLIC  __Thunk146
__Thunk146 proc near
	mov	eax,es:[edi+16]		; Parm5
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+24]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-32]	; Parm1Alias
	push	word ptr es:[edi+28]	; Parm2
	push	dword ptr [bp-24]	; Parm3Alias
	push	dword ptr [bp-16]	; Parm4Alias
	push	dword ptr [bp-8]	; Parm5Alias
	jmp	Free16Alias
__Thunk146 endp

PUBLIC  __Thunk147
__Thunk147 proc near
	mov	eax,es:[edi+16]		; Parm5
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-24]	; Parm1Alias
	push	dword ptr [bp-16]	; Parm2Alias
	push	word ptr es:[edi+24]	; Parm3
	push	word ptr es:[edi+20]	; Parm4
	push	dword ptr [bp-8]	; Parm5Alias
	jmp	Free16Alias
__Thunk147 endp

PUBLIC  __Thunk148
__Thunk148 proc near
	mov	eax,es:[edi+16]		; Parm6
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm3
	call	Get16Alias
	push	word ptr es:[edi+36]	; Parm1
	push	word ptr es:[edi+32]	; Parm2
	push	dword ptr [bp-24]	; Parm3Alias
	push	dword ptr [bp-16]	; Parm4Alias
	push	word ptr es:[edi+20]	; Parm5
	push	dword ptr [bp-8]	; Parm6Alias
	jmp	Free16Alias
__Thunk148 endp

PUBLIC  __Thunk149
__Thunk149 proc near
	mov	eax,es:[edi+16]		; Parm5
	call	GetFirst16Alias
	push	word ptr es:[edi+32]	; Parm1
	push	word ptr es:[edi+28]	; Parm2
	push	word ptr es:[edi+24]	; Parm3
	push	word ptr es:[edi+20]	; Parm4
	push	dword ptr [bp-8]	; Parm5Alias
	jmp	Free16Alias
__Thunk149 endp

PUBLIC  __Thunk150
__Thunk150 proc near
	mov	eax,es:[edi+24]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm3
	call	Get16Alias
	push	word ptr es:[edi+36]	; Parm1
	push	word ptr es:[edi+32]	; Parm2
	push	dword ptr [bp-16]	; Parm3Alias
	push	dword ptr [bp-8]	; Parm4Alias
	push	word ptr es:[edi+20]	; Parm5
	push	dword ptr es:[edi+16]	; Parm6
	jmp	Free16Alias
__Thunk150 endp

PUBLIC  __Thunk151
__Thunk151 proc near
	mov	eax,es:[edi+16]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm3
	call	Get16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	word ptr es:[edi+24]	; Parm2
	push	dword ptr [bp-16]	; Parm3Alias
	push	dword ptr [bp-8]	; Parm4Alias
	jmp	Free16Alias
__Thunk151 endp

PUBLIC  __Thunk152
__Thunk152 proc near
	mov	eax,es:[edi+16]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-24]	; Parm1Alias
	push	word ptr es:[edi+24]	; Parm2
	push	dword ptr [bp-16]	; Parm3Alias
	push	dword ptr [bp-8]	; Parm4Alias
	jmp	Free16Alias
__Thunk152 endp

PUBLIC  __Thunk153
__Thunk153 proc near
	push	ecx
	push	dx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk153 endp

PUBLIC  __Thunk154
__Thunk154 proc near
	push	word ptr es:[edi+36]	; Parm1
	push	dword ptr es:[edi+32]	; Parm2
	push	word ptr es:[edi+28]	; Parm3
	push	dword ptr es:[edi+24]	; Parm4
	push	dword ptr es:[edi+20]	; Parm5
	push	dword ptr es:[edi+16]	; Parm6
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk154 endp

PUBLIC  __Thunk155
__Thunk155 proc near
	push	edi
	push	esi
	push	cx
	push	edx
	push	ax
	call	dword ptr ds:[bx]
	ret
__Thunk155 endp

PUBLIC  __Thunk156
__Thunk156 proc near
	push	esi
	push	cx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk156 endp

PUBLIC  __Thunk157
__Thunk157 proc near
	push	edi
	push	si
	push	ecx
	push	edx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk157 endp

PUBLIC  __Thunk158
__Thunk158 proc near
	push	edi
	push	esi
	push	cx
	push	dx
	push	eax
	call	dword ptr ds:[bx]
	ret
__Thunk158 endp

PUBLIC  __Thunk159
__Thunk159 proc near
	push	word ptr es:[edi+36]	; Parm1
	push	word ptr es:[edi+32]	; Parm2
	push	dword ptr es:[edi+28]	; Parm3
	push	dword ptr es:[edi+24]	; Parm4
	push	word ptr es:[edi+20]	; Parm5
	push	dword ptr es:[edi+16]	; Parm6
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk159 endp

PUBLIC  __Thunk160
__Thunk160 proc near
	mov	eax,es:[edi+16]		; Parm6
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-32]	; Parm1Alias
	push	dword ptr [bp-24]	; Parm2Alias
	push	dword ptr [bp-16]	; Parm3Alias
	push	dword ptr es:[edi+24]	; Parm4
	push	dword ptr es:[edi+20]	; Parm5
	push	dword ptr [bp-8]	; Parm6Alias
	jmp	Free16Alias
__Thunk160 endp

PUBLIC  __Thunk161
__Thunk161 proc near
	mov	eax,es:[edi+16]		; Parm4
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-16]	; Parm1Alias
	push	dword ptr es:[edi+24]	; Parm2
	push	dword ptr es:[edi+20]	; Parm3
	push	dword ptr [bp-8]	; Parm4Alias
	jmp	Free16Alias
__Thunk161 endp

PUBLIC  __Thunk162
__Thunk162 proc near
	mov	eax,es:[edi+16]		; Parm8
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm7
	call	Get16Alias
	mov	eax,es:[edi+24]		; Parm6
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm5
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+40]		; Parm2
	call	Get16Alias
	push	word ptr es:[edi+44]	; Parm1
	push	dword ptr [bp-56]	; Parm2Alias
	push	dword ptr [bp-48]	; Parm3Alias
	push	dword ptr [bp-40]	; Parm4Alias
	push	dword ptr [bp-32]	; Parm5Alias
	push	dword ptr [bp-24]	; Parm6Alias
	push	dword ptr [bp-16]	; Parm7Alias
	push	dword ptr [bp-8]	; Parm8Alias
	jmp	Free16Alias
__Thunk162 endp

PUBLIC  __Thunk163
__Thunk163 proc near
	push	dword ptr es:[edi+36]	; Parm1
	push	dword ptr es:[edi+32]	; Parm2
	push	dword ptr es:[edi+28]	; Parm3
	push	dword ptr es:[edi+24]	; Parm4
	push	dword ptr es:[edi+20]	; Parm5
	push	dword ptr es:[edi+16]	; Parm6
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk163 endp

PUBLIC  __Thunk164
__Thunk164 proc near
	push	word ptr es:[edi+44]	; Parm1
	push	dword ptr es:[edi+40]	; Parm2
	push	dword ptr es:[edi+36]	; Parm3
	push	dword ptr es:[edi+32]	; Parm4
	push	dword ptr es:[edi+28]	; Parm5
	push	dword ptr es:[edi+24]	; Parm6
	push	dword ptr es:[edi+20]	; Parm7
	push	dword ptr es:[edi+16]	; Parm8
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk164 endp

PUBLIC  __Thunk165
__Thunk165 proc near
	mov	eax,es:[edi+32]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	word ptr es:[edi+28]	; Parm2
	push	dword ptr es:[edi+24]	; Parm3
	push	dword ptr es:[edi+20]	; Parm4
	push	dword ptr es:[edi+16]	; Parm5
	jmp	Free16Alias
__Thunk165 endp

PUBLIC  __Thunk166
__Thunk166 proc near
	mov	eax,es:[edi+20]		; Parm3
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-24]	; Parm1Alias
	push	dword ptr [bp-16]	; Parm2Alias
	push	dword ptr [bp-8]	; Parm3Alias
	push	dword ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk166 endp

PUBLIC  __Thunk167
__Thunk167 proc near
	mov	eax,es:[edi+24]		; Parm2
	call	GetFirst16Alias
	push	word ptr es:[edi+28]	; Parm1
	push	dword ptr [bp-8]	; Parm2Alias
	push	dword ptr es:[edi+20]	; Parm3
	push	word ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk167 endp

PUBLIC  __Thunk168
__Thunk168 proc near
	mov	eax,es:[edi+28]		; Parm3
	call	GetFirst16Alias
	mov	eax,es:[edi+36]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-16]	; Parm1Alias
	push	word ptr es:[edi+32]	; Parm2
	push	dword ptr [bp-8]	; Parm3Alias
	push	dword ptr es:[edi+24]	; Parm4
	push	dword ptr es:[edi+20]	; Parm5
	push	dword ptr es:[edi+16]	; Parm6
	jmp	Free16Alias
__Thunk168 endp

PUBLIC  __Thunk169
__Thunk169 proc near
	push	dword ptr es:[edi+36]	; Parm1
	push	word ptr es:[edi+32]	; Parm2
	push	dword ptr es:[edi+28]	; Parm3
	push	dword ptr es:[edi+24]	; Parm4
	push	dword ptr es:[edi+20]	; Parm5
	push	dword ptr es:[edi+16]	; Parm6
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk169 endp

PUBLIC  __Thunk170
__Thunk170 proc near
	mov	eax,es:[edi+36]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	word ptr es:[edi+32]	; Parm2
	push	word ptr es:[edi+28]	; Parm3
	push	dword ptr es:[edi+24]	; Parm4
	push	dword ptr es:[edi+20]	; Parm5
	push	dword ptr es:[edi+16]	; Parm6
	jmp	Free16Alias
__Thunk170 endp

PUBLIC  __Thunk171
__Thunk171 proc near
	mov	eax,es:[edi+16]		; Parm6
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-32]	; Parm1Alias
	push	dword ptr [bp-24]	; Parm2Alias
	push	word ptr es:[edi+28]	; Parm3
	push	dword ptr [bp-16]	; Parm4Alias
	push	word ptr es:[edi+20]	; Parm5
	push	dword ptr [bp-8]	; Parm6Alias
	jmp	Free16Alias
__Thunk171 endp

PUBLIC  __Thunk172
__Thunk172 proc near
	mov	eax,es:[edi+16]		; Parm7
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm6
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+40]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-32]	; Parm1Alias
	push	word ptr es:[edi+36]	; Parm2
	push	word ptr es:[edi+32]	; Parm3
	push	dword ptr [bp-24]	; Parm4Alias
	push	word ptr es:[edi+24]	; Parm5
	push	dword ptr [bp-16]	; Parm6Alias
	push	dword ptr [bp-8]	; Parm7Alias
	jmp	Free16Alias
__Thunk172 endp

PUBLIC  __Thunk173
__Thunk173 proc near
	mov	eax,es:[edi+20]		; Parm8
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm6
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+44]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+48]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-40]	; Parm1Alias
	push	dword ptr [bp-32]	; Parm2Alias
	push	word ptr es:[edi+40]	; Parm3
	push	dword ptr [bp-24]	; Parm4Alias
	push	word ptr es:[edi+32]	; Parm5
	push	dword ptr [bp-16]	; Parm6Alias
	push	word ptr es:[edi+24]	; Parm7
	push	dword ptr [bp-8]	; Parm8Alias
	push	word ptr es:[edi+16]	; Parm9
	jmp	Free16Alias
__Thunk173 endp

PUBLIC  __Thunk174
__Thunk174 proc near
	mov	eax,es:[edi+20]		; Parm6
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+40]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-32]	; Parm1Alias
	push	dword ptr [bp-24]	; Parm2Alias
	push	word ptr es:[edi+32]	; Parm3
	push	dword ptr [bp-16]	; Parm4Alias
	push	word ptr es:[edi+24]	; Parm5
	push	dword ptr [bp-8]	; Parm6Alias
	push	word ptr es:[edi+16]	; Parm7
	jmp	Free16Alias
__Thunk174 endp

PUBLIC  __Thunk175
__Thunk175 proc near
	mov	eax,es:[edi+16]		; Parm8
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm6
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm5
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+44]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-40]	; Parm1Alias
	push	word ptr es:[edi+40]	; Parm2
	push	dword ptr [bp-32]	; Parm3Alias
	push	word ptr es:[edi+32]	; Parm4
	push	dword ptr [bp-24]	; Parm5Alias
	push	dword ptr [bp-16]	; Parm6Alias
	push	word ptr es:[edi+20]	; Parm7
	push	dword ptr [bp-8]	; Parm8Alias
	jmp	Free16Alias
__Thunk175 endp

PUBLIC  __Thunk176
__Thunk176 proc near
	mov	eax,es:[edi+16]		; Parm9
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm8
	call	Get16Alias
	mov	eax,es:[edi+24]		; Parm7
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm6
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm5
	call	Get16Alias
	mov	eax,es:[edi+40]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+48]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-56]	; Parm1Alias
	push	word ptr es:[edi+44]	; Parm2
	push	dword ptr [bp-48]	; Parm3Alias
	push	word ptr es:[edi+36]	; Parm4
	push	dword ptr [bp-40]	; Parm5Alias
	push	dword ptr [bp-32]	; Parm6Alias
	push	dword ptr [bp-24]	; Parm7Alias
	push	dword ptr [bp-16]	; Parm8Alias
	push	dword ptr [bp-8]	; Parm9Alias
	jmp	Free16Alias
__Thunk176 endp

PUBLIC  __Thunk177
__Thunk177 proc near
	mov	eax,es:[edi+16]		; Parm6
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm5
	call	Get16Alias
	mov	eax,es:[edi+24]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-40]	; Parm1Alias
	push	word ptr es:[edi+32]	; Parm2
	push	dword ptr [bp-32]	; Parm3Alias
	push	dword ptr [bp-24]	; Parm4Alias
	push	dword ptr [bp-16]	; Parm5Alias
	push	dword ptr [bp-8]	; Parm6Alias
	jmp	Free16Alias
__Thunk177 endp

PUBLIC  __Thunk178
__Thunk178 proc near
	mov	eax,es:[edi+20]		; Parm7
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm5
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+44]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-32]	; Parm1Alias
	push	word ptr es:[edi+40]	; Parm2
	push	dword ptr [bp-24]	; Parm3Alias
	push	word ptr es:[edi+32]	; Parm4
	push	dword ptr [bp-16]	; Parm5Alias
	push	word ptr es:[edi+24]	; Parm6
	push	dword ptr [bp-8]	; Parm7Alias
	push	word ptr es:[edi+16]	; Parm8
	jmp	Free16Alias
__Thunk178 endp

PUBLIC  __Thunk179
__Thunk179 proc near
	mov	eax,es:[edi+16]		; Parm8
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm6
	call	Get16Alias
	mov	eax,es:[edi+28]		; Parm5
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+40]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+44]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-56]	; Parm1Alias
	push	dword ptr [bp-48]	; Parm2Alias
	push	dword ptr [bp-40]	; Parm3Alias
	push	dword ptr [bp-32]	; Parm4Alias
	push	dword ptr [bp-24]	; Parm5Alias
	push	dword ptr [bp-16]	; Parm6Alias
	push	word ptr es:[edi+20]	; Parm7
	push	dword ptr [bp-8]	; Parm8Alias
	jmp	Free16Alias
__Thunk179 endp

PUBLIC  __Thunk180
__Thunk180 proc near
	mov	eax,es:[edi+16]		; Parm5
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-24]	; Parm1Alias
	push	word ptr es:[edi+28]	; Parm2
	push	dword ptr es:[edi+24]	; Parm3
	push	dword ptr [bp-16]	; Parm4Alias
	push	dword ptr [bp-8]	; Parm5Alias
	jmp	Free16Alias
__Thunk180 endp

PUBLIC  __Thunk181
__Thunk181 proc near
	mov	eax,es:[edi+20]		; Parm12
	call	GetFirst16Alias
	mov	eax,es:[edi+28]		; Parm10
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm8
	call	Get16Alias
	mov	eax,es:[edi+44]		; Parm6
	call	Get16Alias
	mov	eax,es:[edi+52]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+60]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+64]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-56]	; Parm1Alias
	push	dword ptr [bp-48]	; Parm2Alias
	push	word ptr es:[edi+56]	; Parm3
	push	dword ptr [bp-40]	; Parm4Alias
	push	word ptr es:[edi+48]	; Parm5
	push	dword ptr [bp-32]	; Parm6Alias
	push	word ptr es:[edi+40]	; Parm7
	push	dword ptr [bp-24]	; Parm8Alias
	push	word ptr es:[edi+32]	; Parm9
	push	dword ptr [bp-16]	; Parm10Alias
	push	word ptr es:[edi+24]	; Parm11
	push	dword ptr [bp-8]	; Parm12Alias
	push	word ptr es:[edi+16]	; Parm13
	jmp	Free16Alias
__Thunk181 endp

PUBLIC  __Thunk182
__Thunk182 proc near
	mov	eax,es:[edi+16]		; Parm6
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-24]	; Parm1Alias
	push	word ptr es:[edi+32]	; Parm2
	push	word ptr es:[edi+28]	; Parm3
	push	dword ptr [bp-16]	; Parm4Alias
	push	dword ptr es:[edi+20]	; Parm5
	push	dword ptr [bp-8]	; Parm6Alias
	jmp	Free16Alias
__Thunk182 endp

PUBLIC  __Thunk183
__Thunk183 proc near
	mov	eax,es:[edi+16]		; Parm5
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-24]	; Parm1Alias
	push	word ptr es:[edi+28]	; Parm2
	push	dword ptr [bp-16]	; Parm3Alias
	push	word ptr es:[edi+20]	; Parm4
	push	dword ptr [bp-8]	; Parm5Alias
	jmp	Free16Alias
__Thunk183 endp

PUBLIC  __Thunk184
__Thunk184 proc near
	mov	eax,es:[edi+16]		; Parm6
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+32]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+36]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-32]	; Parm1Alias
	push	dword ptr [bp-24]	; Parm2Alias
	push	dword ptr es:[edi+28]	; Parm3
	push	dword ptr [bp-16]	; Parm4Alias
	push	dword ptr es:[edi+20]	; Parm5
	push	dword ptr [bp-8]	; Parm6Alias
	jmp	Free16Alias
__Thunk184 endp

PUBLIC  __Thunk185
__Thunk185 proc near
	mov	eax,es:[edi+16]		; Parm3
	call	GetFirst16Alias
	mov	eax,es:[edi+24]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-16]	; Parm1Alias
	push	dword ptr es:[edi+20]	; Parm2
	push	dword ptr [bp-8]	; Parm3Alias
	jmp	Free16Alias
__Thunk185 endp

PUBLIC  __Thunk186
__Thunk186 proc near
	mov	eax,es:[edi+24]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	word ptr es:[edi+20]	; Parm2
	push	dword ptr es:[edi+16]	; Parm3
	jmp	Free16Alias
__Thunk186 endp

PUBLIC  __Thunk187
__Thunk187 proc near
	mov	eax,es:[edi+44]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	word ptr es:[edi+40]	; Parm2
	push	word ptr es:[edi+36]	; Parm3
	push	word ptr es:[edi+32]	; Parm4
	push	dword ptr es:[edi+28]	; Parm5
	push	word ptr es:[edi+24]	; Parm6
	push	dword ptr es:[edi+20]	; Parm7
	push	dword ptr es:[edi+16]	; Parm8
	jmp	Free16Alias
__Thunk187 endp

PUBLIC  __Thunk188
__Thunk188 proc near
	mov	eax,es:[edi+28]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	word ptr es:[edi+24]	; Parm2
	push	dword ptr es:[edi+20]	; Parm3
	push	word ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk188 endp

PUBLIC  __Thunk189
__Thunk189 proc near
	mov	eax,es:[edi+28]		; Parm7
	call	GetFirst16Alias
	mov	eax,es:[edi+36]		; Parm5
	call	Get16Alias
	mov	eax,es:[edi+44]		; Parm3
	call	Get16Alias
	mov	eax,es:[edi+52]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-32]	; Parm1Alias
	push	word ptr es:[edi+48]	; Parm2
	push	dword ptr [bp-24]	; Parm3Alias
	push	word ptr es:[edi+40]	; Parm4
	push	dword ptr [bp-16]	; Parm5Alias
	push	word ptr es:[edi+32]	; Parm6
	push	dword ptr [bp-8]	; Parm7Alias
	push	word ptr es:[edi+24]	; Parm8
	push	word ptr es:[edi+20]	; Parm9
	push	word ptr es:[edi+16]	; Parm10
	jmp	Free16Alias
__Thunk189 endp

PUBLIC  __Thunk190
__Thunk190 proc near
	mov	eax,es:[edi+28]		; Parm6
	call	GetFirst16Alias
	mov	eax,es:[edi+36]		; Parm4
	call	Get16Alias
	mov	eax,es:[edi+44]		; Parm2
	call	Get16Alias
	mov	eax,es:[edi+48]		; Parm1
	call	Get16Alias
	push	dword ptr [bp-32]	; Parm1Alias
	push	dword ptr [bp-24]	; Parm2Alias
	push	word ptr es:[edi+40]	; Parm3
	push	dword ptr [bp-16]	; Parm4Alias
	push	word ptr es:[edi+32]	; Parm5
	push	dword ptr [bp-8]	; Parm6Alias
	push	word ptr es:[edi+24]	; Parm7
	push	word ptr es:[edi+20]	; Parm8
	push	word ptr es:[edi+16]	; Parm9
	jmp	Free16Alias
__Thunk190 endp

PUBLIC  __Thunk191
__Thunk191 proc near
	mov	eax,es:[edi+24]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	dword ptr es:[edi+20]	; Parm2
	push	word ptr es:[edi+16]	; Parm3
	jmp	Free16Alias
__Thunk191 endp

PUBLIC  __Thunk192
__Thunk192 proc near
	mov	eax,es:[edi+28]		; Parm1
	call	GetFirst16Alias
	push	dword ptr [bp-8]	; Parm1Alias
	push	dword ptr es:[edi+24]	; Parm2
	push	word ptr es:[edi+20]	; Parm3
	push	dword ptr es:[edi+16]	; Parm4
	jmp	Free16Alias
__Thunk192 endp

PUBLIC  __Thunk193
__Thunk193 proc near
	push	word ptr es:[edi+36]	; Parm1
	push	word ptr es:[edi+32]	; Parm2
	push	word ptr es:[edi+28]	; Parm3
	push	word ptr es:[edi+24]	; Parm4
	push	dword ptr es:[edi+20]	; Parm5
	push	dword ptr es:[edi+16]	; Parm6
	call	dword ptr ds:[bx]
	push	dx
	push	ax
	pop	eax
	ret
__Thunk193 endp

PUBLIC  __Thunk194
__Thunk194 proc near
	mov	eax,es:[edi+16]		; Parm5
	call	GetFirst16Alias
	mov	eax,es:[edi+20]		; Parm4
	call	Get16Alias
	push	word ptr es:[edi+32]	; Parm1
	push	word ptr es:[edi+28]	; Parm2
	push	word ptr es:[edi+24]	; Parm3
	push	dword ptr [bp-16]	; Parm4Alias
	push	dword ptr [bp-8]	; Parm5Alias
	jmp	Free16Alias
__Thunk194 endp

_TEXT   ends
        end
