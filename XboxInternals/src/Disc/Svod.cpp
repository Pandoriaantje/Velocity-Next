#include <XboxInternals/Disc/Svod.h>

#include <algorithm>
#include <cstring>
#include <utility>

#include <XboxInternals/Utils.h>
#include <XboxInternals/Fatx/FatxDrive.h>
#include <filesystem>
#include <XboxInternals/IO/LocalIndexableMultiFileIO.h>
#include <XboxInternals/IO/FatxIndexableMultiFileIO.h>
#include <XboxInternals/IO/FatxIO.h>
#include <XboxInternals/IO/FileIO.h>

std::vector<std::string> SVOD::GetDataFilePaths(const std::string &rootDescriptorPath)
{
    SVOD svod(rootDescriptorPath);

    std::string dataFileDirectoryPath = rootDescriptorPath + ".data/";
    // use std::filesystem for cross-platform existence checks
    std::filesystem::path p(Utils::NormalizeFilePath(dataFileDirectoryPath));
    bool dataFileDirectoryExists = std::filesystem::exists(p) && std::filesystem::is_directory(p);

    std::vector<std::string> dataFiles;
    if (dataFileDirectoryExists)
    {
        dataFiles = Utils::FilesInDirectory(dataFileDirectoryPath);
        std::sort(dataFiles.begin(), dataFiles.end());
    }

    return dataFiles;
}

SVOD::SVOD(string rootPath, FatxDrive *drive, bool readFileListing)
    : io(nullptr), rootFile(nullptr), baseAddress(0), offset(0), drive(drive), didReadFileListing(false)
{
    rootPath = Utils::NormalizeFilePath(rootPath, '\\', '/');

    string fileName = rootPath.substr(rootPath.find_last_of("/") + 1);
    contentDirectory = rootPath.substr(0, rootPath.find_last_of("/")) + "/" + fileName + ".data/";

    if (drive == nullptr)
    {
        rootFile = new FileIO(Utils::NormalizeFilePath(rootPath));
    }
    else
    {
        std::string fatxPath = Utils::NormalizeFilePath(rootPath);
        FatxFileEntry *rootFileEntry = drive->GetFileEntry(fatxPath);
        rootFile = new FatxIO(drive->GetFatxIO(rootFileEntry));
    }

    metaData = new XContentHeader(rootFile);

    baseAddress = (metaData->svodVolumeDescriptor.flags & EnhancedGDFLayout) ? 0x2000 : 0x12000;
    offset = (metaData->svodVolumeDescriptor.flags & EnhancedGDFLayout) ? 0x2000 : 0x1000;

    if (metaData->fileSystem != FileSystemSVOD)
        throw string("SVOD: Invalid file system header.\n");

    switch (metaData->contentType)
    {
        case GameOnDemand:
        case InstalledGame:
        case XboxOriginalGame:
            break;
        default:
            throw string("SVOD: Unrecognized content type.\n");
    }

    if (drive == nullptr)
    {
        std::string normalizedDirectory = Utils::NormalizeFilePath(contentDirectory);
        io = new LocalIndexableMultiFileIO(std::move(normalizedDirectory));
    }
    else
    {
        std::string fatxDataDirectory = Utils::NormalizeFilePath(contentDirectory);
        io = new FatxIndexableMultiFileIO(fatxDataDirectory, drive);
    }

    io->SetPosition(baseAddress, 0);
    GdfxReadHeader(io, &header);

    if (readFileListing)
        GetFileListing();
}

SVOD::~SVOD()
{
    if (io)
    {
        io->Close();
        delete io;
        io = nullptr;
    }

    if (rootFile)
    {
        rootFile->Close();
        delete rootFile;
        rootFile = nullptr;
    }

    delete metaData;
    metaData = nullptr;
}

void SVOD::Resign(string kvPath)
{
    if (metaData->magic != CON)
        throw string("SVOD: Can only resign console systems.\n");
    metaData->ResignHeader(kvPath);
}

void SVOD::SectorToAddress(DWORD sector, DWORD *addressInDataFile, DWORD *dataFileIndex)
{
    DWORD trueSector = (sector - (metaData->svodVolumeDescriptor.dataBlockOffset * 2)) % 0x14388;
    *addressInDataFile = trueSector * 0x800;
    *dataFileIndex = (sector - (metaData->svodVolumeDescriptor.dataBlockOffset * 2)) / 0x14388;

    *addressInDataFile += offset;
    *addressInDataFile += ((trueSector / 0x198) + ((trueSector % 0x198 == 0 && trueSector != 0) ? 0 : 1)) * 0x1000;
}

