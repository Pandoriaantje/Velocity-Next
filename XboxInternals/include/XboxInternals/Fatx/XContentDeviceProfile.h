#ifndef XCONTENTDEVICEPROFILE_H
#define XCONTENTDEVICEPROFILE_H

#include <iostream>
#include <memory>
#include <vector>

#include <XboxInternals/Stfs/StfsPackage.h>
#include <XboxInternals/IO/StfsIO.h>
#include <XboxInternals/Account/Account.h>
#include <XboxInternals/Fatx/XContentDeviceItem.h>
#include <XboxInternals/Fatx/XContentDeviceTitle.h>
#include <XboxInternals/Export.h>

class XBOXINTERNALSSHARED_EXPORT XContentDeviceProfile : public XContentDeviceItem
{
public:
    XContentDeviceProfile();
    XContentDeviceProfile(FatxFileEntry *fileEntry, std::shared_ptr<IXContentHeader> profile);
    XContentDeviceProfile(std::string pathOnDevice, std::string rawName, std::shared_ptr<IXContentHeader> profile, DWORD fileSize = 0);

    std::vector<XContentDeviceTitle> titles;

    BYTE* GetProfileID();

    virtual std::wstring GetName();

private:
    std::wstring gamertag;
};

#endif // XCONTENTDEVICEPROFILE_H
