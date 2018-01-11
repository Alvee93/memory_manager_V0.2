#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QFileDialog>
#include <exif.h>
#include <dbmanager.h>
#include <stdio.h>
#include <fstream>
#include<QtCore>
#include<QtGui>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    void extract_exif(QString imgPath);
    //QFileSystemModel *filterModel;

private slots:
    void on_treeView_clicked(const QModelIndex &index);

    void on_listView_clicked(const QModelIndex &index);

    void on_pushButton_clicked();

    void on_quickBrowse_clicked();

    void show_image(QString imgPath);
    void show_image_metadata();
    void show_image_result();
    void show_comboBox();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_listView_2_activated(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QFileSystemModel *dirModel;
    QFileSystemModel *fileModel;
};

#endif // MAINWINDOW_H
