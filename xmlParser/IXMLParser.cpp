/** \file
 ****************************************************************************
 * <P> IXMLParser.cpp - implementation file for basic XML parser written in 
 * ANSI C++for portability. It works by using recursion and a node tree for 
 * breaking down the elements of an XML document.</P>
 *
 * @version     V3.17
 * @author      Frank Vanden Berghen
 *
 * NOTE:
 *
 *   If you add "#define STRICT_PARSING", on the first line of this file
 *   the parser will see the following XML-stream:
 *      <a><b>some text</b><b>other text    </a>
 *   as an error. Otherwise, this string will be equivalent to:
 *      <a><b>some text</b><b>other text</b></a>
 *
 * NOTE:
 *
 *   If you add "#define APPROXIMATE_PARSING" on the first line of this file
 *   the parser will see the following XML-stream:
 *     <data name="n1">
 *     <data name="n2">
 *     <data name="n3" />
 *   as equivalent to the following XML-stream:
 *     <data name="n1" />
 *     <data name="n2" />
 *     <data name="n3" />
 *   This can be useful for badly-formed XML-streams but prevent the use
 *   of the following XML-stream (problem is: tags at contiguous levels
 *   have the same names):
 *     <data name="n1">
 *        <data name="n2">
 *            <data name="n3" />
 *        </data>
 *     </data>
 *
 * NOTE:
 *
 *   If you add "#define _XMLPARSER_NO_MESSAGEBOX_" on the first line of this file
 *   the "openFileHelper" function will always display error messages inside the
 *   console instead of inside a message-box-window. Message-box-windows are
 *   available on windows 9x/NT/2000/XP/Vista/Win7/Win8 only.
 *
 * Copyright (c) 2013, Frank Vanden Berghen - All rights reserved.<br/>
 * See the file <a href="../../AFPL-license.txt">AFPL-license.txt</a> about the licensing terms<br/>
 *
 ****************************************************************************
 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "IXMLParser.h"
#ifdef _XMLWINDOWS
//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // to have IsTextUnicode, MultiByteToWideChar, WideCharToMultiByte to handle unicode files
                     // to have "MessageBoxA" to display error messages for openFilHelper
#endif
#include <wchar.h>

#include <memory.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined( _UNICODE ) && defined( UNDER_CE )
  #undef WCHAR_MAX
  #define WCHAR_MAX 0xffff
#endif

//static char removeCommentsInMiddleOfText=1;

// You can modify the initialization of the variable "IXMLClearTags" below
// to change the clearTags that are currently recognized by the library.
// The number on the second columns is the length of the string inside the
// first column.
// The "<!DOCTYPE" declaration must be the second in the list.
// The "<!--" declaration must be the third in the list.
// All ClearTag Strings must start with the '<' character.
typedef struct { IXMLCStr lpcOpen; int openTagLenMinus1; IXMLCStr lpcClose; int closeTagLen; } ALLIXMLClearTag;
static ALLIXMLClearTag IXMLClearTags[] =
{
    {    _CXML("<![CDATA["),8,  _CXML("]]>")      ,3},
    {    _CXML("<!DOCTYPE"),8,  _CXML(">")        ,1},
    {    _CXML("<!--")     ,3,  _CXML("-->")      ,3},
//    {    _CXML("<![")      ,2,  _CXML("-->")      ,3},
    {    _CXML("<PRE>")    ,4,  _CXML("</PRE>")   ,6},
    {    _CXML("<script")  ,6,  _CXML("</script>"),9}, // GKochaniak, changed length of </script> to 9
    {    _CXML("<Script")  ,6,  _CXML("</Script>"),9}, // else I'm getting extra > > characters when parsing...
    {    _CXML("<!")       ,1,  _CXML(">")        ,1},
    {    NULL              ,0,  NULL           }
};
static int maxIXMLClearTagsOpenTagLen=8; // the maximum of IXMLClearTags[i].openTagLenMinus1 for all i

// You can modify the initialization of the variable "XMLEntities" below
// to change the character entities that are currently recognized by the library.
// The number on the second columns is the length of the string inside the
// first column. Additionally, the syntaxes "&#xA0;" and "&#160;" are recognized.
// The first "&" character must be removed.
typedef struct { IXMLCStr s; int lengthMinus1; IXMLChar c;} IXMLCharacterEntity;
static IXMLCharacterEntity XMLEntities[] =
{
    { _CXML("amp;" ), 4, _CXML('&' )},
    { _CXML("lt;"  ), 3, _CXML('<' )},
    { _CXML("gt;"  ), 3, _CXML('>' )},
    { _CXML("quot;"), 5, _CXML('\"')},
    { _CXML("apos;"), 5, _CXML('\'')},
    { _CXML("nbsp;"), 5, _CXML(' ')},
    { NULL          , 0, _CXML('\0')}
};
static int maxXMLEntitiesLen=5; // the maximum of XMLEntities[i].lengthMinus1 for all i

// When rendering the IXMLNode to a string (using the "IXMLRenderer::getString()" 
// function), you can ask for a beautiful formatting. This formatting is using the
// following indentation character:
#define INDENTCHAR _CXML('\t')

/////////////////////////////////////////////////////////////////////////
//      Here start the abstraction layer to be OS-independent          //
/////////////////////////////////////////////////////////////////////////

// Here is an abstraction layer to access some common string manipulation functions.
// The abstraction layer is currently working for gcc, Microsoft Visual Studio 6.0,
// Microsoft Visual Studio .NET, CC (sun compiler) and Borland C++.
// If you plan to "port" the library to a new system/compiler, all you have to do is
// to edit the following lines.
#ifdef XML_NO_WIDE_CHAR
char myIsTextWideChar(const void *b, int len) { return FALSE; }
#else
    #if defined (UNDER_CE) || !defined(_XMLWINDOWS)
    char myIsTextWideChar(const void *b, int len) // inspired by the Wine API: RtlIsTextUnicode
    {
#ifdef sun
        // for SPARC processors: wchar_t* buffers must always be alligned, otherwise it's a char* buffer.
        if ((((unsigned long)b)%sizeof(wchar_t))!=0) return FALSE;
#endif
        const wchar_t *s=(const wchar_t*)b;

        // buffer too small:
        if (len<(int)sizeof(wchar_t)) return FALSE;

        // odd length test
        if (len&1) return FALSE;

        /* only checks the first 256 bytes */
        if (len>(int)(256/sizeof(wchar_t))) len=256/sizeof(wchar_t);

        // Check for the special byte order:
        if (*((unsigned short*)s) == 0xFFFE) return TRUE;     // IS_TEXT_UNICODE_REVERSE_SIGNATURE;
        if (*((unsigned short*)s) == 0xFEFF) return TRUE;      // IS_TEXT_UNICODE_SIGNATURE

        // checks for ASCII characters in the UNICODE stream
        int i,stats=0;
        for (i=0; i<len; i++) if (s[i]<=(unsigned short)255) stats++;
        if (stats>len/2) return TRUE;

        return FALSE;
    }
    #else
    char myIsTextWideChar(const void *b,int l) { return (char)IsTextUnicode((CONST LPVOID)b,l,NULL); }
    #endif
#endif

#ifdef _XMLWINDOWS
// for Microsoft Visual Studio 6.0 and Microsoft Visual Studio .NET and Borland C++ Builder 6.0
    #ifdef _XMLWIDECHAR
        wchar_t *myMultiByteToWideChar(const char *s, IXMLPullParser::IXMLCharEncoding ce)
        {
            int i;
            if (ce==IXMLPullParser::char_encoding_UTF8) i=(int)MultiByteToWideChar(CP_UTF8,0             ,s,-1,NULL,0);
            else                                        i=(int)MultiByteToWideChar(CP_ACP ,MB_PRECOMPOSED,s,-1,NULL,0);
            if (i<0) return NULL;
            wchar_t *d=(wchar_t *)malloc((i+1)*sizeof(IXMLChar));
            if (ce==IXMLPullParser::char_encoding_UTF8) i=(int)MultiByteToWideChar(CP_UTF8,0             ,s,-1,d,i);
            else                                        i=(int)MultiByteToWideChar(CP_ACP ,MB_PRECOMPOSED,s,-1,d,i);
            d[i]=0;
            return d;
        }
        static inline FILE *xfopen(IXMLCStr filename,IXMLCStr mode) { return _wfopen(filename,mode); }
        static inline int xstrlen(IXMLCStr c)   { return (int)wcslen(c); }
        static inline int xstrnicmp(IXMLCStr c1, IXMLCStr c2, int l) { return _wcsnicmp(c1,c2,l);}
        static inline int xstrncmp(IXMLCStr c1, IXMLCStr c2, int l) { return wcsncmp(c1,c2,l);}
        static inline int xstricmp(IXMLCStr c1, IXMLCStr c2) { return _wcsicmp(c1,c2); }
        static inline IXMLStr xstrstr(IXMLCStr c1, IXMLCStr c2) { return (IXMLStr)wcsstr(c1,c2); }
        static inline IXMLStr xstrcpy(IXMLStr c1, IXMLCStr c2) { return (IXMLStr)wcscpy(c1,c2); }
    #else
/*        char *myWideCharToMultiByte(const wchar_t *s)
        {
            UINT codePage=CP_ACP; if (characterEncoding==IXMLNode::char_encoding_UTF8) codePage=CP_UTF8;
            int i=(int)WideCharToMultiByte(codePage,  // code page
                0,                       // performance and mapping flags
                s,                       // wide-character string
                -1,                       // number of chars in string
                NULL,                       // buffer for new string
                0,                       // size of buffer
                NULL,                    // default for unmappable chars
                NULL                     // set when default char used
                );
            if (i<0) return NULL;
            char *d=(char*)malloc(i+1);
            WideCharToMultiByte(codePage,  // code page
                0,                       // performance and mapping flags
                s,                       // wide-character string
                -1,                       // number of chars in string
                d,                       // buffer for new string
                i,                       // size of buffer
                NULL,                    // default for unmappable chars
                NULL                     // set when default char used
                );
            d[i]=0;
            return d;
        }
        }*/
        static inline FILE *xfopen(IXMLCStr filename,IXMLCStr mode) { return fopen(filename,mode); }
        static inline int xstrlen(IXMLCStr c)   { return (int)strlen(c); }
        #ifdef __BORLANDC__
            static inline int xstrnicmp(IXMLCStr c1, IXMLCStr c2, int l) { return strnicmp(c1,c2,l);}
            static inline int xstricmp(IXMLCStr c1, IXMLCStr c2) { return stricmp(c1,c2); }
        #else
            static inline int xstrnicmp(IXMLCStr c1, IXMLCStr c2, int l) { return _strnicmp(c1,c2,l);}
            static inline int xstricmp(IXMLCStr c1, IXMLCStr c2) { return _stricmp(c1,c2); }
        #endif
        static inline int xstrncmp(IXMLCStr c1, IXMLCStr c2, int l) { return strncmp(c1,c2,l);}
        static inline IXMLStr xstrstr(IXMLCStr c1, IXMLCStr c2) { return (IXMLStr)strstr(c1,c2); }
        static inline IXMLStr xstrcpy(IXMLStr c1, IXMLCStr c2) { return (IXMLStr)strcpy(c1,c2); }
    #endif
#else
// for gcc and CC
    #ifdef XML_NO_WIDE_CHAR
        char *myWideCharToMultiByte(const wchar_t *s) { return NULL; }
    #else
        char *myWideCharToMultiByte(const wchar_t *s)
        {
            const wchar_t *ss=s;
            int i=(int)wcsrtombs(NULL,&ss,0,NULL);
            if (i<0) return NULL;
            char *d=(char *)malloc(i+1);
            wcsrtombs(d,&s,i,NULL);
            d[i]=0;
            return d;
        }
    #endif
    #ifdef _XMLWIDECHAR
        wchar_t *myMultiByteToWideChar(const char *s, IPullParser::IXMLCharEncoding ce)
        {
            const char *ss=s;
            int i=(int)mbsrtowcs(NULL,&ss,0,NULL);
            if (i<0) return NULL;
            wchar_t *d=(wchar_t *)malloc((i+1)*sizeof(wchar_t));
            mbsrtowcs(d,&s,i,NULL);
            d[i]=0;
            return d;
        }
        int xstrlen(IXMLCStr c)   { return wcslen(c); }
        #ifdef sun
        // for CC
           #include <widec.h>
           static inline int xstrnicmp(IXMLCStr c1, IXMLCStr c2, int l) { return wsncasecmp(c1,c2,l);}
           static inline int xstrncmp(IXMLCStr c1, IXMLCStr c2, int l) { return wsncmp(c1,c2,l);}
           static inline int xstricmp(IXMLCStr c1, IXMLCStr c2) { return wscasecmp(c1,c2); }
        #else
        static inline int xstrncmp(IXMLCStr c1, IXMLCStr c2, int l) { return wcsncmp(c1,c2,l);}
            #ifdef __linux__
            // for gcc/linux
            static inline int xstrnicmp(IXMLCStr c1, IXMLCStr c2, int l) { return wcsncasecmp(c1,c2,l);}
            static inline int xstricmp(IXMLCStr c1, IXMLCStr c2) { return wcscasecmp(c1,c2); }
            #else
            #include <wctype.h>
            // for gcc/non-linux (MacOS X 10.3, FreeBSD 6.0, NetBSD 3.0, OpenBSD 3.8, AIX 4.3.2, HP-UX 11, IRIX 6.5, OSF/1 5.1, Cygwin, mingw)
            static inline int xstricmp(IXMLCStr c1, IXMLCStr c2)
            {
                wchar_t left,right;
                do
                {
                    left=towlower(*c1++); right=towlower(*c2++);
                } while (left&&(left==right));
                return (int)left-(int)right;
            }
            static inline int xstrnicmp(IXMLCStr c1, IXMLCStr c2, int l)
            {
                wchar_t left,right;
                while(l--)
                {
                    left=towlower(*c1++); right=towlower(*c2++);
                    if ((!left)||(left!=right)) return (int)left-(int)right;
                }
                return 0;
            }
            #endif
        #endif
        static inline IXMLStr xstrstr(IXMLCStr c1, IXMLCStr c2) { return (IXMLStr)wcsstr(c1,c2); }
        static inline IXMLStr xstrcpy(IXMLStr c1, IXMLCStr c2) { return (IXMLStr)wcscpy(c1,c2); }
        static inline FILE *xfopen(IXMLCStr filename,IXMLCStr mode)
        {
            char *filenameAscii=myWideCharToMultiByte(filename);
            FILE *f;
            if (mode[0]==_CXML('r')) f=fopen(filenameAscii,"rb");
            else                     f=fopen(filenameAscii,"wb");
            free(filenameAscii);
            return f;
        }
    #else
        static inline FILE *xfopen(IXMLCStr filename,IXMLCStr mode) { return fopen(filename,mode); }
        static inline int xstrlen(IXMLCStr c)   { return strlen(c); }
        static inline int xstrnicmp(IXMLCStr c1, IXMLCStr c2, int l) { return strncasecmp(c1,c2,l);}
        static inline int xstrncmp(IXMLCStr c1, IXMLCStr c2, int l) { return strncmp(c1,c2,l);}
        static inline int xstricmp(IXMLCStr c1, IXMLCStr c2) { return strcasecmp(c1,c2); }
        static inline IXMLStr xstrstr(IXMLCStr c1, IXMLCStr c2) { return (IXMLStr)strstr(c1,c2); }
        static inline IXMLStr xstrcpy(IXMLStr c1, IXMLCStr c2) { return (IXMLStr)strcpy(c1,c2); }
    #endif
    static inline int _strnicmp(const char *c1,const char *c2, int l) { return strncasecmp(c1,c2,l);}
#endif

///////////////////////////////////////////////////////////////////////////////
//            the "xmltoc,xmltob,xmltoi,xmltol,xmltof,xmltoa" functions      //
///////////////////////////////////////////////////////////////////////////////
// These 6 functions are not used inside the XMLparser.
// There are only here as "convenience" functions for the user.
// If you don't need them, you can delete them without any trouble.
#ifndef NO_XMLTO_FUNCTIONS
#ifdef _XMLWIDECHAR
    #ifdef _XMLWINDOWS
    // for Microsoft Visual Studio 6.0 and Microsoft Visual Studio .NET and Borland C++ Builder 6.0
        char      xmltob(IXMLCStr t,char      v){ if (t&&(*t)) return (char)_wtoi(t); return v; }
        int       xmltoi(IXMLCStr t,int       v){ if (t&&(*t)) return _wtoi(t); return v; }
        long long xmltol(IXMLCStr t,long long v){ if (t&&(*t)) return _wtoi64(t); return v; }
        double    xmltof(IXMLCStr t,double    v){ if (t&&(*t)) swscanf(t, L"%lf", &v); /*v=_wtof(t);*/ return v; }
    #else
        #ifdef sun
        // for CC
           #include <widec.h>
           char    xmltob(IXMLCStr t,char    v){ if (t) return (char)wstol(t,NULL,10); return v; }
           int     xmltoi(IXMLCStr t,int     v){ if (t) return (int)wstol(t,NULL,10); return v; }
           long long xmltol(IXMLCStr t,long long v){ if (t) return wstol(t,NULL,10); return v; }
        #else
        // for gcc
           char    xmltob(IXMLCStr t,char    v){ if (t) return (char)wcstol(t,NULL,10); return v; }
           int     xmltoi(IXMLCStr t,int     v){ if (t) return (int)wcstol(t,NULL,10); return v; }
           long long xmltol(IXMLCStr t,long long v){ if (t) return wcstol(t,NULL,10); return v; }
        #endif
		double  xmltof(IXMLCStr t,double  v){ if (t&&(*t)) swscanf(t, L"%lf", &v); /*v=_wtof(t);*/ return v; }
    #endif
#else
    #ifdef _XMLWINDOWS
       long long xmltol(IXMLCStr t,long long v){ if (t&&(*t)) return _atoi64(t); return v; }
    #else
       long long xmltol(IXMLCStr t,long long v){ if (t&&(*t)) return atoll(t); return v; }
    #endif
    char      xmltob(IXMLCStr t,char      v){ if (t&&(*t)) return (char)atoi(t); return v; }
    int       xmltoi(IXMLCStr t,int       v){ if (t&&(*t)) return atoi(t); return v; }
    double    xmltof(IXMLCStr t,double    v){ if (t&&(*t)) return atof(t); return v; }
#endif
IXMLCStr xmltoa(IXMLCStr t,      IXMLCStr v){ if (t)       return  t; return v; }
IXMLChar xmltoc(IXMLCStr t,const IXMLChar v){ if (t&&(*t)) return *t; return v; }
#endif

#ifdef _XMLWIDECHAR
    #ifdef _XMLWINDOWS
        int internal_xmltoi(IXMLCStr t,int v){ if (t&&(*t)) return _wtoi(t); return v; }
    #else
        #ifdef sun
        // for CC
           #include <widec.h>
           int internal_xmltoi(IXMLCStr t,int v){ if (t) return (int)wstol(t,NULL,10); return v; }
        #else
        // for gcc
           int internal_xmltoi(IXMLCStr t,int v){ if (t) return (int)wcstol(t,NULL,10); return v; }
        #endif
    #endif
#else
    int internal_xmltoi(IXMLCStr t,int v){ if (t&&(*t)) return atoi(t); return v; }
#endif

/////////////////////////////////////////////////////////////////////////
//                    the "openFileHelper" function                    //
/////////////////////////////////////////////////////////////////////////

