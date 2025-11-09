#include <XboxInternals/Fatx/XContentDeviceProfile.h>
#include <XboxInternals/Utils.h>
#include <XboxInternals/Stfs/StfsPackage.h>

XContentDeviceProfile::XContentDeviceProfile() :
    XContentDeviceItem()
{

}

XContentDeviceProfile::XContentDeviceProfile(FatxFileEntry *fileEntry, IXContentHeader *profile) :
    XContentDeviceItem(fileEntry, profile)
{
}

XContentDeviceProfile::XContentDeviceProfile(std::string pathOnDevice, std::string rawName, IXContentHeader *profile, DWORD fileSize) :
    XContentDeviceItem(pathOnDevice, rawName, profile, fileSize)
{
}

BYTE *XContentDeviceProfile::GetProfileID()
{
    if (content != NULL)
    {
        return content->metaData->profileID;
    }
    else
    {
        if (titles.size() != 0 && titles.at(0).titleSaves.size() != 0)
            return titles.at(0).titleSaves.at(0).content->metaData->profileID;
        else
            return NULL;
    }
}

std::wstring XContentDeviceProfile::GetName()
{
    // check to see if the gamertag is cached
    if (gamertag.size() == 0)
    {
        ConsoleType consoleType;
        if (content != NULL)
            consoleType = content->metaData->certificate.consoleTypeFlags ? DevKit : Retail;
        else
            return L"Unknown Profile";

        try
        {
            auto *stfsPackage = dynamic_cast<StfsPackage*>(content);
            if (stfsPackage == nullptr)
                return L"Unknown Profile";

            std::string accountTemp = Utils::GetTemporaryFileName();
            try
            {
                stfsPackage->ExtractFile("Account", accountTemp);
                Account account(accountTemp, true, consoleType);
                gamertag = account.GetGamertag();
            }
            catch (...)
            {
                Utils::DeleteLocalFile(accountTemp);
                throw;
            }

            Utils::DeleteLocalFile(accountTemp);
        }
        catch (...)
        {
            return L"Unknown Profile";
        }
    }

    return gamertag;
}
