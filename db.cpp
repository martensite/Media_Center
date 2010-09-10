#include "db_def.h"

/* Connect to dababase Filelist  */

record::record(){
    //QString createsql = "CREATE TABLE IF NOT EXISTS Filelist(Name text,Path text);";
    //connect_db();
    create_tbl();
}
record::~record(){
    disconnet_db();
}
bool record::connect_db(){
    int rc = sqlite3_open_v2("./filelist", &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, 0);

    //int rc = sqlite3_open("./filelist", &db);
    if (rc != SQLITE_OK){
            emit kiwiError(Q_FUNC_INFO, tr("open db fail(%1,%2)").arg(rc).arg(sqlite3_errmsg(db)));
            return false;
    }
    else{
            emit kiwiLog(Q_FUNC_INFO, tr("open db ok"));

            return true;
    }
}

/*  Disconnect the database Filelist  */
void record::disconnet_db(){
    if (!db)
            return;
    int rc = sqlite3_close(db);
    if (rc != SQLITE_OK)
            emit kiwiError(Q_FUNC_INFO, tr("close db fail(%1,%2)").arg(rc).arg(sqlite3_errmsg(db)));
    else{
            db = 0;
            emit kiwiLog(Q_FUNC_INFO, tr("close db ok"));
    }
}