// Since each application has its own way to report and deal with errors, you should modify & rewrite
// the following "openFileHelper" function to get an "error reporting mechanism" tailored to your needs.
ITCXMLNode IXMLDomParser::openFileHelper(IXMLCStr filename, IXMLCStr tag)
{
    // parse the file
    IXMLResults pResults;
    ITCXMLNode xnode=parseFile(filename,tag,&pResults);

    // display error message (if any)
    if (pResults.errorCode != IXMLError_None)
    {
        // create message
        char message[2000],*s1=(char*)"",*s3=(char*)""; IXMLCStr s2=_CXML("");
        if (pResults.errorCode==IXMLError_FirstTagNotFound) { s1=(char*)"First Tag should be '"; s2=tag; s3=(char*)"'.\n"; }
#ifdef _XMLWINDOWS
        _snprintf(message,2000,
#else
        snprintf(message,2000,
#endif		 
#ifdef _XMLWIDECHAR
            "XML Parsing error inside file '%S'.\n%S\nAt line %lld, column %i.\n%s%S%s"
#else
            "XML Parsing error inside file '%s'.\n%s\nAt line %lld, column %i.\n%s%s%s"
#endif
            ,filename,IXMLPullParser::getErrorMessage(pResults.errorCode),pResults.nLine,pResults.nColumn,s1,s2,s3);

        // display message
#if defined(_XMLWINDOWS) && !defined(UNDER_CE) && !defined(_XMLPARSER_NO_MESSAGEBOX_)
        MessageBoxA(NULL,message,"XML Parsing error",MB_OK|MB_ICONERROR|MB_TOPMOST);
#else
        printf("%s",message);
#endif
        exit(255);
    }
    return xnode;
}

ICXMLNode IXMLDomParser::openFileHelperNonT(IXMLCStr filename, IXMLCStr tag)
{
    return openFileHelper(filename,tag).deepCopyConstant();
}

//////////////////////////////////////////////////////////////////////////////////
//      Here start the core implementation of the Incredible XMLParser library    //
//////////////////////////////////////////////////////////////////////////////////

// You should normally not change anything below this point.

// If "characterEncoding=ascii" then we assume that all characters have the same length of 1 byte.
// If "characterEncoding=UTF8" then the characters have different lengths (from 1 byte to 4 bytes).
// If "characterEncoding=ShiftJIS" then the characters have different lengths (from 1 byte to 2 bytes).
// This table is used as lookup-table to know the length of a character (in byte) based on the
// content of the first byte of the character.
// (note: if you modify this, you must always have XML_utf8ByteTable[0]=0 ).
static const char XML_utf8ByteTable[256] =
{
    //  0 1 2 3 4 5 6 7 8 9 a b c d e f
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x00
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x10
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x20
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x30
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x40
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x50
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x60
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x70 End of ASCII range
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x80 0x80 to 0xc1 invalid
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x90
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0xa0
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0xb0
    1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xc0 0xc2 to 0xdf 2 byte
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xd0
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,// 0xe0 0xe0 to 0xef 3 byte
    4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1 // 0xf0 0xf0 to 0xf4 4 byte, 0xf5 and higher invalid
};
#ifndef _XMLWIDECHAR
static const char XML_legacyByteTable[256] =
{
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};
static const char XML_sjisByteTable[256] =
{
    //  0 1 2 3 4 5 6 7 8 9 a b c d e f
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x00
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x10
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x20
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x30
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x40
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x50
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x60
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x70
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0x80 0x81 to 0x9F 2 bytes
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0x90
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0xa0
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0xb0
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0xc0
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0xd0
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xe0 0xe0 to 0xef 2 bytes
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 // 0xf0
};
static const char XML_gb2312ByteTable[256] =
{
//  0 1 2 3 4 5 6 7 8 9 a b c d e f
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x00
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x10
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x20
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x30
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x40
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x50
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x60
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x70
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x80
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x90
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xa0 0xa1 to 0xf7 2 bytes
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xb0
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xc0
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xd0
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xe0
    2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1 // 0xf0
};
static const char XML_gbk_big5_ByteTable[256] =
{
    //  0 1 2 3 4 5 6 7 8 9 a b c d e f
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x00
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x10
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x20
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x30
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x40
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x50
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x60
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x70
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0x80 0x81 to 0xfe 2 bytes
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0x90
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xa0
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xb0
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xc0
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xd0
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xe0
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1 // 0xf0
};
#endif

ITCXMLNode ITCXMLNode::emptyITCXMLNode;
ICXMLNode ICXMLNode::emptyICXMLNode;
IXMLNode IXMLNode::emptyIXMLNode;
IXMLClear ITCXMLNode::emptyIXMLClear={ NULL, NULL, NULL,0};
IXMLAttr ITCXMLNode::emptyXMLAttribute={ NULL, NULL,0};

/************************************************************************/
/*                       Some generic "Tool" functions                  */
/************************************************************************/

IXMLCStr IPullParser::getVersion() { return _CXML("v3.17"); }
void freeIXMLString(IXMLStr t){if(t)free(t);}
inline int mmin( const int t1, const int t2 ) { return t1 < t2 ? t1 : t2; }
inline int mmax( const int t1, const int t2 ) { return t1 > t2 ? t1 : t2; }

static inline void myFree(void *p) { if (p) free(p); }
static inline void *myRealloc(void *p, int newsize, int sizeofElem)
{
    if (!p)  return malloc(sizeofElem); 
    p=realloc(p,newsize*sizeofElem);
    //    if (!p)
    //    {
    //        printf("XMLParser Error: Not enough memory! Aborting...\n"); exit(220);
    //    }
    return p;
}

static inline void charmemset(IXMLStr dest,IXMLChar c,int l) { while (l--) *(dest++)=c; }
static inline int xstrncpy(IXMLStr dest, IXMLCStr src, int n) 
{ 
    int i;
    for(i=0;i<n;i++)
    {
        if (!src[i]) { dest[i]=0; return i; }
        dest[i]=src[i];
    }
    return n;
}

// Duplicate a given string.
IXMLStr IXMLStringDup(IXMLCStr lpcData, int cbData)
{
    if (lpcData==NULL) return NULL;
    if (cbData==-1) cbData=(int)xstrlen(lpcData);
    IXMLStr lpcNew=(IXMLStr)malloc((cbData+1)*sizeof(IXMLChar));
    if (lpcNew)
    {
        memcpy(lpcNew, lpcData, cbData*sizeof(IXMLChar));
        lpcNew[cbData] = (IXMLChar)0;
    }
    return lpcNew;
}

/************************************************************************/
/*                                                                      */
/*                          ToXMLStringTool                             */
/*                                                                      */
/************************************************************************/

ToIXMLStringTool::ToIXMLStringTool(IXMLStringLocation loc):
#ifdef _XMLWIDECHAR
buf(NULL),buflen(0){ setLocation(loc);}
#else
XML_ByteTable(XML_utf8ByteTable),characterEncoding(IXMLPullParser::char_encoding_UTF8),buf(NULL),buflen(0){setLocation(loc);}

char ToIXMLStringTool::setCharEncoding(IXMLPullParser::IXMLCharEncoding encoding)
{
    switch(encoding)
    {
    case IXMLPullParser::char_encoding_UTF8:     characterEncoding=encoding; XML_ByteTable=XML_utf8ByteTable; return 0;
    case IXMLPullParser::char_encoding_Legacy:   characterEncoding=encoding; XML_ByteTable=XML_legacyByteTable; return 0;
    case IXMLPullParser::char_encoding_ShiftJIS: characterEncoding=encoding; XML_ByteTable=XML_sjisByteTable; return 0;
    case IXMLPullParser::char_encoding_GB2312:   characterEncoding=encoding; XML_ByteTable=XML_gb2312ByteTable; return 0;
    case IXMLPullParser::char_encoding_Big5:     characterEncoding=encoding; XML_ByteTable=XML_gbk_big5_ByteTable; return 0;
    case IXMLPullParser::char_encoding_GBK:      characterEncoding=encoding; XML_ByteTable=XML_gbk_big5_ByteTable; return 0;
    }
    return 1;
}
IXMLPullParser::IXMLCharEncoding ToIXMLStringTool::getCharEncoding()
{
    return characterEncoding;
}
#endif

void ToIXMLStringTool::setLocation(IXMLStringLocation _loc)
{
    static IXMLCharacterEntity XMLEntities_InAttribute[] =
    {
        { _CXML("amp;" ), 4, _CXML('&' )},
        { _CXML("quot;"), 5, _CXML('\"')},
        { _CXML("apos;"), 5, _CXML('\'')},
        { NULL          , 0, _CXML('\0')}
    };
    static IXMLCharacterEntity XMLEntities_inText[] =
    {
        { _CXML("amp;" ), 4, _CXML('&' )},
        { _CXML("lt;"  ), 3, _CXML('<' )},
        { NULL          , 0, _CXML('\0')}
    };

    loc=_loc;
    switch(_loc)
    {
    case IXMLStringLocation_unknown:     curXMLEntities=XMLEntities; break;
    case IXMLStringLocation_inAttribute: curXMLEntities=XMLEntities_InAttribute; break;
    case IXMLStringLocation_inText:      curXMLEntities=XMLEntities_inText; break;
    }
}
ToIXMLStringTool::IXMLStringLocation ToIXMLStringTool::getLocation(){return loc;}

IXMLStr ToIXMLStringTool::toXMLUnSafe(IXMLStr dest,IXMLCStr source)
{
    IXMLStr dd=dest;
    IXMLChar ch;
    IXMLCharacterEntity *entity,*entityTable=(IXMLCharacterEntity*)curXMLEntities;
#ifdef _XMLWIDECHAR
    while ((ch=*source))
#else
    const char *XML_ByteTable=this->XML_ByteTable;
    while ((ch=*((unsigned char*)source)))
#endif
    {
        entity=entityTable;
        do
        {
            if (ch==entity->c) { *dest=_CXML('&'); dest++;  xstrcpy(dest,entity->s); dest+=entity->lengthMinus1; source++; goto out_of_loop1; }
            entity++;
        } while(entity->s);
#ifdef _XMLWIDECHAR
        *(dest++)=*(source++);
#else
#define COPY_ONE_XML_CHAR(ll,dest,source)                                                                               \
        switch(ll)                                                                                                      \
        {                                                                                                               \
        case 1: *(dest++)=*(source++); break;                                                                           \
        case 2:                                                                                                         \
            {                                                                                                           \
                if (!(source[1])) { *(dest++)='_'; source++; break; }                                                   \
                *dest=*source; dest[1]=source[1]; dest+=2; source+=2; break;                                            \
            }                                                                                                           \
        case 3:                                                                                                         \
            {                                                                                                           \
                if ((!(source[1]))||(!(source[2]))) { *(dest++)='_'; source++; break; }                                 \
                *dest=*source; dest[1]=source[1]; dest[2]=source[2]; dest+=3; source+=3;break;                          \
            }                                                                                                           \
        case 4:                                                                                                         \
            {                                                                                                           \
                if ((!(source[1]))||(!(source[2]))||(!(source[3]))) { *(dest++)='_'; source++; break; }                 \
                *dest=*source; dest[1]=source[1]; dest[2]=source[2]; dest[3]=source[3]; dest+=4; source+=4;break;       \
            }                                                                                                           \
        }

        COPY_ONE_XML_CHAR(XML_ByteTable[(unsigned char)ch],dest,source);
#endif
out_of_loop1:
        ;
    }
    *dest=0;
    return dd;
}

// private (used while rendering):
int ToIXMLStringTool::lengthXMLString(IXMLCStr source)
{
    int r=0;
    IXMLCharacterEntity *entity,*entityTable=(IXMLCharacterEntity*)curXMLEntities;
    IXMLChar ch;
#ifdef _XMLWIDECHAR
    while ((ch=*source))
#else
    const char *XML_ByteTable=this->XML_ByteTable;
    while ((ch=*((unsigned char*)source)))
#endif
    {
        entity=entityTable;
        do
        {
            if (ch==entity->c) { r+=entity->lengthMinus1+1; source++; goto out_of_loop1; }
            entity++;
        } while(entity->s);
#ifdef _XMLWIDECHAR
        r++; source++;
#else
        ch=XML_ByteTable[(unsigned char)ch]; r+=ch; source+=ch;
#endif
out_of_loop1:
        ;
    }
    return r;
}

ToIXMLStringTool::~ToIXMLStringTool(){ clear(); }
void ToIXMLStringTool::clear(){ if (buf) free(buf); buf=NULL; buflen=0; }
IXMLStr ToIXMLStringTool::toXML(IXMLCStr source)
{
    if (!source)
    {
		if (buflen<1) { buflen=1; buf=(IXMLStr)malloc(sizeof(IXMLChar)); }
		*buf=0;
		return buf;
	}
    int l=lengthXMLString(source)+1;
    if (l>buflen) { clear(); buflen=l; buf=(IXMLStr)malloc(l*sizeof(IXMLChar)); }
    return toXMLUnSafe(buf,source);
}

/************************************************************************/
/*                    Childs of IXMLReader                              */
/************************************************************************/

IXMLReaderFile::IXMLReaderFile(IXMLCStr filename)
{
    f=xfopen(filename,_CXML("rb"));
}
IXMLReaderFile::~IXMLReaderFile()
{
    if (f) fclose((FILE*)f);
}
int IXMLReaderFile::read(unsigned char *buffer,int nbyte)
{
    if (!f) return -1;
    return (int)fread(buffer,1,nbyte,(FILE*)f);
}
void IXMLReaderFile::reset()
{
    if (f) fseek((FILE*)f,0,SEEK_SET);
}
IXMLReaderStringSZ::IXMLReaderStringSZ(IXMLCStr _b)
{
    b=(unsigned char*)_b;
    bend=(unsigned char*)(b+((xstrlen(_b)+1)*sizeof(IXMLChar)));
}
int IXMLReaderStringSZ::read(unsigned char *buffer,int nbyte)
{
    int l=mmin(nbyte,(int)(bend-b));
    memcpy(buffer,b,l);
    b+=l;
    return l;
}

/************************************************************************/
/*                                                                      */
/*                         IPullParser                                  */
/*                                                                      */
/************************************************************************/

IPullParser::IPullParser(IXMLReader *_uReader,int _szByteBuffer):
uReader(_uReader), sourceUReader(_uReader), allAttributes(NULL),_isInArray(false),_isHTML(false),
#ifdef _XMLWIDECHAR
autoDetectSource(true),
#if (WCHAR_MAX>70000)
characterEncoding(char_encoding_UTF32),sourceEncoding(char_encoding_UTF32),
#else
characterEncoding(char_encoding_UTF16),sourceEncoding(char_encoding_UTF16),
#endif
#else
autoDetectSource(false),characterEncoding(char_encoding_UTF8),sourceEncoding(char_encoding_UTF8),XML_ByteTable(XML_utf8ByteTable),
#endif
skipNames(NULL),szSkipNameBuffer(0),
charEncodingConverterToWideChar(_uReader), charEncodingConverterFromUTF16(_uReader),
charEncodingConverterFromUTF32(_uReader), bufferChar(NULL),isStreamFinished(false)
{
    reset();
    szByteBuffer=mmax(1024,_szByteBuffer);
}

void IPullParser::setReader(IXMLReader *_uReader)
{
    uReader=_uReader; sourceUReader=_uReader;
    charEncodingConverterToWideChar.r=_uReader;
    charEncodingConverterFromUTF16.r=_uReader;
    charEncodingConverterFromUTF32.r=_uReader;
}

char IPullParser::setBufferSize(int _szByteBuffer)
{
    if (curToken!=IXMLToken_StartDocument) return 1;
    clear();
    szByteBuffer=mmax(1024,_szByteBuffer);
    return 0;
}

void IPullParser::reset()
{
    curToken=IXMLToken_StartDocument;
    _isEmptyNode=false;
    lastError=IXMLError_None;
    depth=0;
    nLine=1;
    pContent = NULL; // GKochaniak, important to initialize, else we may read random memory
    _nAttribute=0;
    isStreamFinished=false;
}

void IPullParser::clear()
{
    if (bufferChar) { free(bufferChar); bufferChar=NULL; }
    if (skipNames) { free(skipNames); skipNames=NULL; szSkipNameBuffer=0; }
}

IXMLCStr IPullParser::getAttribute(IXMLCStr s) const
{
    int i=_nAttribute;
    while(i--)
    {
        if (!xstricmp(allAttributes[i].sName,s)) return allAttributes[i].sValue;
    }
    return NULL;
}

void IPullParser::setHTML(bool isHTML){ _isHTML=isHTML; }
bool IPullParser::isHTML() const { return _isHTML;}
IPullParser::IXMLToken IPullParser::currentToken() const { return curToken; }
IXMLErrorInfo IPullParser::getErrorCode() const { return lastError; }
int IPullParser::getColumnNumber() const { return 0; }
long long IPullParser::getLineNumber() const { return nLine; }
int IPullParser::getDepth() const { return depth; }
IXMLCStr IPullParser::getName() const { return pContent; }
IXMLCStr IPullParser::getText() const { return pContent; }
bool IPullParser::isEmptyNode() const { return _isEmptyNode; }
bool IPullParser::isArrayStart() const { return _isInArray; }
int IPullParser::nAttribute() const { return _nAttribute; }
IXMLAttr *IPullParser::getAttribute(int i) const { return allAttributes+i; }
IXMLCStr IPullParser::getAttributeName(int i) const { return allAttributes[i].sName; }
IXMLCStr IPullParser::getAttributeValue(int i) const { return allAttributes[i].sValue; }
int IPullParser::szAttributeValue(int i) const { return allAttributes[i].szValue; }
const IXMLClear *IPullParser::getClear() const{ return &curClearTag; }
int IPullParser::szText() const { return _szText; }
bool IPullParser::isTextWhiteSpace() const { return _isIgnorableSpace; }

/************************************************************************/
/*       IXMLPullParser: filters to convert to/from UTF16               */
/************************************************************************/

static const int byteMask = 0xBF;
static const int byteMark = 0x80;
static const unsigned int UNI_REPLACEMENT_CHAR=0x0000FFFD;
static const unsigned int halfShift=10; /* used for shifting by 10 bits */
static const unsigned int halfBase = 0x0010000UL;
static const unsigned int UNI_SUR_HIGH_START= 0xD800;
static const unsigned int UNI_SUR_HIGH_END   =0xDBFF;
static const unsigned int UNI_SUR_LOW_START  =0xDC00;
static const unsigned int UNI_SUR_LOW_END    =0xDFFF;
static const unsigned short TRAIL_SURROGATE_MIN = 0xdc00u;
static const unsigned short LEAD_OFFSET         = 0xd800u - (0x10000 >> 10);

IPullParser::IXMLReader_FromUTF16::IXMLReader_FromUTF16(IXMLReader *_r)
:r(_r),destEncoding(char_encoding_UTF8),mbstate(0),len(0),b(NULL){}

IPullParser::IXMLReader_FromUTF16::~IXMLReader_FromUTF16()
{
    if (b) free(b);
}

void IPullParser::IXMLReader_FromUTF16::setSource(IXMLReader *_source){ r=_source; }
IXMLReader *IPullParser::IXMLReader_FromUTF16::getSource(){return r;}

void IPullParser::IXMLReader_FromUTF16::setNormalBufferSize(int nbyte)
{
    int i=nbyte/2;
    if ((i%2)) i--;
    if (bufferSz==i) return;
    bufferSz=i;
    if (b) free(b);
    b=(unsigned char*)malloc(bufferSz);
}
void IPullParser::IXMLReader_FromUTF16::reset()
{
    len=0; mbstate=0;
}

int IPullParser::IXMLReader_FromUTF16::read(unsigned char *target,int nbyte)
{
    if (!b) setNormalBufferSize(nbyte);
    if ((nbyte%2)) nbyte-=nbyte%2;
    len=r->read(b,nbyte);
    if (len<=0) return len;
    if (destEncoding==char_encoding_UTF8)
    {
        const unsigned short *source=(unsigned short*)b,
                             *sourceEnd=(unsigned short*)(b+len);
        unsigned char *targetStart=target;

        unsigned int ch;
        while(source<sourceEnd)
        {

#define GET_UTF32_FROM_UTF16(ch,source)                                                                         \
            ch = *(source++);                                                                                   \
            if ((ch >= UNI_SUR_HIGH_START) && (ch <= UNI_SUR_HIGH_END))                                         \
            {                                                                                                   \
                unsigned int ch2 = *source;                                                                     \
                if (ch2&&((ch2 >= UNI_SUR_LOW_START) && (ch2 <= UNI_SUR_LOW_END)))                              \
                {                                                                                               \
                    ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;       \
                    ++source;                                                                                   \
                }                                                                                               \
            }

#define ENCODE_IN_UTF8(ch,target)                                                               \
            {                                                                                   \
                if (ch < (unsigned int)0x80)                                                    \
                {                                                                               \
                    *target = (unsigned char)(ch);                                              \
                    target++;                                                                   \
                } else if (ch < (unsigned int)0x800)                                            \
                {                                                                               \
                    target[1] = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;          \
                    *target   = (unsigned char)(ch | 0xC0);                                     \
                    target+=2;                                                                  \
                } else if (ch < (unsigned int)0x10000)                                          \
                {                                                                               \
                    target[2] = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;          \
                    target[1] = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;          \
                    *target   = (unsigned char)(ch | 0xE0);                                     \
                    target+=3;                                                                  \
                } else if (ch < (unsigned int)0x110000)                                         \
                {                                                                               \
                    target[3] = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;          \
                    target[2] = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;          \
                    target[1] = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;          \
                    *target   = (unsigned char)(ch | 0xF0);                                     \
                    target+=4;                                                                  \
                } else                                                                          \
                {                                                                               \
                    ch = UNI_REPLACEMENT_CHAR;                                                  \
                    target[2] = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;          \
                    target[1] = (unsigned char)((ch | byteMark) & byteMask); ch >>= 6;          \
                    target[0] = (unsigned char)(ch | 0xE0);                                     \
                    target+=3;                                                                  \
                }                                                                               \
            }

            GET_UTF32_FROM_UTF16(ch,source);
            if (!ch)
            {
                *target=0;
                target++;
                break;
            } else ENCODE_IN_UTF8(ch,target);

        }
        return (int)(target-targetStart);
    }
//     if (destEncoding==char_encoding_UTF32)
//     {
//         const unsigned short *source=(unsigned short*)b,
//                              *sourceEnd=(unsigned short*)(b+len);
//         unsigned int ch,*curr=(unsigned int*)target;
//         while(source<sourceEnd)
//         {
//             GET_UTF32_FROM_UTF16(ch,source);
//             if (!ch)
//             {
//                 *curr=0;
//                 curr++;
//                 break;
//             } else *curr++=ch;
//         }
//         return ((unsigned char*)curr)-target;
//     }
    if (destEncoding==char_encoding_Legacy)
    {
        const wchar_t *bc=(const wchar_t*)b;
        return (int)wcsrtombs((char*)target,&bc,len,(mbstate_t*)&mbstate);
    }
    // other encoding conversions are not coded yet
    return 0;
}

IPullParser::IXMLReader_FromUTF32::IXMLReader_FromUTF32(IXMLReader *r):
IXMLReader_FromUTF16(r){}

int IPullParser::IXMLReader_FromUTF32::read(unsigned char *target,int nbyte)
{
    if (!b) setNormalBufferSize(nbyte);
    if ((nbyte%4)) nbyte-=nbyte%4;
    len=r->read(b,nbyte);
    if (len<=0) return len;
    if (destEncoding==char_encoding_UTF8)
    {
        const unsigned int *source=(unsigned int*)b,
                           *sourceEnd=(unsigned int*)(b+len);
        unsigned char *targetStart=target;
        unsigned int ch;
        while(source<sourceEnd)
        {
            ch=*source; source++;
            if (!ch)
            {
                *target=0;
                target++;
                break;
            } else ENCODE_IN_UTF8(ch,target);
        }
        return (int)(target-targetStart);
    }

#define ENCODE_IN_UTF16(codepoint,curr)                                                 \
            if (codepoint > 0xffff)                                                     \
            {                                                                           \
                *curr++ = (unsigned short)((codepoint>>10)|LEAD_OFFSET);                \
                *curr++ = (unsigned short)((codepoint & 0x3ff)|TRAIL_SURROGATE_MIN);    \
            } else                                                                      \
            {                                                                           \
                *curr++ = (unsigned short)(codepoint);                                  \
            }
//     if (destEncoding==char_encoding_UTF16)
//     {
//         const unsigned int *source=(unsigned int*)b,
//                            *sourceEnd=(unsigned int*)(b+len);
//         unsigned int codepoint;
//         unsigned short *curr=(unsigned short*)target;
//         while(source<sourceEnd)
//         {
//             codepoint=*source; source++;
//             if (!codepoint)
//             {
//                 *curr=0;
//                 curr++;
//                 break;
//             } else ENCODE_IN_UTF16(codepoint,curr);
//         }
//         return ((unsigned char*)curr)-target;
//     }
    if (destEncoding==char_encoding_Legacy)
    {
        const wchar_t *bc=(const wchar_t*)b;
        return (int)wcsrtombs((char*)target,&bc,len,(mbstate_t*)&mbstate);
    }
    // other encoding conversions are not coded yet
    return 0;
}

IPullParser::IXMLReader_ToWideChar::IXMLReader_ToWideChar(IXMLReader *_r)
:r(_r),sourceEncoding(char_encoding_UTF8),mbstate(0),len(0),b(NULL)
{}

IPullParser::IXMLReader_ToWideChar::~IXMLReader_ToWideChar()
{
    if (b) free(b);
}

void IPullParser::IXMLReader_ToWideChar::setSource(IXMLReader *_source){ r=_source; }
IXMLReader *IPullParser::IXMLReader_ToWideChar::getSource(){return r;}

void IPullParser::IXMLReader_ToWideChar::setNormalBufferSize(int nbyte)
{
    if (bufferSz==nbyte/2) return;
    bufferSz=nbyte/2;
    if (b) free(b);
    b=(unsigned char*)malloc(bufferSz);
}
void IPullParser::IXMLReader_ToWideChar::reset()
{
    len=0; mbstate=0;
}
int IPullParser::IXMLReader_ToWideChar::read(unsigned char *dest,int nbyte)
{
    if (!b) setNormalBufferSize(nbyte);
    int i=r->read(b+len,nbyte/2-len);
    if (i<0) return -1;
    len+=i;
    if(!len) return 0;
    if (sourceEncoding==char_encoding_UTF8)
    {
        int codepoint=1;
        unsigned char lead;
        wchar_t *curr=(wchar_t*)dest;
        const unsigned char *s=(const unsigned char *)b,*ends=s+len; 
        char lc;
        while((s<ends)&&(codepoint))
        {
            lead = *((unsigned char*)s);
            lc=XML_utf8ByteTable[(unsigned char)lead];
            if (lc<=1){ codepoint=lead; s++; }
            else if (lc==2)
            { 
                if (s+1>=ends) break;
                codepoint=(((lead&0x1f)<<6)|((s[1])&0x3f)); 
                s+=2; 
            } else if (lc==3) 
            { 
                if (s+2>=ends) break;
                codepoint = (((lead&0x0f)<<12)|((s[1]&0x3f)<<6)|(s[2]&0x3f));
                s+=3; 
            } else if (lc==4) 
            {  
                if (s+3>=ends) break;
                codepoint = (((lead&0x07)<<18)|((s[1]&0x3f)<<12)|((s[2]&0x3f)<<6)|(s[3]&0x3f));
                s+=4;
            } else 
            { 
                codepoint=lead; s++; /* error detected; "s++" must stay here! */ 
            }

#if (WCHAR_MAX>70000)
            *curr++ = (unsigned int)(codepoint);
#else
            ENCODE_IN_UTF16(codepoint,curr)
#endif
        }
        len=(int)(ends-s);
        if (len) memmove(b,s,len);
        return (int)(((unsigned char*)curr)-dest);
    }
    if (sourceEncoding==char_encoding_UTF16)
    {
        // source = utf16 ; destination=utf32 ; sizeof(wchar_t)=4   or    wchar_t is utf32
        const unsigned short *source=(unsigned short*)b,
                             *sourceEnd=(unsigned short*)(b+len);
        unsigned int ch,*curr=(unsigned int*)dest;
        while(source<sourceEnd)
        {
            GET_UTF32_FROM_UTF16(ch,source);
            if (!ch)
            {
                *curr=0;
                curr++;
                break;
            } else *curr++=ch;
        }
        return (int)(((unsigned char*)curr)-dest);
    }
    if (sourceEncoding==char_encoding_UTF32)
    {
        // source=UTF32 ; destination=utf16 ; sizeof(wchar_t)=2   or    wchar_t is utf16
        const unsigned int *source=(unsigned int*)b,
                           *sourceEnd=(unsigned int*)(b+len);
        wchar_t *curr=(wchar_t*)dest;
        unsigned int ch;
        while(source<sourceEnd)
        {
            ch=*source; source++;
            if (!ch)
            {
                *curr=0;
                curr++;
                break;
            } else
            {
                ENCODE_IN_UTF16(ch,curr)
            }
        }
        return (int)(((unsigned char*)curr)-dest);
    }
    const char *bc=(const char*)b;
    int nwc=(int)mbsrtowcs((wchar_t*)dest,&bc,len,(mbstate_t*)&mbstate);
    len=0;
    return nwc*sizeof(IXMLChar);
}

IPullParser::IXMLReader_PassThrough::IXMLReader_PassThrough(IXMLReader *_r):r(_r),pos(b)
{ 
    b[0]=0; b[1]=0; 
    bEnd=b+r->read(b,512);
}
int IPullParser::IXMLReader_PassThrough::read(unsigned char *buffer,int nbyte)
{
    int n=(int)(bEnd-pos);
    if (n>0) memcpy(buffer,pos,n);
    return n;
}
void IPullParser::IXMLReader_PassThrough::skipBOM()
{
    if ((pos[0]==0xef)&&(pos[1]==0xff)) { pos+=2; return; }
    if ((pos[0]==0xff)&&(pos[1]==0xfe)) 
    {
        pos+=2;
        if ((pos[0]==0)&&(pos[1]==0)) pos+=2;
        return;
    }
    if ((pos[0]==0xef)&&(pos[1]==0xbb)&&(pos[2]==0xbf)) { pos+=3; return; }
    if ((pos[0]==0)&&(pos[1]==0)&&(pos[2]==0xfe)&&(pos[3]==0xff)) { pos+=4; return; }
}
int IPullParser::IXMLReader_PassThrough::nByteRead(){ return (int)(bEnd-b); }

void IPullParser::setAutoDetectSourceEncoding(bool _autoDetectSource) { autoDetectSource=_autoDetectSource;}
bool IPullParser::getAutoDetectSourceEncoding() { return autoDetectSource;}
#ifdef _XMLWIDECHAR
char IPullParser::setSourceCharEncoding(IXMLCharEncoding _characterEncoding)
{
    sourceEncoding=_characterEncoding;
#if (WCHAR_MAX>70000)
    if (_characterEncoding==char_encoding_UTF32) 
    {
        characterEncoding=char_encoding_UTF32; 
        uReader=sourceUReader;
        return 0;
    }
#else
    if (_characterEncoding==char_encoding_UTF16) 
    {
        characterEncoding=char_encoding_UTF16; 
        uReader=sourceUReader;
        return 0;
    }
#endif
    switch(_characterEncoding)
    {
    case char_encoding_UTF8:     characterEncoding=_characterEncoding; uReader=&charEncodingConverterToWideChar; charEncodingConverterToWideChar.sourceEncoding=char_encoding_UTF8; return 0;
    case char_encoding_UTF16:    characterEncoding=_characterEncoding; uReader=&charEncodingConverterToWideChar; charEncodingConverterToWideChar.sourceEncoding=char_encoding_UTF16; return 0;
    case char_encoding_UTF32:    characterEncoding=_characterEncoding; uReader=&charEncodingConverterToWideChar; charEncodingConverterToWideChar.sourceEncoding=char_encoding_UTF32; return 0;
    case char_encoding_Legacy:   characterEncoding=_characterEncoding; uReader=&charEncodingConverterToWideChar; charEncodingConverterToWideChar.sourceEncoding=char_encoding_Legacy; return 0;
    }
    return 1;
}
#else
char IPullParser::setCharEncoding(IXMLCharEncoding sourceEncoding, IXMLCharEncoding destinationEncoding)
{
    this->sourceEncoding=sourceEncoding;
    if (sourceEncoding==char_encoding_UTF16)
    {
        charEncodingConverterFromUTF16.r=sourceUReader;
        switch(destinationEncoding)
        {
        case char_encoding_UTF8:     charEncodingConverterFromUTF16.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF16; XML_ByteTable=XML_utf8ByteTable; return 0;
        case char_encoding_Legacy:   charEncodingConverterFromUTF16.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF16; XML_ByteTable=XML_legacyByteTable; return 0;
//         case char_encoding_ShiftJIS: charEncodingConverterFromUTF16.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF16; XML_ByteTable=XML_sjisByteTable; return 0;
//         case char_encoding_GB2312:   charEncodingConverterFromUTF16.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF16; XML_ByteTable=XML_gb2312ByteTable; return 0;
//         case char_encoding_Big5:     charEncodingConverterFromUTF16.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF16; XML_ByteTable=XML_gbk_big5_ByteTable; return 0;
//         case char_encoding_GBK:      charEncodingConverterFromUTF16.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF16; XML_ByteTable=XML_gbk_big5_ByteTable; return 0;
        case char_encoding_UTF32:    return 1; // destination must be multibyte
        }
        return 1;
    }
    if (sourceEncoding==char_encoding_UTF32)
    {
        charEncodingConverterFromUTF32.r=sourceUReader;
        switch(destinationEncoding)
        {
        case char_encoding_UTF8:     charEncodingConverterFromUTF32.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF32; XML_ByteTable=XML_utf8ByteTable; return 0;
        case char_encoding_Legacy:   charEncodingConverterFromUTF32.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF32; XML_ByteTable=XML_legacyByteTable; return 0;
//         case char_encoding_ShiftJIS: charEncodingConverterFromUTF32.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF32; XML_ByteTable=XML_sjisByteTable; return 0;
//         case char_encoding_GB2312:   charEncodingConverterFromUTF32.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF32; XML_ByteTable=XML_gb2312ByteTable; return 0;
//         case char_encoding_Big5:     charEncodingConverterFromUTF32.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF32; XML_ByteTable=XML_gbk_big5_ByteTable; return 0;
//         case char_encoding_GBK:      charEncodingConverterFromUTF32.destEncoding=characterEncoding=destinationEncoding; uReader=&charEncodingConverterFromUTF32; XML_ByteTable=XML_gbk_big5_ByteTable; return 0;
        case char_encoding_UTF16:    return 1; // destination must be multibyte
        }
        return 1;
    }
    if ((sourceEncoding==char_encoding_Legacy)&&
        (destinationEncoding==char_encoding_UTF8))
    {
        characterEncoding=char_encoding_UTF8; 
        //uReader=&charEncodingConverterSystemUTF8; 
        uReader=&charEncodingConverterFromUTF16;
        charEncodingConverterFromUTF16.destEncoding=char_encoding_UTF8;
        charEncodingConverterFromUTF16.r=&charEncodingConverterToWideChar;
        charEncodingConverterToWideChar.sourceEncoding=char_encoding_Legacy;
        XML_ByteTable=XML_utf8ByteTable; return 0;
    }
    if ((sourceEncoding==char_encoding_UTF8)&&
        (destinationEncoding==char_encoding_Legacy))
    {
        characterEncoding=char_encoding_Legacy; 
        //uReader=&charEncodingConverterUTF8System; 
        uReader=&charEncodingConverterFromUTF16;
        charEncodingConverterFromUTF16.destEncoding=char_encoding_Legacy;
        charEncodingConverterFromUTF16.r=&charEncodingConverterToWideChar;
        charEncodingConverterToWideChar.sourceEncoding=char_encoding_UTF8;
        XML_ByteTable=XML_legacyByteTable; 
        return 0;
    }
    if ((sourceEncoding==destinationEncoding)||(sourceEncoding==char_encoding_Legacy))
    {
        uReader=sourceUReader;
        switch(destinationEncoding)
        {
        case char_encoding_UTF8:     characterEncoding=char_encoding_UTF8; XML_ByteTable=XML_utf8ByteTable; return 0;
        case char_encoding_Legacy:   characterEncoding=char_encoding_Legacy; XML_ByteTable=XML_legacyByteTable; return 0;
        case char_encoding_ShiftJIS: characterEncoding=char_encoding_ShiftJIS; XML_ByteTable=XML_sjisByteTable; return 0;
        case char_encoding_GB2312:   characterEncoding=char_encoding_GB2312; XML_ByteTable=XML_gb2312ByteTable; return 0;
        case char_encoding_Big5:     characterEncoding=char_encoding_Big5; XML_ByteTable=XML_gbk_big5_ByteTable; return 0;
        case char_encoding_GBK:      characterEncoding=char_encoding_GBK; XML_ByteTable=XML_gbk_big5_ByteTable; return 0;
        }
        return 1;
    }
    return 1;
}
IPullParser::IXMLCharEncoding IPullParser::getCharEncodingDestination(){ return characterEncoding; }
#endif
IPullParser::IXMLCharEncoding IPullParser::getCharEncodingSource(){ return sourceEncoding; }

IXMLCStr IPullParser::charEncodingText(IPullParser::IXMLCharEncoding e)
{
    switch(e)
    {
    case char_encoding_UTF8: return _CXML("utf-8");
    case char_encoding_UTF16: return _CXML("utf-16");
    case char_encoding_UTF32: return _CXML("utf-32");
    case char_encoding_ShiftJIS: return _CXML("shift-jis");
    case char_encoding_GB2312: return _CXML("GB2312");
    case char_encoding_Big5: return _CXML("Big5");
    case char_encoding_GBK: return _CXML("GBK");
    default: return _CXML("Error");
    }
}

/************************************************************************/
/*              IXMLPullParser : findPath                               */
/************************************************************************/

bool IPullParser::skipBranch(int skipVar)
{
    if (!skipVar) return false;
    int posSkipNameBuffer=0;
    for(;;)
    {
        switch(nextToken())
        {
        case IXMLToken_StartTag: 
            {
                skipVar++; 
#ifndef STRICT_PARSING
                IXMLCStr n=getName();
                int l=(xstrlen(n)+1)*sizeof(IXMLChar);
                if ((int)(posSkipNameBuffer+l+2*sizeof(int))>=szSkipNameBuffer)
                {
                    if (skipNames)
                    {
                        szSkipNameBuffer*=2;
                        skipNames=(unsigned char*)realloc(skipNames,szSkipNameBuffer);
                    } else
                    {
                        szSkipNameBuffer=1024;
                        skipNames=(unsigned char*)malloc(szSkipNameBuffer);
                    }
                }
                posSkipNameBuffer+=sizeof(int);
                memcpy(skipNames+posSkipNameBuffer,n,l);
                int i=posSkipNameBuffer;
                posSkipNameBuffer+=l;
                *((int*)(skipNames+posSkipNameBuffer))=i;
#endif
                break;
            }
        case IXMLToken_EndTag:
            {
#ifdef STRICT_PARSING
                skipVar--; 
                if (!skipVar) return false;
#else
                IXMLCStr n=getName();
                int *ii;
popOneTag:
                skipVar--; 
                if (!skipVar) return false;
                if (!posSkipNameBuffer) 
                    return true;
                ii=(int*)(skipNames+posSkipNameBuffer);
                posSkipNameBuffer=(*ii)-sizeof(int);
                if(xstricmp(n,(IXMLCStr)(skipNames+*ii)))
                    goto popOneTag;
#endif
                break;
            }
        case IXMLToken_EndDocument:
        case IXMLToken_Error: return true; 
        }
    }
    return false;
}

bool IPullParser::findPath(IXMLCStr _path, IXMLChar sep, bool indicesAreZeroBased, IXMLResults *r)
{
    IXMLStr path=IXMLStringDup(_path);
    bool b=findPathNonConst(path,sep,indicesAreZeroBased,r);
    if (path) free(path);
    return b;
}

bool IPullParser::findPathNonConst(IXMLStr path,IXMLChar sep, bool indicesAreZeroBased, IXMLResults *r)
{
    int nDepth=1,i=xstrlen(path),level=0,j=0;
    if (!i) return true;
    while(i--) if (path[i]==sep) nDepth++;
    IXMLStr *fullp=(IXMLStr*)malloc(nDepth*(sizeof(IXMLStr*)+2*sizeof(int)));
    int *nOcc=(int*)(fullp+nDepth),*nOccStart=nOcc+nDepth;

#define RETURN_WITH_ERROR                                               \
    {                                                                   \
        if (r)                                                          \
        {                                                               \
            r->errorCode=getErrorCode();                                \
            r->nLine=getLineNumber(); r->nColumn=getColumnNumber();     \
        }                                                               \
        free(fullp);return false;                                       \
    }

#define RETURN_WITHOUT_ERROR                                            \
    {                                                                   \
        if (r) r->errorCode=IXMLError_None;                             \
        free(fullp);return true;                                        \
    }

    i=0;
    if (path[i]==sep) i++;
    while((path[i]==_CXML('.'))&&(path[i+1]==_CXML('.'))&&(path[i+2]==sep))
    {
        i+=3; j++;
    }
    if (skipBranch(j)) RETURN_WITH_ERROR;
    if (!path[i]) RETURN_WITHOUT_ERROR;

    nDepth=0;
    fullp[0]=path+i;
    while(path[i])
    {
        if(path[i]!=sep) { i++; continue;}

#define INIT_NOCC                                                           \
        if ((i>0)&&(path[i-1]==_CXML(']')))                                 \
        {                                                                   \
            j=i-1;                                                          \
            while((j>0)&&(path[j]!=_CXML('['))) j--;                        \
            int k;                                                          \
            if (indicesAreZeroBased) k=internal_xmltoi(path+j+1,0);         \
            else k=internal_xmltoi(path+j+1,1)-1;                           \
            nOccStart[nDepth]=nOcc[nDepth]=k;                               \
            path[j]=_CXML('\0');                                            \
        } else                                                              \
        {                                                                   \
            nOccStart[nDepth]=nOcc[nDepth]=0;                               \
            path[i]=_CXML('\0');                                            \
        }                                                                   \
        nDepth++;

        INIT_NOCC
        i++;
        fullp[nDepth]=path+i; 
    }

    INIT_NOCC
#undef INIT_NOCC

    for(;;)
    {
        switch(nextToken())
        {
        case IXMLToken_StartTag:
            {
                if (!xstricmp(fullp[level],getName()))
                {
                    if (!nOcc[level])
                    {
                        level++;
                        // we found the right tag name at the right depth
                        if (level>=nDepth) RETURN_WITHOUT_ERROR;
                    } else
                    {
                        nOcc[level]--;
                        if (skipBranch(1)) RETURN_WITH_ERROR;
                    }
                } else
                {
                    if (skipBranch(1)) RETURN_WITH_ERROR;
                }
                break;
            } 
        case IXMLToken_EndTag: 
            if (!level)
            {
                if (_isHTML)
                {
                    RETURN_WITHOUT_ERROR;
                }
                if (r)
                {
                    r->errorCode=IXMLError_UnexpectedEndTag;
                    r->nLine=getLineNumber(); r->nColumn=getColumnNumber();
                }
                free(fullp);
                return false;
            }
            nOcc[level]=nOccStart[level]; level--; 
            break;
        case IXMLToken_EndDocument:
        case IXMLToken_Error: RETURN_WITH_ERROR;
        }
    }
}
#undef RETURN_WITH_ERROR
#undef RETURN_WITHOUT_ERROR

// The following function parses the XML errors into a user friendly string.
// You can edit this to change the output language of the library to something else.
static IXMLCStr getErrorMess(IXMLErrorInfo xerror)
{
    switch (xerror)
    {
    case IXMLError_None:                   return _CXML("No error");
    case IXMLError_MissingEndTag:          return _CXML("Warning: Unmatched end tag");
    case IXMLError_NoXMLTagFound:          return _CXML("Warning: No XML tag found");
    case IXMLError_Empty:                  return _CXML("Error: No XML data");
    case IXMLError_UnmatchedEndTag:        return _CXML("Error: Unmatched end tag");
    case IXMLError_UnexpectedEndTag:       return _CXML("Error: Unexpected end tag");
    case IXMLError_UnmatchedEndClearTag:   return _CXML("Error: Unmatched clear tag end");
    case IXMLError_FileNotFound:           return _CXML("Error: File not found");
    case IXMLError_FirstTagNotFound:       return _CXML("Error: First Tag not found");
    case IXMLError_UnknownCharacterEntity: return _CXML("Error: Unknown character entity");
    case IXMLError_CharacterCodeAbove255:  return _CXML("Error: Character code above 255 is forbidden in MultiByte char mode.");
    case IXMLError_invalidCharInTagName:   return _CXML("Error: Invalid char in tag name");
    case IXMLError_invalidCharInEndTagName:return _CXML("Error: Invalid char in end tag name");
    case IXMLError_invalidCharAtTagEnd:    return _CXML("Error: Invalid char at tag end");
    case IXMLError_invalidCharAtPIEnd:     return _CXML("Error: Invalid char at PI end");
    case IXMLError_AttributeWithNoEqualSign:return _CXML("Error: Attribute with No Equal Sign");
    case IXMLError_QuoteOrDoubleQuoteExpected:return _CXML("Error: Quote or DoubleQuote expected");
    case IXMLError_CharConversionModeError:return _CXML("Error: Unable to convert between WideChar and MultiByte chars");
    case IXMLError_MaxMemoryReached:       return _CXML("Error: DOM Max Memory Reached");
    case IXMLError_MemoryAllocationError:  return _CXML("Error: Memory Allocation error");
    case IXMLError_CannotOpenWriteFile:    return _CXML("Error: Unable to open file for writing");
    case IXMLError_CannotWriteFile:        return _CXML("Error: Cannot write into file");
    case IXMLError_readError:              return _CXML("Error: read stream data");
    case IXMLError_ColonExpected:          return _CXML("Error: colon (:) character expected");
    case IXMLError_invalidCharInNumber:    return _CXML("Error: invalid character inside number");
    case IXMLError_trueExpected:           return _CXML("Error: 'true' string expected");
    case IXMLError_falseExpected:          return _CXML("Error: 'false' string expected");
    case IXMLError_nullExpected:           return _CXML("Error: 'null' string expected");

    case IXMLError_Base64DataSizeIsNotMultipleOf4: return _CXML("Warning: Base64-string length is not a multiple of 4");
    case IXMLError_Base64DecodeTruncatedData:      return _CXML("Warning: Base64-string is truncated");
    case IXMLError_Base64DecodeIllegalCharacter:   return _CXML("Error: Base64-string contains an illegal character");
    case IXMLError_Base64DecodeBufferTooSmall:     return _CXML("Error: Base64 decode output buffer is too small");
    };
    return _CXML("Unknown error");
}

IXMLCStr IPullParser::getErrorMessage(IXMLErrorInfo xerror){ return getErrorMess(xerror); }
IXMLCStr ITCXMLNode::getErrorMessage(IXMLErrorInfo xerror){ return getErrorMess(xerror); }
IXMLCStr IXMLDomParser::getErrorMessage(IXMLErrorInfo xerror){ return getErrorMess(xerror); }
IXMLCStr IXMLRenderer::getErrorMessage(IXMLErrorInfo xerror){ return getErrorMess(xerror); }

/************************************************************************/
/*                                                                      */
/*                         IXMLPullParser                               */
/*                                                                      */
/************************************************************************/

static const char XML_Character_Classes[256] =
{
    //  0 1 2 3 4 5 6 7 8 9 a b c d e f
    8,9,9,9,9,9,9,9,9,7,7,9,9,7,9,9,// 0x00   9: error
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,// 0x10   8: null
    7,9,9,9,9,9,9,9,9,9,9,9,9,4,4,9,// 0x20   7: space char (space,tab,\n,\r)
    6,6,6,6,6,6,6,6,6,6,4,9,9,9,9,9,// 0x30   6: digit (0..9)
    9,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,// 0x40   5: roman letter (a..z)
    5,5,5,5,5,5,5,5,5,5,5,9,9,9,9,4,// 0x50   4: .-_:
    9,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,// 0x60   3: unicode letter
    5,5,5,5,5,5,5,5,5,5,5,9,9,9,9,9,// 0x70
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,// 0x80
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,// 0x90
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,// 0xa0
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,// 0xb0
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,// 0xc0
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,// 0xd0
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,// 0xe0
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3 // 0xf0
};

#ifdef _XMLWIDECHAR
#define XML_isSPACECHAR(ch)((ch<256)&&(XML_Character_Classes[(unsigned char)(ch)]==7))
#define XML_isNAMECHAR(ch) ((ch>=256)||(XML_Character_Classes[(unsigned char)(ch)]<=6))
#define XML_isNumber(ch) ((ch<256)&&(XML_Character_Classes[(unsigned char)(ch)]==6))
#else
#define XML_isSPACECHAR(ch)(XML_Character_Classes[(unsigned char)(ch)]==7)
#define XML_isNAMECHAR(ch) (XML_Character_Classes[(unsigned char)(ch)]<=6)
#define XML_isNumber(ch) (XML_Character_Classes[(unsigned char)(ch)]==6)
#endif

#define XML_peekSimpleChar(i)                                   \
{                                                               \
    if (ptr+i>=pend)                                            \
    {                                                           \
        if (readData(&ptr,&pend,&ptrOut,&pStart)) return 1;     \
    }                                                           \
}

#ifdef _XMLWIDECHAR
#define XML_getNextChar()                           \
{                                                   \
    ptr+=ll;                                        \
    ll=1;                                           \
    XML_peekSimpleChar(1);                          \
    ch=*ptr;                                        \
}
#else
#define XML_getNextChar()                           \
{                                                   \
    ptr+=ll;                                        \
    ll=XML_ByteTable[*((unsigned char*)ptr)];       \
    XML_peekSimpleChar(ll);                         \
    ch=*((unsigned char*)ptr);                      \
}
#endif

#define XML_skipSpaceChars()                        \
    while (XML_isSPACECHAR(ch))                     \
    {                                               \
        if (ch==_CXML('\n'))                        \
        {                                           \
            pStartOfLine=ptr;                       \
            nLine++;                                \
            XML_getNextChar()                       \
            if (ch==_CXML('\r'))                    \
            {                                       \
                pStartOfLine=ptr;                   \
                XML_getNextChar()                   \
            }                                       \
        } else if (ch==_CXML('\r'))                 \
        {                                           \
            pStartOfLine=ptr;                       \
            nLine++;                                \
            XML_getNextChar()                       \
            if (ch==_CXML('\n'))                    \
            {                                       \
                pStartOfLine=ptr;                   \
                XML_getNextChar()                   \
            }                                       \
        } else XML_getNextChar()                    \
    }

#define XML_skipName()                                                  \
{                                                                       \
    XML_getNextChar()                                                   \
    while (XML_isNAMECHAR(ch)) XML_getNextChar()                        \
    if ((ch!=_CXML('/'))&&(ch!=_CXML('='))&&(ch!=_CXML('>'))&&(ch!=_CXML('?'))&&(!XML_isSPACECHAR(ch))) \
    {                                                                   \
        lastError=IXMLError_invalidCharInTagName;                       \
        return 1;                                                       \
    }                                                                   \
}

#define XML_peekSimpleChar2(ii,XX)                              \
{                                                               \
    if (ptr+ii>=pEndPos)                                        \
    {                                                           \
        if (readData(&ptr,NULL,&ptrOut,&pStart))                \
        {                                                       \
            XX                                                  \
            pCurrentPos=ptr;                                    \
            curToken=IXMLToken_Error;                           \
            return IXMLToken_Error;                             \
        }                                                       \
    }                                                           \
    XX                                                          \
}

#ifdef _XMLWIDECHAR
#define XML_getNextChar2()                          \
{                                                   \
    ptr+=ll;                                        \
    ll=1;                                           \
    XML_peekSimpleChar2(1,);                        \
    ch=*ptr;                                        \
}
#else
#define XML_getNextChar2()                          \
{                                                   \
    ptr+=ll;                                        \
    ll=XML_ByteTable[*((unsigned char*)ptr)];       \
    XML_peekSimpleChar2(ll,);                       \
    ch=*((unsigned char*)ptr);                      \
}
#endif


#define XML_skipSpaceChars2()                       \
    while (XML_isSPACECHAR(ch))                     \
    {                                               \
        if (ch==_CXML('\n'))                        \
        {                                           \
            pStartOfLine=ptr;                       \
            nLine++;                                \
            XML_getNextChar2()                      \
            if (ch==_CXML('\r'))                    \
            {                                       \
                pStartOfLine=ptr;                   \
                XML_getNextChar2()                  \
            }                                       \
        } else if (ch==_CXML('\r'))                 \
        {                                           \
            pStartOfLine=ptr;                       \
            nLine++;                                \
            XML_getNextChar2()                      \
            if (ch==_CXML('\n'))                    \
            {                                       \
                pStartOfLine=ptr;                   \
                XML_getNextChar2()                  \
            }                                       \
        } else XML_getNextChar2()                   \
    }

#define XML_skipName2()                                                 \
{                                                                       \
    XML_getNextChar2()                                                  \
    while (XML_isNAMECHAR(ch)) XML_getNextChar2()                       \
    if ((ch!=_CXML('/'))&&(ch!=_CXML('='))&&(ch!=_CXML('>'))&&(ch!=_CXML('?'))&&(!XML_isSPACECHAR(ch))) \
    {                                                                   \
        lastError=IXMLError_invalidCharInTagName;                       \
        pCurrentPos=ptr;                                                \
        curToken=IXMLToken_Error;                                       \
        return IXMLToken_Error;                                         \
    }                                                                   \
}


#define RETURN_CODE(X)  \
{                       \
    pCurrentPos=ptr;    \
    curToken=X;         \
    return X;           \
}

IXMLPullParser::IXMLPullParser(IXMLReader *_uReader,int _szByteBuffer): 
    IPullParser(_uReader,_szByteBuffer), maxAttribute(20),
    nextChar(-1),nextDepth(0)
{
    allAttributes=(IXMLAttr*)malloc(maxAttribute*sizeof(IXMLAttr));
}

void IXMLPullParser::reset()
{
    IPullParser::reset();
    nextChar=-1; nextDepth=0;
}

IXMLPullParser::~IXMLPullParser()
{
    clear(); free(allAttributes);
};

int IXMLPullParser::getColumnNumber() const { return (int)(pCurrentPos-pStartOfLine); }

char IPullParser::readData(IXMLStr *_ptr,IXMLStr *_pend,IXMLStr *_ptrOut, IXMLStr *_pStart)
{
    if (isStreamFinished)
    {
        if (*_ptr>=pEndPos)
        {
#define SHIFT_ALL_TO_BUFFER_START                                              \
            if ((unsigned char*)pContent!=bufferChar)                          \
            {                                                                  \
                size_t offset=(((char*)pContent)-((char*)bufferChar));         \
                pContent=(IXMLStr)bufferChar;                                  \
                *_ptr=(IXMLStr)(((char*)*_ptr)-offset);                        \
                if (_ptrOut) *_ptrOut=(IXMLStr)(((char*)*_ptrOut)-offset);     \
                if (_pStart) *_pStart=(IXMLStr)(((char*)*_pStart)-offset);     \
                pStartOfLine=(IXMLStr)(((char*)pStartOfLine)-offset);         \
                char *t;                                                       \
                for(int j=0;j<_nAttribute;j++)                                 \
                {                                                              \
                    t=(char*)allAttributes[j].sName;                           \
                    allAttributes[j].sName=(IXMLCStr)(t-offset);               \
                    t=(char*)allAttributes[j].sValue;                          \
                    allAttributes[j].sValue=(IXMLCStr)(t-offset);              \
                }                                                              \
            }

            if (((unsigned char*)pContent>=bufferChar)&&(pContent<pEndPos))
            {
                SHIFT_ALL_TO_BUFFER_START
            } else
            {
                *_ptr=(IXMLStr)bufferChar;
                if (_ptrOut) *_ptrOut=(IXMLStr)bufferChar;
                if (_pStart) *_pStart=(IXMLStr)bufferChar;
                pStartOfLine=(IXMLStr)(((char*)pStartOfLine)-szByteBuffer);
            }
            **_ptr=0; pEndPos=((*_ptr)+1);
            if (_pend) *_pend=pEndPos;
        }
        return 0;
    }
    // we are currently at position ptr
    size_t l=0;
    if (((unsigned char*)pContent>=bufferChar)&&(pContent<pEndPos)) l=(((char*)pEndPos)-((char*)pContent));
    if (l>0) 
    {
        if ((unsigned char*)pContent!=bufferChar) memmove(bufferChar,pContent,l);
        else
        {
            if (((unsigned char*)*_ptr)>=(bufferChar+szByteBuffer-100))
            {
                szByteBuffer*=2;
                bufferChar=(unsigned char*)realloc(bufferChar,szByteBuffer);
                uReader->setNormalBufferSize(szByteBuffer);
            }
        }
        SHIFT_ALL_TO_BUFFER_START
    } else
    {
        l=0;
        *_ptr=(IXMLStr)bufferChar;
        if (_ptrOut) *_ptrOut=(IXMLStr)bufferChar;
        if (_pStart) *_pStart=(IXMLStr)bufferChar;
        pStartOfLine=(IXMLStr)(((char*)pStartOfLine)-szByteBuffer);
    }
    int i=uReader->read(bufferChar+l,(int)(szByteBuffer-l));
    if (i<0) { lastError=IXMLError_readError; return 1; }
    pEndPos=(IXMLStr)(bufferChar+l+i);
    if (_pend) *_pend=pEndPos;
    if (!i) isStreamFinished=true;
    return 0;
}

char IXMLPullParser::parseNameAndAttributes(IXMLStr *_ptr, int *c)
{
    int ch=*c,ll=1;
    IXMLStr ptr=*_ptr,pend=pEndPos;
    IXMLStr ptrOut=NULL,// fake;
            pStart=NULL; 
    pContent=ptr;
    XML_skipName();
    pStart=ptr;
    XML_skipSpaceChars();
    *pStart=0;

    /* attribute list */
    _nAttribute=0;
    for(;;) 
    {
        if (!XML_isNAMECHAR(ch))
            break;

        if (_nAttribute==maxAttribute)
        {
            maxAttribute*=2;
            allAttributes=(IXMLAttr*)realloc(allAttributes,maxAttribute*sizeof(IXMLAttr));
            if (!allAttributes) { lastError=IXMLError_MemoryAllocationError; return 1; }
        }

        /* attribute name */
        allAttributes[_nAttribute].sName=ptr;
        _nAttribute++; // must stay here
        XML_skipName();
        pStart=ptr;
        XML_skipSpaceChars();

        if (ch!=_CXML('='))
        {
            lastError=IXMLError_AttributeWithNoEqualSign; return 1;
        }
        *pStart=0;
        XML_getNextChar(); // skip =

        XML_skipSpaceChars();

        int delim;
        if ((ch==_CXML('"'))||(ch==_CXML('\''))) delim=ch;
        else 
        {
            lastError=IXMLError_QuoteOrDoubleQuoteExpected; return 1;
        }
        XML_getNextChar(); // skip first quote
        allAttributes[_nAttribute-1].sValue=ptr;
        if (processText(delim,&ptr,&ch,&allAttributes[_nAttribute-1].szValue)) return 1; // error
        pend=pEndPos;
        ll=1;
        XML_getNextChar(); // skip last quote
        XML_skipSpaceChars();
    }
    *c=ch;
    *_ptr=ptr;
    return 0;
}

char IXMLPullParser::processText(IXMLChar delim,IXMLStr *_ptr, int *nextChar, int *len)
{
    // This function is the opposite of the function "toXMLString". It decodes the escape
    // sequences &amp;, &quot;, &apos;, &lt;, &gt; and replace them by the characters
    // &,",',<,>. This function is used internally by the XML Parser. All the calls to
    // the XML library will always gives you back "decoded" strings.
    //
    bool isIgnorableSpace=true;
    IXMLCharacterEntity *entity;
    IXMLStr ptr=*_ptr,ptrOut=ptr,pStart=ptrOut,pend=pEndPos;
    int ch,j,ll=0;
    XML_getNextChar();
    while ((ch!=delim)&&(ch))
    {
        if (ch==_CXML('\n'))
        {                                           
            *ptrOut=_CXML('\n'); ptrOut++;
            pStartOfLine=ptr;
            nLine++;                                
            XML_getNextChar();
            if (ch==_CXML('\r'))                    
            {                                       
                pStartOfLine=ptr;                   
                XML_getNextChar()                   
            }                                       
        } else if (ch==_CXML('\r'))                 
        {                                           
            *ptrOut=_CXML('\n'); ptrOut++;
            pStartOfLine=ptr;                       
            nLine++;                                
            XML_getNextChar();
            if (ch==_CXML('\n'))                    
            {                                       
                pStartOfLine=ptr;                   
                XML_getNextChar()                   
            }
        } else if (ch==_CXML(' '))
        {
#ifdef _XMLWIDECHAR
            *(ptrOut++)=L' ';
            ptr++;                                          
            XML_peekSimpleChar(1);                          
            ch=*ptr;
#else
            *(ptrOut++)=' ';
            ptr++;
            ll=XML_ByteTable[*((unsigned char*)ptr)];        
            XML_peekSimpleChar(ll);
            ch=*((unsigned char*)ptr);
#endif
        } else if (ch==_CXML('\t'))
        {
#ifdef _XMLWIDECHAR
            *(ptrOut++)=L'\t';
            ptr++;                                          
            XML_peekSimpleChar(1);                          
            ch=*ptr;
#else
            *(ptrOut++)='\t';
            ptr++;
            ll=XML_ByteTable[*((unsigned char*)ptr)];        
            XML_peekSimpleChar(ll);                         
            ch=*((unsigned char*)ptr);
#endif
        } else if (ch==_CXML('&'))
        {
            isIgnorableSpace=false;
            XML_getNextChar(); // skip &
            if (ch==_CXML('#'))
            {
                XML_getNextChar(); // skip #
                j=0;
                if ((ch==_CXML('X'))||(ch==_CXML('x')))
                {
                    XML_getNextChar(); // skip X
                    while (ch!=_CXML(';'))
                    {
                        if ((ch>=_CXML('0'))&&(ch<=_CXML('9'))) j=(j<<4)+ch-_CXML('0');
                        else if ((ch>=_CXML('A'))&&(ch<=_CXML('F'))) j=(j<<4)+ch-_CXML('A')+10;
                        else if ((ch>=_CXML('a'))&&(ch<=_CXML('f'))) j=(j<<4)+ch-_CXML('a')+10;
                        else 
                        {
                            lastError=IXMLError_UnknownCharacterEntity;
                            return 1;
                        }
                        XML_getNextChar(); // skip digit
                    }
                } else
                {
                    while (ch!=_CXML(';'))
                    {
                        if ((ch>=_CXML('0'))&&(ch<=_CXML('9'))) j=(j*10)+ch-_CXML('0');
                        else 
                        {
                            lastError=IXMLError_UnknownCharacterEntity;
                            return 1;
                        }
                        XML_getNextChar(); // skip digit
                    }
                }
                XML_getNextChar(); // skip ;
#ifndef _XMLWIDECHAR
                if (j>255) 
                { 
                    lastError=IXMLError_CharacterCodeAbove255;
                    curToken=IXMLToken_Error;
                    return 1;
                }
#endif
                *ptrOut=(IXMLChar)j; ptrOut++;
            } else
            {
                XML_peekSimpleChar(maxXMLEntitiesLen);
                entity=XMLEntities;
                do
                {
                    if (xstrnicmp(ptr,entity->s,entity->lengthMinus1)==0) { *(ptrOut++)=entity->c; ptr+=entity->lengthMinus1; break; }
                    entity++;
                } while(entity->s);
                if (!entity->s)
                {
                    if (!_isHTML)
                    {
                        lastError=IXMLError_UnknownCharacterEntity;
                        return 1;
                    }
                    *(ptrOut++)=_CXML('&'); ptr++;
                }
#ifdef _XMLWIDECHAR
                ch=*ptr;
#else
                ch=*((unsigned char*)ptr);
#endif
            }
        } else 
        {
            isIgnorableSpace=false;
#ifdef _XMLWIDECHAR
            *(ptrOut++)=ch;
            ptr++;                                          
            XML_peekSimpleChar(1);                          
            ch=*ptr;                                        
#else
            COPY_ONE_XML_CHAR(ll,ptrOut,ptr);
            ll=XML_ByteTable[(unsigned char)(*ptr)];        
            XML_peekSimpleChar(ll);                         
            ch=*((unsigned char*)ptr);
#endif
        }
    }
    *nextChar=ch;
    *_ptr=ptr;
    *ptrOut=0;
    *len=(int)(ptrOut-pStart);
    _isIgnorableSpace=isIgnorableSpace;
    return 0;
}

char IXMLPullParser::findClearTagEnd(IXMLStr *_ptr,IXMLCStr closeTag, int closeTagLen)
{
    IXMLStr ptr=*_ptr,pend=pEndPos,pStart=ptr;
    IXMLStr ptrOut=NULL; // fake;
    int ch,delim=closeTag[0],ll=0;
    XML_getNextChar();
    for(;;)
    {
        while ((ch!=delim)&&(ch))
        {
            if (ch==_CXML('\n'))
            {                                           
                pStartOfLine=ptr;
                nLine++;                                
                XML_getNextChar();
                if (ch==_CXML('\r'))                    
                {                                       
                    pStartOfLine=ptr;                   
                    XML_getNextChar()                   
                }                                       
            } else if (ch==_CXML('\r'))                 
            {                                           
                pStartOfLine=ptr;                       
                nLine++;                                
                XML_getNextChar();
                if (ch==_CXML('\n'))                    
                {                                       
                    pStartOfLine=ptr;                   
                    XML_getNextChar()                   
                }
            } else
            {
                XML_getNextChar();
            }
        }
        if (!ch) break;
        XML_peekSimpleChar(closeTagLen);
        if(!xstrncmp(closeTag, ptr, closeTagLen)) break;
        XML_getNextChar();
    }
    *_ptr=ptr;
    return 0;
}

IXMLPullParser::IXMLToken IXMLPullParser::nextToken()
{
extractNextToken:
    depth=nextDepth;

    if ((curToken==IXMLToken_Error)||(curToken==IXMLToken_EndDocument))
        return curToken;

    if (_isEmptyNode) 
    {
        nextDepth--;
        _isEmptyNode=false;
        curToken=IXMLToken_EndTag;
        return IXMLToken_EndTag; 
    }

    IXMLStr ptr=pCurrentPos;
    IXMLStr ptrOut=NULL,pStart=NULL; // fake;

    if (curToken==IXMLToken_StartDocument)
    {
        if (!bufferChar)
        {
            bufferChar=(unsigned char*)malloc(szByteBuffer);
            if (!bufferChar) { lastError=IXMLError_MemoryAllocationError; RETURN_CODE(IXMLToken_Error); }
        }
        pContent=pStartOfLine=pCurrentPos=pEndPos=ptr=(IXMLStr)(bufferChar+1);

        uReader->setNormalBufferSize(szByteBuffer); uReader->reset();
        IXMLReader_PassThrough passThrough(sourceUReader);

        int l=passThrough.nByteRead();
        if (!l) { lastError=IXMLError_Empty; RETURN_CODE(IXMLToken_Error); }

        if (autoDetectSource)
        {
            IXMLCharEncoding sourceEncoding=guessXMLCharEncodingCode(passThrough.b,l);
#ifdef _XMLWIDECHAR
            setSourceCharEncoding(sourceEncoding);
#else
            IXMLCharEncoding destinationEncoding=getCharEncodingDestination();
            if ((sourceEncoding!=char_encoding_UTF16)&&(sourceEncoding!=char_encoding_Legacy)&&
                (sourceEncoding!=char_encoding_UTF8)) destinationEncoding=sourceEncoding;
            setCharEncoding(sourceEncoding,destinationEncoding);
#endif
        }

        passThrough.skipBOM();

        // empty-out the passThrough buffer
        IXMLReader *temp=uReader->getSource(); 
        if (!temp) { temp=uReader; uReader=&passThrough; XML_peekSimpleChar2(1024, uReader=temp;); }
        else { uReader->setSource(&passThrough); XML_peekSimpleChar2(1024, uReader->setSource(temp);); }
        pStartOfLine=pCurrentPos;
    }
    int ch=nextChar,ll=0;
    if (ch<0) XML_getNextChar2()
    else { nextChar=-1; ll=1; /* we just got out of a text node */ }

    if (!ch)
    {
        RETURN_CODE(IXMLToken_EndDocument);
    }

    if (ch==_CXML('<'))
    {
        // end tag, PI, clear , start tag
        XML_getNextChar2(); // skip <
        if (ch==_CXML('/'))
        {
//            if (parseEndTag(&ptr)) RETURN_CODE(IXMLToken_Error);

            XML_getNextChar2(); // skip /
            pContent=ptr;
            XML_skipName2();
            pStart=ptr;
            XML_skipSpaceChars2();
            if (ch!='>')
            {
                lastError=IXMLError_invalidCharInEndTagName;
                RETURN_CODE(IXMLToken_Error);
            }
            XML_getNextChar2(); // skip >
            *pStart=0;

            if (_isHTML&&
                ((!xstricmp(pContent,_CXML("tbody")))||
                 (!xstricmp(pContent,_CXML("img")))||
                 (!xstricmp(pContent,_CXML("br")))||
                 (!xstricmp(pContent,_CXML("link")))||
                 (!xstricmp(pContent,_CXML("meta")))||
                 (!xstricmp(pContent,_CXML("input")))||
                 (!xstricmp(pContent,_CXML("area")))
                )
               )
            {
                pCurrentPos=ptr;
                goto extractNextToken;
            }

            nextDepth--;
            RETURN_CODE(IXMLToken_EndTag);
        }
        if (ch=='?')
        {
            // processing instruction
            _isEmptyNode=false;
            XML_getNextChar2(); //skip first ?
            if (parseNameAndAttributes(&ptr,&ch)) 
            {
                RETURN_CODE(IXMLToken_Error);
            }
            if (ch!=_CXML('?'))
            {
                lastError=IXMLError_invalidCharAtPIEnd;
                RETURN_CODE(IXMLToken_Error);
            }
            ll=1;
            XML_getNextChar2(); //skip last ?
            if (ch!=_CXML('>'))
            {
                lastError=IXMLError_invalidCharAtPIEnd;
                RETURN_CODE(IXMLToken_Error);
            }
            XML_getNextChar2(); // skip >
            RETURN_CODE(IXMLToken_ProcessingInstruction);
        }
        {
            pContent=ptr;
            XML_peekSimpleChar2(maxIXMLClearTagsOpenTagLen,);
            // test all clear tags
            ALLIXMLClearTag *ctag=IXMLClearTags;
            do
            {
                if (!xstrncmp(ctag->lpcOpen+1, ptr, ctag->openTagLenMinus1))
                {
                    static IXMLCStr docTypeEnd=_CXML(">");
                    ptr+=ctag->openTagLenMinus1;
                    
                    curClearTag.sOpenTag=ctag->lpcOpen;
                    pContent=ptr;

                    // Find the closing tag
                    if (ctag->lpcOpen==IXMLClearTags[1].lpcOpen)
                    {
                        // The <!DOCTYPE need a special treatment so lets do it!
                        ll=0;
                        XML_getNextChar2();
                        while (ch)
                        {
                            if (ch==_CXML('<')) 
                            { 
                                curClearTag.sCloseTag=ctag->lpcClose; 
                                if (findClearTagEnd(&ptr,ctag->lpcClose,2)) RETURN_CODE(IXMLToken_Error);
                                break; 
                            } else if (ch==_CXML('>')) { curClearTag.sCloseTag=docTypeEnd; break; }
                            XML_getNextChar2();
                        }
                    } else 
                    {
                        // find end of clear tag that is not <!DOCTYPE
                        curClearTag.sCloseTag=ctag->lpcClose;
                        if (findClearTagEnd(&ptr, ctag->lpcClose,ctag->closeTagLen)) 
                            RETURN_CODE(IXMLToken_Error);
                    }

                    curClearTag.sValue=pContent;
                    curClearTag.szValue=(int)(ptr-pContent);
                    if (!ptr)
                    {
                        lastError=IXMLError_UnmatchedEndClearTag;
                        RETURN_CODE(IXMLToken_Error);
                    }

                    *ptr=0;
                    ptr+=ctag->closeTagLen;
                    RETURN_CODE(IXMLToken_Clear);
                }
                ctag++;
            } while(ctag->lpcOpen);
        }
        if (parseNameAndAttributes(&ptr,&ch)) 
        {
            RETURN_CODE(IXMLToken_Error);
        }        
        nextDepth++;
        _isEmptyNode=false;
        ll=1;
        if (ch==_CXML('/')) 
        {
            _isEmptyNode=true;
            XML_getNextChar2(); // skip /
        }
        if (ch!=_CXML('>'))
        {
            lastError=IXMLError_invalidCharAtTagEnd;
            RETURN_CODE(IXMLToken_Error);
        }
        XML_getNextChar2(); // skip >

        if (_isHTML)
        {
            if(!xstricmp(pContent,_CXML("tbody")))
            {
                nextDepth--;
                pCurrentPos=ptr;
                goto extractNextToken;
            }

            if((!xstricmp(pContent,_CXML("img")))||
               (!xstricmp(pContent,_CXML("br")))||
               (!xstricmp(pContent,_CXML("link")))||
               (!xstricmp(pContent,_CXML("meta")))||
               (!xstricmp(pContent,_CXML("input")))||
               (!xstricmp(pContent,_CXML("area")))
               )
               _isEmptyNode=true;
        }

        RETURN_CODE(IXMLToken_StartTag);
    } 
    
    // text
    _nAttribute=0;
    pContent=ptr;
    if (processText(_CXML('<'),&ptr,&nextChar,&_szText)) 
    {
        RETURN_CODE(IXMLToken_Error);
    }
    if (nextChar||_szText) RETURN_CODE(IXMLToken_Text);
    RETURN_CODE(IXMLToken_EndDocument);
}

/************************************************************************/
/*                           guessCharEncoding                          */
/************************************************************************/

IXMLCStr IXMLPullParser::guessXMLCharEncoding(unsigned char *b,int l, IXMLChar out[200])
{
    static IXMLCStr utf8Encoding=_CXML("UTF-8"),systemEncoding=_CXML("System");

    if (l<25) { xstrcpy(out,systemEncoding); return out; }
    if (myIsTextWideChar(b,l)) 
    { 
        if (((b[2]==0)&&(b[3]==0))||
            ((b[0]==0)&&(b[1]==0))) { xstrcpy(out,_CXML("UTF-32")); return out; }
        xstrcpy(out,_CXML("UTF-16")); return out; 
    }        
    if ((b[0]==0xef)&&(b[1]==0xbb)&&(b[2]==0xbf)) { xstrcpy(out,utf8Encoding); return out; }

    // Match utf-8 model ?
    IXMLCStr bestGuess=utf8Encoding;
    int i=0;
    while (i<l)
        switch (XML_utf8ByteTable[b[i]])
        {
            case 4: i++; if ((i<l)&&(b[i]& 0xC0)!=0x80) { bestGuess=systemEncoding; i=l; } // 10bbbbbb ?
            case 3: i++; if ((i<l)&&(b[i]& 0xC0)!=0x80) { bestGuess=systemEncoding; i=l; } // 10bbbbbb ?
            case 2: i++; if ((i<l)&&(b[i]& 0xC0)!=0x80) { bestGuess=systemEncoding; i=l; } // 10bbbbbb ?
            case 1: i++; break;
            case 0: i=l;
        }
    // if encoding is specified and different from utf-8 than it's non-utf8
    // otherwise it's utf-8
    char bb[201];
    l=mmin(l,200);
    memcpy(bb,b,l); // copy buf into bb to be able to do "bb[l]=0"
    bb[l]=0;
    b=(unsigned char*)strstr(bb,"encoding");
    if (!b) { xstrcpy(out,bestGuess); return out; }
    b+=8; while XML_isSPACECHAR(*b) b++; if (*b!='=') { xstrcpy(out,bestGuess); return out; }
    b++;  while XML_isSPACECHAR(*b) b++; if ((*b!='\'')&&(*b!='"')) { xstrcpy(out,bestGuess); return out; }
    b++;  while XML_isSPACECHAR(*b) b++;
    if ((_strnicmp((char*)b,"utf-8",5)==0)||
        (_strnicmp((char*)b,"utf8",4)==0))
    {
        if (bestGuess==systemEncoding) { xstrcpy(out,_CXML("ERROR")); return out; }
        xstrcpy(out,utf8Encoding); return out;
    }
    unsigned char *bend=b;
    IXMLStr bwc=out;
    while(*bend&&(*bend!='\'')&&(*bend!='"')) { *bwc=*bend; bend++; bwc++; }
    *bwc=0;
    return out;
}

IPullParser::IXMLCharEncoding IXMLPullParser::guessXMLCharEncodingCode(unsigned char *b,int l)
{
    IXMLChar s[200];
    guessXMLCharEncoding(b,l,s);
    if ((!xstricmp(s,_CXML("utf-8")))||
        (!xstricmp(s,_CXML("utf8")))) return char_encoding_UTF8;
    if ((!xstricmp(s,_CXML("utf-16")))||
        (!xstricmp(s,_CXML("utf16")))) return char_encoding_UTF16;
    if ((!xstricmp(s,_CXML("utf-32")))||
        (!xstricmp(s,_CXML("utf32")))) return char_encoding_UTF32;
    if ((!xstricmp(s,_CXML("shiftjis")))||
        (!xstricmp(s,_CXML("shift-jis")))||
        (!xstricmp(s,_CXML("sjis")))) return char_encoding_ShiftJIS;
    if (!xstricmp(s,_CXML("GB2312"))) return char_encoding_GB2312;
    if (!xstricmp(s,_CXML("Big5"))) return char_encoding_Big5;
    if (!xstricmp(s,_CXML("GBK"))) return char_encoding_GBK;
    if (!xstricmp(s,_CXML("ERROR"))) return char_encoding_error;
    return char_encoding_Legacy;
}

/************************************************************************/
/*                                                                      */
/*                        IJSONPullParser                               */
/*                                                                      */
/************************************************************************/

IJSONPullParser::IJSONPullParser(IXMLReader *uReader,int _szByteBuffer):
IPullParser(uReader,_szByteBuffer),nextChar(-1), tagBufferSize(512), _isEmptyNode(false)
{
    tagNameForArray=IXMLStringDup(_CXML("a"));
    _nAttribute=0; _isIgnorableSpace=false;
    allCurrentTags=(IXMLStr*)malloc(tagBufferSize);
    allCurrentTags[0]=(IXMLStr)(((char*)allCurrentTags)+tagBufferSize);
}

IJSONPullParser::~IJSONPullParser()
{
    if (tagNameForArray) { free(tagNameForArray); tagNameForArray=NULL; }
}

void IJSONPullParser::reset()
{
    IPullParser::reset();
    nextChar=-1; _isEmptyNode=false;
}

void IJSONPullParser::setTagNameForArrays(IXMLCStr arrayName)
{ 
    if (tagNameForArray) free(tagNameForArray);
    tagNameForArray=IXMLStringDup(arrayName); 
}
IXMLCStr IJSONPullParser::getTagNameForArrays(){ return tagNameForArray; }

IXMLCStr IJSONPullParser::guessJSONCharEncoding(unsigned char *b,int l)
{
    static IXMLCStr encodingUTF8=_CXML("utf-8"),encodingUTF16=_CXML("utf-16");
    if (l<4) return encodingUTF8;
    if (((!b[1])&&(!b[3]))||
        ((b[0]==0xFF)&&(b[1]==0xFE))||
        ((b[0]==0xFE)&&(b[1]==0xFF))) return encodingUTF16;
    return encodingUTF8;
}

IPullParser::IXMLCharEncoding IJSONPullParser::guessJSONCharEncodingCode(unsigned char *b,int l)
{
    IXMLCStr s=guessJSONCharEncoding(b,l);
    if ((!xstricmp(s,_CXML("utf-16")))||
        (!xstricmp(s,_CXML("utf16")))) return char_encoding_UTF16;
    if (!xstricmp(s,_CXML("ERROR"))) return char_encoding_error;
    return char_encoding_UTF8;
}

char IJSONPullParser::parseStartList(IXMLStr *_ptr)
{
    int ll=0,ch;
    IXMLStr ptr=*_ptr,pend=pEndPos;
    IXMLStr ptrOut=NULL,// fake;   
        pStart; 
    XML_getNextChar(); // init ch
    XML_skipSpaceChars();
    pContent=ptr;
    if (ch==_CXML('}')) 
    {
        *ptr=0;
        XML_getNextChar();
        *_ptr=ptr;
        _isEmptyNode=true;
        return 0;
    }
    if (ch!=_CXML('"')) { 
        lastError=IXMLError_QuoteOrDoubleQuoteExpected; return 1; }
    XML_getNextChar(); // skip "
    if (parseString(&ptr)) return 1;   

    addToTagList(pContent,_szText);
    XML_getNextChar(); // skip "
    XML_skipSpaceChars();
    if (ch!=_CXML(':')) { lastError=IXMLError_ColonExpected; return 1; }
    XML_getNextChar(); // skip :
    XML_skipSpaceChars();
    *_ptr=ptr;
    return 0;
}
char IJSONPullParser::parseNumber(IXMLStr *_ptr, int *nextChar)
{
    IXMLStr ptr=*_ptr,pend=pEndPos;
    IXMLStr ptrOut=NULL,pStart=NULL; // fake
    pContent=ptr;
    int ch,ll=0;
    XML_getNextChar();
    while((XML_isNumber(ch))||(ch==_CXML('.'))||(ch==_CXML('E'))||(ch==_CXML('e'))||(ch==_CXML('+'))||(ch==_CXML('-')))
    {
        XML_getNextChar();
    }
    if ((ch==_CXML('N'))||(ch==_CXML('n')))
    {
        XML_peekSimpleChar(5);
        XML_getNextChar(); // skip n
        if (((*ptr==_CXML('A'))||(*ptr==_CXML('a')))&&
            ((ptr[1]==_CXML('N'))||(*ptr==_CXML('n'))))
        {
            ll=0; ptr+=2; XML_getNextChar();
            if ((XML_isSPACECHAR(ch))||(ch==_CXML(','))||(ch==_CXML('}'))||(ch==_CXML(']'))||(!ch))
            {
                pContent=(IXMLStr)_CXML("nan"); _szText=3;
                XML_skipSpaceChars();
                *nextChar=*ptr;
                *_ptr=ptr;
                return 0;
            }
        }
        lastError=IXMLError_invalidCharInNumber;
        return 1;
    }
    if ((ch==_CXML('i'))||(ch==_CXML('I')))
    {
        XML_peekSimpleChar(5);
        XML_getNextChar(); // skip n
        if (((*ptr==_CXML('N'))||(*ptr==_CXML('n')))&&
            ((ptr[1]==_CXML('F'))||(*ptr==_CXML('f'))))
        {
            ll=0; ptr+=2; XML_getNextChar();
            if ((XML_isSPACECHAR(ch))||(ch==_CXML(','))||(ch==_CXML('}'))||(ch==_CXML(']'))||(!ch))
            {
                if (*pContent==_CXML('-')) { pContent=(IXMLStr)_CXML("-inf"); _szText=4; }
                else { pContent=(IXMLStr)_CXML("inf"); _szText=3; }
                XML_skipSpaceChars();
                *nextChar=*ptr;
                *_ptr=ptr;
                return 0;
            }
        }
        lastError=IXMLError_invalidCharInNumber;
        return 1;
    }
    _szText=(int)(ptr-pContent);
    if (XML_isSPACECHAR(ch))
    {
        *ptr=0;
        XML_getNextChar();
        XML_skipSpaceChars();
        *nextChar=*ptr;
        *_ptr=ptr;
        return 0;
    }
    if ((ch==_CXML(','))||(ch==_CXML('}'))||(ch==_CXML(']'))||(!ch))
    {
        *nextChar=*ptr;
        *ptr=0;
        *_ptr=ptr;
        return 0;
    }
    lastError=IXMLError_invalidCharInNumber;
    return 1;
}

inline int JSON_HEXVALUE(int a)
{
    if (a<='9') return a-'0';
    if (a<'a') return a-'A'+10;
    return a-'a'+10;
}
//#define JSON_HEXVALUE(a) (((a)<='9')?((a)-'0'):((a)-'A'+10))

char IJSONPullParser::parseString(IXMLStr *_ptr)
{
    IXMLStr ptr=*_ptr,ptrOut=ptr,pStart=ptrOut,pend=pEndPos;
    pContent=ptr;
    int ch,ll=0;
    XML_getNextChar();

    while((ch!=_CXML('"'))&&(ch))
    {
        if (ch==_CXML('\n'))
        {                                           
            *ptrOut=_CXML('\n'); ptrOut++;
            pStartOfLine=ptr;
            nLine++;                                
            XML_getNextChar();
            if (ch==_CXML('\r'))                    
            {                                       
                pStartOfLine=ptr;                   
                XML_getNextChar()                   
            }                                       
        } else if (ch==_CXML('\r'))                 
        {                                           
            *ptrOut=_CXML('\n'); ptrOut++;
            pStartOfLine=ptr;                       
            nLine++;                                
            XML_getNextChar();
            if (ch==_CXML('\n'))                    
            {                                       
                pStartOfLine=ptr;                   
                XML_getNextChar()                   
            }
        } else if (ch==_CXML('\\'))
        {
            XML_getNextChar();
            switch(ch)
            {
            case _CXML('"'): *(ptrOut++)=_CXML('"'); break;
            case _CXML('\\'): *(ptrOut++)=_CXML('\\'); break;
            case _CXML('/'): *(ptrOut++)=_CXML('/'); break;
            case _CXML('b'): *(ptrOut++)=_CXML('\b'); break;
            case _CXML('f'): *(ptrOut++)=_CXML('\f'); break;
            case _CXML('n'): *(ptrOut++)=_CXML('\n'); break;
            case _CXML('r'): *(ptrOut++)=_CXML('\r'); break;
            case _CXML('t'): *(ptrOut++)=_CXML('\t'); break;
            case _CXML('u'):
                {
                    XML_peekSimpleChar(5);
                    unsigned int i= JSON_HEXVALUE(ptr[4])
                                  +(JSON_HEXVALUE(ptr[3])<<4)
                                  +(JSON_HEXVALUE(ptr[2])<<8)
                                  +(JSON_HEXVALUE(ptr[1])<<12);
                    if (!i) return 1;
#ifdef _XMLWIDECHAR
                    *(ptrOut++)=i;
#else
                    ENCODE_IN_UTF8(i,ptrOut);
#endif
                    ptr+=4;
                }
            }
            XML_getNextChar();
        } else
        {
#ifdef _XMLWIDECHAR
            *(ptrOut++)=ch;
            ptr++;                                          
            XML_peekSimpleChar(1);                          
            ch=*ptr;                                        
#else
            COPY_ONE_XML_CHAR(ll,ptrOut,ptr);
            ll=XML_ByteTable[(unsigned char)(*ptr)];        
            XML_peekSimpleChar(ll);                         
            ch=*((unsigned char*)ptr);
#endif
        }
    }
    *ptrOut=0;
    _szText=(int)(ptrOut-pStart);
    *_ptr=ptr;
    return 0;
}

void IJSONPullParser::addToTagList(IXMLStr p,int l)
{
    IXMLStr t=allCurrentTags[depth]-l-1;
    depth++;
    while(t<(IXMLStr)(allCurrentTags+depth+1))
    {
        // double the size and move everything;
        unsigned char *newvtb=(unsigned char *)realloc(allCurrentTags,tagBufferSize*2);
        allCurrentTags=(IXMLStr*)newvtb;
        int i=depth*sizeof(IXMLStr)-4;
        memmove(newvtb+tagBufferSize+i,newvtb+i,tagBufferSize-i);
        tagBufferSize*=2;
        size_t offset=(newvtb+tagBufferSize)-(*((unsigned char**)newvtb));
        for(i=0;i<depth;i++) ((char**)newvtb)[i]=(((char**)newvtb)[i])+offset;
        t=(IXMLStr)(((char*)t)+offset);
    }
    if (l) memcpy(t,p,(l+1)*sizeof(IXMLChar)); else *t=0;
    allCurrentTags[depth]=t;
}

IPullParser::IXMLToken IJSONPullParser::nextToken()
{
    if ((curToken==IXMLToken_Error)||(curToken==IXMLToken_EndDocument))
        return curToken;

    if (_isEmptyNode) 
    {
        _isEmptyNode=false;
        curToken=IXMLToken_EndTag;
        return IXMLToken_EndTag; 
    }

    IXMLStr ptr=pCurrentPos,pend=pEndPos;
    IXMLStr ptrOut=NULL,pStart=NULL; // fake;

    if (curToken==IXMLToken_StartDocument)
    {
        if (!bufferChar)
        {
            bufferChar=(unsigned char*)malloc(szByteBuffer);
            if (!bufferChar) { lastError=IXMLError_MemoryAllocationError; RETURN_CODE(IXMLToken_Error); }
        }
        ptr=pend=pContent=pStartOfLine=pCurrentPos=pEndPos=(IXMLStr)bufferChar;

        uReader->setNormalBufferSize(szByteBuffer); uReader->reset();
        IXMLReader_PassThrough passThrough(sourceUReader);

        int l=passThrough.nByteRead();
        if (!l) { lastError=IXMLError_Empty; RETURN_CODE(IXMLToken_Error); }

        if (autoDetectSource)
        {
            IXMLCharEncoding sourceEncoding=guessJSONCharEncodingCode(passThrough.b,l);
#ifdef _XMLWIDECHAR
            setSourceCharEncoding(sourceEncoding);
#else
            setCharEncoding(sourceEncoding,char_encoding_UTF8);
#endif
        }

        passThrough.skipBOM();

        // empty-out the passThrough buffer
        IXMLReader *temp=uReader->getSource(); 
        if (!temp) { temp=uReader; uReader=&passThrough; XML_peekSimpleChar2(1024, uReader=temp;); }
        else { uReader->setSource(&passThrough); XML_peekSimpleChar2(1024, uReader->setSource(temp);); }
    }
    int ch=nextChar,ll=0;
    if (ch<0) XML_getNextChar2()
    else { nextChar=-1; ll=0; /* we just got out of a number or a comma */ }

    if (!ch)
    {
        RETURN_CODE(IXMLToken_EndDocument);
    }

    pContent=ptr;
    if (ch==_CXML('{'))
    {
        _isInArray=false;
        XML_getNextChar2(); // skip {
        if (parseStartList(&ptr)) RETURN_CODE(IXMLToken_Error);
        RETURN_CODE(IXMLToken_StartTag);
    }
    if (ch==_CXML('['))
    {
        _isInArray=true;
        XML_getNextChar2()
        XML_skipSpaceChars2();
        {
            pContent=tagNameForArray;
            // save tag
            addToTagList(NULL,0);
        }
        RETURN_CODE(IXMLToken_StartTag);
    }
    if (ch==_CXML(','))
    {
        XML_getNextChar2()
        XML_skipSpaceChars2();
        pContent=allCurrentTags[depth]; depth--;
        if (*pContent) { nextChar=_CXML('{'); _isInArray=false; }
        else { pContent=tagNameForArray; nextChar=_CXML('['); _isInArray=true; }
        RETURN_CODE(IXMLToken_EndTag);
    }
    if ((ch==_CXML('}'))||(ch==_CXML(']')))
    {
        XML_getNextChar2()
        XML_skipSpaceChars2();
        pContent=allCurrentTags[depth]; depth--;
        if (*pContent) _isInArray=false;
        else { pContent=tagNameForArray; _isInArray=true; }
        RETURN_CODE(IXMLToken_EndTag);
    }
    if (ch==_CXML('"'))
    {
        XML_getNextChar2(); // skip "
        if (parseString(&ptr)) RETURN_CODE(IXMLToken_Error);
        XML_getNextChar2(); // skip "
        XML_skipSpaceChars2();
        RETURN_CODE(IXMLToken_Text);
    }
    if (ch==_CXML('t'))
    {
        XML_peekSimpleChar2(5,);
        XML_getNextChar2(); // skip t
        if ((*ptr==_CXML('r'))&&
            (ptr[1]==_CXML('u'))&&
            (ptr[2]==_CXML('e')))
        {
            ll=0; ptr+=3; XML_getNextChar2();
            if ((XML_isSPACECHAR(ch))||(ch==_CXML(','))||(ch==_CXML('}'))||(ch==_CXML(']'))||(!ch))
            {
                pContent=(IXMLStr)_CXML("true"); _szText=4;
                XML_skipSpaceChars2();
                RETURN_CODE(IXMLToken_Text);
            }
        }
        lastError=IXMLError_trueExpected;
        RETURN_CODE(IXMLToken_Error);
    }
    if (ch==_CXML('f'))
    {
        XML_peekSimpleChar2(5,);
        XML_getNextChar2(); // skip f
        if ((*ptr==_CXML('a'))&&
            (ptr[1]==_CXML('l'))&&
            (ptr[2]==_CXML('s'))&&
            (ptr[3]==_CXML('e')))
        {
            ll=0; ptr+=4; XML_getNextChar2();
            if ((XML_isSPACECHAR(ch))||(ch==_CXML(','))||(ch==_CXML('}'))||(ch==_CXML(']'))||(!ch))
            {
                pContent=(IXMLStr)_CXML("false"); _szText=5;
                XML_skipSpaceChars2();
                RETURN_CODE(IXMLToken_Text);
            }
        }
        lastError=IXMLError_falseExpected;
        RETURN_CODE(IXMLToken_Error);
    }
    if (ch==_CXML('n'))
    {
        XML_peekSimpleChar2(5,);
        XML_getNextChar2(); // skip n
        if ((*ptr==_CXML('u'))&&
            (ptr[1]==_CXML('l'))&&
            (ptr[2]==_CXML('l')))
        {
            ll=0; ptr+=3; XML_getNextChar2();
            if ((XML_isSPACECHAR(ch))||(ch==_CXML(','))||(ch==_CXML('}'))||(ch==_CXML(']'))||(!ch))
            {
                pContent=(IXMLStr)_CXML("null"); _szText=4;
                XML_skipSpaceChars2();
                RETURN_CODE(IXMLToken_Text);
            }
        }
        lastError=IXMLError_nullExpected;
        RETURN_CODE(IXMLToken_Error);
    }
    if (parseNumber(&ptr,&nextChar)) RETURN_CODE(IXMLToken_Error);
    XML_getNextChar2();
    RETURN_CODE(IXMLToken_Text);
}

/************************************************************************/
/*                                                                      */
/*                            IXMLDomParser                             */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*           BlockMemoryManager used by the IXMLDomParser               */
/************************************************************************/
#if defined( _ARM_ ) || defined( _M_ARM )
#define BMM_ALIGNMENTBYTES          sizeof(int)
#else
#define BMM_ALIGNMENTBYTES          sizeof(char)
#endif
#define BMM_ALIGNMENTBYTESMINUSONE  (BMM_ALIGNMENTBYTES - 1)

class BlockMemoryManager
{
public:
    BlockMemoryManager(int _blockSize);
    ~BlockMemoryManager(){clear();}
    void *alloc(int sz);
    void clear();
    void reset();
    void setBlockSize(int _blockSize);
    int getBlockSize() const { return blockSize; };
    size_t getMemSz();
private:
    unsigned char *nextBlock,*topBlock,*curPosInBlock,*topBigBlock;
    int blockSize,curRemainingSpace;
    size_t memsz;
    // structure of a block is:
    //    unsigned char*: pointer to next block (this is a linked list of blocks)
    //    unsigned char:  all data
};

BlockMemoryManager::BlockMemoryManager(int _blockSize)
:nextBlock(NULL),topBlock(NULL),curPosInBlock(NULL),topBigBlock(NULL), 
 blockSize(_blockSize),curRemainingSpace(0),memsz(0)
{}

size_t BlockMemoryManager::getMemSz(){return memsz;}

void *BlockMemoryManager::alloc(int sz)
{
    // align blocks on multiples of architecture word-size, then discard
    // invalid blocks by returning a null pointer (as an aid in detecting
    // erroneous dereferentations)
    sz = (sz + BMM_ALIGNMENTBYTESMINUSONE) & ~BMM_ALIGNMENTBYTESMINUSONE;
    if (sz <= 0) {
        return NULL;
    }
    curRemainingSpace-=sz;
    if (curRemainingSpace<0)
    {
        if (sz+(int)sizeof(unsigned char*)>blockSize) 
        {
            curRemainingSpace+=sz;
            memsz+=sz+sizeof(unsigned char*);
            unsigned char *bigBlock=(unsigned char*)malloc(sz+sizeof(unsigned char*));
            *((unsigned char**)bigBlock)=topBigBlock; topBigBlock=bigBlock;
            return bigBlock+sizeof(unsigned char*);
        }
        if (nextBlock) 
        {
            // re-use previously allocated blocks
            curPosInBlock=nextBlock+sizeof(unsigned char*);
            nextBlock=*((unsigned char**)(nextBlock));
            curRemainingSpace=blockSize-sizeof(unsigned char*)-sz;
        } else
        {
            unsigned char *newBlock=(unsigned char*)malloc(blockSize);
            memsz+=blockSize;
            *((unsigned char**)(newBlock))=topBlock;
            topBlock=newBlock;
            curPosInBlock=newBlock+sizeof(unsigned char*);
            curRemainingSpace=blockSize-sizeof(unsigned char*)-sz;
        }
    }
    unsigned char*t=curPosInBlock;
    curPosInBlock+=sz; 
#if defined( BMM_CLEARALLOCATEDMEMORY )
    memset(t, 0x00, sz);
#endif
    return t;
}
void BlockMemoryManager::clear()
{
    reset(); // to free the bigblocks
    unsigned char *c=topBlock,*cNext;
    while(c) { cNext=*((unsigned char**)(c)); free(c); c=cNext; }
    curPosInBlock=nextBlock=topBlock=NULL; curRemainingSpace=0; memsz=0;
}
void BlockMemoryManager::reset()
{
    nextBlock=topBlock; curRemainingSpace=0;
    // always free the bigblocks
    unsigned char *c=topBigBlock,*cNext;
    while(c) { cNext=*((unsigned char**)(c)); free(c); c=cNext; }
    topBigBlock=NULL;
}
void BlockMemoryManager::setBlockSize(int _blockSize)
{
    if (blockSize!=_blockSize) { clear(); blockSize=_blockSize; }
}

/************************************************************************/
/*                            IXMLDomParser                             */
/************************************************************************/

IXMLDomParser::IXMLDomParser():
#ifdef _XMLWIDECHAR
    sourceEncoding(IXMLPullParser::char_encoding_UTF16),
    autoDetectSource(true),
#else
    destinationEncoding(IXMLPullParser::char_encoding_UTF8),sourceEncoding(IXMLPullParser::char_encoding_UTF8),
    autoDetectSource(false),
#endif
    skipIgnorableSpaces(true),requiresTrimText(true),trimAttribute(true),
    removeClears(true),removeProcessingInstructions(true),_isHTML(false),
    memBufferSz(1024*1024),pullParserMemSz(1024*1024),maxMemDom(0),memoryManager(NULL)
{}

IXMLDomParser::~IXMLDomParser(){clear();}
void IXMLDomParser::clear()
{
    if (memoryManager) 
    {
        delete((BlockMemoryManager*)memoryManager); memoryManager=NULL; 
        free(vChild); 
        free(vText);
        free(vOrder);
        if (!removeClears) free(vClear);
    }
}
void IXMLDomParser::setDomMemoryBlockSize(int blockSize)
{
    if (memoryManager) ((BlockMemoryManager*)memoryManager)->setBlockSize(blockSize);
    memBufferSz=blockSize;
}
int  IXMLDomParser::getDomMemoryBlockSize(){return memBufferSz;}
void IXMLDomParser::setDomMaxMemorySize(size_t _maxMem){maxMemDom=_maxMem;}
size_t IXMLDomParser::getDomMaxMemorySize(){return maxMemDom;}
void IXMLDomParser::setDefaultPullParserWorkspaceMemorySize(int memSize){ pullParserMemSz=memSize; }
int  IXMLDomParser::getDefaultPullParserWorkspaceMemorySize(){ return pullParserMemSz; }
void IXMLDomParser::setAutoDetectSourceEncoding(bool _autoDetectSource){ autoDetectSource=_autoDetectSource; }
bool IXMLDomParser::getAutoDetectSourceEncoding(){ return autoDetectSource; }
void IXMLDomParser::setSkipIgnorableSpaces(bool _skipIgnorableSpaces){ skipIgnorableSpaces=_skipIgnorableSpaces;}
bool IXMLDomParser::getSkipIgnorableSpaces(){return skipIgnorableSpaces;}
void IXMLDomParser::setTrimTexts(bool _trimText){ requiresTrimText=_trimText;}
bool IXMLDomParser::getTrimTexts(){return requiresTrimText;}
void IXMLDomParser::setTrimAttributes(bool _trimAttribute){ trimAttribute=_trimAttribute;}
bool IXMLDomParser::getTrimAttributes(){return trimAttribute;}
void IXMLDomParser::setRemoveClears(bool _removeClears){ removeClears=_removeClears;}
bool IXMLDomParser::getRemoveClears(){return removeClears;}
void IXMLDomParser::setRemoveProcessingInstructions(bool _removeProcessingInstructions){ removeProcessingInstructions=_removeProcessingInstructions;}
bool IXMLDomParser::getRemoveProcessingInstructions(){return removeProcessingInstructions;}
void IXMLDomParser::setHTML(bool isHTML){ _isHTML=isHTML; }
bool IXMLDomParser::isHTML() const { return _isHTML;}

IXMLPullParser::IXMLCharEncoding IXMLDomParser::getCharEncodingSource(){ return sourceEncoding; }
#ifdef _XMLWIDECHAR
void IXMLDomParser::setSourceCharEncoding(IXMLPullParser::IXMLCharEncoding e){ sourceEncoding=e; }
#else
void IXMLDomParser::setCharEncoding(IXMLPullParser::IXMLCharEncoding e, IXMLPullParser::IXMLCharEncoding d)
{
    sourceEncoding=e;
    destinationEncoding=d;
}
IXMLPullParser::IXMLCharEncoding IXMLDomParser::getCharEncodingDestination(){ return destinationEncoding; }
#endif

ITCXMLNode IXMLDomParser::parse(IPullParser *pp, IXMLCStr tag, IXMLResults *_pResults)
{
    if (_pResults) pResults=_pResults; else pResults=&xmlRes;
    pullParser=pp;
    return genericParse2(false);
}
ICXMLNode IXMLDomParser::parseNonT(IPullParser *pp, IXMLCStr tag, IXMLResults *_pResults)
{
    return parse(pp,tag,_pResults).deepCopyConstant();
}
ITCXMLNode IXMLDomParser::parseOneChild(IPullParser *pp, IXMLResults *_pResults)
{
    if (_pResults) pResults=_pResults; else pResults=&xmlRes;
    pullParser=pp;
    return genericParse2(true);
}
ICXMLNode  IXMLDomParser::parseOneChildNonT(IPullParser *pp, IXMLResults *_pResults)
{
    return parseOneChild(pp,pResults).deepCopyConstant();
}

ITCXMLNode IXMLDomParser::parseString(IXMLCStr lpXMLString, IXMLCStr tag, IXMLResults *_pResults)
{
    if (_pResults) pResults=_pResults; else pResults=&xmlRes;
    if (!lpXMLString)
    {
        pResults->nLine=1;
        pResults->nColumn=0;
        pResults->errorCode=IXMLError_Empty;
        return ITCXMLNode::emptyNode();
    }
    IXMLReaderStringSZ r(lpXMLString);
    IXMLPullParser pp(&r,pullParserMemSz);
    pullParser=&pp;
    return genericParse(tag);
}

ICXMLNode IXMLDomParser::parseStringNonT(IXMLCStr lpXMLString, IXMLCStr tag, IXMLResults *_pResults)
{
    return parseString(lpXMLString,tag,_pResults).deepCopyConstant();
}

ITCXMLNode IXMLDomParser::parseFile(IXMLCStr filename, IXMLCStr tag, IXMLResults *_pResults)
{
    if (_pResults) pResults=_pResults; else pResults=&xmlRes;
    IXMLReaderFile r(filename);
    if (!r.f) { pResults->errorCode=IXMLError_FileNotFound; pResults->nLine=0; pResults->nColumn=0; return ITCXMLNode::emptyNode(); }
    IXMLPullParser pp(&r,pullParserMemSz);
    pullParser=&pp;
    return genericParse(tag);
}

ICXMLNode IXMLDomParser::parseFileNonT(IXMLCStr filename, IXMLCStr tag, IXMLResults *_pResults)
{
    return parseFile(filename,tag,_pResults).deepCopyConstant();
}

#define CREATE_NEW_NODE                                                                     \
                {                                                                           \
                    td->nChild=lastChild-iChild;                                            \
                    if (td->nChild)                                                         \
                    {                                                                       \
                        td->pChild=(ITCXMLNode*)mm->alloc(td->nChild*sizeof(ITCXMLNode));   \
                        memcpy(td->pChild,vChild+iChild,td->nChild*sizeof(ITCXMLNode));     \
                    }                                                                       \
                    td->nText=lastText-iText;                                               \
                    if (td->nText)                                                          \
                    {                                                                       \
                        td->pText=(IXMLCStr*)mm->alloc(td->nText*sizeof(IXMLCStr));         \
                        memcpy(td->pText,vText+iText,td->nText*sizeof(IXMLCStr));           \
                    }                                                                       \
                    td->nClear=lastClear-iClear;                                            \
                    if (td->nClear)                                                         \
                    {                                                                       \
                        td->pClear=(IXMLClear*)mm->alloc(td->nClear*sizeof(IXMLClear));     \
                        memcpy(td->pClear,vClear+iClear,td->nClear*sizeof(IXMLClear));      \
                    }                                                                       \
                    int i=td->nChild+td->nText+td->nClear;                                  \
                    if (i)                                                                  \
                    {                                                                       \
                        td->pOrder=(int*)mm->alloc(i*sizeof(int));                          \
                        memcpy(td->pOrder,vOrder+iOrder,i*sizeof(int));                     \
                    }                                                                       \
                }

IXMLCStr trimText(IXMLCStr t, int *_l)
{
    IXMLCStr tEnd=t+*_l-1;
    while((*t)&&(XML_isSPACECHAR(*t))) t++;
    if (!(*t)) { *_l=0; return t; }
    while(XML_isSPACECHAR(*tEnd)) tEnd--;
    *_l=(int)(tEnd-t+1);
    return t;
}

void *IXMLDomParser::initNTD()
{
    BlockMemoryManager *mm=(BlockMemoryManager*)memoryManager;
    ITCXMLNode::IXMLNodeData *ntd=(ITCXMLNode::IXMLNodeData*)mm->alloc(sizeof(ITCXMLNode::IXMLNodeData));
    ntd->init(NULL,false);
    {
        /* copy tag name */
        IXMLCStr t=pullParser->getName();
        int l=xstrlen(t),j=(l+1)*sizeof(IXMLChar);
#ifdef ANATELLA
        IXMLStr d=(IXMLStr)(((char*)mm->alloc(j+4))+4); *(((int*)d)-1)=l;
#else
        IXMLStr d=(IXMLStr)mm->alloc(j);
#endif
        ntd->pName=(IXMLCStr)d;
        memcpy(d,t,j);
    }
    {
        /* copy all attributes */
        int i,j,na=pullParser->nAttribute(),l;
        IXMLStr d; IXMLCStr t;
        const IXMLAttr *att;
        ntd->nAttribute=na;
        ntd->pAttribute=(IXMLAttr*)mm->alloc(na*sizeof(IXMLAttr));
        for(i=0;i<na;i++)
        {
            att=pullParser->getAttribute(i);

            l=(xstrlen(att->sName)+1)*sizeof(IXMLChar);
            d=(IXMLStr)(ntd->pAttribute[i].sName=(IXMLCStr)mm->alloc(l));
            memcpy((void*)d,att->sName,l);

            l=ntd->pAttribute[i].szValue=att->szValue;
            t=att->sValue;
            if (trimAttribute) t=trimText(t,&l);
            j=(l+1)*sizeof(IXMLChar);
#ifdef ANATELLA
            IXMLStr d=(IXMLStr)(((char*)mm->alloc(j+4))+4); *(((int*)d)-1)=l;
#else
            IXMLStr d=(IXMLStr)mm->alloc(j);
#endif
            ntd->pAttribute[i].sValue=(IXMLCStr)d;
            memcpy((void*)d,t,j);
            d[l]=0;
        }
    }
    return ntd;
}

char IXMLDomParser::genericParseRecur(int iChild,int iText, int iOrder, int iClear, void *_td, bool getOneChild)
{
    ITCXMLNode::IXMLNodeData *td=(ITCXMLNode::IXMLNodeData*)_td;
    BlockMemoryManager *mm=(BlockMemoryManager*)memoryManager;
    int lastChild=iChild,lastText=iText,lastOrder=iOrder,lastClear=iClear;
    IXMLPullParser::IXMLToken tok;
    for(;;)
    {
        if (maxMemDom&&(mm->getMemSz()>=maxMemDom))
        {
            pResults->errorCode=IXMLError_MaxMemoryReached; return 1;
        }
        tok=pullParser->nextToken();
        switch(tok)
        {
            case IXMLPullParser::IXMLToken_StartTag:
            {

#define UPDATE_vORDER(X)                                                                \
                if (lastOrder==nOrder)                                                  \
                {                                                                       \
                    nOrder*=2;                                                          \
                    vOrder=(int*)realloc(vOrder,nOrder*sizeof(int));                    \
                }                                                                       \
                vOrder[lastOrder]=((last##X -i##X )<<2)+(int)(ITCXMLNode::eNode##X );    \
                lastOrder++;

#ifdef APPROXIMATE_PARSING
                if (td->pName&&(!xstricmp(pullParser->getName(),td->pName))){ CREATE_NEW_NODE; return 2; }
addChildAtRightLevel:
#endif

#define INIT_ADD_CHILD                                                                  \
                UPDATE_vORDER(Child);                                                   \
                if (lastChild==nChild)                                                  \
                {                                                                       \
                    nChild*=2;                                                          \
                    vChild=(IXMLNode*)realloc(vChild,nChild*sizeof(ITCXMLNode*));       \
                }                                                                       \
                ITCXMLNode::IXMLNodeData *ntd=(ITCXMLNode::IXMLNodeData*)initNTD();     \
                ntd->pParent=td;                                                        \
                vChild[lastChild].d=ntd;                                                \
                lastChild++;

                INIT_ADD_CHILD

                char errorCode=genericParseRecur(lastChild,lastText,lastOrder,lastClear,ntd,false);
#ifndef STRICT_PARSING
                if (errorCode==1) goto forceCloseTag;
#endif
#ifdef APPROXIMATE_PARSING
                if (errorCode==2) goto addChildAtRightLevel; 
#endif
                if (errorCode) return errorCode;
                if (getOneChild) 
                {
                    CREATE_NEW_NODE
                    return 0;
                }
                break;
            }
        case IXMLPullParser::IXMLToken_EndTag:
            {
                if (getOneChild)
                {
                    pResults->errorCode=IXMLError_NoXMLTagFound; 
                    return 1;
                }
                // gather all contents of:
                //      vChild+iChild..lastChild, 
                //      vText+iText...lastText,
                //      vClear+iClear...lastClear,
                //      vOrder+iOrder..iOrder
                // ...and create a new XMLNode with it!

forceCloseTag:
                CREATE_NEW_NODE
                if((!td->pName)||(xstricmp(pullParser->getName(),td->pName))) // name do not match!
                {
#ifdef STRICT_PARSING
                    if (!pResults->errorCode) pResults->errorCode=IXMLError_UnmatchedEndTag;
                    return 3;
#else
                    if (!pResults->errorCode) pResults->errorCode=IXMLError_MissingEndTag; 
                    return 1;
#endif
                }
                return 0;
            }
        case IXMLPullParser::IXMLToken_EndDocument:
            {
                CREATE_NEW_NODE
                if (td->pName)
                {
#ifdef STRICT_PARSING
                    if (!pResults->errorCode) pResults->errorCode=IXMLError_UnmatchedEndTag;
                    return 3;
#else
                    if (!pResults->errorCode) pResults->errorCode=IXMLError_MissingEndTag; 
                    return 1;
#endif
                }
                return 0;
            }
        case IXMLPullParser::IXMLToken_Text:
            {
                if (skipIgnorableSpaces&&pullParser->isTextWhiteSpace()) 
                    break;
                UPDATE_vORDER(Text);
                if (lastText==nText)
                {
                    nText*=2;
                    vText=(IXMLCStr*)realloc(vText,nText*sizeof(IXMLCStr));
                }
                int l=pullParser->szText(),i;
                IXMLCStr t=pullParser->getText();
                if (requiresTrimText) t=trimText(t,&l);
                i=(l+1)*sizeof(IXMLChar);
#ifdef ANATELLA
                IXMLStr d=(IXMLStr)(((char*)mm->alloc(i+4))+4); *(((int*)d)-1)=l;
#else
                IXMLStr d=(IXMLStr)mm->alloc(i);
#endif
                vText[lastText]=(IXMLCStr)d;
                memcpy((void*)d,t,i);
                d[l]=0;
                lastText++;
                break;
            }
        case IXMLPullParser::IXMLToken_Clear:
            {
                if (removeClears) 
                    break;
                UPDATE_vORDER(Clear);
                if (lastClear==nClear)
                {
                    nClear*=2;
                    vClear=(IXMLClear*)realloc(vClear,nClear*sizeof(IXMLClear));
                }
                const IXMLClear *c=pullParser->getClear();
                int l=(c->szValue+1)*sizeof(IXMLChar);
#ifdef ANATELLA
                IXMLStr t=(IXMLStr)(((char*)mm->alloc(l+4))+4); *(((int*)t)-1)=c->szValue;
#else
                IXMLCStr t=(IXMLCStr)mm->alloc(l);
#endif
                memcpy((void*)t,c->sValue,l);
                vClear[lastClear]=*c;
                vClear[lastClear].sValue=t;
                lastClear++;
                break;
            }
        case IXMLPullParser::IXMLToken_Error:
            {
                pResults->errorCode=pullParser->getErrorCode();
                pResults->nLine=pullParser->getLineNumber();
                pResults->nColumn=pullParser->getColumnNumber();
                return 3;
            }
        case IXMLPullParser::IXMLToken_ProcessingInstruction:
            {
                if (removeProcessingInstructions)
                    break;
                INIT_ADD_CHILD
                ntd->isProcessInstruction=true;
                ntd->nChild=ntd->nClear=ntd->nText=0;
                break;
            }
        };
    }
    return 0;
}

ITCXMLNode IXMLDomParser::genericParse(IXMLCStr tag)
{
    {
    #ifdef _XMLWIDECHAR
        char hasError=pullParser->setSourceCharEncoding(sourceEncoding);
    #else
        char hasError=pullParser->setCharEncoding(sourceEncoding,destinationEncoding);
    #endif
        if(hasError)
        {
            pResults->errorCode=IXMLError_CharConversionModeError;
            pResults->nLine=0; pResults->nColumn=0;
            return ITCXMLNode::emptyNode();
        }
    }
    pullParser->setHTML(_isHTML);
    pullParser->setAutoDetectSourceEncoding(autoDetectSource);

    ITCXMLNode xnode=genericParse2(false);
 
    if (autoDetectSource)
    {
        sourceEncoding=pullParser->getCharEncodingSource();
#ifndef _XMLWIDECHAR
        destinationEncoding=pullParser->getCharEncodingDestination();
#endif
    }

    // If no error occurred
    IXMLErrorInfo lastError=pResults->errorCode;
    if ((lastError==IXMLError_None)||(lastError==IXMLError_MissingEndTag))
    {
        IXMLCStr name=xnode.getName();
        if (tag&&(*tag)&&((!name)||(xstricmp(name,tag))))
        {
            xnode=xnode.getChildNode(tag);
            if (xnode.isEmpty())
            {
                pResults->errorCode=IXMLError_FirstTagNotFound;
                return ITCXMLNode::emptyNode();
            }
        }
    } else
    {
        return ITCXMLNode::emptyNode();
    }
    return xnode;
}

ITCXMLNode IXMLDomParser::genericParse2(bool getOneChild)
{
    if (memoryManager) 
    {
        ((BlockMemoryManager*)memoryManager)->reset();
    } else
    {
        memoryManager=new BlockMemoryManager(memBufferSz);
        nChild=50; vChild=(ITCXMLNode*)malloc(nChild*sizeof(ITCXMLNode)); 
        nText=10; vText=(IXMLCStr*)malloc(nText*sizeof(IXMLCStr));
        nOrder=100; vOrder=(int*)malloc(nOrder*sizeof(int));
        if (!removeClears){ nClear=10; vClear=(IXMLClear*)malloc(nClear*sizeof(IXMLClear)); }
    }
    ITCXMLNode *t=(ITCXMLNode*)((BlockMemoryManager*)memoryManager)->alloc(sizeof(IXMLNode)+sizeof(IXMLNode::IXMLNodeData));
    t->d=(ITCXMLNode::IXMLNodeData*)(t+1);
    t->d->init(NULL,false);
    pResults->errorCode=IXMLError_None; 
    genericParseRecur(0,0,0,0,t->d,getOneChild);
    IXMLErrorInfo pperr=pullParser->getErrorCode();
    if (pperr) pResults->errorCode=pperr;
    pResults->nLine=pullParser->getLineNumber(); 
    pResults->nColumn=pullParser->getColumnNumber();
    ITCXMLNode xnode=*t;
    if (!xnode.nChildNode())
    {
        if (!pResults->errorCode) pResults->errorCode=IXMLError_NoXMLTagFound; 
        return ITCXMLNode::emptyITCXMLNode; 
    }
    if ((xnode.nChildNode()==1)&&(xnode.nElement()==1)) xnode=xnode.getChildNode(); // skip the empty node
    return xnode;
}


/************************************************************************/
/*                                                                      */
/*                          IXMLRenderer                                */
/*                                                                      */
/************************************************************************/

// private:
// Creates an user friendly XML string from a given element with
// appropriate white space and carriage returns.
//
// This recurses through all subnodes then adds contents of the nodes to the
// string.
int IXMLRenderer::getStringR(ITCXMLNode::IXMLNodeData *pEntry, IXMLStr lpcMarker, int nFormat)
{
    int nResult = 0;
    int cb=nFormat<0?0:nFormat;
    int cbElement;
    int nChildFormat=-1;
    int nElementI=pEntry->nChild+pEntry->nText+pEntry->nClear;
    int i,j;
    if ((nFormat>=0)&&(nElementI==1)&&(pEntry->nText==1)&&(!pEntry->isProcessInstruction)) nFormat=-2;

    assert(pEntry);

#define LENSTR(lpc) (lpc ? xstrlen(lpc) : 0)

    // If the element has no name then assume this is the head node.
    if (pEntry->pName)
    {
        cbElement = (int)xstrlen(pEntry->pName);
        // "<elementname "
        if (lpcMarker)
        {
            if (cb) charmemset(lpcMarker, INDENTCHAR, cb);
            nResult = cb;
            lpcMarker[nResult++]=_CXML('<');
            if (pEntry->isProcessInstruction) lpcMarker[nResult++]=_CXML('?');
            xstrcpy(&lpcMarker[nResult], pEntry->pName);
            nResult+=cbElement;
            lpcMarker[nResult++]=_CXML(' ');
        } else
        {
            nResult+=cbElement+2+cb;
            if (pEntry->isProcessInstruction) nResult++;
        }

        // Enumerate attributes and add them to the string
        IXMLAttr *pAttr=pEntry->pAttribute;
        for (i=0; i<pEntry->nAttribute; i++)
        {
            // "Attrib
            cb = (int)LENSTR(pAttr->sName);
            if (cb)
            {
                if (lpcMarker) xstrcpy(&lpcMarker[nResult], pAttr->sName);
                nResult += cb;
                // "Attrib=Value "
                if (pAttr->sValue)
                {
                    cb=(int)txAtt.lengthXMLString(pAttr->sValue);
                    if (lpcMarker)
                    {
                        lpcMarker[nResult]=_CXML('=');
                        lpcMarker[nResult+1]=_CXML('"');
                        if (cb) txAtt.toXMLUnSafe(&lpcMarker[nResult+2],pAttr->sValue);
                        lpcMarker[nResult+cb+2]=_CXML('"');
                    }
                    nResult+=cb+3;
                }
                if (lpcMarker) lpcMarker[nResult] = _CXML(' ');
                nResult++;
            }
            pAttr++;
        }

        if (pEntry->isProcessInstruction)
        {
            if (lpcMarker)
            {
                lpcMarker[nResult-1]=_CXML('?');
                lpcMarker[nResult]=_CXML('>');
            }
            nResult++;
            if (nFormat!=-1)
            {
                if (lpcMarker) lpcMarker[nResult]=_CXML('\n');
                nResult++;
            }
        } else
        {
            // If there are child nodes we need to terminate the start tag
            if (nElementI)
            {
                if (lpcMarker) lpcMarker[nResult-1]=_CXML('>');
                if (nFormat>=0)
                {
                    if (lpcMarker) lpcMarker[nResult]=_CXML('\n');
                    nResult++;
                }
            } else nResult--;
        }
    }

    // Calculate the child format for when we recurse.  This is used to
    // determine the number of spaces used for prefixes.
    if (nFormat!=-1)
    {
//        if (cbElement&&(!pEntry->isProcessInstruction)) nChildFormat=nFormat+1;
        if (pEntry->pName) nChildFormat=nFormat+1;
        else nChildFormat=nFormat;
    }

    // Enumerate through remaining children
    for (i=0; i<nElementI; i++)
    {
        j=pEntry->pOrder[i];
        switch((ITCXMLNode::IXMLElementType)(j&3))
        {
        // Text nodes
        case ITCXMLNode::eNodeText:
            {
                // "Text"
                IXMLCStr pChildText=pEntry->pText[j>>2];
                cb = (int)txTxt.lengthXMLString(pChildText);
                if (cb)
                {
                    if (nFormat>=0)
                    {
                        if (lpcMarker)
                        {
                            charmemset(&lpcMarker[nResult],INDENTCHAR,nFormat+1);
                            txTxt.toXMLUnSafe(&lpcMarker[nResult+nFormat+1],pChildText);
                            lpcMarker[nResult+nFormat+1+cb]=_CXML('\n');
                        }
                        nResult+=cb+nFormat+2;
                    } else
                    {
                        if (lpcMarker) txTxt.toXMLUnSafe(&lpcMarker[nResult], pChildText);
                        nResult += cb;
                    }
                }
                break;
            }

        // Clear type nodes
        case ITCXMLNode::eNodeClear:
            {
                IXMLClear *pChildClear=pEntry->pClear+(j>>2);
                // "OpenTag"
                cb = (int)LENSTR(pChildClear->sOpenTag);
                if (cb)
                {
                    if (nFormat!=-1)
                    {
                        if (lpcMarker)
                        {
                            charmemset(&lpcMarker[nResult], INDENTCHAR, nFormat+1);
                            xstrcpy(&lpcMarker[nResult+nFormat+1], pChildClear->sOpenTag);
                        }
                        nResult+=cb+nFormat+1;
                    }
                    else
                    {
                        if (lpcMarker)xstrcpy(&lpcMarker[nResult], pChildClear->sOpenTag);
                        nResult += cb;
                    }
                }

                // "OpenTag Value"
                cb = (int)LENSTR(pChildClear->sValue);
                if (cb)
                {
                    if (lpcMarker) xstrcpy(&lpcMarker[nResult], pChildClear->sValue);
                    nResult += cb;
                }

                // "OpenTag Value CloseTag"
                cb = (int)LENSTR(pChildClear->sCloseTag);
                if (cb)
                {
                    if (lpcMarker) xstrcpy(&lpcMarker[nResult], pChildClear->sCloseTag);
                    nResult += cb;
                }

                if (nFormat!=-1)
                {
                    if (lpcMarker) lpcMarker[nResult] = _CXML('\n');
                    nResult++;
                }
                break;
            }

        // Element nodes
        case ITCXMLNode::eNodeChild:
            {
                // Recursively add child nodes
                nResult += getStringR(pEntry->pChild[j>>2].d, lpcMarker ? lpcMarker + nResult : 0, nChildFormat);
                break;
            }
        default: break;
        }
    }

    if ((pEntry->pName)&&(!pEntry->isProcessInstruction))
    {
        // If we have child entries we need to use long XML notation for
        // closing the element - "<elementname>blah blah blah</elementname>"
        if (nElementI)
        {
            // "</elementname>\0"
            if (lpcMarker)
            {
                if (nFormat >=0)
                {
                    charmemset(&lpcMarker[nResult], INDENTCHAR,nFormat);
                    nResult+=nFormat;
                }

                lpcMarker[nResult]=_CXML('<'); lpcMarker[nResult+1]=_CXML('/');
                nResult += 2;
                xstrcpy(&lpcMarker[nResult], pEntry->pName);
                nResult += cbElement;

                lpcMarker[nResult]=_CXML('>');
                if (nFormat == -1) nResult++;
                else
                {
                    lpcMarker[nResult+1]=_CXML('\n');
                    nResult+=2;
                }
            } else
            {
                if (nFormat>=0) nResult+=cbElement+4+nFormat;
                else if (nFormat==-1) nResult+=cbElement+3;
                else nResult+=cbElement+4;
            }
        } else
        {
            // If there are no children we can use shorthand XML notation -
            // "<elementname/>"
            // "/>\0"
            if (lpcMarker)
            {
                lpcMarker[nResult]=_CXML('/'); lpcMarker[nResult+1]=_CXML('>');
                if (nFormat != -1) lpcMarker[nResult+2]=_CXML('\n');
            }
            nResult += nFormat == -1 ? 2 : 3;
        }
    }

    return nResult;
}

#undef LENSTR

// Create an XML string
// @param       int nFormat             - 0 if no formatting is required
//                                        otherwise nonzero for formatted text
//                                        with carriage returns and indentation.
// @param       int *pnSize             - [out] pointer to the size of the
//                                        returned string not including the
//                                        NULL terminator.
// @return      IXMLStr                  - created XML string.
IXMLCStr IXMLRenderer::getString(ITCXMLNode x, int nFormat, int *pnSize)
{
    if (!x.d) { if (pnSize) *pnSize=0; return NULL; }
#ifndef _XMLWIDECHAR
    if (txTxt.setCharEncoding(characterEncoding)) return NULL;
    txAtt.setCharEncoding(characterEncoding);
#endif
    nFormat=nFormat?0:-1;
    // Recursively Calculate the size of the XML string
    int cbStr=getStringR(x.d,NULL,nFormat);
    if (cbStr>maxStrLen)
    {
        maxStrLen=cbStr;
        if (buf) free(buf);
        // Alllocate memory for the XML string + the NULL terminator
        buf=(IXMLStr)malloc((cbStr+1)*sizeof(IXMLChar));
    }
    // create recursively the XML string.
    getStringR(x.d, buf, nFormat);
    buf[cbStr]=_CXML('\0');
    if (pnSize) *pnSize = cbStr;
    sz=cbStr;
    return buf;
}
IXMLCStr IXMLRenderer::getString(ICXMLNode x, int f, int *p){return getString(ITCXMLNode(x.d),f,p);}
IXMLCStr IXMLRenderer::getString(IXMLNode x, int f, int *p){return getString(ITCXMLNode(x.d),f,p);}

int IXMLRenderer::szString(){return sz;}

IXMLRenderer::IXMLRenderer():
#ifndef _XMLWIDECHAR
characterEncoding(IXMLPullParser::char_encoding_UTF8),
#endif
buf(NULL),maxStrLen(-1),sz(0),
txAtt(ToIXMLStringTool::IXMLStringLocation_inAttribute),txTxt(ToIXMLStringTool::IXMLStringLocation_inText)
{}

#ifndef _XMLWIDECHAR
void IXMLRenderer::setCharEncoding(IXMLPullParser::IXMLCharEncoding e){characterEncoding=e;}
IXMLPullParser::IXMLCharEncoding IXMLRenderer::getCharEncoding(){return characterEncoding;}
#endif

IXMLRenderer::~IXMLRenderer(){clear();}
void IXMLRenderer::clear(){ if (buf) free(buf); buf=NULL; maxStrLen=0; sz=0; }

IXMLErrorInfo IXMLRenderer::writeToFile(ITCXMLNode x, IXMLCStr filename, IXMLCStr encoding, char nFormat)
{
    if (x.isEmpty()) return IXMLError_None;
    FILE *f=xfopen(filename,_CXML("wb"));
    if (!f) return IXMLError_CannotOpenWriteFile;
#ifdef _XMLWIDECHAR
    #if (WCHAR_MAX>70000)
        unsigned char h[4]={ 0xFF, 0xFE,0,0 };
        if (!fwrite(h,4,1,f)) 
        {
            fclose(f);
            return IXMLError_CannotWriteFile;
        }
        if ((!x.isProcessInstruction())&&((x.getName())||(!x.getChildNode().isProcessInstruction())))
        {
            if (!fwrite(L"<?xml version=\"1.0\" encoding=\"utf-32\"?>\n",sizeof(wchar_t)*40,1,f))
            {
                fclose(f);
                return IXMLError_CannotWriteFile;
            }
        }
    #else
        unsigned char h[2]={ 0xFF, 0xFE };
        if (!fwrite(h,2,1,f)) 
        {
            fclose(f);
            return IXMLError_CannotWriteFile;
        }
        if ((!x.isProcessInstruction())&&((x.getName())||(!x.getChildNode().isProcessInstruction())))
        {
            if (!fwrite(L"<?xml version=\"1.0\" encoding=\"utf-16\"?>\n",sizeof(wchar_t)*40,1,f))
            {
                fclose(f);
                return IXMLError_CannotWriteFile;
            }
        }
    #endif
#else
    if ((!x.isProcessInstruction())&&((x.getName())||(!x.getChildNode().isProcessInstruction())))
    {
        if (characterEncoding==IXMLPullParser::char_encoding_UTF8)
        {
            // header so that windows recognize the file as UTF-8:
            unsigned char h[3]={0xEF,0xBB,0xBF}; 
            if (!fwrite(h,3,1,f)) 
            {
        	    fclose(f);
                return IXMLError_CannotWriteFile;
            }
            encoding="utf-8";
        }
        if (!encoding) encoding=IPullParser::charEncodingText(characterEncoding);
        if (fprintf(f,"<?xml version=\"1.0\" encoding=\"%s\"?>\n",encoding)<0) 
        {
        	fclose(f);
            return IXMLError_CannotWriteFile;
        }
    } else
    {
        if (characterEncoding==IXMLPullParser::char_encoding_UTF8)
        {
            unsigned char h[3]={0xEF,0xBB,0xBF}; 
            if (!fwrite(h,3,1,f)) 
            {
            	fclose(f);
                return IXMLError_CannotWriteFile;
            }
        }
    }
#endif
    int i;
    IXMLCStr t=getString(x,nFormat,&i);
    if (!fwrite(t,sizeof(IXMLChar)*i,1,f)) 
    {
       clear(); // this "free()" the XML String
       fclose(f);
       return IXMLError_CannotWriteFile;
    }
    if (fclose(f)!=0) 
    {
        clear(); // this "free()" the XML String
        return IXMLError_CannotWriteFile;
    }
    clear(); // this "free()" the XML String
    return IXMLError_None;
}
IXMLErrorInfo IXMLRenderer::writeToFile(ICXMLNode x, IXMLCStr f, IXMLCStr e, char n){return writeToFile(ITCXMLNode(x.d),f,e,n);}
IXMLErrorInfo IXMLRenderer::writeToFile(IXMLNode x, IXMLCStr f, IXMLCStr e, char n){return writeToFile(ITCXMLNode(x.d),f,e,n);}

/************************************************************************/
/*                                                                      */
/*                 ITCXMLNode, ICXMLNode, IXMLNode                      */
/*                                                                      */
/************************************************************************/

ITCXMLNode::ITCXMLNode(IXMLNodeData *p){ d=p; }
//ITCXMLNode::ITCXMLNode(const ITCXMLNode &A){ /* shallow copy */ d=A.d; }
ITCXMLNode& ITCXMLNode::operator=( const ITCXMLNode& A ){ d=A.d; /* shallow copy */ return *this; }

void ICXMLNode::emptyTheNode()
{
    IXMLNodeData *dd=d; // warning: must stay this way!
    if (*dd->memBuffer==0)
    {
        free(dd->memBuffer);
        d=NULL;
    }
}
ICXMLNode::ICXMLNode(const ICXMLNode &A)
{
    // shallow copy
    d=A.d;
    if (d) (*d->memBuffer)++ ;
}
ICXMLNode& ICXMLNode::operator=( const ICXMLNode& A )
{
    // shallow copy
    if (this!=&A)
    {
        if (d) { (*d->memBuffer)--; emptyTheNode(); }
        d=A.d;
        if (d) (*d->memBuffer)++;
    }
    return *this;
}

ICXMLNode::~ICXMLNode()
{
    if (!d) return;
    (*d->memBuffer)--;
    emptyTheNode();
}

IXMLNode::IXMLNode(IXMLNodeData *pParent, IXMLStr pName, char isDeclaration)
{
    d=(IXMLNodeData*)malloc(sizeof(IXMLNodeData));
    d->init(pParent,isDeclaration);
    d->ref_count=1;
    updateName_WOSD(pName);
}
IXMLNode::~IXMLNode()
{
    if (!d) return;
    d->ref_count--;
    emptyTheNode(0);
}

IXMLNode& IXMLNode::operator=( const IXMLNode& A )
{
    // shallow copy
    if (this != &A)
    {
        if (d) { d->ref_count--; emptyTheNode(0); }
        d=A.d;
        if (d) (d->ref_count) ++ ;
    }
    return *this;
}

IXMLNode::IXMLNode(const IXMLNode &A)
{
    // shallow copy
    d=A.d;
    if (d) (d->ref_count)++ ;
}

IXMLNode ITCXMLNode::deepCopy() const
{
    if (!d) return IXMLNode::emptyIXMLNode;
    IXMLNode x(NULL,IXMLStringDup(d->pName),d->isProcessInstruction);
    IXMLNodeData *p=x.d;
    int n=d->nAttribute;
    if (n)
    {
        p->nAttribute=n; p->pAttribute=(IXMLAttr*)malloc(n*sizeof(IXMLAttr));
        while (n--)
        {
            p->pAttribute[n].sName=IXMLStringDup(d->pAttribute[n].sName);
            p->pAttribute[n].sValue=IXMLStringDup(d->pAttribute[n].sValue);
            p->pAttribute[n].szValue=d->pAttribute[n].szValue;
        }
    }
    if (d->pOrder)
    {
        n=(d->nChild+d->nText+d->nClear)*sizeof(int); p->pOrder=(int*)malloc(n); memcpy(p->pOrder,d->pOrder,n);
    }
    n=d->nText;
    if (n)
    {
        p->nText=n; p->pText=(IXMLCStr*)malloc(n*sizeof(IXMLCStr));
        while(n--) p->pText[n]=IXMLStringDup(d->pText[n]);
    }
    n=d->nClear;
    if (n)
    {
        p->nClear=n; p->pClear=(IXMLClear*)malloc(n*sizeof(IXMLClear));
        while (n--)
        {
            p->pClear[n].sCloseTag=d->pClear[n].sCloseTag;
            p->pClear[n].sOpenTag=d->pClear[n].sOpenTag;
            p->pClear[n].sValue=IXMLStringDup(d->pClear[n].sValue);
            p->pClear[n].szValue=d->pClear[n].szValue;
        }
    }
    n=d->nChild;
    if (n)
    {
        p->nChild=n; p->pChild=(ITCXMLNode*)malloc(n*sizeof(IXMLNode));
        while (n--)
        {
            ((IXMLNode*)p->pChild)[n].d=NULL;
            ((IXMLNode*)p->pChild)[n]=d->pChild[n].deepCopy();
            ((IXMLNode*)p->pChild)[n].d->pParent=p;
        }
    }
    return x;
}

int ITCXMLNode::deepCopyConstantR1() const
{
    int l=sizeof(IXMLNodeData)+(d->nChild+d->nText+d->nClear)*sizeof(int),
        n=d->nAttribute;
    if (d->pName) l+=(xstrlen(d->pName)+1)*sizeof(IXMLChar);
    l+=n*sizeof(IXMLAttr);
    while (n--)
    {
        l+=(xstrlen(d->pAttribute[n].sName)+1)*sizeof(IXMLChar);
        l+=(d->pAttribute[n].szValue+1)*sizeof(IXMLChar);
    }
    n=d->nText;
    l+=n*sizeof(IXMLCStr);
    while(n--) l+=(xstrlen(d->pText[n])+1)*sizeof(IXMLChar);
    n=d->nClear;
    l+=n*sizeof(IXMLClear);
    while (n--) l+=(d->pClear[n].szValue+1)*sizeof(IXMLChar);
    n=d->nChild;
    l+=n*sizeof(IXMLNode);
    while (n--) l+=d->pChild[n].deepCopyConstantR1();
    return l;
}

ICXMLNode ITCXMLNode::deepCopyConstantR2(int *nref, unsigned char **_mem) const
{
    ICXMLNode x;
    unsigned char *mem=*_mem;
    IXMLNodeData *p=x.d=(IXMLNodeData*)mem; mem+=sizeof(IXMLNodeData);
    int l;
    IXMLCStr t;
    p->isProcessInstruction=d->isProcessInstruction;
    p->memBuffer=nref;
    t=d->pName; l=(xstrlen(t)+1)*sizeof(IXMLChar);
    memcpy(mem,t,l); p->pName=(IXMLCStr)mem; mem+=l;

    int n;
    p->nAttribute=n=d->nAttribute;
    if (n)
    {
        p->pAttribute=(IXMLAttr*)mem; mem+=n*sizeof(IXMLAttr);
        while (n--)
        {
            t=d->pAttribute[n].sName; l=(xstrlen(t)+1)*sizeof(IXMLChar);
            memcpy(mem,t,l); p->pAttribute[n].sName=(IXMLCStr)mem; mem+=l;
            l=p->pAttribute[n].szValue=d->pAttribute[n].szValue;
            t=d->pAttribute[n].sValue; l=(l+1)*sizeof(IXMLChar);
            memcpy(mem,t,l); p->pAttribute[n].sValue=(IXMLCStr)mem; mem+=l;
        }
    }
    if (d->pOrder)
    {
        n=(d->nChild+d->nText+d->nClear)*sizeof(int); p->pOrder=(int*)mem; memcpy(mem,d->pOrder,n); mem+=n;
    }
    p->nText=n=d->nText;
    if (n)
    {
        p->pText=(IXMLCStr*)mem; mem+=n*sizeof(IXMLCStr);
        while(n--) 
        {
            t=d->pText[n]; l=(xstrlen(t)+1)*sizeof(IXMLChar);
            memcpy(mem,t,l); p->pText[n]=(IXMLCStr)mem; mem+=l;
        }
    }
    p->nClear=n=d->nClear;
    if (n)
    {
        p->pClear=(IXMLClear*)mem; mem+=n*sizeof(IXMLClear);
        while (n--)
        {
            p->pClear[n].sCloseTag=d->pClear[n].sCloseTag;
            p->pClear[n].sOpenTag=d->pClear[n].sOpenTag;
            l=p->pClear[n].szValue=d->pClear[n].szValue;
            t=d->pClear[n].sValue; l=(l+1)*sizeof(IXMLChar);
            memcpy(mem,t,l); p->pClear[n].sValue=(IXMLCStr)mem; mem+=l;
        }
    }
    p->nChild=n=d->nChild;
    if (n)
    {
        p->pChild=(ITCXMLNode*)mem; mem+=n*sizeof(ITCXMLNode);
        *_mem=mem;
        while (n--)
        {
            ((ICXMLNode*)p->pChild)[n].d=NULL;
            ((ICXMLNode*)p->pChild)[n]=d->pChild[n].deepCopyConstantR2(nref,_mem); // *nref++
            ((ICXMLNode*)p->pChild)[n].d->pParent=p;
        }
    } else *_mem=mem;
    return x;
}

ICXMLNode ITCXMLNode::deepCopyConstant() const
{
    if (!d) return ICXMLNode::emptyICXMLNode;
    int l=deepCopyConstantR1();
    int *i=(int*)malloc(l+sizeof(int));*i=1;
    unsigned char *p=(unsigned char*)(i+1);
    return deepCopyConstantR2(i,&p);
}

/************************************************************************/
/*                update functions from the IXMLNode                    */
/************************************************************************/

IXMLCStr IXMLNode::updateName_WOSD(IXMLStr pName)
{
    if (!d) { free(pName); return NULL; }
    if (d->pName&&(pName!=d->pName)) free((void*)d->pName);
    d->pName=pName;
    return pName;
}

// private:
void ITCXMLNode::IXMLNodeData::init(struct IXMLNodeData *_pParent, char isDeclaration)
{
    pName=NULL;
    pParent=_pParent;
    nChild=0;    nText=0;    nClear=0;    nAttribute=0;
    pChild=NULL; pText=NULL; pClear=NULL; pAttribute=NULL;
    pOrder=NULL;
    isProcessInstruction=isDeclaration;
    memBuffer=NULL;
}

IXMLNode IXMLNode::createXMLTopNode_WOSD(IXMLStr pName, char isDeclaration) { return IXMLNode(NULL,pName,isDeclaration); }
IXMLNode IXMLNode::createXMLTopNode(IXMLCStr pName, char isDeclaration) { return IXMLNode(NULL,IXMLStringDup(pName),isDeclaration); }

// private:
IXMLElementPosition IXMLNode::findPosition(IXMLNodeData *d, int index, IXMLElementType xxtype)
{
    if (index<0) return -1;
    int i=0,j=(int)((index<<2)+xxtype),*o=d->pOrder; while (o[i]!=j) i++; return i;
}

// private:
// update "order" information when deleting a content of a IXMLNode
int IXMLNode::removeOrderElement(IXMLNodeData *d, IXMLElementType t, int index)
{
    int n=d->nChild+d->nText+d->nClear, *o=d->pOrder,i=findPosition(d,index,t);
    memmove(o+i, o+i+1, (n-i)*sizeof(int));
    for (;i<n;i++)
        if ((o[i]&3)==(int)t) o[i]-=4;
    // We should normally do:
    // d->pOrder=(int)realloc(d->pOrder,n*sizeof(int));
    // but we skip reallocation because it's too time consuming.
    // Anyway, at the end, it will be free'd completely at once.
    return i;
}

void *IXMLNode::addToOrder(int *_pos, int nc, void *p, int size, IXMLElementType xtype)
{
    //  in: *_pos is the position inside d->pOrder ("-1" means "EndOf")
    // out: *_pos is the index inside p
    p=myRealloc(p,(nc+1),size);
    int n=d->nChild+d->nText+d->nClear;
    d->pOrder=(int*)myRealloc(d->pOrder,n+1,sizeof(int));
    int pos=*_pos,*o=d->pOrder;

    if ((pos<0)||(pos>=n)) { *_pos=nc; o[n]=(int)((nc<<2)+xtype); return p; }

    int i=pos;
    memmove(o+i+1, o+i, (n-i)*sizeof(int));

    while ((pos<n)&&((o[pos]&3)!=(int)xtype)) pos++;
    if (pos==n) { *_pos=nc; o[n]=(int)((nc<<2)+xtype); return p; }

    o[i]=o[pos];
    for (i=pos+1;i<=n;i++) if ((o[i]&3)==(int)xtype) o[i]+=4;

    *_pos=pos=o[pos]>>2;
    memmove(((char*)p)+(pos+1)*size,((char*)p)+pos*size,(nc-pos)*size);

    return p;
}

// Add a child node to the given element.
IXMLNode IXMLNode::addChild_priv(IXMLStr pName, char isDeclaration, int pos)
{
    if (!pName) return emptyIXMLNode;
    d->pChild=(ITCXMLNode*)addToOrder(&pos,d->nChild,d->pChild,sizeof(IXMLNode),eNodeChild);
    IXMLNode *p=(IXMLNode*)d->pChild+pos;
    p->d=NULL;
    *p=IXMLNode(d,pName,isDeclaration);
    d->nChild++;
    return *p;
}

// Add an attribute to an element.
IXMLAttr *IXMLNode::addAttribute_priv(IXMLStr pName, IXMLStr pValuev)
{
    if (!pName) return &emptyXMLAttribute;
    if (!d) { myFree(pName); myFree(pValuev); return &emptyXMLAttribute; }
    int nc=d->nAttribute;
    d->pAttribute=(IXMLAttr*)myRealloc(d->pAttribute,(nc+1),sizeof(IXMLAttr));
    IXMLAttr *pAttr=d->pAttribute+nc;
    pAttr->sName= pName;
    pAttr->sValue = pValuev;
    d->nAttribute++;
    return pAttr;
}

// Add text to the element.
IXMLCStr IXMLNode::addText_priv(IXMLStr pValue, int pos)
{
    if (!pValue) return NULL;
    if (!d) { myFree(pValue); return NULL; }
    d->pText=(IXMLCStr*)addToOrder(&pos,d->nText,d->pText,sizeof(IXMLStr),eNodeText);
    d->pText[pos]=pValue;
    d->nText++;
    return pValue;
}

// Add clear (unformatted) text to the element.
IXMLClear *IXMLNode::addClear_priv(IXMLStr pValue, IXMLCStr lpcOpen, IXMLCStr lpcClose, int pos)
{
    if (!pValue) return &emptyIXMLClear;
    if (!d) { myFree(pValue); return &emptyIXMLClear; }
    d->pClear=(IXMLClear *)addToOrder(&pos,d->nClear,d->pClear,sizeof(IXMLClear),eNodeClear);
    IXMLClear *pNewClear=d->pClear+pos;
    pNewClear->sValue = pValue;
    if (!lpcOpen) lpcOpen=IXMLClearTags->lpcOpen;
    if (!lpcClose) lpcClose=IXMLClearTags->lpcClose;
    pNewClear->sOpenTag= lpcOpen;
    pNewClear->sCloseTag = lpcClose;
    d->nClear++;
    return pNewClear;
}

int IXMLNode::detachFromParent(IXMLNodeData *d)
{
    IXMLNode *pa=(IXMLNode*)d->pParent->pChild;
    int i=0;
    while (((void*)(pa[i].d))!=((void*)d)) i++;
    d->pParent->nChild--;
    if (d->pParent->nChild) memmove(pa+i,pa+i+1,(d->pParent->nChild-i)*sizeof(IXMLNode));
    else { free(pa); d->pParent->pChild=NULL; }
    return removeOrderElement(d->pParent,eNodeChild,i);
}

//ICXMLNode::~ICXMLNode(){}
void IXMLNode::deleteNodeContent()
{
    if (!d) return;
    if (d->pParent) { detachFromParent(d); d->pParent=NULL; d->ref_count--; }
    emptyTheNode(1);
}

void IXMLNode::emptyTheNode(char force)
{
    IXMLNodeData *dd=d; // warning: must stay this way!
    if ((dd->ref_count==0)||force)
    {
        if (d->pParent) detachFromParent(d);
        int i;
        IXMLNode *pc;
        for(i=0; i<dd->nChild; i++)
        {
            pc=(IXMLNode*)dd->pChild+i;
            pc->d->pParent=NULL;
            pc->d->ref_count--;
            pc->emptyTheNode(force);
        }
        myFree(dd->pChild);
        for(i=0; i<dd->nText; i++) free((void*)dd->pText[i]);
        myFree(dd->pText);
        for(i=0; i<dd->nClear; i++) free((void*)dd->pClear[i].sValue);
        myFree(dd->pClear);
        for(i=0; i<dd->nAttribute; i++)
        {
            free((void*)dd->pAttribute[i].sName);
            if (dd->pAttribute[i].sValue) free((void*)dd->pAttribute[i].sValue);
        }
        myFree(dd->pAttribute);
        myFree(dd->pOrder);
        myFree((void*)dd->pName);
        dd->init(NULL,false);
    }
    if (dd->ref_count==0)
    {
        free(dd);
        d=NULL;
    }
}

IXMLNode IXMLNode::addChild(IXMLNode childNode, int pos)
{
    IXMLNodeData *dc=childNode.d;
    if ((!dc)||(!d)) return childNode;
    if (!dc->pName)
    {
        // this is a root node: todo: correct fix
        int j=pos;
        while (dc->nChild)
        {
            addChild(((IXMLNode*)dc->pChild)[0],j);
            if (pos>=0) j++;
        }
        return childNode;
    }
    if (dc->pParent) { if ((detachFromParent(dc)<=pos)&&(dc->pParent==d)) pos--; } else dc->ref_count++;
    dc->pParent=d;
//     int nc=d->nChild;
//     d->pChild=(IXMLNode*)myRealloc(d->pChild,(nc+1),memoryIncrease,sizeof(IXMLNode));
    d->pChild=(ITCXMLNode*)addToOrder(&pos,d->nChild,d->pChild,sizeof(IXMLNode),eNodeChild);
    d->pChild[pos].d=dc;
    d->nChild++;
    return childNode;
}

void IXMLNode::deleteAttribute(int i)
{
    if ((!d)||(i<0)||(i>=d->nAttribute)) return;
    d->nAttribute--;
    IXMLAttr *p=d->pAttribute+i;
    free((void*)p->sName);
    if (p->sValue) free((void*)p->sValue);
    if (d->nAttribute) memmove(p,p+1,(d->nAttribute-i)*sizeof(IXMLAttr)); else { free(p); d->pAttribute=NULL; }
}

void IXMLNode::deleteAttribute(IXMLAttr *a){ if (a) deleteAttribute(a->sName); }
void IXMLNode::deleteAttribute(IXMLCStr pName)
{
    int j=0;
    getAttribute(pName,&j);
    if (j) deleteAttribute(j-1);
}

IXMLAttr *IXMLNode::updateAttribute_WOSD(IXMLStr lpcNewValue, IXMLStr lpcNewName,int i)
{
    if (!d) { if (lpcNewValue) free(lpcNewValue); if (lpcNewName) free(lpcNewName); return NULL; }
    if (i>=d->nAttribute)
    {
        if (lpcNewName) return addAttribute_WOSD(lpcNewName,lpcNewValue);
        return NULL;
    }
    IXMLAttr *p=d->pAttribute+i;
    if (p->sValue&&p->sValue!=lpcNewValue) free((void*)p->sValue);
    p->sValue=lpcNewValue;
    if (lpcNewName&&p->sName!=lpcNewName) { free((void*)p->sName); p->sName=lpcNewName; };
    return p;
}

IXMLAttr *IXMLNode::updateAttribute_WOSD(IXMLAttr *newAttribute, IXMLAttr *oldAttribute)
{
    if (oldAttribute) return updateAttribute_WOSD((IXMLStr)newAttribute->sValue,(IXMLStr)newAttribute->sName,oldAttribute->sName);
    return addAttribute_WOSD((IXMLStr)newAttribute->sName,(IXMLStr)newAttribute->sValue);
}

IXMLAttr *IXMLNode::updateAttribute_WOSD(IXMLStr lpcNewValue, IXMLStr lpcNewName,IXMLCStr lpcOldName)
{
    int j=0;
    getAttribute(lpcOldName,&j);
    if (j) return updateAttribute_WOSD(lpcNewValue,lpcNewName,j-1);
    else
    {
        if (lpcNewName) return addAttribute_WOSD(lpcNewName,lpcNewValue);
        else             return addAttribute_WOSD(IXMLStringDup(lpcOldName),lpcNewValue);
    }
}

int IXMLNode::indexText(IXMLCStr pValue) const
{
    if (!d) return -1;
    int i,l=d->nText;
    if (!pValue) { if (l) return 0; return -1; }
    IXMLCStr *p=d->pText;
    for (i=0; i<l; i++) if (pValue==p[i]) return i;
    return -1;
}

void IXMLNode::deleteText(int i)
{
    if ((!d)||(i<0)||(i>=d->nText)) return;
    d->nText--;
    IXMLCStr *p=d->pText+i;
    free((void*)*p);
    if (d->nText) memmove(p,p+1,(d->nText-i)*sizeof(IXMLCStr)); else { free(p); d->pText=NULL; }
    removeOrderElement(d,eNodeText,i);
}

void IXMLNode::deleteText(IXMLCStr pValue) { deleteText(indexText(pValue)); }

IXMLCStr IXMLNode::updateText_WOSD(IXMLStr lpcNewValue, int i)
{
    if (!d) { if (lpcNewValue) free(lpcNewValue); return NULL; }
    if (i>=d->nText) return addText_WOSD(lpcNewValue);
    IXMLCStr *p=d->pText+i;
    if (*p!=lpcNewValue) { free((void*)*p); *p=lpcNewValue; }
    return lpcNewValue;
}

IXMLCStr IXMLNode::updateText_WOSD(IXMLStr lpcNewValue, IXMLCStr lpcOldValue)
{
    if (!d) { if (lpcNewValue) free(lpcNewValue); return NULL; }
    int i=indexText(lpcOldValue);
    if (i>=0) return updateText_WOSD(lpcNewValue,i);
    return addText_WOSD(lpcNewValue);
}

void IXMLNode::deleteClear(int i)
{
    if ((!d)||(i<0)||(i>=d->nClear)) return;
    d->nClear--;
    IXMLClear *p=d->pClear+i;
    free((void*)p->sValue);
    if (d->nClear) memmove(p,p+1,(d->nClear-i)*sizeof(IXMLClear)); else { free(p); d->pClear=NULL; }
    removeOrderElement(d,eNodeClear,i);
}

int IXMLNode::indexClear(IXMLCStr pValue) const
{
    if (!d) return -1;
    int i,l=d->nClear;
    if (!pValue) { if (l) return 0; return -1; }
    IXMLClear *p=d->pClear;
    for (i=0; i<l; i++) if (pValue==p[i].sValue) return i;
    return -1;
}

void IXMLNode::deleteClear(IXMLCStr pValue) { deleteClear(indexClear(pValue)); }
void IXMLNode::deleteClear(IXMLClear *a) { if (a) deleteClear(a->sValue); }

IXMLClear *IXMLNode::updateClear_WOSD(IXMLStr lpcNewContent, int i)
{
    if (!d) { if (lpcNewContent) free(lpcNewContent); return NULL; }
    if (i>=d->nClear) return addClear_WOSD(lpcNewContent);
    IXMLClear *p=d->pClear+i;
    if (lpcNewContent!=p->sValue) { free((void*)p->sValue); p->sValue=lpcNewContent; }
    return p;
}

IXMLClear *IXMLNode::updateClear_WOSD(IXMLStr lpcNewContent, IXMLCStr lpcOldValue)
{
    if (!d) { if (lpcNewContent) free(lpcNewContent); return NULL; }
    int i=indexClear(lpcOldValue);
    if (i>=0) return updateClear_WOSD(lpcNewContent,i);
    return addClear_WOSD(lpcNewContent);
}

IXMLClear *IXMLNode::updateClear_WOSD(IXMLClear *newP,IXMLClear *oldP)
{
    if (oldP) return updateClear_WOSD((IXMLStr)newP->sValue,(IXMLStr)oldP->sValue);
    return NULL;
}

IXMLElementPosition IXMLNode::positionOfText     (int i) const { if (i>=d->nText ) i=d->nText-1;  return findPosition(d,i,eNodeText ); }
IXMLElementPosition IXMLNode::positionOfClear    (int i) const { if (i>=d->nClear) i=d->nClear-1; return findPosition(d,i,eNodeClear); }
IXMLElementPosition IXMLNode::positionOfChildNode(int i) const { if (i>=d->nChild) i=d->nChild-1; return findPosition(d,i,eNodeChild); }
IXMLElementPosition IXMLNode::positionOfText (IXMLCStr pValue) const { return positionOfText (indexText (pValue)); }
IXMLElementPosition IXMLNode::positionOfClear(IXMLCStr pValue) const { return positionOfClear(indexClear(pValue)); }
IXMLElementPosition IXMLNode::positionOfClear(IXMLClear *a) const { if (a) return positionOfClear(a->sValue); return positionOfClear(); }
IXMLElementPosition IXMLNode::positionOfChildNode(IXMLNode x)  const
{
    if ((!d)||(!x.d)) return -1;
    IXMLNodeData *dd=x.d;
    ITCXMLNode *pc=(ITCXMLNode*)d->pChild;
    int i=d->nChild;
    while (i--) if (pc[i].d==dd) return findPosition(d,i,eNodeChild);
    return -1;
}
IXMLElementPosition IXMLNode::positionOfChildNode(IXMLCStr name, int count) const
{
    if (!name) return positionOfChildNode(count);
    int j=0;
    do { getChildNode(name,&j); if (j<0) return -1; } while (count--);
    return findPosition(d,j-1,eNodeChild);
}

IXMLNode       IXMLNode::addChild(IXMLCStr pName, char isDeclaration, IXMLElementPosition pos)
                { return addChild_priv(IXMLStringDup(pName),isDeclaration,pos); }
IXMLNode       IXMLNode::addChild_WOSD(IXMLStr pName, char isDeclaration, IXMLElementPosition pos)
                { return addChild_priv(pName,isDeclaration,pos); }
IXMLAttr *IXMLNode::addAttribute(IXMLCStr pName, IXMLCStr pValue)
                { return addAttribute_priv(IXMLStringDup(pName),IXMLStringDup(pValue)); }
IXMLAttr *IXMLNode::addAttribute_WOSD(IXMLStr pName, IXMLStr pValuev)
                { return addAttribute_priv(pName,pValuev); }
IXMLCStr       IXMLNode::addText(IXMLCStr pValue, IXMLElementPosition pos)
                { return addText_priv(IXMLStringDup(pValue),pos); }
IXMLCStr       IXMLNode::addText_WOSD(IXMLStr pValue, IXMLElementPosition pos)
                { return addText_priv(pValue,pos); }
IXMLClear      *IXMLNode::addClear(IXMLCStr pValue, IXMLCStr lpcOpen, IXMLCStr lpcClose, IXMLElementPosition pos)
                { return addClear_priv(IXMLStringDup(pValue),lpcOpen,lpcClose,pos); }
IXMLClear      *IXMLNode::addClear_WOSD(IXMLStr pValue, IXMLCStr lpcOpen, IXMLCStr lpcClose, IXMLElementPosition pos)
                { return addClear_priv(pValue,lpcOpen,lpcClose,pos); }
IXMLCStr       IXMLNode::updateName(IXMLCStr pName)
                { return updateName_WOSD(IXMLStringDup(pName)); }
IXMLAttr *IXMLNode::updateAttribute(IXMLAttr *newAttribute, IXMLAttr *oldAttribute)
                { return updateAttribute_WOSD(IXMLStringDup(newAttribute->sValue),IXMLStringDup(newAttribute->sName),oldAttribute->sName); }
IXMLAttr *IXMLNode::updateAttribute(IXMLCStr lpcNewValue, IXMLCStr lpcNewName,int i)
                { return updateAttribute_WOSD(IXMLStringDup(lpcNewValue),IXMLStringDup(lpcNewName),i); }
IXMLAttr *IXMLNode::updateAttribute(IXMLCStr lpcNewValue, IXMLCStr lpcNewName,IXMLCStr lpcOldName)
                { return updateAttribute_WOSD(IXMLStringDup(lpcNewValue),IXMLStringDup(lpcNewName),lpcOldName); }
IXMLCStr       IXMLNode::updateText(IXMLCStr lpcNewValue, int i)
                { return updateText_WOSD(IXMLStringDup(lpcNewValue),i); }
IXMLCStr       IXMLNode::updateText(IXMLCStr lpcNewValue, IXMLCStr lpcOldValue)
                { return updateText_WOSD(IXMLStringDup(lpcNewValue),lpcOldValue); }
IXMLClear      *IXMLNode::updateClear(IXMLCStr lpcNewContent, int i)
                { return updateClear_WOSD(IXMLStringDup(lpcNewContent),i); }
IXMLClear      *IXMLNode::updateClear(IXMLCStr lpcNewValue, IXMLCStr lpcOldValue)
                { return updateClear_WOSD(IXMLStringDup(lpcNewValue),lpcOldValue); }
IXMLClear     *IXMLNode::updateClear(IXMLClear *newP,IXMLClear *oldP)
                { return updateClear_WOSD(IXMLStringDup(newP->sValue),oldP->sValue); }

/************************************************************************/
/*      Navigation functions from  ITCXMLNode, ICXMLNode, IXMLNode      */
/************************************************************************/

int ITCXMLNode::nChildNode(IXMLCStr name) const
{
    if (!d) return 0;
    int i,j=0,n=d->nChild;
    ITCXMLNode *pc=d->pChild;
    for (i=0; i<n; i++)
    {
        if (xstricmp(pc->d->pName, name)==0) j++;
        pc++;
    }
    return j;
}

ITCXMLNode ITCXMLNode::getChildNode(IXMLCStr name, int *j) const
{
    if (!d) return emptyITCXMLNode;
    int i=0,n=d->nChild;
    if (j) i=*j;
    ITCXMLNode *pc=d->pChild+i;
    for (; i<n; i++)
    {
        if (!xstricmp(pc->d->pName, name))
        {
            if (j) *j=i+1;
            return *pc;
        }
        pc++;
    }
    return emptyITCXMLNode;
}

ITCXMLNode ITCXMLNode::getChildNode(IXMLCStr name, int j) const
{
    if (!d) return emptyITCXMLNode;
    if (j>=0)
    {
        int i=0;
        while (j-->0) getChildNode(name,&i);
        return getChildNode(name,&i);
    }
    int i=d->nChild;
    while (i--) if (!xstricmp(name,d->pChild[i].d->pName)) break;
    if (i<0) return emptyITCXMLNode;
    return getChildNode(i);
}

ITCXMLNode ITCXMLNode::getChildNodeByPath(IXMLCStr _path,bool indicesAreZeroBased,IXMLChar sep) const
{
    if (!_path) return emptyITCXMLNode;
    IXMLStr path=IXMLStringDup(_path);
    ITCXMLNode x=getChildNodeByPathNonConst(path,indicesAreZeroBased,sep);
    free(path);
    return x;
}

ITCXMLNode ITCXMLNode::getChildNodeByPathNonConst(IXMLStr path,bool indicesAreZeroBased,IXMLChar sep) const
{
    if (!path) return emptyITCXMLNode;
    if (!(*path)) return (*this);

#define GET_NEXT_CHILD_NODE_XN                                                              \
        if (!counterPos) xn=xbase.getChildNode(path);                                       \
        else                                                                                \
        {                                                                                   \
            int k;                                                                          \
            if (indicesAreZeroBased) k=internal_xmltoi(counterPos+1,0);                     \
            else k=internal_xmltoi(counterPos+1,1)-1;                                       \
            if (path==counterPos) xn=xbase.getChildNode(k);                                 \
            else                                                                            \
            {                                                                               \
                *counterPos=0;                                                              \
                xn=xbase.getChildNode(path,k);                                              \
                *counterPos=_CXML('[');                                                     \
            }                                                                               \
            counterPos=NULL;                                                                \
        }


#define SEARCH_BEFORE_END_NODE(RR)                                               \
    ITCXMLNode xn,xbase=*this;                                                   \
    if (*path==sep) path++;                                                      \
    IXMLStr tend1=path, counterPos=NULL;                                         \
    while(*tend1)                                                                \
    {                                                                            \
        if (*tend1==_CXML('[')) counterPos=tend1;                                \
        if (*tend1==sep) break; tend1++;                                         \
    }                                                                            \
    while(*tend1)                                                                \
    {                                                                            \
        *tend1=0;                                                                \
        GET_NEXT_CHILD_NODE_XN                                                   \
        *tend1=sep;                                                              \
        if (xn.isEmpty()) return RR;                                             \
        xbase=xn;                                                                \
        tend1++;                                                                 \
        path=tend1;                                                              \
        while(*tend1)                                                            \
        {                                                                        \
            if (*tend1==_CXML('[')) counterPos=tend1;                            \
            if (*tend1==sep) break; tend1++;                                     \
        }                                                                        \
    }                                                                            \
    GET_NEXT_CHILD_NODE_XN

    SEARCH_BEFORE_END_NODE(emptyITCXMLNode);
    return xn;
}

IXMLCStr ITCXMLNode::getElementByPath(IXMLCStr _path,bool indicesAreZeroBased,IXMLChar sep) const
{
    if (!_path) return NULL;
    IXMLStr path=IXMLStringDup(_path);
    IXMLCStr s=getElementByPathNonConst(path,indicesAreZeroBased,sep);
    free(path);
    return s;
}
IXMLCStr ITCXMLNode::getElementByPathNonConst(IXMLStr path,bool indicesAreZeroBased,IXMLChar sep) const
{
    if (!path) return NULL;
    if (!(*path)) return getText();
    SEARCH_BEFORE_END_NODE(NULL);
    IXMLCStr t=xn.getText();
    if (t) return t;
    t=xbase.getAttribute(path);
    if (t) return t;
    return xn.getClear().sValue;
}

ITCXMLNode ITCXMLNode::getChildNodeWithAttribute(IXMLCStr name,IXMLCStr attributeName,IXMLCStr attributeValue, int *k) const
{
    int i=0,j;
    if (k) i=*k;
    ITCXMLNode x;
    IXMLCStr t;
    do
    {
        x=getChildNode(name,&i);
        if (!x.isEmpty())
        {
            if (attributeValue)
            {
                j=0;
                do
                {
                    t=x.getAttribute(attributeName,&j);
                    if (t&&(xstricmp(attributeValue,t)==0)) { if (k) *k=i; return x; }
                } while (t);
            } else
            {
                if (x.isAttributeSet(attributeName)) { if (k) *k=i; return x; }
            }
        }
    } while (!x.isEmpty());
    return emptyITCXMLNode;
}

#define RETURN_ICXML_NODE(dd) ICXMLNode r; r.d=dd; if (r.d) (*r.d->memBuffer)++; return r;
#define RETURN_IXML_NODE(dd)  IXMLNode  r; r.d=dd; if (r.d) r.d->ref_count++;    return r;

ITCXMLNode    ITCXMLNode::getParentNode() const { if ((!d)||(!d->pParent)) return emptyITCXMLNode; return ITCXMLNode(d->pParent); }
ICXMLNode     ICXMLNode::getParentNode () const { if ((!d)||(!d->pParent)) return emptyICXMLNode;  RETURN_ICXML_NODE(d->pParent); }
IXMLNode      IXMLNode::getParentNode  () const { if ((!d)||(!d->pParent)) return emptyIXMLNode;   RETURN_IXML_NODE(d->pParent); }

      ICXMLNode ICXMLNode::getChildNode(IXMLCStr name, int *j) const
{ ITCXMLNode t=ITCXMLNode::getChildNode(name,j); RETURN_ICXML_NODE(t.d);}
       IXMLNode  IXMLNode::getChildNode(IXMLCStr name, int *j)  const
{ ITCXMLNode t=ITCXMLNode::getChildNode(name,j); RETURN_IXML_NODE(t.d);}
      ICXMLNode ICXMLNode::getChildNode(IXMLCStr name, int j) const
{ ITCXMLNode t=ITCXMLNode::getChildNode(name,j); RETURN_ICXML_NODE(t.d);}
       IXMLNode  IXMLNode::getChildNode(IXMLCStr name, int j)  const
{ ITCXMLNode t=ITCXMLNode::getChildNode(name,j); RETURN_IXML_NODE(t.d);}
      ICXMLNode ICXMLNode::getChildNodeByPath(IXMLCStr p,bool indicesAreZeroBased,IXMLChar sep) const
{ ITCXMLNode t=ITCXMLNode::getChildNodeByPath(p,indicesAreZeroBased,sep); RETURN_ICXML_NODE(t.d);}
        IXMLNode IXMLNode::getChildNodeByPath(IXMLCStr p,bool indicesAreZeroBased,IXMLChar sep) const
{ ITCXMLNode t=ITCXMLNode::getChildNodeByPath(p,indicesAreZeroBased,sep); RETURN_IXML_NODE(t.d);}
      ICXMLNode ICXMLNode::getChildNodeByPathNonConst(IXMLStr p,bool indicesAreZeroBased,IXMLChar sep) const
{ ITCXMLNode t=ITCXMLNode::getChildNodeByPathNonConst(p,indicesAreZeroBased,sep); RETURN_ICXML_NODE(t.d);}
        IXMLNode IXMLNode::getChildNodeByPathNonConst(IXMLStr p,bool indicesAreZeroBased,IXMLChar sep) const
{ ITCXMLNode t=ITCXMLNode::getChildNodeByPathNonConst(p,indicesAreZeroBased,sep); RETURN_IXML_NODE(t.d);}
      ICXMLNode ICXMLNode::getChildNodeWithAttribute(IXMLCStr n,IXMLCStr an,IXMLCStr av, int *k) const
{ ITCXMLNode t=ITCXMLNode::getChildNodeWithAttribute(n,an,av,k); RETURN_ICXML_NODE(t.d);}
        IXMLNode IXMLNode::getChildNodeWithAttribute(IXMLCStr n,IXMLCStr an,IXMLCStr av, int *k) const
{ ITCXMLNode t=ITCXMLNode::getChildNodeWithAttribute(n,an,av,k); RETURN_IXML_NODE(t.d);}

#undef RETURN_ICXML_NODE
#undef RETURN_IXML_NODE

// Find an attribute on an node.
IXMLCStr ITCXMLNode::getAttribute(IXMLCStr lpcAttrib, int *j) const
{
    if (!d) return NULL;
    int i=0,n=d->nAttribute;
    if (j) i=*j;
    IXMLAttr *pAttr=d->pAttribute+i;
    for (; i<n; i++)
    {
        if (xstricmp(pAttr->sName, lpcAttrib)==0)
        {
            if (j) *j=i+1;
            return pAttr->sValue;
        }
        pAttr++;
    }
    return NULL;
}

char ITCXMLNode::isAttributeSet(IXMLCStr lpcAttrib) const
{
    if (!d) return FALSE;
    int i,n=d->nAttribute;
    IXMLAttr *pAttr=d->pAttribute;
    for (i=0; i<n; i++)
    {
        if (xstricmp(pAttr->sName, lpcAttrib)==0)
        {
            return TRUE;
        }
        pAttr++;
    }
    return FALSE;
}

IXMLCStr ITCXMLNode::getAttribute(IXMLCStr name, int j) const
{
    if (!d) return NULL;
    int i=0;
    while (j-->0) getAttribute(name,&i);
    return getAttribute(name,&i);
}

IXMLCStr ITCXMLNode::getName() const { if (!d) return NULL; return d->pName;   }
int ITCXMLNode::nText()       const { if (!d) return 0;    return d->nText;      }
int ITCXMLNode::nChildNode()  const { if (!d) return 0;    return d->nChild;     }
int ITCXMLNode::nAttribute()  const { if (!d) return 0;    return d->nAttribute; }
int ITCXMLNode::nClear()      const { if (!d) return 0;    return d->nClear;     }
int ITCXMLNode::nElement()    const { if (!d) return 0;    return d->nAttribute+d->nChild+d->nText+d->nClear; }
IXMLClear      ITCXMLNode::getClear         (int i) const { if ((!d)||(i>=d->nClear    )) return emptyIXMLClear;     return d->pClear[i];     }
IXMLAttr ITCXMLNode::getAttribute     (int i) const { if ((!d)||(i>=d->nAttribute)) return emptyXMLAttribute; return d->pAttribute[i]; }
IXMLCStr      ITCXMLNode::getAttributeName (int i) const { if ((!d)||(i>=d->nAttribute)) return NULL;              return d->pAttribute[i].sName;  }
IXMLCStr      ITCXMLNode::getAttributeValue(int i) const { if ((!d)||(i>=d->nAttribute)) return NULL;              return d->pAttribute[i].sValue; }
IXMLCStr      ITCXMLNode::getText          (int i) const { if ((!d)||(i>=d->nText     )) return NULL;              return d->pText[i];      }
ITCXMLNode    ITCXMLNode::getChildNode     (int i) const { if ((!d)||(i>=d->nChild    )) return emptyITCXMLNode;   return d->pChild[i];     }
ICXMLNode     ICXMLNode::getChildNode      (int i) const { if ((!d)||(i>=d->nChild    )) return emptyICXMLNode;    return ((ICXMLNode*)d->pChild)[i]; }
IXMLNode      IXMLNode::getChildNode       (int i) const { if ((!d)||(i>=d->nChild    )) return emptyIXMLNode;     return ((IXMLNode*)d->pChild)[i]; }
char          ITCXMLNode::isProcessInstruction(  ) const { if (!d) return 0;             return d->isProcessInstruction; }
char          ITCXMLNode::isEmpty          (     ) const { return (d==NULL); }
ITCXMLNode     ITCXMLNode::emptyNode        (     )       { return ITCXMLNode::emptyITCXMLNode; }
ICXMLNode      ICXMLNode::emptyNode        (     )       { return ICXMLNode::emptyICXMLNode; }
IXMLNode       IXMLNode::emptyNode        (     )       { return IXMLNode::emptyIXMLNode; }

//////////////////////////////////////////////////////////
//                                                      //
//               base64 conversion functions.           //
//                                                      //
//////////////////////////////////////////////////////////

static const char base64Fillchar = _CXML('='); // used to mark partial words at the end

// this lookup table defines the base64 encoding
IXMLCStr base64EncodeTable=_CXML("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

// Decode Table gives the index of any valid base64 character in the Base64 table]
// 96: '='  -   97: space char   -   98: illegal char   -   99: end of string
const unsigned char base64DecodeTable[] = {
    99,98,98,98,98,98,98,98,98,97,  97,98,98,97,98,98,98,98,98,98,  98,98,98,98,98,98,98,98,98,98,  //00 -29
    98,98,97,98,98,98,98,98,98,98,  98,98,98,62,98,98,98,63,52,53,  54,55,56,57,58,59,60,61,98,98,  //30 -59
    98,96,98,98,98, 0, 1, 2, 3, 4,   5, 6, 7, 8, 9,10,11,12,13,14,  15,16,17,18,19,20,21,22,23,24,  //60 -89
    25,98,98,98,98,98,98,26,27,28,  29,30,31,32,33,34,35,36,37,38,  39,40,41,42,43,44,45,46,47,48,  //90 -119
    49,50,51,98,98,98,98,98,98,98,  98,98,98,98,98,98,98,98,98,98,  98,98,98,98,98,98,98,98,98,98,  //120 -149
    98,98,98,98,98,98,98,98,98,98,  98,98,98,98,98,98,98,98,98,98,  98,98,98,98,98,98,98,98,98,98,  //150 -179
    98,98,98,98,98,98,98,98,98,98,  98,98,98,98,98,98,98,98,98,98,  98,98,98,98,98,98,98,98,98,98,  //180 -209
    98,98,98,98,98,98,98,98,98,98,  98,98,98,98,98,98,98,98,98,98,  98,98,98,98,98,98,98,98,98,98,  //210 -239
    98,98,98,98,98,98,98,98,98,98,  98,98,98,98,98,98                                               //240 -255
};

IXMLParserBase64Tool::~IXMLParserBase64Tool(){ clear(); }

void IXMLParserBase64Tool::clear(){ if (buf) free(buf); buf=NULL; buflen=0; }

int IXMLParserBase64Tool::encodeLength(int inlen, char formatted)
{
    unsigned int i=((inlen-1)/3*4+4+1);
    if (formatted) i+=inlen/54;
    return i;
}

IXMLStr IXMLParserBase64Tool::encode(unsigned char *inbuf, unsigned int inlen, char formatted)
{
    int i=encodeLength(inlen,formatted),k=17,eLen=inlen/3,j;
    alloc(i*sizeof(IXMLChar));
    IXMLStr curr=(IXMLStr)buf;
    for(i=0;i<eLen;i++)
    {
        // Copy next three bytes into lower 24 bits of int, paying attention to sign.
        j=(inbuf[0]<<16)|(inbuf[1]<<8)|inbuf[2]; inbuf+=3;
        // Encode the int into four chars
        *(curr++)=base64EncodeTable[ j>>18      ];
        *(curr++)=base64EncodeTable[(j>>12)&0x3f];
        *(curr++)=base64EncodeTable[(j>> 6)&0x3f];
        *(curr++)=base64EncodeTable[(j    )&0x3f];
        if (formatted) { if (!k) { *(curr++)=_CXML('\n'); k=18; } k--; }
    }
    eLen=inlen-eLen*3; // 0 - 2.
    if (eLen==1)
    {
        *(curr++)=base64EncodeTable[ inbuf[0]>>2      ];
        *(curr++)=base64EncodeTable[(inbuf[0]<<4)&0x3F];
        *(curr++)=base64Fillchar;
        *(curr++)=base64Fillchar;
    } else if (eLen==2)
    {
        j=(inbuf[0]<<8)|inbuf[1];
        *(curr++)=base64EncodeTable[ j>>10      ];
        *(curr++)=base64EncodeTable[(j>> 4)&0x3f];
        *(curr++)=base64EncodeTable[(j<< 2)&0x3f];
        *(curr++)=base64Fillchar;
    }
    *(curr++)=0;
    return (IXMLStr)buf;
}

unsigned int IXMLParserBase64Tool::decodeSize(IXMLCStr data,IXMLErrorInfo *xe)
{
    if (!data) return 0;
    if (xe) *xe=IXMLError_None;
    int size=0;
    unsigned char c;
    //skip any extra characters (e.g. newlines or spaces)
    while (*data)
    {
#ifdef _XMLWIDECHAR
        if (*data>255) { if (xe) *xe=IXMLError_Base64DecodeIllegalCharacter; return 0; }
#endif
        c=base64DecodeTable[(unsigned char)(*data)];
        if (c<97) size++;
        else if (c==98) { if (xe) *xe=IXMLError_Base64DecodeIllegalCharacter; return 0; }
        data++;
    }
    if (xe&&(size%4!=0)) *xe=IXMLError_Base64DataSizeIsNotMultipleOf4;
    if (size==0) return 0;
    do { data--; size--; } while(*data==base64Fillchar); size++;
    return (unsigned int)((size*3)/4);
}

unsigned char IXMLParserBase64Tool::decode(IXMLCStr data, unsigned char *buf, int len, IXMLErrorInfo *xe)
{
    if (!data) return 0;
    if (xe) *xe=IXMLError_None;
    int i=0,p=0;
    unsigned char d,c;
    for(;;)
    {

#ifdef _XMLWIDECHAR
#define BASE64DECODE_READ_NEXT_CHAR(c)                                              \
        do {                                                                        \
            if (data[i]>255){ c=98; break; }                                        \
            c=base64DecodeTable[(unsigned char)data[i++]];                       \
        }while (c==97);                                                             \
        if(c==98){ if(xe)*xe=IXMLError_Base64DecodeIllegalCharacter; return 0; }
#else
#define BASE64DECODE_READ_NEXT_CHAR(c)                                           \
        do { c=base64DecodeTable[(unsigned char)data[i++]]; }while (c==97);   \
        if(c==98){ if(xe)*xe=IXMLError_Base64DecodeIllegalCharacter; return 0; }
#endif

        BASE64DECODE_READ_NEXT_CHAR(c)
        if (c==99) { return 2; }
        if (c==96)
        {
            if (p==(int)len) return 2;
            if (xe) *xe=IXMLError_Base64DecodeTruncatedData;
            return 1;
        }

        BASE64DECODE_READ_NEXT_CHAR(d)
        if ((d==99)||(d==96)) { if (xe) *xe=IXMLError_Base64DecodeTruncatedData;  return 1; }
        if (p==(int)len) {      if (xe) *xe=IXMLError_Base64DecodeBufferTooSmall; return 0; }
        buf[p++]=(unsigned char)((c<<2)|((d>>4)&0x3));

        BASE64DECODE_READ_NEXT_CHAR(c)
        if (c==99) { if (xe) *xe=IXMLError_Base64DecodeTruncatedData;  return 1; }
        if (p==(int)len)
        {
            if (c==96) return 2;
            if (xe) *xe=IXMLError_Base64DecodeBufferTooSmall;
            return 0;
        }
        if (c==96) { if (xe) *xe=IXMLError_Base64DecodeTruncatedData;  return 1; }
        buf[p++]=(unsigned char)(((d<<4)&0xf0)|((c>>2)&0xf));

        BASE64DECODE_READ_NEXT_CHAR(d)
        if (d==99 ) { if (xe) *xe=IXMLError_Base64DecodeTruncatedData;  return 1; }
        if (p==(int)len)
        {
            if (d==96) return 2;
            if (xe) *xe=IXMLError_Base64DecodeBufferTooSmall;
            return 0;
        }
        if (d==96) { if (xe) *xe=IXMLError_Base64DecodeTruncatedData;  return 1; }
        buf[p++]=(unsigned char)(((c<<6)&0xc0)|d);
    }
}
#undef BASE64DECODE_READ_NEXT_CHAR

void IXMLParserBase64Tool::alloc(int newsize)
{
    if ((!buf)&&(newsize)) { buf=malloc(newsize); buflen=newsize; return; }
    if (newsize>buflen) { buf=realloc(buf,newsize); buflen=newsize; }
}

unsigned char *IXMLParserBase64Tool::decode(IXMLCStr data, int *outlen, IXMLErrorInfo *xe)
{
    if (xe) *xe=IXMLError_None;
    if (!data) { *outlen=0; return (unsigned char*)""; }
    unsigned int len=decodeSize(data,xe);
    if (outlen) *outlen=len;
    if (!len) return NULL;
    alloc(len+1);
    if(!decode(data,(unsigned char*)buf,len,xe)){ return NULL; }
    return (unsigned char*)buf;
}
