# -------------------------------------------------
# Project created by QtCreator 2010-08-24T15:09:10
# -------------------------------------------------
CONFIG += no_lflags_merge

TARGET = Media_Center
TEMPLATE = app
SOURCES += main.cpp \
    db.cpp      \
    mainwindow.cpp
#    qsql_sqlite.cpp
HEADERS += mainwindow.h \
    db_def.h    \
    sqlite/sqlite3.h
#    qsql_sqlite.h
FORMS += mainwindow.ui
RESOURCES += res.qrc

LIBS += -Lsqlite
LIBS += -lsqlite3
LIBS += -Llib

#LIBS += -Lsqldrivers
QT += sql
