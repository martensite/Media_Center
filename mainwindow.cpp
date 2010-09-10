#include <QListWidgetItem>
#include <QListWidget>
#include <QWidget>
#include <QList>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QString>
#include <QUrl>
#include "sqlite/sqlite3.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "db_def.h"

record* abc=0;
//abc=new record;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    abc=new record;
    abc->connect_db();

    ui->setupUi(this);
    //Db= QSqlDatabase::addDatabase("QSQLITE","filelist");
    readDB();
}

MainWindow::~MainWindow()
{
    //sqlite3_free_table(result);
    //sqlite3_close(db);
    sqlite3_shutdown();
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
//**** Read records from database and process them into tablewidget
int MainWindow::readDB(){
    //abc = new record;
    QStringList result=abc->read_db();
    ui->fileList->setRowCount(result.count()/2);
    int rows=ui->fileList->rowCount();
    int cols=2;

    //set row number of the filelist
    //ui->fileList->setRowCount(query.size());
    int i=0,j=0;
    for (i=0;i<rows;i++) {
          for (j=0;j<cols;j++) {
              ui->fileList->setItem(i,j,new QTableWidgetItem(QString::QString(result[i*cols+j])));
              //printf("%s\t", result[(i+1)*cols+j]);
          }
    }
    /*
    int cols=3;
    int rows=query.size();

    int i,j;
    for (i=0;i<rows;i++) {
          for (j=0;j<cols;j++) {
              ui->fileList->setItem(i+1,j,new QTableWidgetItem(QString::QString(query.nextResult())));
              //printf("%s\t", result[(i+1)*cols+j]);
          }
      }
      */
    /*
    int row_num=ui->fileList->rowCount();
    ui->fileList->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->fileList->item(row_num,1)->setFlags(Qt::ItemIsEditable );
    */
    update_filelist();
    return 0;
}
void MainWindow::on_addButton_clicked(bool checked)
{
    // Open files (QStringList)
    QStringList fileNames=QFileDialog::getOpenFileNames(this,tr("Select the file you want to share"),\
                                                   ".",\
                                                  "Video Files (*.mpeg *.mpg *.mov *.avi  *.3gp *.ps *.flv *.wmv *.mp4 *.rm *.rmvb) ;;Audio Files ( *.mp3 *.ogg *.aac *.qt )");
    if (fileNames.isEmpty()){
        return;
    }else{
            add_records(fileNames);
    }
    //int row_num=ui->fileList->rowCount();

    //add_record(cur_file,row_num);


        //ui->fileList->item(row_num,0)->setFlags(Qt::ItemIsEditable);


    /* for single file every opening file process
    QString fileName=QFileDialog::getOpenFileName(this,tr("Select the file you want to share"),    \
                                                   "C:",\
                                                  "All Files (*.*);;\
                                                  ;;Video Files (*.mpeg *.mpg *.mov *.avi  *.3gp *.ps *.flv *.mp4 *.rm *.rmvb) \
                                                  ;;Audio Files ( *.mp3 *.ogg *.aac )");
    */


/*
    QFileDialog *fd=new QFileDialog;
    if(fd->exec()==QFileDialog::Accepted)
    {
        fd->setViewMode( QFileDialog::Detail );
        //fd->getOpenFileNames(this,)
        QString filetype=QString("AllFiles(*.*);;VideoFiles(*.mpeg *.mpg *.mov *.avi  *.3gp *.ps *.flv *.mp4 *.rm );;AudioFiles( *.mp3 *.ogg *.aac )");
        QStringList filters=filetype.split(";;");
        //fd->setFilters(filters);
        //fd->setFileMode(QFileDialog::ExistingFiles);
        fd->getOpenFileNames(this,tr("Select the file you want to share"),"C:",filters);

          QStringList fileNames=fd->selectedFiles();
          //Iterator of fileName
          //for(QStringList::Iterator it=fileNames.begin();it!=fileNames.end();it++)
          //{QWarning(QString QString::QString("miao");}

          ui->fileList->addItems(fileNames);

          //QWarning(s);
        }
    */
}

void MainWindow::on_removeButton_clicked(bool checked)
{
    //TODO: set curItem to be the path column.....(I'm stupid enough)
    QTableWidgetItem *curItem = ui->fileList->currentItem();
        if (curItem) {
                int row = ui->fileList->row(curItem);
                QTableWidgetItem *curItem_path=ui->fileList->item(row,1);
                abc->remove_record(curItem_path->text());
                ui->fileList->removeRow(row);

                // Set cursor focusing
                if (ui->fileList->currentRow() > 0)
                    ui->fileList->setCurrentCell(row-1,0);
                else
                    ui->fileList->setCurrentCell(row,0);
                        //ui->fileList->update();
        }
        update_filelist();

        if (!curItem)
                return;
}

void MainWindow::on_fileList_itemSelectionChanged()
{
    QTableWidgetItem *curItem = ui->fileList->currentItem();

        if(curItem){
            ui->selectedfileLabel->setText("You selected: " + curItem->text());
                //ui.emailLabel->setText("Email: " + curItem->data(Qt::UserRole).toString());
        } else {
                ui->selectedfileLabel->setText("<No item selected>");
                //ui.emailLabel->clear();
        }
}
/*  Double-click for rename the item... but compiler complain no QModelIndex defined
void MainWindow::on_fileList_doubleClicked(QModelIndex index)
{
    QListWidgetItem *curItem = ui->fileList->currentItem();
    if(curItem){
        int row = ui->fileList->row(curItem);
        ui->fileList->doubleClicked(row);
}
*/

void MainWindow::on_clearButton_clicked(bool checked)
{
    while (ui->fileList->rowCount() > 0)
            ui->fileList->removeRow(0);

    //Db.exec(clearsql);

    //char* clearsql = "TRUNCATE TABLE Filelist;";
    QString clearsql = "TRUNCATE TABLE Filelist;";

    //sqlite3_exec(db, clearsql.toLatin1(), callback, 0, &errMsg);
    //exec_SQL(db,clearsql);
    /*
    if(errMsg){
        QMessageBox err;
        err.setText(errMsg);
    }
    */
    //clear database
    abc->clear_db();
    update_filelist();
    ui->selectedfileLabel->setText("<No item selected>");
}

void MainWindow::on_updateButton_clicked(bool checked)
{
    //readDB();
    update_filelist();
}

void MainWindow::on_renameButton_clicked(bool checked)
{
    QTableWidgetItem *curItem = ui->fileList->item((ui->fileList->currentRow()),0);
    //QString orig_name=(curItem->text());

    if (curItem) {
            //int row = ui->fileList->row(curItem);
        bool ok;
        QString hint;
        QString old_name=curItem->text();
        if(curItem){
            hint=curItem->text();

        }
        else
            hint="<new name>";


        QString new_name=QInputDialog::getText(this, "Rename your share file",
                                                  "Please Input the new name:", QLineEdit::Normal,hint, &ok);


        if (ok && (!new_name.isEmpty())){
            ui->fileList->setItem((ui->fileList->currentRow()),0, new QTableWidgetItem(new_name));
            abc->update_db(old_name,new_name);
        }
        else{
            hint='\0';
        }

        update_filelist();
    }

}
void MainWindow::add_records(QStringList fileNames)
{
    int row_num=ui->fileList->rowCount();
    QStringList dup_list;
    QStringList add_list;
    //classfy  and list the duplicated files and added files
    for ( QStringList::iterator it = fileNames.begin(); it != fileNames.end(); ++it )
    {
        //int row_num=ui->fileList->rowCount();
        QString cur_file= *it;
        int i=0;
        if(row_num!=0){
            for(i=0;i<row_num;i++){
                if(cur_file==(ui->fileList->item(i,1)->text()))
                {
                    dup_list<<cur_file;
                }
            }
            //add_list<<cur_file;
         }
        else{
            add_list<<cur_file;
        }
    }

    if(add_list.empty()){
        QMessageBox dup_file;
        dup_file.setText("You share the duplicated item!!");
        dup_file.setDetailedText(dup_list.join("\n"));
        dup_file.exec();

        add_list.clear();
        dup_list.clear();
        update_filelist();
        return;
    }
    else        //there is  file needing to add
    {
        for ( QStringList::iterator it_add = add_list.begin(); it_add != add_list.end(); ++it_add )
        {
            int row_num=ui->fileList->rowCount();
            QString add_file= *it_add;
            // add to "path column" of filelist
            ui->fileList->setRowCount(row_num+1);
            ui->fileList->setItem(row_num,1,new QTableWidgetItem(add_file));
            QStringList file_element=add_file.split("/",QString::SkipEmptyParts);

            //Get the file name (only file name)
            QString file_name=file_element.at(file_element.count()-1);
            //Save to database
            abc->add_db(file_name,add_file);
            // add to "name column" of filelist
            ui->fileList->setItem(row_num,0,new QTableWidgetItem(file_name));
            ui->fileList->item(row_num,1)->setFlags(Qt::ItemIsEditable );
            ui->fileList->setEditTriggers(QAbstractItemView::DoubleClicked);




            //int rc = sqlite3_open("filelist.db", &db);
            //Db.open();


            //QString createsql = "CREATE TABLE Filelist(Name VARCHAR(32),Path VARCHAR(10)) IF NOT EXISTS;";
            //int rc=sqlite3_exec(db, createsql.toLatin1(), callback, 0, &errMsg);


            //Example sql:"INSERT INTO Filelist VALUES(NULL, 'Fred', '09990123456');"
            //char* insertsql="INSERT INTO Filelist VALUES(NULL, '"+file_name+", '"+add_file+" ');";
            /*
            QString insertsql="INSERT INTO Filelist (Name, Path) VALUES(file_name,add_file);";
            Db.exec(insertsql);
            const char* abc=insertsql.toLatin1();
            */

            //char* insertsql="INSERT INTO Filelist (Name, Path) VALUES('abc.cpp','c:\\abc.cpp');";
            //char* insertsql="INSERT INTO Filelist VALUES(NULL, file_name,add_file ');";
            //char* zsql=sqlite3_mprintf("INSERT INTO Filelist VALUES(%Q,%Q);",file_name.toLatin1(),add_file.toLatin1());

            //QString insertsql="INSERT INTO Filelist (Name, Path) VALUES(file_name,add_file);";
            //exec_SQL(db,insertsql);

            //int rc=sqlite3_exec(db,zsql, callback, 0, &errMsg);
            //sqlite3_free(zsql);
            //rc=sqlite3_finalize(statement);

            //add_list.clear();
        }
        if(dup_list.empty()){
            add_list.clear();
            dup_list.clear();
            update_filelist();
            return;
        }else{
            QMessageBox dup_file;
            dup_file.setText("You share the duplicated item!!");
            dup_file.setDetailedText(dup_list.join("\n"));
            dup_file.exec();

            add_list.clear();
            dup_list.clear();
            update_filelist();
        }
    }
        //add_record(cur_file);
}

void MainWindow::update_filelist()
{
    ui->fileList->update();
    ui->lcdNumber->display(ui->fileList->rowCount());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event){
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    QStringList fileNames;

    for(QList<QUrl>::iterator it= urls.begin();it!=urls.end();++it)
    {
        QUrl cur_file=*it;
        QString file=cur_file.toLocalFile();
        fileNames=fileNames<<file;
    }


    for (int i = 0; i < urls.size(); ++i)
         urls.at(i).toLocalFile();
        /*
    if (fileNames.isEmpty())
        return;
        */
    add_records(fileNames);

}
