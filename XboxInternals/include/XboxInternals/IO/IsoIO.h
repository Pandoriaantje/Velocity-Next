#pragma once

#include <string>
#include <XboxInternals/IO/BaseIO.h>
#include <XboxInternals/Disc/Gdfx.h>
#include <XboxInternals/Export.h>

// Forward declaration
namespace XboxInternals::Iso {
    class IsoImage;
}

class XBOXINTERNALS_EXPORT IsoIO : public BaseIO
{
public:
    IsoIO(BaseIO *isoIO, GdfxFileEntry *entry, XboxInternals::Iso::IsoImage *iso);
    IsoIO(BaseIO *isoIO, std::string isoFilePath, XboxInternals::Iso::IsoImage *iso);

    void ReadBytes(BYTE *outBuffer, DWORD len) override;
    void WriteBytes(BYTE *buffer, DWORD len) override;

    UINT64 GetPosition() override;
    void SetPosition(UINT64 position, std::ios_base::seekdir dir = std::ios_base::beg) override;

    UINT64 Length() override;

    void Flush() override;
    void Close() override;

private:
    BaseIO *isoIO;
    GdfxFileEntry *entry;
    XboxInternals::Iso::IsoImage *iso;
    UINT64 virtualPosition;
};
