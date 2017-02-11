/****************************************************************************/
/*! \mainpage Incredible XML Parser library
 * \section intro_sec Introduction
 *
 * @version     V3.17
 * @author      Frank Vanden Berghen
 *
 * Copyright (c) 2013, Frank Vanden Berghen - All rights reserved.<br/>
 * See the file <a href="../../AFPL-license.txt">AFPL-license.txt</a> about the licensing terms<br/>
 *
 * The Incredible XML Parser library is an advanced non-validating XML parser written in ANSI 
 * C++ for portability.<br/> The main objectives of the Incredible XML Parser library are:
 * <ol>
 * <li>user-friendliness (i.e. it should be easy to use).</li>
 * <li> speed & scalability (it should process terabyte size XML files in a few hours).</li>
 * <li> Small foot-print & no dependencies (i.e. this must remain a small library, easy to include & compile).</li>
 * </ol>
 * 
 * The Incredible XML Parser library includes 2 parsers: It has:
 * <ol>
 * <li>A very fast XML Pull Parser (IXMLPullParser) that requires very little memory to run. The Pull
 *    Parser is very fast but it does not offer the flexibility and the user-friendliness
 *    of a full-fledged DOM parser.</li>
 * <li>A fast XML DOM parser (built on-top of the Pull Parsers)(IXMLDomParser) that provides more 
 *    comfort when manipulating XML elements. It works by using recursion and building
 *    a node tree for breaking down the elements of an XML document.</li>
 * <li>An ultra fast JSON Pull Parser (IJSONPullParser) that requires very little memory to run. The JSON Pull Parser 
 * is ultra fast and is compatible with the Incredible XML DOM Parser so that you can build in-memory 
 * a node tree that allows you to easily explore your JSON file.</li>
 * </ol>
 *
 * The Incredible XML DOM Parser, the Incredible XML Pull Parser and the Incredible JSON Pull Parser can 
 * all process terabyte-size XML/JSON files in a few hours on commodity hardware with very low memory consumption 
 * (i.e. less than a few megabyte).
 *
 * <a name="charEncoding"/>
 * The three parsers (the Pull Parser and the DOM parser) generate strings either in
 * "char*" or in "wchar_t*" mode. In "char*" mode, the Incredible XML Parser supports 
 * nearly any currently known character encodings (and it's very easy to add new 
 * ones if required). In "wchar_t*" mode, the Incredible XML Parser manipulates 
 * utf-16 strings. The Incredible XML Parser also automatically converts between
 * character encodings (e.g. it automatically converts from "utf-8" to "utf-16" 
 * when using the "wchar_t* version of the library). The Incredible XML Parser is
 * the only Small Foot-print, Non-validating XML parser that supports a very
 * wide range of different character encodings.
 * 
 * <a name="dataStream"/>
 * The three parsers (the Incredible Pull Parsers and the Incredible DOM parser) are 
 * working on a stream of data: This means that you don't need to load into memory the 
 * complete XML/JSON file (or the complete XML/JSON String): You only need to provide a 
 * function (i.e. the "read" function of a IXMLReader object) that returns 
 * different small "chunks" of the XML/JSON stream. 
 * This has several advantages: 
 * <ol>
 * <li>you are not limited anymore by your RAM memory size.</li>
 * <li>very reduced and (more or less) constant memory consumption.</li>
 * <li>you can process very easily stream-lined data (such as data coming from an 
 *    HTTP connection or the data coming from the decompression of a ZIP file).</li>
 * </ol>
 * 
 * <a name="inPlacePull"/>
 * The Incredible XML&JSON Pull parsers are 100% "in-place" parsers. This means that they do 
 * NOT copy strings: they only initializes different pointers to the memory buffer 
 * containing the XML/JSON data (There is however one inevitable memory copy when converting
 * between different character encodings: for example when the Pull Parser is forced 
 * to convert the characters from "utf-16" to "utf-8"). "In-place" parsers are a lot 
 * faster because they do not require copying the whole data into separate buffers. 
 * The Incredible XML&JSON Pull parsers are thus one of the fastest XML parser available 
 * (and they might even be the fastest).
 * 
 * You can configure the size of all the memory buffers used inside the Incredible XML&JSON 
 * Parsers. When you setup small buffer sizes, it reduces the memory consumption of the parser 
 * but it also usually slightly increases the computation time. The default buffer 
 * sizes are optimal to have a good speed on a normal-size PC. 
 * 
 * <a name="smallestMemoryPull"/> 
 * These 3 functionalities:
 * <ol>
 * <li>the possibility to work on stream-lined data (i.e. constant memory consumption).</li>
 * <li>100% "in-place" parser (no memory buffer required to store "copied" strings).</li>
 * <li>all memory buffer sizes are configurable.</li>
 * </ol>
 * ...ensures that the Incredible XML/JSON Pull Parser is the XML/JSON Parser with the SMALLEST 
 * memory consumption amongst all parsers.
 *
 * <a name="easyToUsePull"/>
 * All the strings returned by the XML/JSON Pull Parser are zero-terminated so that you can 
 * directly and very easily use them. For example, you can write: 
 * \code
 *      printf("name=%s",pullParser.getName());
 * \endcode
 * because "getName()" returns a zero-terminated char* (or wchar_t*). The Incredible XML/JSON 
 * Parser is the only "in-place" Pull parser that returns zero-terminated strings without 
 * penalty hit (i.e. without copying the whole string into a separate buffer). It's thus
 * a lot more "usable" than all other "in-place" Pull Parsers.
 * 
 * <a name="hotStart"/>
 * The XML DOM parser is able to "hot start" to create a node tree out of a sub-section 
 * of the original XML file. This means that, if you have a XML File such as this one:
 * \code
 * <AllCustomers>
 *     <OneCustomer> <name>Frank  </name> <age>38</age> </OneCustomer>
 *     <OneCustomer> <name>Sabrina</name> <age>36</age> </OneCustomer>
 *     <OneCustomer> <name>David  </name> <age>33</age> </OneCustomer>
 * </AllCustomers>
 * \endcode
 * ...you will typically call the XML DOM parser 3 times (i.e. one time for each customer).
 * When the DOM parser "hot starts", it always re-uses the same RAM memory space as the 
 * previous call so that no additional memory allocations occurs. It is thus extremely fast. 
 * Since we are building in memory a XMLNode structure that only contains ONE customer 
 * at-a-time, the memory consumption is very small (and independent of the total size of 
 * the XML file!). The "hot start" functionality is unique and very important because it 
 * allows us to use a very flexible DOM-style Parser on UNLIMITED XML file size (see example7()).
 * 
 * The Incredible XML Parser is the only DOM-Style parser that is able to work on UNLIMITED 
 * XML/JSON file size (all other DOM-Style parsers are always limited to file size smaller than 
 * a few MegaByte). The Incredible XML Parser is thus the only parser that allows you to 
 * very easily analyze very complex XML/JSON files (thanks to the easy-to-use DOM-style parser) 
 * of UNLIMITED size.
 * 
 * <a name="smallestMemoryDom"/>
 * The main bottleneck in any DOM-Style parser is always the memory allocations. If you 
 * remove this bottleneck (as inside the Incredible XML Parser), you obtain a parser that 
 * is between 10 to 100 times faster (depending of the structure of the XML/JSON). This 
 * explains why the Incredible XML DOM parser is also the fastest DOM-Style parser currently 
 * available. The Incredible XML DOM parser does not perform any memory allocations to 
 * build the different node trees (Thanks to the "Hot Start" functionality)(i.e. In the 
 * above example: There are no memory allocations to build each XMLNode structure for each 
 * of the customer). The extreme speed of the Incredible XML DOM parser allows to easily 
 * manipulate extremely large XML files (i.e. terabyte XML files are processed in a few 
 * hours on commodity hardware).
 * 
 * \subsection summary1 To summarize: 
 * <ol>
 * <li> The Incredible XML Pull Parser has one of the lowest memory consumption amongst all XML Pull parsers.</li>
 * <li> The Incredible JSON Pull Parser has one of the lowest memory consumption amongst all JSON Pull parsers.</li>
 * <li> The Incredible XML DOM  Parser has the lowest memory consumption amongst all XML DOM parsers.</li>
 * <li> The Incredible XML Pull Parser is one of the fastest XML Pull parser.</li>
 * <li> The Incredible XML JSON Parser is one of the fastest JSON Pull parser.</li>
 * <li> The Incredible XML DOM  Parser is the fastest XML DOM parser.</li>
 * <li> The Incredible XML DOM  Parser is the only DOM parser able to work on UNLIMITED file size.</li>
 * <li> The 2 Incredible XML Parsers are able to handle nearly any character encodings.</li>
 * <li> The 3 Incredible Parsers fully support "char*" mode and "wchar_t*" mode.</li>
 * <li> The 3 Incredible Parsers are able to handle stream-lined data.</li>
 * <li> The 3 Incredible Parsers are 100% thread-safe (more precisely: they are reentrant).</li>
 * <li> The Incredible XML Dom Parser provides an ultra fast XPATH support.</li>
 * <li> The Incredible XML Pull Parser is one of the easiest-to-use XML Pull parsers.</li>
 * <li> The Incredible JSON Pull Parser is one of the easiest-to-use JSON Pull parsers.</li>
 * <li> The Incredible XML Dom Parser is a good replacement for the old XMLParser.</li>
 * </ol>
 * The points 8 to 13 here above are very UNCOMMON inside small foot-print, non-validating XML Parsers.
 * 
 * \section tutorial First Tutorial
 * You can follow a simple <a href="../../xmlParser.html">Tutorial</a> to know the basics...
 *
 * By default, the Incredible XML DOM parser creates a tree of ITCXMLNode. Because of the
 * "hot start" functionality, this tree will disappear at the next call to the DOM parser 
 * (because the Incredible DOM parser always re-uses the same memory space to store the tree 
 * to avoid any memory allocation). The name ITCXMLNode is the acronym of "Incredible 
 * Transient Constant XMLNode". "Transient" means that the tree disappear at each call to 
 * the DOM parser. "Constant" means that you cannot change the tree (i.e. it's read-only)
 * (e.g. you cannot add or remove any child nodes). 
 * 
 * You can always convert a ITCXMLNode to a ICXMLNode (note that the 'T' letter in ICXMLNode
 * is missing because it's not "transient" anymore), so that the tree obtained with the DOM 
 * parser still remains in memory after a new call to the DOM parser.
 *
 * You can always convert a ITCXMLNode (or a ICXMLNode) to a IXMLNode (note that the 'T' and
 * 'C' letters in IXMLNode are missing because this object is not "transient" nor "constant" 
 * anymore). You can edit/update IXMLNode's using the classical, well-known functions 
 * (e.g. the function addChildNode(), addAttribute(), deleteNodeContent(), etc.). The IXMLNode 
 * class is 100% compatible with the old, well-known XMLNode class from the old XMLParser 
 * library.
 *
 * \subsection summary2 To summarize:
 * When using the Incredible DOM parser, you have access to 3 types of XMLNodes:
 * <ol>
 * <li> ITCXMLNode: Transient & Constant XMLNode: 
 *   <ul><li><b>upside:</b> Very fast to create because the DOM Parser does not need to 
 *                        perform any memory allocation to create ITCXMLNode's.</li>
 *       <li><b>Downside:</b> Disappear at each new call to the DOM Parser, not editable.</li> 
 *   </ul>
 * </li>
 * <li> ICXMLNode: Constant XMLNode: 
 *   <ul><li><b>upside:</b> Quite efficient because we need to perform only ONE memory 
 *                       allocation to create a complete tree of ICXMLNode.</li>
 *       <li><b>downside:</b> Not editable.</li> 
 *   </ul>
 * </li>
 * <li>  IXMLNode: Editable XMLNode:
 *   <ul><li><b>upside:</b> Editable, 100% equivalent to the XMLNode class from the old library.</li>
 *       <li><b>downside:</b> Quite Slow because we need to perform many memory 
 *                     allocations to create each IXMLNode.</li> 
 *   </ul>
 * </li>
 * </ol>                       
 * For most operations, these 3 type of XMLNodes are interchangeable (however only the 
 * IXMLNode support "editing" operations). The main difference between these 3 XMLNode classes
 * comes from the way to manage the memory allocations.
 *
 * \section usage General usage: How to include the IXMLParser library inside your project.
 *
 * The library is composed of only two files: <a href="../../IXMLParser.cpp">IXMLParser.cpp</a> 
 * and <a href="../../IXMLParser.h">IXMLParser.h</a>. These are the ONLY 2 files that you need 
 * when using the library inside your own projects.
 *
 * All the functions of the library are documented inside the comments of the file
 * <a href="../../IXMLParser.h">IXMLParser.h</a>. These comments can be transformed in
 * full-fledged HTML documentation using the DOXYGEN software: simply type: "doxygen doxy.cfg"
 *
 * By default, the IXMLParser library uses (char*) for string representation.To use the (wchar_t*)
 * version of the library, you need to define the "_UNICODE" preprocessor definition variable
 * (This is usually done inside your project definition file) (If you are using Visual Studio, 
 * then this is done automatically for you by the IDE).
 *
 * \section example Advanced Tutorial and Many Examples of usage.
 *
 * Some very small introductory examples are described inside the Tutorial file
 * <a href="../../IXMLParser.html">IXMLParser.html</a>
 *
 * Some additional small examples are also inside the file <a href="../../IXMLTest.cpp">IXMLTest.cpp</a>
 * (for the "char*" version of the library) and inside the file
 * <a href="../../IXMLTestUnicode.cpp">IXMLTestUnicode.cpp</a> (for the "wchar_t*"
 * version of the library). If you have a question, please review these additional examples
 * before sending an e-mail to the author.
 *
 * To build the examples:
 * - linux/unix: type "make"
 * - solaris: type "make -f makefile.solaris"
 * - windows: Visual Studio: double-click on IXMLParser.sln
 *
 * \section debugging Debugging with the IXMLParser library
 *
 * \subsection debugwin Debugging under WINDOWS
 *
 * The file <a href="../../IXML_Autoexp.txt">IXML_Autoexp.txt</a> contains some
 * "tweaks" that improve substantially the display of the content of the ITCXMLNode, 
 * ICXMLNode & IXMLNode objects inside the Visual Studio Debugger. Believe me, once 
 * you have seen inside the debugger the "smooth" display of the ITCXMLNode objects, 
 * you cannot live without it anymore!
 *
 * The Incredible XML Parser library is designed to minimize the quantity of memory 
 * allocations. As long as you are using ITCXMLNode objects or ICXMLNode objects, the
 * number of memory allocations remains extremely low. However, manipulating IXMLNode 
 * objects requires many allocations or re-allocations. Inside Visual C++, the "debug 
 * versions" of the memory allocation functions are very slow: Do not forget to compile
 * in "release mode" to get maximum speed.
 * 
 * When I had to debug a software that was using the IXMLNode objects, it was usually
 * a nightmare because the library was really slow in debug mode (because of the
 * slow memory allocations in Debug mode). To solve this problem, during debugging 
 * sessions of codes that include IXMLNode, I am now using a very fast DLL version of 
 * the IXMLParser Library (the DLL is compiled in release mode). Using the
 * DLL version of the IXMLParser Library allows me to have lightening XML parsing speed
 * even in debug! Other than that, the DLL version is useless: In the release version 
 * of my tool, I always use the normal, ".cpp"-based, IXMLParser Library (I simply include 
 * the <a href="../../IXMLParser.cpp">IXMLParser.cpp</a> and
 * <a href="../../IXMLParser.h">IXMLParser.h</a> files into the project).
 *
 ****************************************************************************/

