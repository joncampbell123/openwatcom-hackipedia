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
            ctl_rint            d1;
            ctl_info      d2;
        } d3;
    } d4;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_text            d1;
            ctl_info      d2;
        } d3;
    } d5;
} Ctl_setscr = {
6,
{ CTL_CHECK, SETSCR_JUMPYSCROLL, FALSE,offsetof( dlg_data, JumpyScroll ) },
{ CTL_CHECK, SETSCR_LINEBASED, FALSE,offsetof( dlg_data, LineBased ) },
{ CTL_CHECK, SETSCR_SAVEPOSITION, FALSE,offsetof( dlg_data, SavePosition ) },
{ CTL_CHECK, SETSCR_AUTOMESSAGECLEAR, FALSE,offsetof( dlg_data, AutoMessageClear ) },
{ CTL_RINT, SETSCR_PAGELINESEXPOSED, FALSE,offsetof( dlg_data, PageLinesExposed ) , 0, -1 },
{ CTL_TEXT, SETSCR_FILEENDSTRING, FALSE,offsetof( dlg_data, FileEndString ) , FILEENDSTRINGWIDTH },
};

