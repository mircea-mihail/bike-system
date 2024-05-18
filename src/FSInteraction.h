#ifndef F_S_INTERACTION_H
#define F_S_INTERACTION_H

// file system
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "SPIFFS.h"

class FSInteraction
{
private:
    static bool m_canWriteToFs;
    
public:
    static bool init();

    static bool canWriteToFs();

    static bool deleteFileContents(const char* p_filePath);

    static bool copyFileContents(const char* p_donorFilePath, const char* p_receiveFilePath);

    static bool printFileContents(const char* p_filePath);

    static bool appendStringToFile(const char* p_filePath, char *p_string);

};

#endif