#ifndef __INCLUDE_IXML_NODE__
#define __INCLUDE_IXML_NODE__

#include <stdlib.h>

#if defined(UNICODE) || defined(_UNICODE)
// If you comment the next "define" line then the library will never "switch to" _UNICODE (wchar_t*) mode (16/32 bits per characters).
// This is useful when you get error messages like:
//    'IXMLNode::openFileHelper' : cannot convert parameter 2 from 'const char [5]' to 'const wchar_t *'
// The _XMLWIDECHAR preprocessor variable force the IXMLParser library into either utf16/32-mode (the proprocessor variable
// must be defined) or utf8-mode(the pre-processor variable must be undefined).
#define _XMLWIDECHAR
#endif

#if defined(WIN32) || defined(UNDER_CE) || defined(_WIN32) || defined(WIN64) || defined(__BORLANDC__)
// comment the next line if you are under windows and the compiler is not Microsoft Visual Studio (6.0 or .NET) or Borland
#define _XMLWINDOWS
#endif

#ifdef IXMLDLLENTRY
#undef IXMLDLLENTRY
#endif
#ifdef _USE_IXMLPARSER_DLL
#ifdef _DLL_EXPORTS_
#define IXMLDLLENTRY __declspec(dllexport)
#else
#define IXMLDLLENTRY __declspec(dllimport)
#endif
#else
#define IXMLDLLENTRY
#endif

// uncomment the next line if you want no support for wchar_t* (no need for the <wchar.h> or <tchar.h> libraries anymore to compile)
//#define XML_NO_WIDE_CHAR

#ifdef XML_NO_WIDE_CHAR
#undef _XMLWINDOWS
#undef _XMLWIDECHAR
#endif

#ifdef _XMLWINDOWS
#include <tchar.h>
#else
#define IXMLDLLENTRY
#ifndef XML_NO_WIDE_CHAR
#include <wchar.h> // to have 'wcsrtombs' for ANSI version
                   // to have 'mbsrtowcs' for WIDECHAR version
#endif
#endif

/** @defgroup IXMLParserGeneral The Incredible XML Parser */

// Some common types for char set portable code
#ifdef _XMLWIDECHAR
    #define _CXML(c) L ## c
    typedef const wchar_t *IXMLCStr;
    typedef wchar_t *IXMLStr;
    typedef wchar_t IXMLChar;
#else
    #define _CXML(c) c
    typedef const char *IXMLCStr;
    typedef char *IXMLStr;
    typedef char IXMLChar;
#endif

#ifndef FALSE
#define FALSE 0
#endif /* FALSE */
#ifndef TRUE
#define TRUE 1
#endif /* TRUE */

/// Enumeration for XML parse errors.
typedef enum IXMLErrorInfo
{
    IXMLError_None = 0,
    IXMLError_MissingEndTag,
    IXMLError_NoXMLTagFound,
    IXMLError_Empty,
    IXMLError_UnmatchedEndTag,
    IXMLError_UnexpectedEndTag,
    IXMLError_UnmatchedEndClearTag,
    IXMLError_FileNotFound,
    IXMLError_FirstTagNotFound,
    IXMLError_UnknownCharacterEntity,
    IXMLError_CharacterCodeAbove255,
    IXMLError_invalidCharInTagName,
    IXMLError_invalidCharInEndTagName,
    IXMLError_invalidCharAtTagEnd,
    IXMLError_invalidCharAtPIEnd,
    IXMLError_AttributeWithNoEqualSign,
    IXMLError_QuoteOrDoubleQuoteExpected,
    IXMLError_CharConversionModeError,
    IXMLError_MaxMemoryReached,
    IXMLError_MemoryAllocationError,
    IXMLError_readError,
    IXMLError_CannotOpenWriteFile,
    IXMLError_CannotWriteFile,
    IXMLError_ColonExpected,
    IXMLError_invalidCharInNumber,
    IXMLError_trueExpected,
    IXMLError_falseExpected,
    IXMLError_nullExpected,

    IXMLError_Base64DataSizeIsNotMultipleOf4,
    IXMLError_Base64DecodeIllegalCharacter,
    IXMLError_Base64DecodeTruncatedData,
    IXMLError_Base64DecodeBufferTooSmall
} IXMLErrorInfo;


/// Structure used to obtain error details if the parse fails.
typedef struct IXMLResults {
    IXMLErrorInfo errorCode;
    long long nLine;
    int nColumn;
} IXMLResults;

/// Structure for XML clear (unformatted) node (usually comments)
typedef struct IXMLClear {
    IXMLCStr sValue,sOpenTag,sCloseTag;
    int szValue;
} IXMLClear;

/// Structure for XML attribute.
typedef struct IXMLAttr {
    IXMLCStr sName,sValue;
    int szValue;
} IXMLAttr;

class ITCXMLNode;
class ICXMLNode;

/** @defgroup Streaming Streaming Data to an Incredible XML Parser
 * @ingroup Parsing
 * @{ */

///The IXMLReader is used to "stream" data inside the Incredible XML Pull Parser
class IXMLDLLENTRY IXMLReader{
public:
    IXMLReader(){}
    virtual ~IXMLReader(){}
    virtual int read(unsigned char *buffer,int nbyte)=0; ///< The read() method is used to "stream" data inside the Pull Parser.
    /**< The read() method must fill-in the memory buffer "buffer" with new data.
     *   At most "nbyte" bytes of data must be written inside the memory buffer.
     * \return The exact number of byte written inside the memory buffer (this must be smaller than "nbyte". 
     * If the read() function returns zero, it means that the data stream is finished.
     * If the read() function returns -1, it means that the data stream is corrupted and we must abort.
     */
    virtual void setNormalBufferSize(int nbyte){}
    /**< This method is called before any call to the "read()" method.
     * It gives you an hint about the future value of the "nbyte" parameter 
     * of the "read()" method so that you can allocate internal buffers to the right size.
     */

