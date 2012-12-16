/*
 *  shlguid.h   Shell GUIDs
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

#ifndef _ENABLE_AUTODEPEND
 #pragma read_only_file;
#endif

#include <isguids.h>
#include <exdisp.h>
#include <shldisp.h>
#ifndef RC_INVOKED
    #include <knownfolders.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* GUIDs */
/* Some of these are also in shobjidl.h. */
EXTERN_C const CLSID    CLSID_NetworkDomain;
EXTERN_C const CLSID    CLSID_NetworkServer;
EXTERN_C const CLSID    CLSID_NetworkShare;
EXTERN_C const CLSID    CLSID_MyComputer;
EXTERN_C const CLSID    CLSID_Internet;
EXTERN_C const CLSID    CLSID_RecycleBin;
EXTERN_C const CLSID    CLSID_ControlPanel;
EXTERN_C const CLSID    CLSID_Printers;
EXTERN_C const CLSID    CLSID_MyDocuments;
EXTERN_C const CLSID    CLSID_PictureVideoLibrary;
EXTERN_C const CLSID    CLSID_MusicLibrary;
EXTERN_C const CLSID    CLSID_DocumentLibrary;
#if (_WIN32_IE >= 0x0400)
EXTERN_C const GUID     CATID_BrowsableShellExt;
EXTERN_C const GUID     CATID_BrowseInPlace;
EXTERN_C const GUID     CATID_DeskBand;
EXTERN_C const GUID     CATID_InfoBand;
EXTERN_C const GUID     CATID_CommBand;
#endif
EXTERN_C const FMTID    FMTID_Intshcut;
EXTERN_C const FMTID    FMTID_InternetSite;
EXTERN_C const GUID     CGID_Explorer;
EXTERN_C const GUID     CGID_ShellDocView;
#if (_WIN32_IE >= 0x0400)
EXTERN_C const GUID     CGID_ShellServiceObject;
EXTERN_C const GUID     CGID_ExplorerBarDoc;
#endif
EXTERN_C const IID      IID_INewShortcutHookA;
EXTERN_C const IID      IID_IShellBrowser;
EXTERN_C const IID      IID_IShellView;
EXTERN_C const IID      IID_IContextMenu;
EXTERN_C const IID      IID_IShellIcon;
EXTERN_C const IID      IID_IShellFolder;
EXTERN_C const IID      IID_IShellExtInit;
EXTERN_C const IID      IID_IShellPropSheetExt;
EXTERN_C const IID      IID_IPersistFolder;
EXTERN_C const IID      IID_IExtractIconA;
EXTERN_C const IID      IID_IShellDetails;
EXTERN_C const IID      IID_IShellLinkA;
EXTERN_C const IID      IID_ICopyHookA;
EXTERN_C const IID      IID_IFileViewerA;
EXTERN_C const IID      IID_ICommDlgBrowser;
EXTERN_C const IID      IID_IEnumIDList;
EXTERN_C const IID      IID_IFileViewerSite;
EXTERN_C const IID      IID_IContextMenu2;
EXTERN_C const IID      IID_IShellExecuteHook;
EXTERN_C const IID      IID_IPropSheetPage;
EXTERN_C const IID      IID_INewShortcutHookW;
EXTERN_C const IID      IID_IFileViewerW;
EXTERN_C const IID      IID_IShellLinkW;
EXTERN_C const IID      IID_IExtractIconW;
EXTERN_C const IID      IID_IShellExecuteHookW;
EXTERN_C const IID      IID_ICopyHookW;
EXTERN_C const IID      IID_IRemoteComputer;
#if (_WIN32_IE >= 0x0400)
EXTERN_C const IID      IID_IQueryInfo;
#endif
EXTERN_C const IID      IID_IBriefcaseStg;
EXTERN_C const IID      IID_IShellView2;
#if (_WIN32_IE >= 0x0400)
EXTERN_C const IID      IID_IURLSearchHook;
EXTERN_C const IID      IID_ISearchContext;
EXTERN_C const IID      IID_IURLSearchHook2;
EXTERN_C const IID      IID_IDefViewID;
#endif
#if (_WIN32_IE >= 0x0500)
EXTERN_C const CLSID    CLSID_FolderShortcut;
#endif
#if (_WIN32_IE >= 0x0400)
EXTERN_C const IID      IID_IDockingWindowSite;
EXTERN_C const IID      IID_IDockingWindowFrame;
EXTERN_C const IID      IID_IShellIconOverlay;
EXTERN_C const IID      IID_IShellIconOverlayIdentifier;
EXTERN_C const IID      IID_ICommDlgBrowser2;
EXTERN_C const IID      IID_IShellFolderViewCB;
EXTERN_C const CLSID    CLSID_CFSIconOverlayManager;
EXTERN_C const IID      IID_IShellIconOverlayManager;
EXTERN_C const IID      IID_IThumbnailCapture;
#endif
#if (_WIN32_IE >= 0x0500)
EXTERN_C const IID      IID_IShellImageStore;
#if (NTDDI_VERSION < 0x06000000)
EXTERN_C const CLSID    CLSID_ShellThumbnailDiskCache;
#endif
#endif
#if (_WIN32_IE >= 0x0400)
EXTERN_C const IID      IID_IContextMenu3;
#endif
EXTERN_C const GUID     SID_DefView;
EXTERN_C const GUID     CGID_DefView;
EXTERN_C const CLSID    CLSID_MenuBand;
EXTERN_C const IID      IID_IShellFolderBand;
EXTERN_C const IID      IID_IDefViewFrame;
EXTERN_C const GUID     VID_LargeIcons;
EXTERN_C const GUID     VID_SmallIcons;
EXTERN_C const GUID     VID_List;
EXTERN_C const GUID     VID_Details;
EXTERN_C const GUID     VID_Tile;
EXTERN_C const GUID     VID_Thumbnails;
EXTERN_C const GUID     VID_ThumbStrip;
#if (_WIN32_IE >= 0x0400)
EXTERN_C const IID      IID_IDiscardableBrowserProperty;
EXTERN_C const IID      IID_IShellChangeNotify;
EXTERN_C const CLSID    CLSID_CUrlHistory;
EXTERN_C const CLSID    CLSID_CURLSearchHook;
EXTERN_C const IID      IID_IObjMgr;
EXTERN_C const IID      IID_IACList;
EXTERN_C const IID      IID_IACList2;
EXTERN_C const IID      IID_ICurrentWorkingDirectory;
EXTERN_C const CLSID    CLSID_AutoComplete;
EXTERN_C const CLSID    CLSID_ACLHistory;
EXTERN_C const CLSID    CLSID_ACListISF;
EXTERN_C const CLSID    CLSID_ACLMRU;
EXTERN_C const CLSID    CLSID_ACLMulti;
#endif
#if (_WIN32_IE >= 0x0600)
EXTERN_C const CLSID    CLSID_ACLCustomMRU;
#endif
#if (_WIN32_IE >= 0x0500)
EXTERN_C const CLSID    CLSID_ProgressDialog;
EXTERN_C const IID      IID_IProgressDialog;
#endif
#if (_WIN32_IE >= 0x0400)
EXTERN_C const GUID     SID_STopLevelBrowser;
#endif
EXTERN_C const CLSID    CLSID_FileTypes;
#if (_WIN32_IE >= 0x0400)
EXTERN_C const CLSID    CLSID_ActiveDesktop;
EXTERN_C const IID      IID_IActiveDesktop;
EXTERN_C const IID      IID_IActiveDesktopP;
EXTERN_C const IID      IID_IADesktopP2;
EXTERN_C const IID      IID_ISynchronizedCallBack;
EXTERN_C const IID      IID_IQueryAssociations;
EXTERN_C const CLSID    CLSID_QueryAssociations;
EXTERN_C const IID      IID_IColumnProvider;
EXTERN_C const CLSID    CLSID_LinkColumnProvider;
EXTERN_C const GUID     CGID_ShortCut;
EXTERN_C const IID      IID_INamedPropertyBag;
EXTERN_C const CLSID    CLSID_InternetButtons;
#endif
#if (_WIN32_IE >= 0x0500)
EXTERN_C const CLSID    CLSID_MSOButtons;
EXTERN_C const CLSID    CLSID_ToolbarExtButtons;
#endif
#if (_WIN32_IE >= 0x0400)
EXTERN_C const CLSID    CLSID_DarwinAppPublisher;
EXTERN_C const CLSID    CLSID_DocHostUIHandler;
#endif
#if (_WIN32_IE >= 0x0500)
EXTERN_C const IID      IID_IShellFolder2;
EXTERN_C const FMTID    FMTID_ShellDetails;
EXTERN_C const FMTID    FMTID_Storage;
EXTERN_C const FMTID    FMTID_ImageProperties;
EXTERN_C const FMTID    FMTID_CustomImageProperties;
EXTERN_C const FMTID    FMTID_Displaced;
EXTERN_C const FMTID    FMTID_Briefcase;
EXTERN_C const FMTID    FMTID_Misc;
EXTERN_C const FMTID    FMTID_WebView;
EXTERN_C const FMTID    FMTID_MUSIC;
EXTERN_C const FMTID    FMTID_DRM;
EXTERN_C const FMTID    FMTID_Volume;
EXTERN_C const FMTID    FMTID_Query;
EXTERN_C const IID      IID_IEnumExtraSearch;
EXTERN_C const CLSID    CLSID_HWShellExecute;
EXTERN_C const CLSID    CLSID_DragDropHelper;
EXTERN_C const CLSID    CLSID_CAnchorBrowsePropertyPage;
EXTERN_C const CLSID    CLSID_CImageBrowsePropertyPage;
EXTERN_C const CLSID    CLSID_CDocBrowsePropertyPage;
EXTERN_C const IID      IID_IFileSystemBindData;
EXTERN_C const GUID     SID_STopWindow;
EXTERN_C const GUID     SID_SGetViewFromViewDual;
EXTERN_C const CLSID    CLSID_FolderItem;
EXTERN_C const CLSID    CLSID_FolderItemsMultiLevel;
EXTERN_C const CLSID    CLSID_NewMenu;
EXTERN_C const GUID     BHID_SFObject;
EXTERN_C const GUID     BHID_SFUIObject;
EXTERN_C const GUID     BHID_SFViewObject;
EXTERN_C const GUID     BHID_Storage;
EXTERN_C const GUID     BHID_Stream;
EXTERN_C const GUID     BHID_LinkTargetItem;
EXTERN_C const GUID     BHID_StorageEnum;
EXTERN_C const GUID     BHID_Transfer;
EXTERN_C const GUID     BHID_PropertyStore;
EXTERN_C const GUID     BHID_ThumbnailHandler;
EXTERN_C const GUID     BHID_EnumItems;
EXTERN_C const GUID     BHID_DataObject;
EXTERN_C const GUID     BHID_AssociationArray;
EXTERN_C const GUID     BHID_Filter;
#endif
#if (_WIN32_IE >= 0x0600)
EXTERN_C const GUID     SID_CtxQueryAssociations;
#endif
EXTERN_C const IID      IID_IDocViewSite;
EXTERN_C const CLSID    CLSID_QuickLinks;
EXTERN_C const CLSID    CLSID_ISFBand;
EXTERN_C const IID      IID_CDefView;
EXTERN_C const CLSID    CLSID_ShellFldSetExt;
EXTERN_C const GUID     SID_SMenuBandChild;
EXTERN_C const GUID     SID_SMenuBandParent;
EXTERN_C const GUID     SID_SMenuPopup;
EXTERN_C const GUID     SID_SMenuBandBottomSelected;
EXTERN_C const GUID     SID_SMenuBandBottom;
EXTERN_C const GUID     SID_MenuShellFolder;
EXTERN_C const GUID     SID_SMenuBandContextMenuModifier;
EXTERN_C const GUID     SID_SMenuBandBKContextMenu;
EXTERN_C const GUID     CGID_MENUDESKBAR;
EXTERN_C const GUID     SID_SMenuBandTop;
EXTERN_C const CLSID    CLSID_MenuToolbarBase;
EXTERN_C const IID      IID_IBanneredBar;
EXTERN_C const CLSID    CLSID_MenuBandSite;
EXTERN_C const GUID     SID_SCommDlgBrowser;
EXTERN_C const GUID     CPFG_LOGON_USERNAME;
EXTERN_C const GUID     CPFG_LOGON_PASSWORD;
EXTERN_C const GUID     CPFG_SMARTCARD_USERNAME;
EXTERN_C const GUID     CPFG_SMARTCARD_PIN;
EXTERN_C const GUID     FOLDERTYPEID_NotSpecified;
EXTERN_C const GUID     FOLDERTYPEID_Invalid;
EXTERN_C const GUID     FOLDERTYPEID_Documents;
EXTERN_C const GUID     FOLDERTYPEID_Pictures;
EXTERN_C const GUID     FOLDERTYPEID_MusicDetails;
EXTERN_C const GUID     FOLDERTYPEID_MusicIcons;
EXTERN_C const GUID     FOLDERTYPEID_Games;
EXTERN_C const GUID     FOLDERTYPEID_ControlPanelCategory;
EXTERN_C const GUID     FOLDERTYPEID_ControlPanelClassic;
EXTERN_C const GUID     FOLDERTYPEID_Printers;
EXTERN_C const GUID     FOLDERTYPEID_RecycleBin;
EXTERN_C const GUID     FOLDERTYPEID_SoftwareExplorer;
EXTERN_C const GUID     FOLDERTYPEID_CompressedFolder;
EXTERN_C const GUID     FOLDERTYPEID_Contacts;
EXTERN_C const GUID     FOLDERTYPEID_Library;
EXTERN_C const GUID     FOLDERTYPEID_NetworkExplorer;
EXTERN_C const GUID     FOLDERTYPEID_UserFiles;
EXTERN_C const GUID     SYNCMGR_OBJECTID_Icon;
EXTERN_C const GUID     SYNCMGR_OBJECTID_EventStore;
EXTERN_C const GUID     SYNCMGR_OBJECTID_ConflictStore;
EXTERN_C const GUID     SYNCMGR_OBJECTID_BrowseContent;
EXTERN_C const GUID     SYNCMGR_OBJECTID_ShowSchedule;
EXTERN_C const GUID     SYNCMGR_OBJECTID_QueryBeforeActivate;
EXTERN_C const GUID     SYNCMGR_OBJECTID_QueryBeforeDeactivate;
EXTERN_C const GUID     SYNCMGR_OBJECTID_QueryBeforeEnable;
EXTERN_C const GUID     SYNCMGR_OBJECTID_QueryBeforeDisable;
EXTERN_C const GUID     SYNCMGR_OBJECTID_QueryBeforeDelete;
EXTERN_C const GUID     SYNCMGR_OBJECTID_EventLinkClick;
EXTERN_C const GUID     EP_NavPane;
EXTERN_C const GUID     EP_Commands;
EXTERN_C const GUID     EP_Commands_Organize;
EXTERN_C const GUID     EP_Commands_View;
EXTERN_C const GUID     EP_DetailsPane;
EXTERN_C const GUID     EP_PreviewPane;
EXTERN_C const GUID     EP_QueryPane;
EXTERN_C const GUID     EP_AdvQueryPane;

