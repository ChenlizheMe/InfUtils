#pragma once

#include <stdio.h>

namespace infutils
{
struct InfAppMetadata
{
    const char *appName;
    int versionMajor;
    int versionMinor;
    int versionPatch;
    const char *appID;

    InfAppMetadata()
    {
        appName = nullptr;
        versionMajor = 0;
        versionMinor = 0;
        versionPatch = 0;
        appID = nullptr;
    }
    InfAppMetadata(const char *appName, int versionMajor, int versionMinor,
                   int versionPatch, const char *appID)
        : appName(appName), versionMajor(versionMajor),
          versionMinor(versionMinor), versionPatch(versionPatch), appID(appID)
    {
    }

    char *GetVersionString()
    {
        static char versionString[32];
        snprintf(versionString, sizeof(versionString), "%d.%d.%d", versionMajor,
                 versionMinor, versionPatch);
        return versionString;
    }

    char *GetInfoString()
    {
        static char infoString[256];
        snprintf(infoString, sizeof(infoString), "%s %d.%d.%d (%s)", appName,
                 versionMajor, versionMinor, versionPatch, appID);
        return infoString;
    }
};
} // namespace infutils
