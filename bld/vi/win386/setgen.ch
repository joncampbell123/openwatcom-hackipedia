/**** DO NOT MODIFY THIS FILE BY HAND. CREATED BY PARSECTL ****/


struct {
    int            num_ctls;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
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
            ctl_info      d2;
        } d3;
    } d9;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_text            d1;
            ctl_info      d2;
        } d3;
    } d10;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_text            d1;
            ctl_info      d2;
        } d3;
    } d11;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_rint            d1;
            ctl_info      d2;
        } d3;
    } d12;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_text            d1;
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
} Ctl_setgen = {
15,
{ CTL_CHECK, SETGEN_UNDO, FALSE,offsetof( dlg_data, Undo ) },
{ CTL_CHECK, SETGEN_AUTOSAVE, FALSE,offsetof( dlg_data, AutoSave ) },
{ CTL_CHECK, SETGEN_SAVECONFIG, FALSE,offsetof( dlg_data, SaveConfig ) },
{ CTL_CHECK, SETGEN_SAVEONBUILD, FALSE,offsetof( dlg_data, SaveOnBuild ) },
{ CTL_CHECK, SETGEN_BEEPFLAG, FALSE,offsetof( dlg_data, BeepFlag ) },
{ CTL_CHECK, SETGEN_QUITMOVESFORWARD, FALSE,offsetof( dlg_data, QuitMovesForward ) },
{ CTL_CHECK, SETGEN_SAMEFILECHECK, FALSE,offsetof( dlg_data, SameFileCheck ) },
{ CTL_CHECK, SETGEN_MODAL, FALSE,offsetof( dlg_data, Modal ) },
{ CTL_CHECK, SETGEN_CASEIGNORE, FALSE,offsetof( dlg_data, CaseIgnore ) },
{ CTL_CHECK, SETGEN_SEARCHWRAP, FALSE,offsetof( dlg_data, SearchWrap ) },
{ CTL_TEXT, SETGEN_WORD, FALSE,offsetof( dlg_data, Word ) , WORDWIDTH },
{ CTL_TEXT, SETGEN_WORDALT, FALSE,offsetof( dlg_data, WordAlt ) , WORDWIDTH },
{ CTL_RINT, SETGEN_AUTOSAVEINTERVAL, FALSE,offsetof( dlg_data, AutoSaveInterval ) , 1, -1 },
{ CTL_TEXT, SETGEN_TMPDIR, FALSE,offsetof( dlg_data, TmpDir ) , TMPDIRWIDTH },
{ CTL_TEXT, SETGEN_HISTORYFILE, FALSE,offsetof( dlg_data, HistoryFile ) , HISTORYFILEWIDTH },
};