    virtual void setSource(IXMLReader *source){} ///< do not use
    virtual IXMLReader *getSource(){return NULL;} ///< do not use
    virtual void reset(){} ///< do not use
};

/// The IXMLReaderFile is used to "stream" data coming from a file inside the Incredible XML Pull Parser
class IXMLDLLENTRY IXMLReaderFile: public IXMLReader {
public:
    IXMLReaderFile(IXMLCStr filename);
    ~IXMLReaderFile();
    int read(unsigned char *buffer,int nbyte);
    void reset();
    void *f;
};

/// The IXMLReaderStringSZ is used to "stream" data coming from an in-memory zero-terminated String inside the Incredible XML Pull Parser
class IXMLDLLENTRY IXMLReaderStringSZ: public IXMLReader {
public:
    IXMLReaderStringSZ(IXMLCStr b);
    int read(unsigned char *buffer,int nbyte);
    unsigned char *b,*bend;
};

/** @} */

/** @defgroup Parsing Parsing XML or JSON files/strings to get an ITCXMLNode, ICXMLNode or IXMLNode structure
 * @ingroup IXMLParserGeneral
 * To create new ITCXMLNode's, you can either use:
 * <ol>
 * <li>the IXMLDomParser class (For XML&JSON): Refer to example1(), example4(), example12().</li>
 * <li>the IXMLPullParser class (for XML): Refer to example7().</li>
 * <li>the IJSONPullParser class (for JSON): Refer to example12().</li>
 * </ol>
 * To get ICXMLNode from ITCXMLNode, use the deepCopyConstant() method.<br/>
 * To get IXMLNode from ITCXMLNode, use the deepCopy() method.
 * @{ */

/// Pure Virtual Class defining a basic template for a Pull Parser
/** Do not use this class: Use instead one of the Children classes: IXMLPullParser or IJSONPullParser
 */
class IXMLDLLENTRY IPullParser 
{
public:
    IPullParser(IXMLReader *_uReader,int _szByteBuffer);
    virtual ~IPullParser(){}

    void setReader(IXMLReader *_uReader);
    char setBufferSize(int _szByteBuffer);   ///< Change the memory buffer size
    /**< You can only change the buffer size when the token==IXMLToken_StartDocument
     * \return "0" if you successfully changed the buffer size.
     */
    void setHTML(bool isHTML); ///< special case: the XML file is HTML
    bool isHTML() const;       ///< is the XML file an HTML file?

    virtual void reset();  ///< re-start the pull parser (set token=IXMLToken_StartDocument)
    /**< This does not free the memory buffers and it also keeps the current char encoding settings. */

    virtual void clear(); ///< free all internal memory buffers
    /**< Call this function when you have finished using this object to release memory used by the 
     * internal buffers.*/

    typedef enum IXMLToken {
        IXMLToken_StartDocument=0,
        IXMLToken_StartTag=1,
        IXMLToken_EndTag=2,
        IXMLToken_Text=3,
        IXMLToken_Clear=4,
        IXMLToken_ProcessingInstruction=5,
        IXMLToken_EndDocument=6,
        IXMLToken_Warning=7,
        IXMLToken_Error=8
    } IXMLToken;
    virtual IXMLToken nextToken()=0;     ///< read the data stream a little bit more to get the next token
    IXMLToken currentToken() const;      ///< get the current token

    bool findPath(IXMLCStr path, IXMLChar sep='/', bool indicesAreZeroBased=true, IXMLResults *r=NULL);      ///< read the data stream to reach a XML tag with the specified path
    bool findPathNonConst(IXMLStr path,IXMLChar sep='/', bool indicesAreZeroBased=true, IXMLResults *r=NULL);///< read the data stream to reach a XML tag with the specified path

    IXMLCStr getName() const;      ///< if the current token==IXMLToken_StartTag, return the tag name
    bool isEmptyNode() const;      ///< if the current token==IXMLToken_StartTag, tell if the tag is directly closed or not
    /**< For example: 
     * <ul>
     * <li>isEmptyNode() will return false: 
     * @code 
     * <a></a> 
     * @endcode
     * </li>
     * <li>isEmptyNode() will return true: 
     * @code 
     * <a/> 
     * @endcode
     * </li>
     * </ul>
     */

    bool isArrayStart() const;                         ///< used during JSON parsing to separate arrays([) from object({).
    /**< During JSON parsing only: If the current token==IXMLToken_StartTag or IXMLToken_EndTag
     * This tells you if we are inside an JSON array ([) or inside an JSON object ({).
     */
    int nAttribute() const;                            ///< if the current token==IXMLToken_StartTag, return number of attributes
    IXMLAttr *getAttribute(int i=0) const;        ///< if the current token==IXMLToken_StartTag, return ith attribute
    IXMLCStr getAttributeName(int i=0) const;          ///< if the current token==IXMLToken_StartTag, return ith attribute name
    IXMLCStr getAttributeValue(int i=0) const;         ///< if the current token==IXMLToken_StartTag, return ith attribute value
    IXMLCStr getAttribute(IXMLCStr s) const;           ///< if the current token==IXMLToken_StartTag, return the value of the attribute with the given name
    int szAttributeValue(int i=0) const;               ///< if the current token==IXMLToken_StartTag, return ith attribute size

    IXMLCStr getText() const;                          ///< if the current token==IXMLToken_Text, return the text
    int szText() const;                                ///< if the current token==IXMLToken_Text, return the text size in char
    bool isTextWhiteSpace() const;                     ///< if the current token==IXMLToken_Text, return true if the text is ignorable (i.e.composed of white space only)

    const IXMLClear *getClear() const;                 ///< if the current token==IXMLToken_Clear, return the clear tag

    IXMLErrorInfo getErrorCode() const;                    ///< if the current token==IXMLToken_Error, return the error code
    long long getLineNumber() const;                   ///< return the current line number
    virtual int getColumnNumber() const;               ///< return the current column number expressed in byte or wchar
    int getDepth() const;                              ///< return the current depth

    // setup
    void setAutoDetectSourceEncoding(bool autoDetectSource=false);
    bool getAutoDetectSourceEncoding();
    /**< When the "AutoDetectSourceEncoding" flag is set, the parser try to guess the character encoding. 
     * It then change the current character encoding used inside the parser accordingly. The guess is 
     * based on the content of a buffer of length 512 bytes that contains the first bytes of the 
     * file/string to be parsed. There are several heuristics used to do the guess. One of the heuristic 
     * is based on the "encoding" attribute inside the XML text. The original XML specifications forbids 
     * to use this attribute to do the guess but we still use it nevertheless (this is the behavior of 
     * 99% of the parsers).
     * 
     * By default, the "AutoDetectSourceEncoding" flag is OFF in char* mode.
     * By default, the "AutoDetectSourceEncoding" flag is ON in wchar_t* mode.
     */

    /// Enumeration for XML character encoding.
    typedef enum IXMLCharEncoding
    {
        char_encoding_error=0,
        char_encoding_UTF32=1,
        char_encoding_UTF16=2,
        char_encoding_UTF8=3,
        char_encoding_Legacy=4,
        char_encoding_ShiftJIS=5,
        char_encoding_GB2312=6,
        char_encoding_Big5=7,
        char_encoding_GBK=8     // this is actually the same as Big5
    } IXMLCharEncoding;

#ifdef _XMLWIDECHAR
    virtual char setSourceCharEncoding(IXMLCharEncoding e=char_encoding_UTF16);
    /**< Set the current encoding of the source XML text. This can be char_encoding_UTF16, 
     *   char_encoding_UTF32, char_encoding_UTF8 or char_encoding_Legacy. The "char_encoding_Legacy" is the 
     *   system wide character encoding setup inside windows (typically, it's ISO-8859-1).
     * \return "0" when there are no errors. If you try to set an unrecognized encoding then the return value will be "1" to signal an error.
     */
#else
    virtual char setCharEncoding(IXMLCharEncoding sourceEncoding=char_encoding_UTF8, IXMLCharEncoding destinationEncoding=char_encoding_UTF8);
    virtual IXMLCharEncoding getCharEncodingDestination();
    /**< Set the current encoding of the source XML text and the desired encoding of the
     *   strings returned by the Pull Parser.
     * \return "0" when there are no errors. If you try to set an unrecognized encoding then the return value will be "1" to signal an error.
     */
#endif
    virtual IXMLCharEncoding getCharEncodingSource();
    static IXMLCStr charEncodingText(IXMLCharEncoding e); ///< return the text-version of the given char encoding


    IXMLCStr getVersion(); ///< Return the IXMLParser library version number
    static IXMLCStr getErrorMessage(IXMLErrorInfo error); ///< this gives you a user-friendly explanation of the parsing error

protected:
    // These are functions and structures used internally by the IXMLPullParser class (don't bother about them):
    IXMLReader *uReader,*sourceUReader;
    IXMLAttr *allAttributes;
    int szByteBuffer,_nAttribute;
    IXMLToken curToken;
    int depth,_szText;
    long long nLine;
    bool _isEmptyNode,_isIgnorableSpace,_isInArray,_isHTML;
    IXMLErrorInfo lastError;
    IXMLClear curClearTag;
    IXMLStr pContent;

    bool autoDetectSource;
    IXMLCharEncoding characterEncoding,sourceEncoding;
    const char *XML_ByteTable;

    bool skipBranch(int skipVar);
    unsigned char *skipNames;
    int szSkipNameBuffer;

    class IXMLReader_ToWideChar: public IXMLReader{
    public:
        IXMLReader_ToWideChar(IXMLReader *r);
        ~IXMLReader_ToWideChar();
        void setNormalBufferSize(int nbyte);
        void setSource(IXMLReader *source);
        IXMLReader *getSource();
        int read(unsigned char *buffer,int nbyte);
        void reset();
        IXMLReader *r;
        IXMLCharEncoding sourceEncoding; // system or utf8, utf16, utf32
        int mbstate,bufferSz,len;
        unsigned char *b;
    };
    class IXMLReader_FromUTF16: public IXMLReader{
    public:
        IXMLReader_FromUTF16(IXMLReader *r);
        ~IXMLReader_FromUTF16();
        void setNormalBufferSize(int nbyte);
        void setSource(IXMLReader *source);
        IXMLReader *getSource();
        int read(unsigned char *buffer,int nbyte);
        void reset();
        IXMLReader *r;
        IXMLCharEncoding destEncoding; // system or utf8 
        int mbstate,bufferSz,len;
        unsigned char *b;
    };
    class IXMLReader_FromUTF32: public IXMLReader_FromUTF16{
    public:
        IXMLReader_FromUTF32(IXMLReader *r);
        int read(unsigned char *buffer,int nbyte);
        //IXMLCharEncoding destEncoding; // system or utf8 
    };
    class IXMLReader_PassThrough: public IXMLReader{
    public:
        IXMLReader_PassThrough(IXMLReader *r);
        int read(unsigned char *buffer,int nbyte);
        IXMLReader *r;
        unsigned char b[512],*pos,*bEnd;
        void skipBOM();
        int nByteRead();
    };
    IXMLReader_ToWideChar charEncodingConverterToWideChar;
    IXMLReader_FromUTF16 charEncodingConverterFromUTF16;
    IXMLReader_FromUTF32 charEncodingConverterFromUTF32;

