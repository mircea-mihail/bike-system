#ifndef F_S_INTERACTION_H
#define F_S_INTERACTION_H

// file system
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "SPIFFS.h"
#include "SD.h"

#include "generalUtility.h"

#define IDX_FILE_SEPARATOR_CHAR '_'

class FSInteraction
{
private:
    static bool m_canWriteToFs;
    
public:
    /// @brief initialises the object and enables file writing
    ///     nothing from this class works if init failed or was not called
    /// @return if from now on is able to write to fs or not
    static bool init();

    /// @brief lets the user if file writing is possible
    /// @return gets the value of m_canWriteToFs 
    static bool canWriteToFs();

    /// @brief deletes the contents of a file, excluding the file itself
    /// @param p_filePath path of the file to delete contents of 
    /// @return if operation was succesful (bool)
    static bool deleteFileContents(const char* p_filePath);

    /// @brief copies the contents of a file to another file
    /// @param p_donorFilePath the path of the file to get content form
    /// @param p_receiveFilePath the path of the file to write to
    /// @return if operation was succesful (bool)
    static bool copyFileContents(const char* p_donorFilePath, const char* p_receiveFilePath);

    /// @brief prints the contents of a file to the serial 
    /// @param p_filePath the path of the file to read from
    /// @return if operation was succesful (bool)
    static bool printFileContents(const char* p_filePath);
    
    /// @brief appends a string to a given file
    /// @param p_filePath the file to append to 
    /// @param p_string string to append
    /// @return if operation was succesful (bool)
    static bool appendStringToFile(const char* p_filePath, std::string p_string);

    static size_t getFileSize(const char* p_filePath);

    static int getLatestVersion(const char * p_dirname, const char *  p_fileName);
};

#endif