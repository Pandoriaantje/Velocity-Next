#pragma once

#include <string>

#include <XboxInternals/Export.h>
#include <XboxInternals/IO/IndexableMultiFileIO.h>

class FatxDrive;

// FATX-aware implementation that exposes device segments as an indexed stream.
class XBOXINTERNALS_EXPORT FatxIndexableMultiFileIO : public IndexableMultiFileIO {
public:
    FatxIndexableMultiFileIO(std::string fileDirectory, FatxDrive *drive);
    ~FatxIndexableMultiFileIO() override;

protected:
    void loadDirectories(std::string path) override;
    std::unique_ptr<BaseIO> openFile(std::string path) override;

private:
    FatxDrive *drive;
};