    unsigned char *bufferChar;
    IXMLStr pCurrentPos,pEndPos,pStartOfLine;
    bool isStreamFinished;
    char readData(IXMLStr *ptr,IXMLStr *pend,IXMLStr *ptrOut,IXMLStr *pStart);
};

/// Class Defining the Incredible XML Pull Parser
/**
 * This class defines the Incredible XML Pull Parser.<br/>
 * To create a new instance of the Incredible XML Pull Parser, you must provide 2 parameters:
 * <ol>
 * <li><b>Parameter 1:</b> an IXMLReader object that is used to "stream" data inside the parser.</li>
 * <li><b>Parameter 2:</b> a parameter that defines the size of the memory buffer used for streaming. 
 * More precisely: This parameter defines the size of buffer that is passed as first parameter of the 
 * IXMLReader::read() method.<br/><br/>
 * If the user-supplied value defines a buffer that is not big enough to contain one complete Token and 
 * its related data (i.e. one IXMLToken_Text or one IXMLToken_StartTag), then the Pull Parser will double 
 * the memory buffer size. The Incredible XML Pull Parser will continue to double the size of the memory 
 * buffer (amortized memory allocation) until it's large enough to contain one complete Token. When parsing
 * the next tokens, the Pull Parser re-uses the same memory space, so that </li>
 * </ol>
 * A small value for the Parameter 2 might slow down the Pull Parser significantly because it will be forced to:
 *   <ul>
 *   <li>make many calls to the IXMLReader::read() method (because each call returns only a few bytes).</li>
 *   <li>make many memory re-allocation until the buffer is large enough to contain one complete token.</li>
 *   </ul>
 */
class IXMLDLLENTRY IXMLPullParser : public IPullParser 
{
public:
    IXMLPullParser(IXMLReader *uReader,int _szByteBuffer=1024*1024);
    ~IXMLPullParser();
    
    void reset();                  ///< re-start the pull parser (set token=IXMLToken_StartDocument)
    /**< This does not free the memory buffers and it also keeps the current char encoding settings. */

    IXMLToken nextToken();         ///< read the data stream a little bit more to get the next token
    int getColumnNumber() const;   ///< return the current column number expressed in byte or wchar

    static IXMLCStr guessXMLCharEncoding(unsigned char *b,int l, IXMLChar out[200]); ///< guess the Char Encoding of the text inside the buffer
    static IXMLCharEncoding guessXMLCharEncodingCode(unsigned char *b,int l); ///< guess the Char Encoding Code of the text inside the buffer

private:
// These are functions and structures used internally by the IXMLPullParser class (don't bother about them):
    int maxAttribute;
    int nextChar,nextDepth;

    char parseNameAndAttributes(IXMLStr *_ptr, int *c);
    //inline char parseEndTag(IXMLStr *_ptr);
    char processText(IXMLChar delim,IXMLStr *_ptr, int *nextChar, int *len);
    char findClearTagEnd(IXMLStr *ptr,IXMLCStr closeTag,int closeTagLen);
};

/// To parse JSON data strings, use the Incredible JSON Pull Parser (see example 12)
class IXMLDLLENTRY IJSONPullParser: public IPullParser
{
public:
    IJSONPullParser(IXMLReader *uReader,int _szByteBuffer=1024*1024);
    ~IJSONPullParser();

    void reset();           ///< re-start the pull parser (set token=IXMLToken_StartDocument)
    /**< This does not free the memory buffers. */

    IXMLToken nextToken();  ///< read the data stream a little bit more to get the next token

    void setTagNameForArrays(IXMLCStr arrayName);
    IXMLCStr getTagNameForArrays();

    static IXMLCStr guessJSONCharEncoding(unsigned char *b,int l); ///< guess the Char Encoding of the text inside the buffer
    static IXMLCharEncoding guessJSONCharEncodingCode(unsigned char *b,int l); ///< guess the Char Encoding Code of the text inside the buffer

private:
    IXMLStr *allCurrentTags;
    IXMLStr tagNameForArray;
    int nextChar,tagBufferSize;
    bool _isEmptyNode;
    char parseStartList(IXMLStr *ptr);
    char parseString(IXMLStr *ptr);
    char parseNumber(IXMLStr *ptr, int *nextChar);
    void addToTagList(IXMLStr p,int l);
};

/// Class Defining the Incredible XML Dom Parser
class IXMLDLLENTRY IXMLDomParser
{
public:
    IXMLDomParser();    /// Create a new Incredible XML DOM parser
    ~IXMLDomParser();
    void clear();
    /**< Call this function when you have finished using this object to release memory used by the 
     * internal buffers.
     * <b>WARNING:</b> After calling the "clear()" function, all the UTCXMLNodes produced by this object 
     * are becoming invalid: You cannot access them anymore (the same thing happens when you delete the 
     * IXMLDomParser object). If you still need to access the content of some UTCXMLNodes after calling
     * the "clear()" function, you need to convert them in UCXMLNodes or IXMLNodes (using the 
     * "deepCopyConstant()" or deepCopy()" functions).
     */

    void setDomMemoryBlockSize(int blockSize=1024*1024);
    /**< To reduce the number of memory allocation calls performed during parsing, the 
     *   Incredible XML DOM Parser allocate "big" blocks of memory. Each block store many 
     *   ITCXMLNode objects. When all the blocks are "full", then a new memory allocation 
     *   call is performed to ask for one new "big" block. A small value for the blockSize
     *   will force the parser to make many memory allocation calls and might slow-down the 
     *   DOM Parser significantly. The IXMLDomParser class re-uses the memory allocated 
     *   during the previous parsing. Thus, most of the time, the parsing is performed 
     *   without any memory allocation at all.
     */
    int  getDomMemoryBlockSize(); ///< Returns the DomMemoryBlockSize parameter

    void setDomMaxMemorySize(size_t maxMem=0);
    /**< The DomMaxMemorySize parameter defines an upper bound on the memory consumption of 
     *   the DOM Parser. By default, there are no limit. When the memory limit is reached, the
     *   parse* functions return an empty ITCXMLNode and the errorCode is IXMLError_MaxMemoryReached.
     */
    size_t getDomMaxMemorySize(); ///< Returns the DomMaxMemorySize parameter (zero means "no limit")

    void setDefaultPullParserWorkspaceMemorySize(int memSize=1024*1024);
    /**< The IXMLDomParser::parseString(), IXMLDomParser::parseFile() and IXMLDomParser::openFileHelper() 
     *  are creating internally a new temporary Pull Parser that is used to perform the required parsing. 
     *  The size of the internal memory buffer of the internal Pull Parser is defined here using the 
     *  "DefaultPullParserWorkspaceMemorySize" parameter.
     */
    int  getDefaultPullParserWorkspaceMemorySize(); ///< return the "DefaultPullParserWorkspaceMemorySize" parameter

    void setHTML(bool isHTML);     
    /**< The IXMLDomParser::parseString(), IXMLDomParser::parseFile() and IXMLDomParser::openFileHelper() 
     *  are creating internally a new temporary Pull Parser that is used to perform the required parsing. 
     *  Use this parameter to define if the internal Pull Parser is actually parsing a HTML string (rather 
     *  than a simple XML string).
     */
    bool isHTML() const;           ///< is the XML file/string an HTML file?

    void setSkipIgnorableSpaces(bool skipIgnorableSpaces=true);
    /**< In most situations, text fields containing only white spaces (and carriage returns)
     *   are useless. Even more, these "empty" text fields are annoying because they increase the
     *   complexity of the user's code for parsing. So, 99% of the time, it's better to drop
     *   the "empty" text fields. However The XML specification indicates that no white spaces
     *   should be lost when parsing the file. So to be perfectly XML-compliant, you should set
     *   skipIgnorableSpaces=false. A note of caution: if you set "skipIgnorableSpaces=false", 
     *   the parser will be slower and your code will be more complex.
     */

    bool getSkipIgnorableSpaces(); ///< return the skipIgnorableSpaces parameter

    void setTrimTexts(bool trimTexts=true);
    /**<  It's very common to be forced to remove the white spaces (and the carriage returns)
     *    located at the start and at the end of all text fields.But, to be perfectly XML-compliant, 
     *    you should set "trimTexts=false". A note of caution: if you set "trimTexts=false", 
     *    the parser will be slower and your code will most likely be more complex.
     */
    
    bool getTrimTexts(); ///< return the trimTexts parameter

    void setTrimAttributes(bool trimAttributes=true);
    /**<  It's very common to be forced to remove the white spaces (and the carriage returns)
     *    located at the start and at the end of all the attribute values. But, to be perfectly 
     *    XML-compliant, you should set "trimAttributes=false". A note of caution: if you set 
     *    "trimAttributes=false", the parser will be slower and your code will most likely be more 
     *    complex.
     */
    bool getTrimAttributes(); ///< return the trimAttributes parameter

    void setRemoveClears(bool removeClears=true);
    /**< For efficiency reason, it's better to set "removeClears=true" because the DOM Parser
     *   can then skip any Clear tags (it allows to reduce the computation time and the memory 
     *   requirements). If you still need to get access to Clear tags inside the XMLNode tree, 
     *   set "removeClears=false".
     */
    bool getRemoveClears(); ///< return the removeClears parameter

    void setRemoveProcessingInstructions(bool removeProcessingInstructions=true);
    /**< For efficiency reason, it's better to set "removeProcessingInstructions=true" because 
     *   the DOM Parser can then skip any processing instructions (It allows to reduce the 
     *   computation time and the memory requirements). If you still need to get access to 
     *   processing instruction inside the XMLNode tree, set "removeProcessingInstructions=false".
     */
    bool getRemoveProcessingInstructions(); ///< return the removeProcessingInstructions parameter

    bool getAutoDetectSourceEncoding(); ///< return the autoDetectSourceEncoding parameter
    static IXMLCStr getErrorMessage(IXMLErrorInfo error); ///< this gives you a user-friendly explanation of the parsing error

