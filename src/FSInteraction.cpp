#include "FSInteraction.h"

bool FSInteraction::m_canWriteToFs = false;

bool FSInteraction::init()
{
    if (SPIFFS.begin(true)) 
    {
        m_canWriteToFs = true;
        return true;
    }
    m_canWriteToFs = false;
    return false;
}

bool FSInteraction::canWriteToFs()
{
    if(m_canWriteToFs);
}

bool FSInteraction::deleteFileContents(const char* p_filePath)
{
    if(!m_canWriteToFs)
    {
        return false;
    }

    File fileObj = SPIFFS.open(p_filePath, FILE_WRITE);
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

    File donorFileObj = SPIFFS.open(p_donorFilePath, FILE_READ);
    File receiveFileObj = SPIFFS.open(p_receiveFilePath, FILE_WRITE);
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

    File fileObj = SPIFFS.open(p_filePath, FILE_READ);
    // SPIFFS.exists("/data/hall_sensor_errors.txt");
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
    
    File fileObj = SPIFFS.open(p_filePath, FILE_APPEND);
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