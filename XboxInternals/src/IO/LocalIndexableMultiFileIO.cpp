#include <XboxInternals/IO/LocalIndexableMultiFileIO.h>

#include <algorithm>
#include <memory>
#include <utility>

#include <XboxInternals/IO/FileIO.h>
#include <XboxInternals/Utils.h>

LocalIndexableMultiFileIO::LocalIndexableMultiFileIO(std::string fileDirectory)
{
    loadDirectories(std::move(fileDirectory));

    if (files.empty()) {
        throw std::string("MultiFileIO: Directory is empty\n");
    }

    currentIO = std::make_unique<FileIO>(files.front());
}

LocalIndexableMultiFileIO::~LocalIndexableMultiFileIO()
{
    if (currentIO) {
        currentIO->Close();
    }
}

void LocalIndexableMultiFileIO::loadDirectories(std::string path)
{
    files = Utils::FilesInDirectory(std::move(path));
    std::sort(files.begin(), files.end());
}

std::unique_ptr<BaseIO> LocalIndexableMultiFileIO::openFile(std::string path)
{
    return std::make_unique<FileIO>(std::move(path));
}
