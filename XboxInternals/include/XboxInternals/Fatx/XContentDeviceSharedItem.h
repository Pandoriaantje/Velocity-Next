#ifndef XCONTENTDEVICESHAREDITEM_H
#define XCONTENTDEVICESHAREDITEM_H

#include <XboxInternals/Stfs/IXContentHeader.h>
#include <XboxInternals/Fatx/XContentDeviceItem.h>
#include <XboxInternals/Export.h>

class XBOXINTERNALSSHARED_EXPORT XContentDeviceSharedItem : public XContentDeviceItem
{
public:
    XContentDeviceSharedItem();
    XContentDeviceSharedItem(FatxFileEntry *fileEntry, IXContentHeader *content, std::vector<std::string> contentFilePaths=std::vector<std::string>());
    XContentDeviceSharedItem(std::string pathOnDevice, std::string rawName, IXContentHeader *content, UINT64 fileSize, std::vector<std::string> contentFilePaths=std::vector<std::string>());
};

#endif // XCONTENTDEVICESHAREDITEM_H
