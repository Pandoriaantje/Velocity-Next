#pragma once

#include <XboxInternals/IO/BaseIO.h>
#include <XboxInternals/Stfs/StfsPackage.h>

class XBOXINTERNALS_EXPORT StfsIO : public BaseIO
{
public:
    StfsIO(BaseIO *io, StfsPackage *package, StfsFileEntry entry);
    ~StfsIO() override;

    StfsFileEntry GetStfsFileEntry();

    void SetPosition(UINT64 position, std::ios_base::seekdir dir = std::ios_base::beg) override;
    UINT64 GetPosition() override;
    UINT64 Length() override;

    void ReadBytes(BYTE *outBuffer, DWORD len) override;
    void WriteBytes(BYTE *buffer, DWORD len) override;

    void Flush() override;
    void Close() override;

    void Resize(UINT64 size);

private:
    BaseIO *io;
    StfsPackage *package;
    StfsFileEntry entry;

    UINT64 entryPosition;
    bool didChangeSize;
    DWORD blocksAllocated;
};
