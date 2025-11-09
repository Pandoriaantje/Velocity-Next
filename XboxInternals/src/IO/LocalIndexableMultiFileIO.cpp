#include <XboxInternals/IO/LocalIndexableMultiFileIO.h>

#include <algorithm>
#include <utility>

#include <XboxInternals/IO/FileIO.h>
#include <XboxInternals/Utils.h>

LocalIndexableMultiFileIO::LocalIndexableMultiFileIO(std::string fileDirectory)
{
    loadDirectories(std::move(fileDirectory));

    if (files.empty()) {
        throw std::string("MultiFileIO: Directory is empty\n");
    }

    currentIO = new FileIO(files.front());
}

LocalIndexableMultiFileIO::~LocalIndexableMultiFileIO()
{
    if (currentIO) {
        currentIO->Close();
        delete currentIO;
        currentIO = nullptr;
    }
}

void LocalIndexableMultiFileIO::loadDirectories(std::string path)
{
    files = Utils::FilesInDirectory(std::move(path));
    std::sort(files.begin(), files.end());
}

BaseIO* LocalIndexableMultiFileIO::openFile(std::string path)
{
    return new FileIO(std::move(path));
}
