#include <XboxInternals/IO/StfsIO.h>

#include <XboxInternals/Stfs/StfsPackage.h>

StfsIO::StfsIO(BaseIO *io, StfsPackage *package, StfsFileEntry entry) :
    io(io), package(package), entry(entry), didChangeSize(false)
{
    package->GenerateBlockChain(&this->entry);
    blocksAllocated = this->entry.blocksForFile;
    SetPosition(0);
}

StfsIO::~StfsIO()
{
    Flush();
}

StfsFileEntry StfsIO::GetStfsFileEntry()
{
    return entry;
}

void StfsIO::SetPosition(UINT64 position, std::ios_base::seekdir dir)
{
    if (dir == std::ios_base::cur)
        position += this->entryPosition;
    else if (dir == std::ios_base::end)
        position = (Length() + position);

    bool resizeNecessary = (position > Length());
    if (resizeNecessary)
        Resize(position);

    this->entryPosition = position;

    DWORD blockNum = (position == Length()) ? entry.blockChain.back() : entry.blockChain.at(position / 0x1000);
    UINT64 packagePosition = this->package->BlockToAddress(blockNum) + (position % 0x1000);

    io->SetPosition(packagePosition);
}

UINT64 StfsIO::GetPosition()
{
    return this->entryPosition;
}

UINT64 StfsIO::Length()
{
    return this->entry.fileSize;
}

void StfsIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    DWORD maxRead = 0x1000 - (this->entryPosition % 0x1000);
    UINT64 endingPosition = this->entryPosition + len;

    while (len >= maxRead)
    {
        io->ReadBytes(outBuffer, maxRead);
        len -= maxRead;
        outBuffer += maxRead;
        SetPosition(maxRead, std::ios_base::cur);
    }

    if (len != 0)
        io->ReadBytes(outBuffer, len);

    SetPosition(endingPosition);
}

void StfsIO::WriteBytes(BYTE *buffer, DWORD len)
{
    DWORD endingPosition = GetPosition() + len;
    if (endingPosition > Length())
    {
        didChangeSize = true;

        if ((endingPosition + 0xFFF) / 0x1000 > (Length() + 0xFFF) / 0x1000)
        {
            DWORD amountOfBlocksToAllocate = ((endingPosition - entry.fileSize) + 0xFFF) / 0x1000;
            this->package->metaData->stfsVolumeDescriptor.allocatedBlockCount += amountOfBlocksToAllocate;
            this->package->metaData->stfsVolumeDescriptor.unallocatedBlockCount -= amountOfBlocksToAllocate;

            DWORD previousBlock = this->entry.blockChain.back();
            while (amountOfBlocksToAllocate-- > 0)
            {
                DWORD currentBlock = this->package->AllocateBlock();

                this->package->SetNextBlock(previousBlock, currentBlock);

                this->entry.blockChain.push_back(currentBlock);
                previousBlock = currentBlock;
                blocksAllocated++;
            }

            this->package->SetNextBlock(previousBlock, INT24_MAX);
            this->package->metaData->WriteVolumeDescriptor();
        }

        entry.fileSize = endingPosition;
    }

    WORD maxWrite = 0x1000 - (this->entryPosition % 0x1000);
    while (len >= maxWrite)
    {
        io->WriteBytes(buffer, maxWrite);
        len -= maxWrite;
        buffer += maxWrite;
        SetPosition(maxWrite, std::ios_base::cur);
    }

    if (len != 0)
        io->WriteBytes(buffer, len);

    SetPosition(endingPosition);
}

void StfsIO::Flush()
{
    if (didChangeSize)
    {
        this->io->SetPosition(this->entry.fileEntryAddress + 0x29);
        this->io->Write((INT24)blocksAllocated, LittleEndian);
        this->io->Write((INT24)blocksAllocated, LittleEndian);

        this->io->SetPosition(this->entry.fileEntryAddress + 0x34);
        this->io->Write((DWORD)entry.fileSize);
    }

    this->io->Flush();
}

void StfsIO::Close()
{
    Flush();
}

void StfsIO::Resize(UINT64 size)
{
    if (size == this->entry.fileSize)
        return;

    DWORD originalSize = this->entry.fileSize;
    this->entry.fileSize = (DWORD)size;
    didChangeSize = true;

    DWORD newBlockCount = (size + 0xFFF) / 0x1000;
    DWORD originalBlockCount = (originalSize + 0xFFF) / 0x1000;
    this->entry.blocksForFile = newBlockCount;

    if (originalBlockCount > newBlockCount)
    {
        DWORD amountOfBlocksToDeallocate = originalBlockCount - newBlockCount;
        for (DWORD i = 0; i < amountOfBlocksToDeallocate; i++)
        {
            DWORD currentBlock = this->entry.blockChain.back();
            this->entry.blockChain.pop_back();

            io->SetPosition(this->package->GetHashAddressOfBlock(currentBlock) + 0x15);
            io->Write((INT24)PreviouslyAllocated);
        }

        this->package->SetNextBlock(this->entry.blockChain.back(), INT24_MAX);
        this->package->metaData->stfsVolumeDescriptor.allocatedBlockCount -= amountOfBlocksToDeallocate;
        this->package->metaData->stfsVolumeDescriptor.unallocatedBlockCount += amountOfBlocksToDeallocate;
        this->package->metaData->WriteVolumeDescriptor();
    }
    else if (originalBlockCount < newBlockCount)
    {
        DWORD amountOfBlocksToAllocate = newBlockCount - originalBlockCount;
        DWORD previousBlock = this->entry.blockChain.back();
        for (DWORD i = 0; i < amountOfBlocksToAllocate; i++)
        {
            DWORD currentBlock = this->package->AllocateBlock();
            this->package->SetNextBlock(previousBlock, currentBlock);
            this->entry.blockChain.push_back(currentBlock);
            previousBlock = currentBlock;
        }

        this->package->SetNextBlock(previousBlock, INT24_MAX);
    }

    Flush();
}
