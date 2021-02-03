#pragma once
#define CONFIG_FILE "config.ini"
#include "SQLConnection.h"
#include "IniParser.h"


class SQLSERVER_API SQLServer : public SQLConnection
{
public:

    struct PhotoType
    {
        SAString photoBytes;
        SAString photoName;
        SAString photoExtension;
    };

    SQLServer() = default;

    SQLServer(const ConnectParams& connect_string);

    ~SQLServer();
    void Connect();
    void Connect(const ConnectParams& connect_string);
    void InsertPhoto(const Photo& photo);
    /*Get all photos int the vector*/
    std::vector<PhotoType> GetAllPhotos();
    void RollBack();
    void Disconnect();
    void ClearTable(const std::string& table);
    void DeleteRecord(int id);
    bool CheckTableExists(const std::string& table);
    void GetIniParams(const std::string& path);
    void CreatePhotosTable(const std::string& table);
    /*create files from the vector of photos*/
    static void CreatePhotos(const std::vector<PhotoType>& photos);

protected:
    /*Nested class for exceptions*/
    class SQLServerException :public SQLException
    {
    public:
        SQLServerException() = default;
        void GetParams(const SAException& ex);
    };
    /*Execute query*/
    void ExecSQLQuery(const std::string& query);
    /*Get path to .exe file*/
    std::string ExePath();
    SAConnection m_connection;
    SQLServerException sql_error;
    ConnectParams params;
};