void SVOD::ReadFileListing(vector<GdfxFileEntry> *entryList, DWORD sector, int size, string path)
{
    DWORD eAddr, eIndex;
    SectorToAddress(sector, &eAddr, &eIndex);
    io->SetPosition(eAddr, static_cast<int>(eIndex));

    GdfxFileEntry current{};

    while (true)
    {
        io->GetPosition(&current.address, &current.fileIndex);

        if (!GdfxReadFileEntry(io, &current) && size != 0)
            break;

        if (current.attributes & GdfxDirectory)
        {
            DWORD seekAddr, seekIndex;
            io->GetPosition(&seekAddr, &seekIndex);

            ReadFileListing(&current.files, current.sector, current.size, path + current.name + "/");

            io->SetPosition(seekAddr, static_cast<int>(seekIndex));
        }

        current.filePath = path;
        entryList->push_back(current);

        eAddr += (current.nameLen + 0x11) & 0xFFFFFFFC;
        io->SetPosition(eAddr);

        if (io->ReadDword() == 0xFFFFFFFF)
        {
            if ((size - 0x800) <= 0)
            {
                std::sort(entryList->begin(), entryList->end(), DirectoryFirstCompareGdfxEntries);
                return;
            }
            else
            {
                size -= 0x800;
                SectorToAddress(++sector, &eAddr, &eIndex);
                io->SetPosition(eAddr, static_cast<int>(eIndex));
                io->GetPosition(&eAddr, &eIndex);
            }
        }

        io->SetPosition(eAddr);
        current.files.clear();
    }

    std::sort(entryList->begin(), entryList->end(), DirectoryFirstCompareGdfxEntries);
}

GdfxFileEntry SVOD::GetFileEntry(string path, vector<GdfxFileEntry> *listing)
{
    string entryName = path.substr(1, path.substr(1).find_first_of('/'));

    size_t i = 0;
    for (; i < listing->size(); i++)
    {
        if (listing->at(i).name == entryName)
        {
            if (path.substr(1).find_first_of('/') == string::npos)
                return listing->at(i);
            else
                break;
        }
    }

    return GetFileEntry(path.substr(entryName.length() + 1), &listing->at(i).files);
}

SvodIO SVOD::GetSvodIO(string path)
{
    return GetSvodIO(GetFileEntry(path, &root));
}

SvodIO SVOD::GetSvodIO(GdfxFileEntry entry)
{
    return SvodIO(metaData, std::move(entry), io);
}

void SVOD::Rehash(void (*progress)(DWORD, DWORD, void*), void *arg)
{
    DWORD fileCount = io->FileCount();
    BYTE master[0x1000] = {0};
    BYTE level0[0x1000] = {0};
    BYTE currentBlock[0x1000];
    BYTE prevHash[0x14] = {0};

    for (DWORD i = fileCount; i--;)
    {
        io->SetPosition(0x2000, static_cast<int>(i));
        DWORD hashTableCount = ((io->CurrentFileLength() - 0x2000) + 0xCCFFF) / 0xCD000;
        DWORD totalBlockCount = (io->CurrentFileLength() - 0x1000 - (hashTableCount * 0x1000)) >> 0xC;

        for (DWORD x = 0; x < hashTableCount; x++)
        {
            io->SetPosition(0x2000 + x * 0xCD000, static_cast<int>(i));

            DWORD blockCount = (totalBlockCount >= 0xCC) ? 0xCC : totalBlockCount % 0xCC;
            totalBlockCount -= 0xCC;

            for (DWORD y = 0; y < blockCount; y++)
            {
                io->ReadBytes(currentBlock, 0x1000);
                HashBlock(currentBlock, level0 + y * 0x14);
            }

            io->SetPosition(0x1000 + x * 0xCD000, static_cast<int>(i));
            io->WriteBytes(level0, 0x1000);

            HashBlock(level0, master + x * 0x14);
            memset(level0, 0, 0x1000);
        }

        memcpy(master + hashTableCount * 0x14, prevHash, 0x14);

        io->SetPosition(0, static_cast<int>(i));
        io->WriteBytes(master, 0x1000);

        HashBlock(master, prevHash);
        memset(master, 0, 0x1000);

        if (progress)
            progress(fileCount - i, fileCount, arg);
    }

    memcpy(metaData->svodVolumeDescriptor.rootHash, prevHash, 0x14);
    metaData->WriteVolumeDescriptor();

    DWORD dataLen = ((metaData->headerSize + 0xFFF) & 0xFFFFF000) - 0x344;
    BYTE *buff = new BYTE[dataLen];

    rootFile->SetPosition(0x344);
    rootFile->ReadBytes(buff, dataLen);

    const auto sha1 = Botan::HashFunction::create_or_throw("SHA-1");
    sha1->update(buff, dataLen);
    sha1->final(metaData->headerHash);

    delete [] buff;

    metaData->WriteMetaData();
}

void SVOD::HashBlock(BYTE *block, BYTE *outHash)
{
    const auto sha1 = Botan::HashFunction::create_or_throw("SHA-1");
    sha1->update(block, 0x1000);
    sha1->final(outHash);
}

void SVOD::WriteFileEntry(GdfxFileEntry *entry)
{
    io->SetPosition(entry->address, static_cast<int>(entry->fileIndex));
    GdfxWriteFileEntry(io, entry);
}

DWORD SVOD::GetSectorCount()
{
    io->SetPosition(0, static_cast<int>(io->FileCount() - 1));
    DWORD fileLen = io->CurrentFileLength() - 0x2000;

    return (io->FileCount() * 0x14388) + ((fileLen - (0x1000 * (fileLen / 0xCD000))) / 0x800);
}

std::string SVOD::GetContentName()
{
    std::string headerHashBeginning = Utils::ConvertToHexString(metaData->headerHash, 16);
    std::string firstByteOfTitleID = Utils::ConvertToHexString((metaData->titleID >> 24) & 0xFF);

    return headerHashBeginning + firstByteOfTitleID;
}

void SVOD::GetFileListing()
{
    if (!didReadFileListing)
    {
        didReadFileListing = true;
        ReadFileListing(&root, header.rootSector, header.rootSize, "/");
    }
}
