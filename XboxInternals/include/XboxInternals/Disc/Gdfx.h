#ifndef GDFX_H
#define GDFX_H

#include <XboxInternals/TypeDefinitions.h>
#include <XboxInternals/IO/BaseIO.h>
#include <iostream>
#include <vector>

using std::string;
using std::vector;

struct GdfxHeader
{
    BYTE magic[0x14];   // MICROSOFT*XBOX*MEDIA
    DWORD rootSector;
    DWORD rootSize;
    WINFILETIME creationTime;
};

struct GdfxFileEntry
{
    // in the file
    DWORD unknown;
    DWORD sector;
    DWORD size;
    BYTE attributes;
    BYTE nameLen;
    string name;

    // extra stuff
    string filePath;
    vector<GdfxFileEntry> files;
    DWORD address;
    DWORD fileIndex;
    DWORD magic;  // file type identifier (first 4 bytes)
};

enum GdfxDirentAttributesutes
{
    GdfxReadOnly = 0x01,
    GdfxHidden = 0x02,
    GdfxSystem = 0x04,
    GdfxDirectory = 0x10,
    GdfxArchive = 0x20,
    GdfxDevice = 0x40,
    GdfxNormal = 0x80
};

// Read the GDFX header, seek io to position of header beforehand
void GdfxReadHeader(BaseIO *io, GdfxHeader *header);

// Read the next file entry in the listing, returns false on listing end
bool GdfxReadFileEntry(BaseIO *io, GdfxFileEntry *entry);

// Write a file entry back to the listing
void GdfxWriteFileEntry(BaseIO *io, GdfxFileEntry *entry);

// Order entries so directories come first
int DirectoryFirstCompareGdfxEntries(const GdfxFileEntry &a, const GdfxFileEntry &b);

#endif // GDFX_H


