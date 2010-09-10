#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "db_def.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    int readDB();

private slots:
    void on_renameButton_clicked(bool checked);
    void on_updateButton_clicked(bool checked);
    void on_clearButton_clicked(bool checked);
    void on_fileList_itemSelectionChanged();
    void on_removeButton_clicked(bool checked);
    void on_addButton_clicked(bool checked);
    void add_records(QStringList fileNames);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void update_filelist();

};

#endif // MAINWINDOW_H