    /// Parse an XML string and return the root of a IXMLNode tree representing the string
    ITCXMLNode parseString(IXMLCStr lpIXMLString, IXMLCStr tag=NULL, IXMLResults *pResults=NULL);
    ICXMLNode  parseStringNonT(IXMLCStr lpIXMLString, IXMLCStr tag=NULL, IXMLResults *pResults=NULL);
    /**< The "parseString" function parse an XML string and return the root of a IXMLNode tree. The "opposite" of this function is
     * the function "IXMLRenderer::GetString()" that re-creates an XML string from an IXMLNode tree. If the XML document is corrupted, the
     * "parseString" method will initialize the "pResults" variable with some information that can be used to trace the error.
     * If you still want to parse the file, you can use the APPROXIMATE_PARSING option as explained inside the note at the
     * beginning of the "IXMLParser.cpp" file.
     *
     * @param lpIXMLString the XML string to parse
     * @param tag  the name of the first tag inside the XML file. If the tag parameter is omitted, this function returns a node that represents the head of the xml document including the declaration term (<? ... ?>).
     * @param pResults a pointer to a XMLResults variable that will contain some information that can be used to trace the XML parsing error. You can have a user-friendly explanation of the parsing error with the "getError" function.
     */

    /// Parse an XML file and return the root of a IXMLNode tree representing the file
    ITCXMLNode parseFile(IXMLCStr filename, IXMLCStr tag=NULL, IXMLResults *pResults=NULL);
    ICXMLNode  parseFileNonT(IXMLCStr filename, IXMLCStr tag=NULL, IXMLResults *pResults=NULL);
    /**< The "parseFile" function parse an XML file and return the root of a ITCXMLNode tree. The "opposite" of this function is
     * the function "IXMLRenderer::writeToFile()" that re-creates an XML file from an IXMLNode tree. If the XML document is corrupted, the
     * "parseFile" method will initialize the "pResults" variable with some information that can be used to trace the error.
     * If you still want to parse the file, you can use the APPROXIMATE_PARSING option as explained inside the note at the
     * beginning of the "IXMLParser.cpp" file.
     *
     * @param filename the path to the XML file to parse
     * @param tag the name of the first tag inside the XML file. If the tag parameter is omitted, this function returns a node that represents the head of the xml document including the declaration term (<? ... ?>).
     * @param pResults a pointer to a XMLResults variable that will contain some information that can be used to trace the XML parsing error. You can have a user-friendly explanation of the parsing error with the "getError" function.
     */

    /// Parse an XML file and return the root of a IXMLNode tree representing the file. A very crude error checking is made
    ITCXMLNode openFileHelper(IXMLCStr filename, IXMLCStr tag=NULL);
    ICXMLNode  openFileHelperNonT(IXMLCStr filename, IXMLCStr tag=NULL);
    /**< The "openFileHelper" function reports to the screen all the warnings and errors that occurred during parsing of the XML file.
     * Since each application has its own way to report and deal with errors, you should rather use the "parseFile" function to parse 
     * XML files and program yourself thereafter an "error reporting" tailored for your needs (instead of using the very crude 
     * "error reporting" mechanism included inside the "openFileHelper" function).
     *
     * If the XML document is corrupted, the "openFileHelper" method will:
     *         - display an error message on the console (or inside a messageBox for windows).
     *         - stop execution (exit).
     *
     * I strongly suggest that you write your own "openFileHelper" method tailored to your needs. If you still want to parse
     * the file, you can use the APPROXIMATE_PARSING option as explained inside the note at the beginning of the "IXMLParser.cpp" file.
     *
     * @param filename the path of the XML file to parse.
     * @param tag the name of the first tag inside the XML file. If the tag parameter is omitted, this function returns a node that represents the head of the xml document including the declaration term (<? ... ?>).
     */

    ITCXMLNode parse(IPullParser *pp, IXMLCStr tag=NULL, IXMLResults *pResults=NULL);
    ICXMLNode  parseNonT(IPullParser *pp, IXMLCStr tag=NULL, IXMLResults *pResults=NULL);
    /**< The "parse" function read ALL the tokens produced by the Incredible XML Pull Parser and return the root of the 
     * corresponding ITCXMLNode tree. If the XML document is corrupted, the "parse" method will initialize the "pResults" 
     * variable with some information that can be used to trace the error. If you still want to parse the file, you can 
     * use the APPROXIMATE_PARSING option as explained inside the note at the beginning of the "IXMLParser.cpp" file.
     *
     * @param pp a pointer to the IXMLPullParser that is used to produce the ITCXMLNode tree.
     * @param tag  the name of the first tag inside the XML file. If the tag parameter is omitted, this function returns a node that represents the head of the xml document including the declaration term (<? ... ?>).
     * @param pResults a pointer to a XMLResults variable that will contain some information that can be used to trace the XML parsing error. You can have a user-friendly explanation of the parsing error with the "getError" function.
     */

    ITCXMLNode parseOneChild(IPullParser *pp, IXMLResults *pResults=NULL);
    ICXMLNode  parseOneChildNonT(IPullParser *pp, IXMLResults *pResults=NULL);
    /**< The "parseOneChild" function does NOT read ALL the tokens produced by the Incredible XML Pull Parser. It stops as soon
     * as it obtained a complete ChildNode xNode (see example 7). It then returns the xNode ChildNode.
     * If the XML document is corrupted, the "parseOneChild" method will initialize the "pResults" variable with some 
     * information that can be used to trace the error. If you still want to parse the file, you can use the APPROXIMATE_PARSING 
     * option as explained inside the note at the beginning of the "IXMLParser.cpp" file. This function is typically used 
     * when using the "hot start" functionality of the Incredible XML Parser, to be able process unlimited file size while, at the
     * same time, using very little RAM memory.
     *
     * @param pp a pointer to the IXMLPullParser that is used to produce the xNode ChildNode.
     * @param pResults a pointer to a XMLResults variable that will contain some information that can be used to trace the XML parsing error. You can have a user-friendly explanation of the parsing error with the "getError" function.
     */

    IXMLPullParser::IXMLCharEncoding getCharEncodingSource();
#ifdef _XMLWIDECHAR
    void setAutoDetectSourceEncoding(bool autoDetectSource=true); 
    /**< The parseString, parseFile and openFileHelper are creating internally a new temporary Pull Parser
    *   that is used to perform the required parsing. The IXMLDomParser::AutoDetectSourceEncoding parameter is used 
    *   to initialize the IXMLPullParser::AutoDetectSourceEncoding parameter of the Pull Parser.
    */
    void setSourceCharEncoding(IXMLPullParser::IXMLCharEncoding e=IXMLPullParser::char_encoding_UTF16);
    /**< The parseString, parseFile and openFileHelper are creating internally a new temporary Pull Parser
    *   that is used to perform the required parsing. The IXMLDomParser::SourceCharEncoding parameter is used 
    *   to initialize the IXMLPullParser::SourceCharEncoding parameter of the Pull Parser.
    */
private:
#else
    void setAutoDetectSourceEncoding(bool autoDetectSource=false);
    /**< The parseString, parseFile and openFileHelper are creating internally a new temporary Pull Parser
    *   that is used to perform the required parsing. The IXMLDomParser::AutoDetectSourceEncoding parameter is used 
    *   to initialize the IXMLPullParser::AutoDetectSourceEncoding parameter of the Pull Parser.
    */
    void setCharEncoding(IXMLPullParser::IXMLCharEncoding sourceEncoding=IXMLPullParser::char_encoding_UTF8, 
                         IXMLPullParser::IXMLCharEncoding destinationEncoding=IXMLPullParser::char_encoding_UTF8);
    /**< The parseString, parseFile and openFileHelper are creating internally a new temporary Pull Parser
    *   that is used to perform the required parsing. The IXMLDomParser::sourceCharEncoding and 
    *   IXMLDomParser::destinationEncoding parameters are used to initialize the IXMLPullParser::sourceCharEncoding
    *   and the IXMLPullParser::destinationEncoding parameters of the Pull Parser.
    */
    IXMLPullParser::IXMLCharEncoding getCharEncodingDestination();
private:
    IXMLPullParser::IXMLCharEncoding destinationEncoding;
#endif
    
    // These are functions and structures used internally by the IXMLDomParser class (don't bother about them):
    IXMLPullParser::IXMLCharEncoding sourceEncoding;
    bool autoDetectSource,skipIgnorableSpaces,requiresTrimText,trimAttribute,
         removeClears,removeProcessingInstructions,_isHTML;
    int memBufferSz,pullParserMemSz;
    size_t maxMemDom;
    ITCXMLNode genericParse(IXMLCStr tag);
    ITCXMLNode genericParse2(bool getOneChild);
    char genericParseRecur(int iChild,int iText, int iOrder, int iClear, void *td, bool getOneChild);
    void *initNTD();

    IPullParser *pullParser;
    IXMLResults *pResults,xmlRes;
    ITCXMLNode *vChild; int nChild;
    IXMLCStr  *vText;  int nText;
    IXMLClear *vClear; int nClear;
    int       *vOrder; int nOrder;

    void *memoryManager;
};
/** @} */

/** @defgroup Navigate Navigate the XMLNode structure
 * @ingroup IXMLParserGeneral
 * You explore the XML data moving inside a structure of ITCXMLNode's (or ICXMLNode's, or IXMLNode's).<br/>
 * For general navigation, please refer to example1().<br/>
 * You should pay extra attention when using the ITCXMLNode::getParentNode() method: see example10().
 * @{ */

class IXMLNode;
class ICXMLNode;
struct XMLNode;

/// Main Class representing a Incredible Transient Constant XML node
/**
 * Most operations are performed using this class.
 * \note The constructors of the ITCXMLNode class are protected, so use instead one of these four methods to get your first instance of ITCXMLNode:
 * <ul>
 *    <li>IXMLDomParser::parseString</li>
 *    <li>IXMLDomParser::parseFile</li>
 *    <li>IXMLDomParser::openFileHelper</li>
 *    <li>IXMLDomParser::parse</li>
 *    <li>IXMLDomParser::parseOneChild</li>
 * </ul> */
class IXMLDLLENTRY ITCXMLNode
{
friend class IXMLDomParser;
friend class IXMLNode;
friend class ICXMLNode;
friend class IXMLRenderer;
friend XMLNode deepConvert(ITCXMLNode x);

public:
    IXMLCStr getName() const;                                       ///< name of the node
    IXMLCStr getText(int i=0) const;                                ///< return ith text field
    int nText() const;                                             ///< nbr of text field
    ITCXMLNode getParentNode() const;                                 ///< return the parent node
    ITCXMLNode getChildNode(int i=0) const;                           ///< return ith child node
    ITCXMLNode getChildNode(IXMLCStr name, int i)  const;              ///< return ith child node with specific name (return an empty node if failing). If i==-1, this returns the last IXMLNode with the given name
    ITCXMLNode getChildNode(IXMLCStr name, int *i=NULL) const;         ///< return next child node with specific name (return an empty node if failing)
    ITCXMLNode getChildNodeWithAttribute(IXMLCStr tagName,
                                      IXMLCStr attributeName,
                                      IXMLCStr attributeValue=NULL,
                                      int *i=NULL)  const;         ///< return child node with specific name/attribute (return an empty node if failing)
    ITCXMLNode getChildNodeByPath(IXMLCStr path, bool indicesAreZeroBased=true, IXMLChar sep='/') const;
                                                                   ///< return the first child node with specific path
    ITCXMLNode getChildNodeByPathNonConst(IXMLStr path,bool indicesAreZeroBased=true,IXMLChar sep='/') const;
                                                                   ///< return the first child node with specific path
    IXMLCStr getElementByPath(IXMLCStr xpath,bool indicesAreZeroBased=true,IXMLChar sep='/') const;
                                                                    ///< return the text content of the given simplified XPATH
    IXMLCStr getElementByPathNonConst(IXMLStr xpath,bool indicesAreZeroBased=true,IXMLChar sep='/') const;
                                                                    ///< return the text content of the given simplified XPATH