QStringList record::read_db(){
    if (!connect_db())
        return QStringList();       //return empty string
    int rc=0, rc1=0;
    QStringList result;
    sqlite3_stmt *pStmt=0;
    const char *zTail;
    do{
            rc1 = sqlite3_prepare_v2(db, "SELECT * FROM Filelist;", -1, &pStmt, &zTail);
            if (rc1 != SQLITE_OK){
                    emit kiwiError(Q_FUNC_INFO, tr("sqlite3_prepare_v2 fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc = KiwiSQLitePrepareError;
                    break;
            }
            if (!pStmt){
                    emit kiwiError(Q_FUNC_INFO, tr("statement handle is null!"));
                    rc = KiwiSQLiteStatementNULL;
                    break;
            }
            do{
            rc1 = sqlite3_step(pStmt);
                if (rc1 == SQLITE_DONE){
                        //result = "(not-found)";
                        break;
                }

                if (rc1 != SQLITE_ROW){
                        emit kiwiError(Q_FUNC_INFO, tr("result not ready!"));
                        rc = KiwiSQLiteResultNotReady;
                        break;
                }
                    for(int i=0;i<2;i++){
                        char* s=(char*)sqlite3_column_text(pStmt,i);
                        result.append(QString::fromUtf8(s));
                    }
            }while(rc1 != SQLITE_DONE);

            return result;
            //char *s = (char*)sqlite3_column_text(pStmt, 0);
            /*
            if (!s){
                    emit kiwiError(Q_FUNC_INFO, tr("cacheFileDir is null!"));
                    rc = 10;
                    break;
            }
            */
            //result = QStringList(s);
    }while(0);
    if (pStmt)
            sqlite3_finalize(pStmt);
    pStmt = 0;
    return result;
/*
    int rows=0,cols=0;
    char** result,*errMsg;
    QString querysql = "SELECT * FROM Filelist;";
    sqlite3_get_table(db , querysql.toLatin1(), &result , &rows, &cols, &errMsg);
    return *result;
*/
}

bool record::create_tbl(){
    ColumnDef_t columns;
    if (!connect_db())
        return false;
    int rc=exec_SQL(db,"CREATE TABLE IF NOT EXISTS Filelist ( name text, path text )");
    if (rc != SQLITE_OK){
            emit kiwiError(Q_FUNC_INFO, tr("open db fail(%1,%2)").arg(rc).arg(sqlite3_errmsg(db)));
            return false;
    }
    else{
            emit kiwiLog(Q_FUNC_INFO, tr("open db ok"));
            return true;
    }
}
//**** Update record in the database ****
bool record::update_db(QString old_name, QString new_name){
    if (!connect_db())
         return false;
    int rc=0, rc1;
    sqlite3_stmt *pStmt=0;
    const char *zTail;

    do{
        rc1 = sqlite3_prepare_v2(db, "update Filelist set name=:new_name where name=:old_name", -1, &pStmt, &zTail);
            if (rc1 != SQLITE_OK){
                    emit kiwiError(Q_FUNC_INFO, tr("sqlite3_prepare_v2 fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc=KiwiSQLitePrepareError;
                    break;
            }
            if (!pStmt){
                    emit kiwiError(Q_FUNC_INFO, tr("statement handle is null!"));
                    rc=KiwiSQLiteStatementNULL;
                    break;
            }
            rc1 = sqlite3_bind_text(pStmt, 1, new_name.toUtf8().data(), -1, SQLITE_TRANSIENT);
            if (rc1 != SQLITE_OK){
                    emit kiwiError(Q_FUNC_INFO, tr("bind  'new_name' fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc=KiwiSQLiteBindFail;
                    break;
            }
            rc1 = sqlite3_bind_text(pStmt, 2, old_name.toUtf8().data(), -1, SQLITE_TRANSIENT);
            if (rc1 != SQLITE_OK){
                    emit kiwiError(Q_FUNC_INFO, tr("bind 'old_name' fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc=KiwiSQLiteBindFail;
                    break;
            }
            rc1 = sqlite3_step(pStmt);
            if (rc1 != SQLITE_DONE){
                    emit kiwiError(Q_FUNC_INFO, tr("sqlite3_step fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc=KiwiSQLiteExecuteFail;
                    break;
            }
        }while(0);
    if (pStmt)
            sqlite3_finalize(pStmt);
    pStmt = 0;
    if (rc == 0)
            emit kiwiLog(Q_FUNC_INFO, tr("(%1): %2 ").arg(old_name).arg(new_name));
    //return (rc == 0);
}

//**** Add a record to database****
int record::add_db(QString name,QString path){
    if (!connect_db())
            return -1;
    int rc=0, rc1;
    sqlite3_stmt *pStmt=0;
    const char *zTail;

    do{
            rc1 = sqlite3_prepare_v2(db, "insert into Filelist(name,path) values (:name, :path)", -1, &pStmt, &zTail);
            if (rc1 != SQLITE_OK){
                    emit kiwiError(Q_FUNC_INFO, tr("sqlite3_prepare_v2 fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc=KiwiSQLitePrepareError;
                    break;
            }
            if (!pStmt){
                    emit kiwiError(Q_FUNC_INFO, tr("statement handle is null!"));
                    rc=KiwiSQLiteStatementNULL;
                    break;
            }
            rc1 = sqlite3_bind_text(pStmt, 1, name.toUtf8().data(), -1, SQLITE_TRANSIENT);
            if (rc1 != SQLITE_OK){
                    emit kiwiError(Q_FUNC_INFO, tr("bind column 'name' fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc=KiwiSQLiteBindFail;
                    break;
            }
            rc1 = sqlite3_bind_text(pStmt, 2, path.toUtf8().data(), -1, SQLITE_TRANSIENT);
            if (rc1 != SQLITE_OK){
                    emit kiwiError(Q_FUNC_INFO, tr("bind column 'path' fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc=KiwiSQLiteBindFail;
                    break;
            }
            rc1 = sqlite3_step(pStmt);
            if (rc1 != SQLITE_DONE){
                    emit kiwiError(Q_FUNC_INFO, tr("sqlite3_step fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc=KiwiSQLiteExecuteFail;
                    break;
            }
        }while(0);
    if (pStmt)
            sqlite3_finalize(pStmt);
    pStmt = 0;
    if (rc == 0)
            emit kiwiLog(Q_FUNC_INFO, tr("new map(%1): %2 ").arg(name).arg(path));
    return (rc == 0);
}

bool record::remove_record(QString path){
    if (!connect_db())
            return -1;
    int rc=0, rc1;
    sqlite3_stmt *pStmt=0;
    const char *zTail;

    do{
        rc1 = sqlite3_prepare_v2(db, "delete from Filelist where path=:path", -1, &pStmt, &zTail);
            if (rc1 != SQLITE_OK){
                    emit kiwiError(Q_FUNC_INFO, tr("sqlite3_prepare_v2 fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc=KiwiSQLitePrepareError;
                    break;
            }
            if (!pStmt){
                    emit kiwiError(Q_FUNC_INFO, tr("statement handle is null!"));
                    rc=KiwiSQLiteStatementNULL;
                    break;
            }
            rc1 = sqlite3_bind_text(pStmt, 1, path.toLatin1().data(), -1, SQLITE_TRANSIENT);
            if (rc1 != SQLITE_OK){
                    emit kiwiError(Q_FUNC_INFO, tr("bind column 'name' fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc=KiwiSQLiteBindFail;
                    break;
            }

            rc1 = sqlite3_step(pStmt);
            if (rc1 != SQLITE_DONE){
                    emit kiwiError(Q_FUNC_INFO, tr("sqlite3_step fail!(%1,%2)").arg(rc1).arg(sqlite3_errmsg(db)));
                    rc=KiwiSQLiteExecuteFail;
                    break;
            }
        }while(0);
    if (pStmt)
            sqlite3_finalize(pStmt);
    pStmt = 0;
    if (rc == 0)
        emit kiwiLog(Q_FUNC_INFO, tr(" record:(%1)deleted.").arg(path));
    return (rc == 0);
}
void record::clear_db(){
    /*
    if (!connect_db())
            return;
            */
    int rc=0, rc1;
    sqlite3_stmt *pStmt=0;
    const char *zTail;
    QString clearsql="DELETE FROM Filelist";
    rc=exec_SQL(db,clearsql);
}

/*  Execute simple SQL statements   */
int exec_SQL(sqlite3 *db, QString sql)
{
        int rc = 0;
        sqlite3_stmt *pStmt = 0;
        const char *zTail;
        do{
                int rc1 = sqlite3_prepare_v2(db, sql.toLatin1().data(), -1, &pStmt, &zTail);
                if (rc1 != SQLITE_OK){
                        //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_ERROR, __FUNCTION__, 0, "sqlite3_prepare_v2 fail!(%d,%s)\n", rc1, sqlite3_errmsg(db));
                        rc = KiwiSQLitePrepareError;
                        break;
                }
                if (!pStmt){
                        //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_ERROR, __FUNCTION__, 0, "statement handle is null!\n");
                        rc = KiwiSQLiteStatementNULL;
                        break;
                }
                rc1=sqlite3_step(pStmt);
                if (rc1!=SQLITE_DONE){
                        //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_ERROR, __FUNCTION__, 0, "sqlite3_step fail!(%d,%s)\n", rc1, sqlite3_errmsg(db));
                        rc = KiwiSQLiteExecuteFail;
                        break;
                }
        }while(0);
        if (pStmt)
                sqlite3_finalize(pStmt);
        //if (rc==0)
        //	UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_INFO, __FUNCTION__, 0, "execute ok.\n");
        return rc;
}


QMap<QString, ColumnDef_t> QueryTableDefinition_SQLite(sqlite3 *db, QString tabName, int *pRet){
    QMap<QString, ColumnDef_t> colList;
    int rc = 0, rc1;
    sqlite3_stmt *pStmt = 0;
    const char *zTail;
    do{
            QString sql = "PRAGMA table_info(%1)";
            sql = sql.arg(tabName);
            rc1 = sqlite3_prepare_v2(db, sql.toLatin1().data(), -1, &pStmt, &zTail);
            if (rc1 != SQLITE_OK){
                    //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_ERROR, __FUNCTION__, 0, "sqlite3_prepare_v2 fail!(%d,%s)\n", rc1, sqlite3_errmsg(dblite));
                    rc = KiwiSQLitePrepareError;
                    break;
            }
            if (!pStmt){
                    //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_ERROR, __FUNCTION__, 0, "statement handle is null!\n");
                    rc = KiwiSQLiteStatementNULL;
                    break;
            }
            while (1){
                    rc1 = sqlite3_step(pStmt);
                    if (rc1 == SQLITE_DONE)
                            break;
                    if (rc1 != SQLITE_ROW){
                            //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_ERROR, __FUNCTION__, 0, "result not ready!\n");
                            rc = KiwiSQLiteResultNotReady;
                            break;
                    }
                    //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_DEBUG, __FUNCTION__, 0, "colCount=%d\n", colCount);
                    ColumnDef_t col;
                    memset(&col, 0, sizeof(ColumnDef_t));
                    //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_DEBUG, __FUNCTION__, 0, "col[0]=%d\n", sqlite3_column_int(pStmt, 0));
                    //col.index = sqlite3_column_int(pStmt, 0);
                    char *s = (char*)sqlite3_column_text(pStmt, 1);
                    //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_DEBUG, __FUNCTION__, 0, "col[1]=%s\n", s);
                    if (s){
                            strncpy(col.name, s, 64);
                            col.name[63] = '\0';
                    }
                    else
                            col.name[0] = '\0';
                    s=(char*)sqlite3_column_text(pStmt, 2);
                    //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_DEBUG, __FUNCTION__, 0, "col[2]=%s\n", s);
                    if (s){
                            //strncpy(col.dataType, (char*)sqlite3_column_text(pStmt, 2), 32);
                            //col.dataType[31] = '\0';
                    }
                    else
                            //col.dataType[0] = '\0';
                    //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_DEBUG, __FUNCTION__, 0, "col[3]=%d\n", sqlite3_column_int(pStmt, 3));
                    //col.notNull = sqlite3_column_int(pStmt, 3);
                    s = (char*)sqlite3_column_text(pStmt, 4);
                    //UTIL4_MessageLog(CB_CYBERHOOD_LOGFILE, MSG_LOG_DEBUG, __FUNCTION__, 0, "col[4]=%s\n", s);
                    if (s){
                            //strncpy(col.defValue, (char*)sqlite3_column_text(pStmt, 4), 32);
                            //col.defValue[31] = '\0';
                    }
                    else
                            //col.defValue[0] = '\0';
                    colList.insert(col.name, col);
            }
            if (rc > 0)
                    break;

    }while(0);
    if (pStmt)
            sqlite3_finalize(pStmt);

    if (rc > 0){
            if (pRet)
                    *pRet = rc;
    }
    else{
            if (pRet)
                    *pRet = 0;
    }
    return colList;
}



/*
QString createsql = "CREATE TABLE Filelist("
               "Name VARCHAR(32),"
               "Path VARCHAR(10));";

QString insertsql = "INSERT INTO Contact VALUES(NULL, 'Fred', '09990123456');";

QString querysql = "SELECT * FROM Contact;";

*/
