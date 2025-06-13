#include "FSInteraction.h"

bool FSInteraction::m_canWriteToFs = false;

bool FSInteraction::init()
{
    if(!SD.begin()) 
    {
        Serial.println("Card Mount Failed");
        m_canWriteToFs = false;
        return false;
    }

    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return false;
    }

    m_canWriteToFs = true;
    return true;
}

bool FSInteraction::canWriteToFs()
{
    return m_canWriteToFs;
}

bool FSInteraction::deleteFileContents(const char* p_filePath)
{
    if(!m_canWriteToFs)
    {
        return false;
    }

    File fileObj = SD.open(p_filePath, FILE_WRITE);
    if(fileObj)
    {
        fileObj.close();
        return true;
    }
    return false;
}

bool FSInteraction::copyFileContents(const char* p_donorFilePath, const char* p_receiveFilePath)
{
    if(!m_canWriteToFs)
    {
        return false;
    }

    File donorFileObj = SD.open(p_donorFilePath, FILE_READ);
    File receiveFileObj = SD.open(p_receiveFilePath, FILE_WRITE);
    if(donorFileObj && receiveFileObj)
    {
        int readChar = donorFileObj.read();
        while(readChar != -1)
        {
            receiveFileObj.write((uint8_t)readChar);
            readChar = donorFileObj.read();
        }
        receiveFileObj.close();
        donorFileObj.close();
        return true;
    }
    return false;
}

bool FSInteraction::printFileContents(const char* p_filePath)
{
    if(!m_canWriteToFs)
    {
        return false;
    }

    File fileObj = SD.open(p_filePath, FILE_READ);
    if(fileObj)
    {
        int readChar = fileObj.read();
        while(readChar != -1)
        {
            Serial.print((char) readChar);
            readChar = fileObj.read();
        }
        fileObj.flush();
        Serial.flush();
        fileObj.close();
        return true;
    }
    return false;
}

bool FSInteraction::appendStringToFile(const char* p_filePath, char *p_string)
{
    if(!m_canWriteToFs)
    {
        return false;
    }
    
    File fileObj = SD.open(p_filePath, FILE_APPEND);
    if(fileObj)
    {
        if (fileObj.print(p_string)) 
        {
            fileObj.flush();
            fileObj.close();
            return true;
        }
    }
    return false;
}

size_t FSInteraction::getFileSize(const char* p_filePath)
{
    if(!m_canWriteToFs)
    {
        return 0;
    }

    File fileObj = SD.open(p_filePath, FILE_READ);
    if(fileObj)
    {
        return fileObj.size();
    }
    return 0;
}


int FSInteraction::getLatestVersion(const char * p_dirname, const char *p_fileName)
{
    if(!m_canWriteToFs)
    {
        return -1;
    }

    File root = SD.open(p_dirname);
    if(!root)
    {
        Serial.println("Failed to open directory");
        return -1;
    }
    if(!root.isDirectory())
    {
        Serial.println("Not a directory");
        return -1;
    }

    File file = root.openNextFile();
    
    int maxFileIdx = 0;
    char fileIdxBuffer[MAX_SIZE_OF_FILE_NAME];

    // the file name looks like this:
    // fileName.txt
    // and the file with the index in front:
    // 1_fileName.txt
    while(file)
    {
        Serial.print("found a file: ");
        Serial.println(file.name());

        if(!file.isDirectory())
        {
            char *checkFilePtr;
            const char *currentFilePtr = file.name();
            checkFilePtr = strstr(currentFilePtr, p_fileName);
            
            if(checkFilePtr != NULL)
            {
                Serial.println("filename is within file");
                int idx = 0;
                while(IDX_FILE_SEPARATOR_CHAR != currentFilePtr[idx] && p_fileName[0] != currentFilePtr[idx])
                {   
                    fileIdxBuffer[idx] = currentFilePtr[idx];
                    idx ++;
                }
                fileIdxBuffer[idx] = '\0';
                int fileIdx = atoi(fileIdxBuffer);
                
                if(fileIdx > maxFileIdx)
                {
                    maxFileIdx = fileIdx;
                }
            }
        }
        file = root.openNextFile();
    }
    return maxFileIdx;
}

// SPIFFS.exists("/data/hall_sensor_errors.txt");