    int nChildNode(IXMLCStr name) const;                            ///< return the number of child node with specific name
    int nChildNode() const;                                        ///< nbr of child node
    IXMLAttr getAttribute(int i=0) const;                      ///< return ith attribute
    IXMLCStr      getAttributeName(int i=0) const;                  ///< return ith attribute name
    IXMLCStr      getAttributeValue(int i=0) const;                 ///< return ith attribute value
    char  isAttributeSet(IXMLCStr name) const;                      ///< test if an attribute with a specific name is given
    IXMLCStr getAttribute(IXMLCStr name, int i) const;               ///< return ith attribute content with specific name (return a NULL if failing)
    IXMLCStr getAttribute(IXMLCStr name, int *i=NULL) const;         ///< return next attribute content with specific name (return a NULL if failing)
    int nAttribute() const;                                        ///< nbr of attribute
    IXMLClear getClear(int i=0) const;                              ///< return ith clear field (comments)
    int nClear() const;                                            ///< nbr of clear field
    int nElement() const;                                          ///< nbr of different contents for current node
    char isEmpty() const;                                          ///< is this node Empty?
    char isProcessInstruction() const;                              ///< is this node a Process instruction <? .... ?>
    IXMLNode deepCopy() const;                                      ///< deep copy (duplicate/clone) a IXMLNode
    ICXMLNode deepCopyConstant() const;                             ///< deep copy (duplicate/clone) a IXMLNode
    static ITCXMLNode emptyNode();                                    ///< return IXMLNode::emptyIXMLNode;
    static IXMLCStr getErrorMessage(IXMLErrorInfo error); ///< this gives you a user-friendly explanation of the parsing error

    ITCXMLNode(const ITCXMLNode &A){d=A.d;};                             ///< to allow shallow/fast copy:
    ITCXMLNode& operator=( const ITCXMLNode& A );                        ///< to allow shallow/fast copy:
    ITCXMLNode(): d(NULL){};
    static ITCXMLNode emptyITCXMLNode;             // you should use ITCXMLNode::emptyNode(); instead
    static IXMLClear emptyIXMLClear;
    static IXMLAttr emptyXMLAttribute;

protected:
// These are functions and structures used internally by the ITCXMLNode class (don't bother about them):

    typedef struct IXMLNodeData // to allow shallow copy and "intelligent/smart" pointers (automatic delete):
    {
        IXMLCStr                pName;           // Element name (=NULL if root)
        int                     nChild,          // Number of child nodes
                                nText,           // Number of text fields
                                nClear,          // Number of Clear fields (comments)
                                nAttribute;      // Number of attributes
        char                    isProcessInstruction;   // Whether node is an XML Process Instruction - '<?xml ?>'
        struct IXMLNodeData     *pParent;        // Pointer to parent element (=NULL if root)
        ITCXMLNode              *pChild;         // Array of child nodes
        IXMLCStr                *pText;          // Array of text fields
        IXMLClear               *pClear;         // Array of clear fields
        IXMLAttr           *pAttribute;     // Array of attributes
        int                     *pOrder;         // order of the child_nodes,text_fields,clear_fields
        union 
        {
            int ref_count;   // for garbage collection in IXMLNode (smart pointers)
            int *memBuffer;  // for garbage collection in ICXMLNode(smart pointers)
        };
        inline void init(struct IXMLNodeData *pParent, char isDeclaration);
    } IXMLNodeData;
   IXMLNodeData *d;

   typedef enum IXMLElementType
   {
       eNodeChild=0,
       eNodeText=1,
       eNodeClear=2
   } IXMLElementType;

private:
   int deepCopyConstantR1()const;
   ICXMLNode deepCopyConstantR2(int *nref, unsigned char **_p) const;

   ITCXMLNode(IXMLNodeData *p);
   ITCXMLNode(const ICXMLNode &A);                ///< to forbid mixing different kind of XMLNodes
   ITCXMLNode& operator=( const ICXMLNode& A );   ///< to forbid mixing different kind of XMLNodes
   ITCXMLNode(const IXMLNode &A);                 ///< to forbid mixing different kind of XMLNodes
   ITCXMLNode& operator=( const IXMLNode& A );    ///< to forbid mixing different kind of XMLNodes
};

/// Main Class representing a Incredible Non-Transient and Constant XML node
/**
 * Use the ICXMLNode class when you want to keep a XMLNode structure in memory after
 * a new call to one of the parse method of the IXMLDomParser.
 * The constructors of the ITCXMLNode class are protected, so use instead one of these four methods to get your first instance of ICXMLNode:
 * <ul>
 *    <li>ITCXMLNode::deepCopyConstant</li>
 *    <li>IXMLDomParser::parseStringNonT</li>
 *    <li>IXMLDomParser::parseFileNonT</li>
 *    <li>IXMLDomParser::openFileHelperNonT</li>
 *    <li>IXMLDomParser::parseNonT</li>
 *    <li>IXMLDomParser::parseOneChildNonT</li>
 * </ul> 
 */
 class IXMLDLLENTRY ICXMLNode: public ITCXMLNode
{
friend class IXMLRenderer;
public:
    ICXMLNode getParentNode() const;                                 ///< return the parent node
    ICXMLNode getChildNode(int i=0) const;                           ///< return ith child node
    ICXMLNode getChildNode(IXMLCStr name, int i)  const;             ///< return ith child node with specific name (return an empty node if failing). If i==-1, this returns the last IXMLNode with the given name
    ICXMLNode getChildNode(IXMLCStr name, int *i=NULL) const;        ///< return next child node with specific name (return an empty node if failing)
    ICXMLNode getChildNodeWithAttribute(IXMLCStr tagName,
                                        IXMLCStr attributeName,
                                        IXMLCStr attributeValue=NULL,
                                        int *i=NULL)  const;         ///< return child node with specific name/attribute (return an empty node if failing)
    ICXMLNode getChildNodeByPath(IXMLCStr path,bool indicesAreZeroBased=true,IXMLChar sep='/') const;   ///< return the first child node with specific path
    ICXMLNode getChildNodeByPathNonConst(IXMLStr  path,bool indicesAreZeroBased=true,IXMLChar sep='/') const;
    static ICXMLNode emptyNode();                 ///< return an empty Node;

    ~ICXMLNode();
    ICXMLNode(const ICXMLNode &A);                ///< to allow shallow/fast copy:
    ICXMLNode& operator=( const ICXMLNode& A );   ///< to allow shallow/fast copy:
    ICXMLNode():ITCXMLNode(){};
    static ICXMLNode emptyICXMLNode;

private:
    inline void emptyTheNode();
};

/** @} */

/** @defgroup xmlModify Edit/Update the IXMLNode structure
 * @ingroup IXMLParserGeneral
 *  The functions in this group allows you to create from scratch (or update) a IXMLNode structure. Start by creating your top
 *  node with the "createXMLTopNode" function and then add new nodes with the "addChild" function. The parameter 'pos' gives
 *  the position where the childNode, the text or the XMLClearTag will be inserted. The default value (pos=-1) inserts at the
 *  end. The value (pos=0) insert at the beginning (Insertion at the beginning is slower than at the end). <br>
 *
 *  REMARK: 0 <= pos < nChild()+nText()+nClear() <br/>
 *  
 *  See the functions example3(), example5() and example6() for some usage examples.
 **/
/// \ingroup xmlModify IXMLElementPosition are not interchangeable with simple indexes
typedef int IXMLElementPosition;

/**
 * @ingroup Navigate 
 * @brief Main Class representing an Incredible Non-Transient and Editable XML node
 *
 * Use the IXMLNode class when you want to modify/update/edit a XMLNode structure.
 * The constructors of the ITCXMLNode class are protected, so use instead one of these four methods to get your first instance of IXMLNode:
 * <ul>
 *    <li>ITCXMLNode::deepCopy</li>
 *     <li>ICXMLNode::deepCopy</li>
 *     <li>IXMLNode::createXMLTopNode</li>
 * </ul> 
 */
class IXMLDLLENTRY IXMLNode: public ITCXMLNode
{
friend class IXMLRenderer;
friend class ITCXMLNode;
public:
    IXMLNode getParentNode() const;                                 ///< return the parent node
    IXMLNode getChildNode(int i=0) const;                           ///< return ith child node
    IXMLNode getChildNode(IXMLCStr name, int i)  const;              ///< return ith child node with specific name (return an empty node if failing). If i==-1, this returns the last IXMLNode with the given name
    IXMLNode getChildNode(IXMLCStr name, int *i=NULL) const;         ///< return next child node with specific name (return an empty node if failing)
    IXMLNode getChildNodeWithAttribute(IXMLCStr tagName,
                                        IXMLCStr attributeName,
                                        IXMLCStr attributeValue=NULL,
                                        int *i=NULL)  const;         ///< return child node with specific name/attribute (return an empty node if failing)
    IXMLNode getChildNodeByPath(IXMLCStr path,bool indicesAreZeroBased=true,IXMLChar sep='/')const;   ///< return the first child node with specific path
    IXMLNode getChildNodeByPathNonConst(IXMLStr  path,bool indicesAreZeroBased=true,IXMLChar sep='/')const;
    static IXMLNode emptyNode(); ///< return an empty Node;

    ~IXMLNode();
    IXMLNode(const IXMLNode &A);                                     ///< to allow shallow/fast copy:
    IXMLNode& operator=( const IXMLNode& A );                        ///< to allow shallow/fast copy:
    IXMLNode():ITCXMLNode(){};
    static IXMLNode emptyIXMLNode;

