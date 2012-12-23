/*
 *  schemdef.h / schemadef.h    Schema definition macros
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

#ifndef SCHEMADEF_H
#define SCHEMADEF_H

#ifndef _ENABLE_AUTODEPEND
 #pragma read_only_file;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Schema definition version */
#define SCHEMADEF_VERSION   1

/* Theme Manager schema definition macros */
#define BEGIN_TM_SCHEMA( x )
#define BEGIN_TM_PROPS()            enum PropValues { DummyProp = 49,
#define BEGIN_TM_ENUM( x )          enum x {
#define BEGIN_TM_CLASS_PARTS( x )   enum x##PARTS { x##PartFiller0,
#define BEGIN_TM_PART_STATES( x )   enum x##STATES { x##StateFiller0,
#define TM_PROP( p1, p2, p3, p4 )   p2##_##p3 = (p1),
#define TM_ENUM( p1, p2, p3 )       p2##_##p3 = (p1),
#define TM_PART( p1, p2, p3 )       p2##_##p3 = (p1),
#define TM_STATE( p1, p2, p3 )      p2##_##p3 = (p1),
#define END_TM_CLASS_PARTS()        };
#define END_TM_PART_STATES()        };
#define END_TM_PROPS()              };
#define END_TM_ENUM()               };
#define END_TM_SCHEMA( x )

/* Theme Manager property information */
struct TMPROPINFO {
    LPCWSTR pszName;
    SHORT   sEnumVal;
    BYTE    bPrimVal;
};

/* Theme Manager schema information */
struct TMSCHEMAINFO {
    DWORD                   dwSize;
    int                     iSchemaDefVersion;
    int                     iThemeMgrVersion;
    int                     iPropCount;
    const struct TMPROPINFO *pPropTable;
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SCHEMADEF_H */
