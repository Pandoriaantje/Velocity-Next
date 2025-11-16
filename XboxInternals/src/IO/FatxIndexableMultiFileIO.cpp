#include <XboxInternals/IO/FatxIndexableMultiFileIO.h>

#include <algorithm>
#include <memory>
#include <utility>

#include <XboxInternals/Fatx/FatxDrive.h>
#include <XboxInternals/IO/FatxIO.h>

FatxIndexableMultiFileIO::FatxIndexableMultiFileIO(std::string fileDirectory, FatxDrive *drive)
    : drive(drive)
{
    loadDirectories(std::move(fileDirectory));

    if (files.empty()) {
        throw std::string("MultiFileIO: Directory is empty\n");
    }

    currentIO = openFile(files.front());
}

FatxIndexableMultiFileIO::~FatxIndexableMultiFileIO()
{
    if (currentIO) {
        currentIO->Close();
    }
}

void FatxIndexableMultiFileIO::loadDirectories(std::string path)
{
    FatxFileEntry *directory = drive->GetFileEntry(path);
    drive->GetChildFileEntries(directory);

    for (size_t i = 0; i < directory->cachedFiles.size(); i++) {
        FatxFileEntry directoryFile = directory->cachedFiles.at(i);
        if (directoryFile.nameLen == FATX_ENTRY_DELETED) {
            continue;
        }

        std::string filePath = directoryFile.path + directoryFile.name;
        files.push_back(filePath);
    }

    std::sort(files.begin(), files.end());
}

std::unique_ptr<BaseIO> FatxIndexableMultiFileIO::openFile(std::string path)
{
    FatxFileEntry *entry = drive->GetFileEntry(path);
    return std::make_unique<FatxIO>(drive->GetFatxIO(entry));
}