    /** @defgroup creation Creating from scratch a IXMLNode structure
     * @ingroup xmlModify
     * See the function example3() for some usage examples.
     * @{ */
    static IXMLNode createXMLTopNode(IXMLCStr pName=NULL, char isDeclaration=FALSE);                ///< Create the top node of an IXMLNode structure
    IXMLNode        addChild(IXMLCStr pName, char isDeclaration=FALSE, IXMLElementPosition pos=-1); ///< Add a new child node
    IXMLNode        addChild(IXMLNode nodeToAdd, IXMLElementPosition pos=-1);                       ///< If the "nodeToAdd" has some parents, it will be detached from it's parents before being attached to the current IXMLNode
    IXMLAttr  *addAttribute(IXMLCStr pName, IXMLCStr pValuev);                                 ///< Add a new attribute
    IXMLCStr        addText(IXMLCStr pValue, IXMLElementPosition pos=-1);                           ///< Add a new text content
    IXMLClear      *addClear(IXMLCStr pValue, IXMLCStr lpcOpen=NULL, IXMLCStr lpcClose=NULL, IXMLElementPosition pos=-1);
    /**< Add a new clear tag
     * @param lpcOpen default value "<![CDATA["
     * @param lpcClose default value "]]>"
     */
    /** @} */

    /** @defgroup xmlUpdate Updating Nodes
     * @ingroup xmlModify
     * See the functions example3() and example6() for some usage examples.
     * @{
     */
    IXMLCStr       updateName(IXMLCStr pName);                                                  ///< change node's name
    IXMLAttr *updateAttribute(IXMLAttr *newAttribute, IXMLAttr *oldAttribute);       ///< if the attribute to update is missing, a new one will be added
    IXMLAttr *updateAttribute(IXMLCStr lpcNewValue, IXMLCStr lpcNewName=NULL,int i=0);       ///< if the attribute to update is missing, a new one will be added
    IXMLAttr *updateAttribute(IXMLCStr lpcNewValue, IXMLCStr lpcNewName,IXMLCStr lpcOldName);///< set lpcNewName=NULL if you don't want to change the name of the attribute if the attribute to update is missing, a new one will be added
    IXMLCStr       updateText(IXMLCStr lpcNewValue, int i=0);                                     ///< if the text to update is missing, a new one will be added
    IXMLCStr       updateText(IXMLCStr lpcNewValue, IXMLCStr lpcOldValue);                        ///< if the text to update is missing, a new one will be added
    IXMLClear     *updateClear(IXMLCStr lpcNewContent, int i=0);                                  ///< if the clearTag to update is missing, a new one will be added
    IXMLClear     *updateClear(IXMLClear *newP,IXMLClear *oldP);                                    ///< if the clearTag to update is missing, a new one will be added
    IXMLClear     *updateClear(IXMLCStr lpcNewValue, IXMLCStr lpcOldValue);                       ///< if the clearTag to update is missing, a new one will be added
    /** @} */

    /** @defgroup xmlDelete Deleting Nodes or Attributes
     * @ingroup xmlModify
     * See the functions example3() and example5() for some usage examples.
     * @{
     */
    /// The "deleteNodeContent" function forces the deletion of the content of this IXMLNode and the subtree.
    void deleteNodeContent();
    /**< \note The IXMLNode instances that are referring to the part of the subtree that has been deleted CANNOT be used anymore!!. Unexpected results will occur if you continue using them. */
    void deleteAttribute(int i=0);                   ///< Delete the ith attribute of the current IXMLNode
    void deleteAttribute(IXMLCStr pName);          ///< Delete the attribute with the given name (the "strcmp" function is used to find the right attribute)
    void deleteAttribute(IXMLAttr *anAttribute); ///< Delete the attribute with the name "anAttribute->pName" (the "strcmp" function is used to find the right attribute)
    void deleteText(int i=0);                        ///< Delete the Ith text content of the current IXMLNode
    void deleteText(IXMLCStr pValue);              ///< Delete the text content "pValue" inside the current IXMLNode (direct "pointer-to-pointer" comparison is used to find the right text)
    void deleteClear(int i=0);                       ///< Delete the Ith clear tag inside the current IXMLNode
    void deleteClear(IXMLCStr pValue);             ///< Delete the clear tag "pValue" inside the current IXMLNode (direct "pointer-to-pointer" comparison is used to find the clear tag)
    void deleteClear(IXMLClear *p);                   ///< Delete the clear tag "p" inside the current IXMLNode (direct "pointer-to-pointer" comparison on the pName of the clear tag is used to find the clear tag)
    /** @} */

    /** @defgroup xmlWOSD ???_WOSD functions.
     * @ingroup xmlModify
     *  The strings given as parameters for the "add" and "update" methods that have a name with
     *  the postfix "_WOSD" (that means "WithOut String Duplication")(for example "addText_WOSD")
     *  will be free'd by the IXMLNode class. For example, it means that this is incorrect:
     *  \code
     *     xNode.addText_WOSD("foo");
     *     xNode.updateAttribute_WOSD("#newcolor" ,NULL,"color");
     *  \endcode
     *  In opposition, this is correct:
     *  \code
     *     xNode.addText("foo");
     *     xNode.addText_WOSD(stringDup("foo"));
     *     xNode.updateAttribute("#newcolor" ,NULL,"color");
     *     xNode.updateAttribute_WOSD(stringDup("#newcolor"),NULL,"color");
     *  \endcode
     *  Typically, you will never do:
     *  \code
     *     char *b=(char*)malloc(...);
     *     xNode.addText(b);
     *     free(b);
     *  \endcode
     *  ... but rather:
     *  \code
     *     char *b=(char*)malloc(...);
     *     xNode.addText_WOSD(b);
     *  \endcode
     *  ('free(b)' is performed by the IXMLNode class)
     * @{ */
    static IXMLNode createXMLTopNode_WOSD(IXMLStr pName, char isDeclaration=FALSE);                     ///< Create the top node of an IXMLNode structure
    IXMLNode        addChild_WOSD(IXMLStr pName, char isDeclaration=FALSE, IXMLElementPosition pos=-1);  ///< Add a new child node
    IXMLAttr  *addAttribute_WOSD(IXMLStr pName, IXMLStr pValue);                                 ///< Add a new attribute
    IXMLCStr        addText_WOSD(IXMLStr pValue, IXMLElementPosition pos=-1);                            ///< Add a new text content
    IXMLClear      *addClear_WOSD(IXMLStr pValue, IXMLCStr lpcOpen=NULL, IXMLCStr lpcClose=NULL, IXMLElementPosition pos=-1); ///< Add a new clear Tag

    IXMLCStr        updateName_WOSD(IXMLStr pName);                                                  ///< change node's name
    IXMLAttr  *updateAttribute_WOSD(IXMLAttr *newAttribute, IXMLAttr *oldAttribute);      ///< if the attribute to update is missing, a new one will be added
    IXMLAttr  *updateAttribute_WOSD(IXMLStr lpcNewValue, IXMLStr lpcNewName=NULL,int i=0);        ///< if the attribute to update is missing, a new one will be added
    IXMLAttr  *updateAttribute_WOSD(IXMLStr lpcNewValue, IXMLStr lpcNewName,IXMLCStr lpcOldName); ///< set lpcNewName=NULL if you don't want to change the name of the attribute if the attribute to update is missing, a new one will be added
    IXMLCStr        updateText_WOSD(IXMLStr lpcNewValue, int i=0);                                     ///< if the text to update is missing, a new one will be added
    IXMLCStr        updateText_WOSD(IXMLStr lpcNewValue, IXMLCStr lpcOldValue);                        ///< if the text to update is missing, a new one will be added
    IXMLClear      *updateClear_WOSD(IXMLStr lpcNewContent, int i=0);                                  ///< if the clearTag to update is missing, a new one will be added
    IXMLClear      *updateClear_WOSD(IXMLClear *newP,IXMLClear *oldP);                                   ///< if the clearTag to update is missing, a new one will be added
    IXMLClear      *updateClear_WOSD(IXMLStr lpcNewValue, IXMLCStr lpcOldValue);                       ///< if the clearTag to update is missing, a new one will be added
    /** @} */

    /** @defgroup xmlPosition Position helper functions (use in conjunction with the update&add functions
     * @ingroup xmlModify
     * These are some useful functions when you want to insert a childNode, a text or a XMLClearTag in the
     * middle (at a specified position) of a IXMLNode tree already constructed. The value returned by these
     * methods is to be used as last parameter (parameter 'pos') of addChild, addText or addClear.
     * @{ */
    IXMLElementPosition positionOfText(int i=0) const;
    IXMLElementPosition positionOfText(IXMLCStr pValue) const;
    IXMLElementPosition positionOfClear(int i=0) const;
    IXMLElementPosition positionOfClear(IXMLCStr pValue) const;
    IXMLElementPosition positionOfClear(IXMLClear *a) const;
    IXMLElementPosition positionOfChildNode(int i=0) const;
    IXMLElementPosition positionOfChildNode(IXMLNode x) const;
    IXMLElementPosition positionOfChildNode(IXMLCStr name, int i=0) const; ///< return the position of the ith childNode with the specified name if (name==NULL) return the position of the ith childNode
    /** @} */

private:
// These are functions used internally by the IXMLNode class (don't bother about them):
    IXMLNode(IXMLNodeData *pParent, IXMLStr pName, char isDeclaration);
    void *addToOrder(int *_pos, int nc, void *p, int size, IXMLElementType xtype);
    int indexText(IXMLCStr pValue) const;
    int indexClear(IXMLCStr pValue) const;
    IXMLNode addChild_priv(IXMLStr,char,int);
    IXMLAttr *addAttribute_priv(IXMLStr,IXMLStr);
    IXMLCStr addText_priv(IXMLStr,int);
    IXMLClear *addClear_priv(IXMLStr,IXMLCStr,IXMLCStr,int);
    void emptyTheNode(char force);
    static inline IXMLElementPosition findPosition(IXMLNodeData *d, int index, IXMLElementType xtype);
    static int removeOrderElement(IXMLNodeData *d, IXMLElementType t, int index);
    static int detachFromParent(IXMLNodeData *d);
};

/** @defgroup XMLStringTool The simplest & fastest way to create an XML string
* @ingroup CreatingXML
* See the function example11() for some usage examples.
* @{ */

/// Helper class to create XML files using "printf", "fprintf", "cout",... functions
/** The ToIXMLStringTool class helps you creating XML files using "printf", "fprintf", "cout",... functions.
 * The "ToIXMLStringTool" class is processing strings so that all the characters
 * &,",',<,> are replaced by their XML equivalent:
 * \verbatim &amp;, &quot;, &apos;, &lt;, &gt; \endverbatim
 * Using the "ToIXMLStringTool class" and the "fprintf function" is THE most efficient
 * way to produce VERY large XML documents VERY fast.
 * \note If you are creating from scratch an XML file using the provided IXMLNode class
 * you must not use the "ToIXMLStringTool" class (because the "IXMLNode" class does the
 * processing job for you during rendering).*/