/* Aliases */
#define IID_IShellCopyHookA IID_ICopyHookA
#define IID_IShellCopyHookW IID_ICopyHookW

/* Map generic GUID names to the appropriate ANSI or Unicode version. */
#ifdef UNICODE
    #define IID_IExtractIcon        IID_IExtractIconW
    #define IID_IFileViewer         IID_IFileViewerW
    #define IID_INewShortcutHook    IID_INewShortcutHookW
    #define IID_IShellCopyHook      IID_IShellCopyHookW
    #define IID_IShellExecuteHook   IID_IShellExecuteHookW
    #define IID_IShellLink          IID_IShellLinkW
#else
    #define IID_IExtractIcon        IID_IExtractIconA
    #define IID_IFileViewer         IID_IFileViewerA
    #define IID_INewShortcutHook    IID_INewShortcutHookA
    #define IID_IShellCopyHook      IID_IShellCopyHookA
    #define IID_IShellExecuteHook   IID_IShellExecuteHookA
    #define IID_IShellLink          IID_IShellLinkA
#endif

/* String GUIDs */
#define STR_MYDOCS_CLSID    "{450D8FBA-AD25-11D0-98A8-0800361BB1103}"

/* Property set GUIDs */
#define PSGUID_INTERNETSHORTCUT \
    { 0x000214A0, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 }
