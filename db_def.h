#ifndef DB_DEF_H
#define DB_DEF_H

#include <QString>
#include <QSqlDatabase>
#include <QtSql>
#include <QSqlError>
#include <QMap>
#include "qsql_sqlite.h"
#include "sqlite/sqlite3.h"



//QSqlDatabase Db = QSqlDatabase::addDatabase("QSQLITE");
//QSqlDatabase Db;
//Db.setDatabaseName("filelist.db");
//QSqlQuery query;


typedef struct
{
        //int index;
        char name[64];
        char path[128];
} ColumnDef_t;

class record : public QObject
{
        Q_OBJECT
public:
    record();
    ~record();
    bool connect_db();
    void disconnet_db();
    bool create_tbl();

    QStringList read_db();
    bool update_db(QString old_name, QString new_name);
    int add_db(QString name,QString path);
    bool remove_record(QString path);
    void clear_db();
    QString find(QString keyword);
signals:
        void kiwiLog(QString location, QString text);
        void kiwiWarning(QString location, QString text);
        void kiwiError(QString location, QString text);
private:
        ColumnDef_t columns;
        sqlite3 *db;
        QString filelist;       //dabase name:Filelist
};

int exec_SQL(sqlite3 *db, QString sql);

QMap<QString, ColumnDef_t> QueryTableDefinition_SQLite(sqlite3 *db, QString tabName, int *pRet);

//sqlite3 *db=NULL;

/*
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  int i;
  for(i=0; i<argc; i++)
  {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}
*/
/*
//QString result;
QString createsql = "CREATE TABLE Filelist(Name VARCHAR(32),  \
                                    Path VARCHAR(10));";
QString querysql = "SELECT * FROM Filelist;";

QString deletesql ="DELETE FROM Filelist WHERE {};";
QString clearsql = "DELETE * FROM Filelist;";
*/

/*
char* createsql = "CREATE TABLE Filelist(Name VARCHAR(32),  \
                                    Path VARCHAR(10));";

char* querysql = "SELECT * FROM Filelist;";

char* deletesql ="DELETE FROM Filelist WHERE {};";
char* clearsql = "TRUNCATE TABLE Filelist;";
*/

// This is the callback function to display the select data in the table

/*
char* insertsql = "INSERT INTO Filelist VALUES(NULL, 'Fred', '09990123456');";
*/

#define KiwiSQLitePrepareError 1
#define KiwiSQLiteStatementNULL 2
#define KiwiSQLiteBindFail 3
#define KiwiSQLiteFetchError 4
#define KiwiSQLiteExecuteFail 5
#define KiwiSQLiteResultNotReady 6

#endif // DB_DEF_H
