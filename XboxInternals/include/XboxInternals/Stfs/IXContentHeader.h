#pragma once

#include <string>

#include <XboxInternals/Export.h>
#include <XboxInternals/Stfs/XContentHeader.h>

// Lightweight mixin for STFS/SVOD containers that exposes common metadata helpers.
class XBOXINTERNALS_EXPORT IXContentHeader
{
public:
    IXContentHeader();
    virtual ~IXContentHeader();

    // Inspect a package on disk and return which filesystem it uses.
    static FileSystem GetFileSystem(const std::string &filePath);

    // Convenience accessor for the canonical FATX mount path of this package.
    std::string GetFatxFilePath();

    // Primary metadata pointer (used in new code)
    XContentHeader *metaData = nullptr;

    // Backwards-compatible alias used by older callers (`svod->metadata`)
    // This is a reference to metaData so updates are reflected.
    XContentHeader *&metadata;
};
