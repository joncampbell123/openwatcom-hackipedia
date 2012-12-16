/**** DO NOT MODIFY THIS FILE BY HAND. CREATED BY PARSECTL ****/


struct {
    int            num_ctls;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_combo            d1;
            ctl_info      d2;
        } d3;
    } d0;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d1;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d2;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d3;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d4;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d5;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d6;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d7;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d8;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_rint            d1;
            ctl_info      d2;
        } d3;
    } d9;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d10;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_rint            d1;
            ctl_info      d2;
        } d3;
    } d11;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d12;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_rint            d1;
            ctl_info      d2;
        } d3;
    } d13;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_text            d1;
            ctl_info      d2;
        } d3;
    } d14;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d15;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d16;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_text            d1;
            ctl_info      d2;
        } d3;
    } d17;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d18;
} Ctl_setfs = {
19,
{ CTL_COMBO, SETFS_LANGUAGESELECT, FALSE,offsetof( dlg_data, Language ) , 0, VI_LANG_FIRST, VI_LANG_LAST },
{ CTL_CHECK, SETFS_PPKEYWORDONLY, FALSE,offsetof( dlg_data, PPKeywordOnly ) },
{ CTL_CHECK, SETFS_CMODE, FALSE,offsetof( dlg_data, CMode ) },
{ CTL_CHECK, SETFS_READENTIREFILE, FALSE,offsetof( dlg_data, ReadEntireFile ) },
{ CTL_CHECK, SETFS_READONLYCHECK, FALSE,offsetof( dlg_data, ReadOnlyCheck ) },
{ CTL_CHECK, SETFS_IGNORECTRLZ, FALSE,offsetof( dlg_data, IgnoreCtrlZ ) },
{ CTL_CHECK, SETFS_CRLFAUTODETECT, FALSE,offsetof( dlg_data, CRLFAutoDetect ) },
{ CTL_CHECK, SETFS_WRITECRLF, FALSE,offsetof( dlg_data, WriteCRLF ) },
{ CTL_CHECK, SETFS_EIGHTBITS, FALSE,offsetof( dlg_data, EightBits ) },
{ CTL_RINT, SETFS_TABAMOUNT, FALSE,offsetof( dlg_data, TabAmount ) , 0, -1 },
{ CTL_CHECK, SETFS_REALTABS, FALSE,offsetof( dlg_data, RealTabs ) },
{ CTL_RINT, SETFS_HARDTAB, FALSE,offsetof( dlg_data, HardTab ) , 0, -1 },
{ CTL_CHECK, SETFS_AUTOINDENT, FALSE,offsetof( dlg_data, AutoIndent ) },
{ CTL_RINT, SETFS_SHIFTWIDTH, FALSE,offsetof( dlg_data, ShiftWidth ) , 0, -1 },
{ CTL_TEXT, SETFS_TAGFILENAME, FALSE,offsetof( dlg_data, TagFileName ) , TAGFILENAMEWIDTH },
{ CTL_CHECK, SETFS_IGNORETAGCASE, FALSE,offsetof( dlg_data, IgnoreTagCase ) },
{ CTL_CHECK, SETFS_TAGPROMPT, FALSE,offsetof( dlg_data, TagPrompt ) },
{ CTL_TEXT, SETFS_GREPDEFAULT, FALSE,offsetof( dlg_data, GrepDefault ) , GREPDEFAULTWIDTH },
{ CTL_CHECK, SETFS_SHOWMATCH, FALSE,offsetof( dlg_data, ShowMatch ) },
};

