#pragma once

// File name: "EnumToString.h"
#undef DECL_ENUM_ELEMENT
#undef BEGIN_ENUM
#undef END_ENUM

#ifndef GENERATE_ENUM_STRINGS
#define DEFINE_ENUM( element,id ) element=id
#define BEGIN_ENUM( ENUM_NAME ) typedef enum tag##ENUM_NAME
#define END_ENUM( ENUM_NAME ) ENUM_NAME; \
            char* GetString##ENUM_NAME(enum tag##ENUM_NAME index);
#else
#define DEFINE_ENUM( element,id ) {id, #element}
#define BEGIN_ENUM( ENUM_NAME ) std::unordered_map<int, char*> gs_##ENUM_NAME =
#define END_ENUM( ENUM_NAME ) ; char* GetString##ENUM_NAME(enum \
            tag##ENUM_NAME index){ return gs_##ENUM_NAME [index]; }
#endif

/*#ifndef GENERATE_ENUM_STRINGS
#define DEFINE_ENUM( element,id ) element=id
#define BEGIN_ENUM( ENUM_NAME ) typedef enum tag##ENUM_NAME
#define END_ENUM( ENUM_NAME ) ENUM_NAME; \
            char* GetString##ENUM_NAME(enum tag##ENUM_NAME index);
#else
#define DEFINE_ENUM( element,id ) #element
#define BEGIN_ENUM( ENUM_NAME ) char* gs_##ENUM_NAME [] =
#define END_ENUM( ENUM_NAME ) ; char* GetString##ENUM_NAME(enum \
            tag##ENUM_NAME index){ return gs_##ENUM_NAME [index]; }
#endif*/

/*#ifndef GENERATE_ENUM_STRINGS
#define DEFINE_ENUM( element,id ) element=id,
#define BEGIN_ENUM( ENUM_NAME ) typedef enum tag##ENUM_NAME {
#define END_ENUM( ENUM_NAME ) } ENUM_NAME; \
            char* GetString##ENUM_NAME(enum tag##ENUM_NAME index);
#else
#define DEFINE_ENUM( element,id ) gs_##ENUM_NAME [id] = #element;
#define BEGIN_ENUM( ENUM_NAME ) char *gs_##ENUM_NAME[];
#define END_ENUM( ENUM_NAME ) char* GetString##ENUM_NAME(enum \
            tag##ENUM_NAME index){ return gs_##ENUM_NAME [index]; }
#endif*/