#define PSGUID_INTERNETSITE \
    { 0x000214A1, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 }
#if (_WIN32_IE >= 0x0500)
    #define PSGUID_SHELLDETAILS \
        { 0x28636AA6, 0x953D, 0x11D2, 0xB5, 0xD6, 0x00, 0xC0, 0x4F, 0xD9, 0x18, 0xD0 }
    #define PSGUID_IMAGEPROPERTIES \
        { 0x14B81DA1, 0x0135, 0x4D31, 0x96, 0xD9, 0x6C, 0xBF, 0xC9, 0x67, 0x1A, 0x99 }
    #define PSGUID_CUSTOMIMAGEPROPERTIES \
        { 0x7ECD8B0E, 0xC136, 0x4A9B, 0x94, 0x11, 0x4E, 0xBD, 0x66, 0x73, 0xCC, 0xC3 }
    #define PSGUID_DISPLACED \
        { 0x9B174B33, 0x40FF, 0x11D2, 0xA2, 0x7E, 0x00, 0xC0, 0x4F, 0xC3, 0x08, 0x71 }
    #define PSGUID_BRIEFCASE \
        { 0x328D8B21, 0x7729, 0x4BFC, 0x95, 0x4C, 0x90, 0x2B, 0x32, 0x9D, 0x56, 0xB0 }
    #define PSGUID_MISC \
        { 0x9B174B34, 0x40FF, 0x11D2, 0xA2, 0x7E, 0x00, 0xC0, 0x4F, 0xC3, 0x08, 0x71 }
    #define PSGUID_WEBVIEW \
        { 0xF2275480, 0xF782, 0x4291, 0xBF, 0x94, 0xF1, 0x36, 0x93, 0x51, 0x3A, 0xEC }
    #define PSGUID_MUSIC \
        { 0x56A3372E, 0xCE9C, 0x11D2, 0x9F, 0x0E, 0x00, 0x60, 0x97, 0xC6, 0x86, 0xF6 }
    #define PSGUID_DRM \
        { 0xAEAC19E4, 0x89AE, 0x4508, 0xB9, 0xB7, 0xBB, 0x86, 0x7A, 0xBE, 0xE2, 0xED }
    #define PSGUID_VIDEO \
        { 0x64440491, 0x4C8B, 0x11D1, 0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03 }
    #define PSGUID_AUDIO \
        { 0x64440490, 0x4C8B, 0x11D1, 0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03 }
    #define PSGUID_CONTROLPANEL \
        { 0x305CA226, 0xD286, 0x468E, 0xB8, 0x48, 0x2B, 0x2E, 0x8E, 0x69, 0x7B, 0x74 }
    #define PSGUID_VOLUME \
        { 0x9B174B35, 0x40FF, 0x11D2, 0xA2, 0x7E, 0x00, 0xC0, 0x4F, 0xC3, 0x08, 0x71 }
    #define PSGUID_SHARE \
        { 0xD8C3986F, 0x813B, 0x449C, 0x84, 0x5D, 0x87, 0xB9, 0x5D, 0x67, 0x4A, 0xDE }
    #define PSGUID_LINK \
        { 0xB9B4B3FC, 0x2B51, 0x4A42, 0xB5, 0xD8, 0x32, 0x41, 0x46, 0xAF, 0xCF, 0x25 }
    #define PSGUID_QUERY_D \
        { 0x49691C90, 0x7E17, 0x101A, 0xA9, 0x1C, 0x08, 0x00, 0x2B, 0x2E, 0xCD, 0xA9 }
    #define PSGUID_SUMMARYINFORMATION \
        { 0xF29F85E0, 0x4FF9, 0x1068, 0xAB, 0x91, 0x08, 0x00, 0x2B, 0x27, 0xB3, 0xD9 }
    #define PSGUID_DOCUMENTSUMMARYINFORMATION \
        { 0xD5CDD502, 0x2E9C, 0x101B, 0x93, 0x97, 0x08, 0x00, 0x2B, 0x2C, 0xF9, 0xAE }
    #define PSGUID_MEDIAFILESUMMARYINFORMATION \
        { 0x64440492, 0x4C8B, 0x11D1, 0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03 }
    #define PSGUID_IMAGESUMMARYINFORMATION \
        { 0x6444048F, 0x4C8B, 0x11D1, 0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03 }
