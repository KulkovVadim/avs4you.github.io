
#ifndef VERSION_H
#define VERSION_H

#define VER_STRINGIFY(d)            #d
#define TO_STR(v)                   VER_STRINGIFY(v)

#ifdef VER_PRODUCT_VERSION
# define VER_FILEVERSION            VER_PRODUCT_VERSION_COMMAS
# define VER_FILEVERSION_STR        TO_STR(VER_PRODUCT_VERSION)

# define VER_PRODUCTVERSION         VER_FILEVERSION
# define VER_PRODUCTVERSION_STR     TO_STR(VER_PRODUCT_VERSION)
#else
# define VER_STR_LONG(mj,mn,b,r)    VER_STRINGIFY(mj) "." VER_STRINGIFY(mn) "." VER_STRINGIFY(b) "." VER_STRINGIFY(r) "\0"
# define VER_STR_SHORT(mj,mn)       VER_STRINGIFY(mj) "." VER_STRINGIFY(mn) "\0"

# define VER_NUM_MAJOR              99
# define VER_NUM_MINOR              99
# define VER_NUM_BUILD              99
# define VER_NUM_REVISION           99
# define VER_NUMBER                 VER_NUM_MAJOR,VER_NUM_MINOR,VER_NUM_BUILD,VER_NUM_REVISION
# define VER_STRING                 VER_STR_LONG(VER_NUM_MAJOR,VER_NUM_MINOR,VER_NUM_BUILD,VER_NUM_REVISION)
# define VER_STRING_SHORT           VER_STR_SHORT(VER_NUM_MAJOR,VER_NUM_MINOR)

# define VER_FILEVERSION            VER_NUMBER
# define VER_FILEVERSION_STR        VER_STRING

# define VER_PRODUCTVERSION         VER_FILEVERSION
# define VER_PRODUCTVERSION_STR     VER_STRING_SHORT
#endif

#ifndef COPYRIGHT_YEAR
# define COPYRIGHT_YEAR              2026
#endif

#define VER_COMPANYNAME_STR         "Ascensio System SIA\0"
#define VER_LEGALCOPYRIGHT_STR      "© Ascensio System SIA " TO_STR(COPYRIGHT_YEAR) ". All rights reserved.\0"
#define VER_COMPANYDOMAIN_STR       "www.avs4you.com\0"
#define ABOUT_COPYRIGHT_STR         VER_LEGALCOPYRIGHT_STR
#define VER_FILEDESCRIPTION_STR     "Heygen plugin\0"
#define VER_INTERNALNAME_STR        "Heygen.dll\0"
#define VER_LEGALTRADEMARKS1_STR    "All Rights Reserved\0"
#define VER_LEGALTRADEMARKS2_STR    VER_LEGALTRADEMARKS1_STR
#define VER_ORIGINALFILENAME_STR    "Heygen.dll\0"
#define VER_PRODUCTNAME_STR         "Heygen plugin\0"

#define VER_LANG_AND_CHARSET_STR    "040904E4"
#define VER_LANG_ID                 0x0409
#define VER_CHARSET_ID              1252

#endif // VERSION_H

