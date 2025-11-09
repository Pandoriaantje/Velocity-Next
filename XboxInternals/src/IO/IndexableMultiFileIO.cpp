#include <XboxInternals/IO/IndexableMultiFileIO.h>

#include <algorithm>

IndexableMultiFileIO::IndexableMultiFileIO()
    : addressInFile(0), fileIndex(0), currentIO(nullptr) {}

IndexableMultiFileIO::~IndexableMultiFileIO() = default;

void IndexableMultiFileIO::SetPosition(DWORD newAddressInFile, int desiredFileIndex)
{
    if (currentIO == nullptr && files.empty()) {
        throw std::string("MultiFileIO: No files are loaded.");
    }

    if (desiredFileIndex == -1 || static_cast<DWORD>(desiredFileIndex) == fileIndex) {
        if (newAddressInFile >= CurrentFileLength()) {
            throw std::string("MultiFileIO: Cannot seek beyond the end of the file\n");
        }

        currentIO->SetPosition(newAddressInFile);
        addressInFile = newAddressInFile;
        return;
    }

    if (desiredFileIndex < 0 || static_cast<size_t>(desiredFileIndex) >= files.size()) {
        throw std::string("MultiFileIO: Specified file index is out of range\n");
    }

    if (currentIO != nullptr) {
        currentIO->Close();
        delete currentIO;
        currentIO = nullptr;
    }

    currentIO = openFile(files.at(static_cast<size_t>(desiredFileIndex)));
    if (newAddressInFile >= CurrentFileLength()) {
        throw std::string("MultiFileIO: Cannot seek beyond the end of the file\n");
    }

    addressInFile = newAddressInFile;
    fileIndex = static_cast<DWORD>(desiredFileIndex);
    currentIO->SetPosition(addressInFile);
}

void IndexableMultiFileIO::GetPosition(DWORD *addressOut, DWORD *fileIndexOut)
{
    if (addressOut) {
        *addressOut = addressInFile;
    }
    if (fileIndexOut) {
        *fileIndexOut = fileIndex;
    }
}

DWORD IndexableMultiFileIO::CurrentFileLength()
{
    if (currentIO == nullptr) {
        throw std::string("MultiFileIO: No current file is open\n");
    }

    currentIO->SetPosition(0, std::ios_base::end);
    DWORD fileLen = static_cast<DWORD>(currentIO->GetPosition());
    currentIO->SetPosition(addressInFile);
    return fileLen;
}

void IndexableMultiFileIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    while (len > 0) {
        DWORD bytesLeft = CurrentFileLength() - addressInFile;
        DWORD amountToRead = (bytesLeft > len) ? len : bytesLeft;

        currentIO->ReadBytes(outBuffer, amountToRead);

        if (len >= bytesLeft && (fileIndex + 1) < FileCount()) {
            SetPosition(0, static_cast<int>(fileIndex + 1));
        } else if (len < bytesLeft) {
            SetPosition(addressInFile + len);
        }

        len -= amountToRead;
        outBuffer += amountToRead;
    }
}

void IndexableMultiFileIO::WriteBytes(BYTE *buffer, DWORD len)
{
    while (len > 0) {
        DWORD bytesLeft = CurrentFileLength() - addressInFile;
        DWORD amountToWrite = (bytesLeft > len) ? len : bytesLeft;

        currentIO->Write(buffer, amountToWrite);

        if (len >= bytesLeft) {
            SetPosition(0, static_cast<int>(fileIndex + 1));
        } else {
            SetPosition(addressInFile + len);
        }

        len -= amountToWrite;
        buffer += amountToWrite;
    }
}

void IndexableMultiFileIO::Close()
{
    if (currentIO) {
        currentIO->Close();
    }
}

void IndexableMultiFileIO::Flush()
{
    if (currentIO) {
        currentIO->Flush();
    }
}

UINT64 IndexableMultiFileIO::Length()
{
    if (currentIO == nullptr) {
        return 0;
    }
    return currentIO->Length();
}

DWORD IndexableMultiFileIO::FileCount()
{
    return static_cast<DWORD>(files.size());
}

void IndexableMultiFileIO::SetPosition(UINT64, std::ios_base::seekdir)
{
    throw std::string("MultiFileIO: Unused function has been called.\n");
}

UINT64 IndexableMultiFileIO::GetPosition()
{
    throw std::string("MultiFileIO: Unused function has been called.\n");
}