#endif

/* Property identifiers */
#if (_WIN32_IE >= 0x0500)
    #define PID_FINDDATA                0
    #define PID_NETRESOURCE             1
    #define PID_DESCRIPTIONID           2
    #define PID_WHICHFOLDER             3
    #define PID_NETWORKLOCATION         4
    #define PID_COMPUTERNAME            5
    #define PID_DISPLACED_FROM          2
    #define PID_DISPLACED_DATE          3
    #define PID_SYNC_COPY_IN            2
    #define PID_MISC_STATUS             2
    #define PID_MISC_ACCESSCOUNT        3
    #define PID_MISC_OWNER              4
    #define PID_HTMLINFOTIPFILE         5
    #define PID_MISC_PICS               6
    #define PID_DISPLAY_PROPERTIES      0
    #define PID_INTROTEXT               1
    #define PIDSI_ARTIST                2
    #define PIDSI_SONGTITLE             3
    #define PIDSI_ALBUM                 4
    #define PIDSI_YEAR                  5
    #define PIDSI_COMMENT               6
    #define PIDSI_TRACK                 7
    #define PIDSI_GENRE                 11
    #define PIDSI_LYRICS                12
    #define PIDDRSI_PROTECTED           2
    #define PIDDRSI_DESCRIPTION         3
    #define PIDDRSI_PLAYCOUNT           4
    #define PIDDRSI_PLAYSTARTS          5
    #define PIDDRSI_PLAYEXPIRES         6
    #define PIDVSI_STREAM_NAME          2
    #define PIDVSI_FRAME_WIDTH          3
    #define PIDVSI_FRAME_HEIGHT         4
    #define PIDVSI_FRAME_COUNT          5
    #define PIDVSI_FRAME_RATE           6
    #define PIDVSI_TIMELENGTH           7
    #define PIDVSI_DATA_RATE            8
    #define PIDVSI_SAMPLE_SIZE          9
    #define PIDVSI_COMPRESSION          10
    #define PIDVSI_STREAM_NUMBER        11
    #define PIDASI_FORMAT               2
    #define PIDASI_TIMELENGTH           3
    #define PIDASI_AVG_DATA_RATE        4
    #define PIDASI_SAMPLE_RATE          5
    #define PIDASI_SAMPLE_SIZE          6
    #define PIDASI_CHANNEL_COUNT        7
    #define PIDASI_STREAM_NUMBER        8
    #define PIDASI_STREAM_NAME          9
    #define PIDASI_COMPRESSION          10
    #define PID_CONTROLPANEL_CATEGORY   2
    #define PID_VOLUME_FREE             2
    #define PID_VOLUME_CAPACITY         3
    #define PID_VOLUME_FILESYSTEM       4
    #define PID_SHARE_CSC_STATUS        2
    #define PID_LINK_TARGET             2
    #define PID_LINK_TARGET_TYPE        3
    #define PID_QUERY_RANK              2
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
