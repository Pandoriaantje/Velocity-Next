#ifndef XEXAESIO_H
#define XEXAESIO_H

#include <XboxInternals/IO/BaseIO.h>
#include <XboxInternals/IO/XexBaseIO.h>
#include <XboxInternals/Xex/XexDefinitions.h>

#include <botan_all.h>

#include <memory>

class Xex;

#define XEX_NULL_BUFFER_SIZE 	0x10000

class XexAesIO : public XexBaseIO
{
public:
    XexAesIO(BaseIO *io, Xex *xex, const BYTE *key);
    ~XexAesIO();

    void ReadBytes(BYTE *outBuffer, DWORD readLength);

    void WriteBytes(BYTE *buffer, DWORD len);

    UINT64 Length();

private:
    BYTE curIV[XEX_AES_BLOCK_SIZE];
    BYTE curDecryptedBlock[XEX_AES_BLOCK_SIZE];
    UINT64 curAesBlockAddress;
    std::unique_ptr<Botan::BlockCipher> aes;

    void AesCbcDecrypt(const BYTE *bufferEnc);
};

#endif // XEXAESIO_H
