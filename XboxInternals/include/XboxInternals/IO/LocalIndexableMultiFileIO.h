#pragma once

#include <string>

#include <XboxInternals/Export.h>
#include <XboxInternals/IO/IndexableMultiFileIO.h>

// Provides indexed access over a set of local disk files.
class XBOXINTERNALS_EXPORT LocalIndexableMultiFileIO : public IndexableMultiFileIO {
public:
    explicit LocalIndexableMultiFileIO(std::string fileDirectory);
    ~LocalIndexableMultiFileIO() override;

protected:
    void loadDirectories(std::string path) override;
    BaseIO* openFile(std::string path) override;
};
