#pragma once

#include <string>
#include <XboxInternals/IO/BaseIO.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#else
    #include <fstream>
#endif

// Cross-platform large file I/O
// Uses native Windows API on Windows, falls back to fstream on other platforms
class BigFileIO : public BaseIO
{
public:
    BigFileIO(std::string filePath, bool create = false);
    ~BigFileIO();

    void ReadBytes(BYTE *outBuffer, DWORD len) override;
    void WriteBytes(BYTE *buffer, DWORD len) override;
    UINT64 Length() override;
    UINT64 GetPosition() override;
    void SetPosition(UINT64 position, std::ios_base::seekdir dir = std::ios_base::beg) override;
    void Close() override;
    void Flush() override;

private:
    std::string filePath;

#ifdef _WIN32
    HANDLE hFile;
#else
    std::fstream* fstr;
    UINT64 length;
#endif
};