class IXMLDLLENTRY ToIXMLStringTool
{
public:
    /// Enumeration for XML String location.
    typedef enum IXMLStringLocation
    {
        IXMLStringLocation_unknown=0,
        IXMLStringLocation_inAttribute=1,
        IXMLStringLocation_inText=2
    } IXMLStringLocation;

    ToIXMLStringTool(IXMLStringLocation loc=IXMLStringLocation_unknown);
    ~ToIXMLStringTool();
    void clear();///<call this function when you have finished using this object to release memory used by the internal buffer.
    void setLocation(IXMLStringLocation loc=IXMLStringLocation_unknown);
    IXMLStringLocation getLocation();

    IXMLStr toXML(IXMLCStr source);///< returns a pointer to an internal buffer that contains a XML-encoded string based on the "source" parameter.

    /** The "toXMLUnSafe" function is deprecated because there is a possibility of
     * "destination-buffer-overflow". It converts the string
     * "source" to the string "dest". */
    IXMLStr toXMLUnSafe(IXMLStr dest,IXMLCStr source); ///< deprecated: use "toXML" instead
    int lengthXMLString(IXMLCStr source);              ///< deprecated: use "toXML" instead

#ifndef _XMLWIDECHAR
    char setCharEncoding(IXMLPullParser::IXMLCharEncoding encoding=IXMLPullParser::char_encoding_UTF8);
    IXMLPullParser::IXMLCharEncoding getCharEncoding();
private:
// These are functions and structures used internally by the ToIXMLStringTool class (don't bother about them):
    const char *XML_ByteTable;
    IXMLPullParser::IXMLCharEncoding characterEncoding;
#endif

private:
    IXMLStr buf;
    int buflen;
    IXMLStringLocation loc;
    void *curXMLEntities;
};
/** @} */

/** @defgroup CreatingXML Creating XML files or XML Strings.
 * @ingroup IXMLParserGeneral
 * To create XML strings (or files), you can either use the ToIXMLStringTool class 
 * (This is the most efficient solution and also the easiest to use)
 * or the IXMLRenderer class (to transform ITCXMLNode to XML strings). <br/><br/>
 * See the functions example1(), example3(), example4(), example9() and example11() for some usage examples.
 * @{ */

/// Class Defining the Incredible XML Renderer.
class IXMLDLLENTRY IXMLRenderer
{
public:
    IXMLRenderer();
    ~IXMLRenderer();
    static IXMLCStr getErrorMessage(IXMLErrorInfo error); ///< this gives you a user-friendly explanation of the parsing error

     /** @defgroup CreateString Create an XML string starting from the current IXMLNode.
     * @ingroup CreatingXML
     * If the nFormat parameter is zero, no formatting is required otherwise this returns a pointer to an internal buffer that 
     * contains an user friendly XML string with appropriate white spaces and carriage returns that represents textually the content 
     * of the IXMLNode x. If pnSize is given it returns the size in character of the string. The same memory buffer is re-used
     * at each call of the "getString" or "writeToFile" function. This allows very fast rendering because there will be 
     * no new memory allocations performed when calling several times the "getString" function. <br/><br/>
     * See the functions example3() and example4() for some usage examples.
     * @{ */
    IXMLCStr getString(ITCXMLNode x, int nFormat=1, int *pnSize=NULL); ///< Create an XML string starting from the current ITCXMLNode.
    IXMLCStr getString(ICXMLNode x, int nFormat=1, int *pnSize=NULL);  ///< Create an XML string starting from the current ICXMLNode.
    IXMLCStr getString(IXMLNode x, int nFormat=1, int *pnSize=NULL);   ///< Create an XML string starting from the current IXMLNode.
    int szString(); ///< the size of the last XML string computed.
    void clear();///< Call this function when you have finished using this object to release memory used by the internal buffer.
    /** @} */

    /** @defgroup writeToFile Save the content of an IXMLNode inside a file.
     * @ingroup CreatingXML
     * If nFormat==0, no formatting is required otherwise this returns an user friendly XML string from a given element with appropriate white spaces and carriage returns.
     * If the global parameter "characterEncoding==encoding_UTF8", then the "encoding" parameter is ignored and always set to "utf-8".
     * If the global parameter "characterEncoding==encoding_ShiftJIS", then the "encoding" parameter is ignored and always set to "SHIFT-JIS".
     * If "_XMLWIDECHAR=1", then the "encoding" parameter is ignored and always set to "utf-16".
     * If no "encoding" parameter is given the "ISO-8859-1" encoding is used. <br/><br/>
     * See the functions example1() and example9() for some usage examples.
     * @{ */
    IXMLErrorInfo writeToFile(ITCXMLNode x, IXMLCStr filename,IXMLCStr encoding=NULL,char nFormat=1); ///< Save the content of an ITCXMLNode inside a file
    IXMLErrorInfo writeToFile(ICXMLNode x, IXMLCStr filename,IXMLCStr encoding=NULL,char nFormat=1);  ///< Save the content of an ITCXMLNode inside a file
    IXMLErrorInfo writeToFile(IXMLNode x, IXMLCStr filename,IXMLCStr encoding=NULL,char nFormat=1);   ///< Save the content of an ITCXMLNode inside a file
    /** @} */

#ifndef _XMLWIDECHAR
    IXMLPullParser::IXMLCharEncoding characterEncoding;
public:
    void setCharEncoding(IXMLPullParser::IXMLCharEncoding e=IXMLPullParser::char_encoding_UTF8);
    IXMLPullParser::IXMLCharEncoding getCharEncoding();
#endif

private:
// These are functions and structures used internally by the IXMLRenderer class (don't bother about them):
    IXMLStr buf;
    int maxStrLen,sz;
    ToIXMLStringTool txAtt,txTxt;
    int getStringR(ITCXMLNode::IXMLNodeData *pEntry, IXMLStr lpcMarker, int nFormat);
};
/** @} */

/** @defgroup StringAlloc String Allocation/Free functions
 * @ingroup xmlModify
 * See the function example2() for some usage examples.
 * @{ */
/// Duplicate (copy in a new allocated buffer) the source string.
IXMLDLLENTRY IXMLStr IXMLStringDup(IXMLCStr source, int cbData=-1);
/**< This is
 * a very handy function when used with all the "IXMLNode::*_WOSD" functions (\link xmlWOSD \endlink).
 * @param cbData If !=0 then cbData is the number of chars to duplicate. New strings allocated with
 * this function should be free'd using the "freeIXMLString" function. */

/// to free the string allocated inside the "stringDup" function.
IXMLDLLENTRY void freeIXMLString(IXMLStr t); // {free(t);}
/** @} */

/** @defgroup atoX ato? like functions
 * @ingroup IXMLParserGeneral
 * The "xmlto?" functions are equivalents to the atoi, atol, atof functions.
 * The only difference is: If the parameter "s" is NULL, than the return value
 * is "defautValue". These 6 functions are only here as "convenience" functions for the
 * user (they are not used inside the IXMLParser). If you don't need them, you can
 * delete them without any trouble.
 * @{ */
#ifndef NO_XMLTO_FUNCTIONS
IXMLDLLENTRY char      xmltob(IXMLCStr s,char defautValue=0);
IXMLDLLENTRY int       xmltoi(IXMLCStr s,int defautValue=0);
IXMLDLLENTRY long long xmltol(IXMLCStr s,long long defautValue=0);
IXMLDLLENTRY double    xmltof(IXMLCStr s,double defautValue=.0);
IXMLDLLENTRY IXMLCStr  xmltoa(IXMLCStr s,IXMLCStr defautValue=_CXML(""));
IXMLDLLENTRY IXMLChar  xmltoc(IXMLCStr s,const IXMLChar defautValue=_CXML('\0'));
#endif
/** @} */

/** @defgroup IXMLParserBase64Tool Helper class to include binary data inside XML strings using "Base64 encoding".
 * @ingroup IXMLParserGeneral
 * See the function example8() for some usage examples.
 * @{ */
/// Helper class to include binary data inside XML strings using "Base64 encoding".
/** The "IXMLParserBase64Tool" class allows you to include any binary data (images, sounds,...)
 * into an XML document using "Base64 encoding". This class is completely
 * separated from the rest of the IXMLParser library and can be removed without any problem.
 * To include some binary data into an XML file, you must convert the binary data into
 * standard text (using "encode"). To retrieve the original binary data from the
 * b64-encoded text included inside the XML file, use "decode". Alternatively, these
 * functions can also be used to "encrypt/decrypt" some critical data contained inside
 * the XML (it's not a strong encryption at all, but sometimes it can be useful).
 */
class IXMLDLLENTRY IXMLParserBase64Tool
{
public:
    IXMLParserBase64Tool(): buf(NULL),buflen(0){}
    ~IXMLParserBase64Tool();
    void clear();///< Call this function when you have finished using this object to release memory used by the internal buffer.

    /**
     * @param formatted If "formatted"=true, some space will be reserved for a carriage-return every 72 chars. */
    static int encodeLength(int inBufLen, char formatted=0); ///< return the length of the base64 string that encodes a data buffer of size inBufLen bytes.

    /**
     * The "base64Encode" function returns a string containing the base64 encoding of "inByteLen" bytes
     * from "inByteBuf". If "formatted" parameter is true, then there will be a carriage-return every 72 chars.
     * The string will be free'd when the IXMLParserBase64Tool object is deleted.
     * All returned strings are sharing the same memory space. */
    IXMLStr encode(unsigned char *inByteBuf, unsigned int inByteLen, char formatted=0); ///< returns a pointer to an internal buffer containing the base64 string containing the binary data encoded from "inByteBuf"

    /// returns the number of bytes which will be decoded from "inString".
    static unsigned int decodeSize(IXMLCStr inString, IXMLErrorInfo *xe=NULL);

    /**
     * The "decode" function returns a pointer to a buffer containing the binary data decoded from "inString"
     * The output buffer will be free'd when the IXMLParserBase64Tool object is deleted.
     * All output buffer are sharing the same memory space.
     * @param inString If "instring" is malformed, NULL will be returned */
    unsigned char* decode(IXMLCStr inString, int *outByteLen=NULL, IXMLErrorInfo *xe=NULL); ///< returns a pointer to an internal buffer containing the binary data decoded from "inString"

    /**
     * decodes data from "inString" to "outByteBuf". You need to provide the size (in byte) of "outByteBuf"
     * in "inMaxByteOutBuflen". If "outByteBuf" is not large enough or if data is malformed, then "FALSE"
     * will be returned; otherwise "TRUE". */
    static unsigned char decode(IXMLCStr inString, unsigned char *outByteBuf, int inMaxByteOutBuflen, IXMLErrorInfo *xe=NULL); ///< deprecated.

private:
    void *buf;
    int buflen;
    void alloc(int newsize);
};
/** @} */

#undef IXMLDLLENTRY

#endif
