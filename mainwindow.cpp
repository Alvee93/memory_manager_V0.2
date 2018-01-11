#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlQueryModel>
#include <QSqlQuery>

#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

QString folderPath, absFilePath;
static const QString path = "m_managerV01.sqlite";
DbManager db(path);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString sPath = "F:/";
    dirModel = new QFileSystemModel (this);                     // setting up the directory model on tree view
    dirModel ->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirModel -> setRootPath(sPath);
    ui->treeView->setModel(dirModel);

    ui->treeView->hideColumn(1);    //hiding unnecessary columns on tree view
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);

    fileModel = new QFileSystemModel (this);                    // setting up the file model on list view
    fileModel ->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    fileModel -> setRootPath(sPath);

    QStringList filters;            // list view filters to show only image files
    filters << "*.JPG" << "*.PNG" << "*.ico" << "*.JPEG";
    fileModel->setNameFilters(filters);
    fileModel -> setNameFilterDisables(false);
    ui->listView->setModel(fileModel);

    show_comboBox();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QString sPath = dirModel->fileInfo(index).absoluteFilePath();

    ui->listView->setRootIndex(fileModel->setRootPath(sPath));

    folderPath = sPath;
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    if(!folderPath.isEmpty())
        {
        QString filename = fileModel->fileInfo(index).absoluteFilePath();;
        show_image(filename);
        absFilePath = filename;
        //db.addImg(filename);
        //ui -> label -> setNum(db.imgExists(filename));
    }
}

void MainWindow::on_pushButton_clicked()
{
    ui -> tabWidget->setCurrentIndex(1);
    // here the exif extractor will perform its work!!!
    db.createTable();

    extract_exif(absFilePath);

    show_image_metadata();

    show_image_result();
}

void MainWindow::on_quickBrowse_clicked()
{
    QFileDialog dialog(this);
        dialog.setNameFilter(tr("Images(*.jpg *.png *.xpm)"));
        dialog.setViewMode(QFileDialog::Detail);
        QString filePath = QFileDialog::getOpenFileName(this,
                           tr("Open Images"), "",
                           tr("Image Files (*jpg *png *bmp)"));
        show_image(filePath);
        absFilePath = filePath;
}

void MainWindow::show_image(QString imgPath)
{
    if (!imgPath.isEmpty()){
        QPixmap pix(imgPath);

        int w = ui->imgLabel->width ();
        int h = ui->imgLabel->height ();
        ui->imgLabel->setPixmap (pix.scaled (w,h,Qt::KeepAspectRatio));

        int w2 = ui->imgLabel_2->width ();
        int h2 = ui->imgLabel_2->height ();
        ui->imgLabel_2->setPixmap (pix.scaled (w2,h2,Qt::KeepAspectRatio));

        //extract_exif(imgPath);                        // parsing the exif metadata
    }
}

void MainWindow::show_image_metadata()
{
    QSqlQueryModel *modal = new QSqlQueryModel;
    QSqlQuery query;
    bool success = false;
    query.prepare("SELECT make, model, software, bps, width, height, description, orientation, copyright, datetime, "
                  "o_datetime, d_datetime, subsecond, exposure, f_stop, iso, s_distance, e_bias, flash, metering_mode, "
                  "focal_length, focal_length_35mm, latitude, longitude, altitude, min_focal_length, max_focal_length, "
                  "min_f_stop, max_f_stop, lens_make, lens_model FROM images WHERE path = (:path)");
    query.bindValue(":path", absFilePath);
    success = query.exec();
    if(!success){
        qDebug() << "Fetching metadata failed: " << query.lastError();
    }
/*
    if (query.exec())
    {
       if (query.next())
       {
           modal->setQuery(query);
           ui->tableView->setModel(modal);// it exists
       }
    }*/
    modal->setQuery(query);
    ui->tableView->setModel(modal);
}

void MainWindow::show_image_result()
{
    QSqlQueryModel *modal = new QSqlQueryModel;
    QSqlQuery query;
    bool success = false;
    QString make = ui->comboBox->currentText();
    query.prepare("SELECT path FROM images WHERE make = (:make)");
    query.bindValue(":make", make);
    success = query.exec();
    if(!success){
        qDebug() << "Fetching metadata failed: " << query.lastError();
    }
/*
    if (query.exec())
    {
       if (query.next())
       {
           modal->setQuery(query);
           ui->tableView->setModel(modal);// it exists
       }
    }*/
    modal->setQuery(query);
    ui->listView_2->setModel(modal);

}

void MainWindow::show_comboBox()
{
    QSqlQueryModel *modal = new QSqlQueryModel;
    QSqlQuery query;
    bool success = false;
    query.prepare("SELECT DISTINCT make FROM images");
    success = query.exec();
    if(!success){
        qDebug() << "Fetching make failed: " << query.lastError();
    }

    modal->setQuery(query);
    ui->comboBox->setModel(modal);
}

void MainWindow::extract_exif(QString imgPath)
{
    QByteArray ba = imgPath.toLatin1();
    const char *temp = ba.data();

    FILE *fp = fopen(temp,"rb");// binPix; ;

    fseek(fp, 0, SEEK_END);
    unsigned long fsize = ftell(fp);
    rewind(fp);
    unsigned char *buf = new unsigned char[fsize];

    if (fread(buf, 1, fsize, fp) != fsize) {
        ui->exifError->setText("Cannot read file.");
        delete[] buf;
        //return 0;
      }

    fclose(fp);

    easyexif::EXIFInfo result;
    int code = result.parseFrom(buf, fsize);
    delete[] buf;
    if (code) {
        ui->exifError->setText("Error parsing Exif data.");
      }

    QString make = result.Make.c_str();
    QString model = result.Model.c_str();
    QString software = result.Software.c_str();
    int bps = result.BitsPerSample;
    int width = result.ImageWidth;
    int height = result.ImageHeight;
    QString description = result.ImageDescription.c_str();
    int orientation = result.Orientation;
    QString copyright = result.Copyright.c_str();
    QString datetime = result.DateTime.c_str();
    QString o_datetime = result.DateTimeOriginal.c_str();
    QString d_datetime = result.DateTimeDigitized.c_str();
    QString subsecond = result.SubSecTimeOriginal.c_str();
    int exposure = 1.0 / result.ExposureTime;
    double f_stop = result.FNumber;
    int iso = result.ISOSpeedRatings;
    double s_distance = result.SubjectDistance;
    double e_bias = result.ExposureBiasValue;
    int flash = result.Flash;
    int metering_mode = result.MeteringMode;
    double focal_length = result.FocalLength;
    double focal_length_35mm = result.FocalLengthIn35mm;
    double latitude = result.GeoLocation.Latitude;
    double longitude = result.GeoLocation.Longitude;
    double altitude = result.GeoLocation.Altitude;
    double min_focal_length = result.LensInfo.FocalLengthMin;
    double max_focal_length = result.LensInfo.FocalLengthMax;
    double min_f_stop = result.LensInfo.FStopMin;
    double max_f_stop = result.LensInfo.FStopMax;
    QString lens_make = result.LensInfo.Make.c_str();
    QString lens_model = result.LensInfo.Model.c_str();

    db.addImg(imgPath, make, model, software, bps, width, height, description, orientation, copyright, datetime, o_datetime, d_datetime, subsecond, exposure, f_stop, iso, s_distance, e_bias, flash, metering_mode, focal_length, focal_length_35mm, latitude, longitude, altitude, min_focal_length, max_focal_length, min_f_stop, max_f_stop, lens_make, lens_model);

    /*ui->cMake-> setText(result.Make.c_str());
    ui->cMod-> setText(result.Model.c_str());
    ui->eSoft-> setText(result.Software.c_str());
    ui->iTitle-> setText(result.ImageDescription.c_str());
    ui->eTime-> setText(QString("1/ %1 Sec").arg(1.0 / result.ExposureTime));//format: setText(QString("%1").arg(c)
    ui->iSpeedRating-> setNum(result.ISOSpeedRatings);
    ui->iBPS-> setNum(result.BitsPerSample);
    ui->iWidth-> setText(QString(result.ImageWidth));
    ui->iHeight-> setText(QString(result.ImageHeight));
    ui->iOrientation-> setNum(result.Orientation);
    ui->copyR-> setText(result.Copyright.c_str());
    ui->iDTime-> setText(result.DateTime.c_str());
    ui->iiDTOri-> setText(result.DateTimeOriginal.c_str());
    ui->iDTDi-> setText(result.DateTimeDigitized.c_str());
    ui->sSTO-> setText(result.SubSecTimeOriginal.c_str());
    ui->fStop-> setText("f/" + QString::number(result.FNumber));
    ui->sDistance-> setText(QString::number(result.SubjectDistance));
    ui->eBias-> setText(QString::number(result.ExposureBiasValue) + " EV");
    ui->rFlash-> setText("Flash: " + QString::number(result.Flash));
    ui->rMetering-> setText(QString::number(result.MeteringMode)); // ///////////////
    ui->fLength-> setText(QString("%2 mm").arg(result.FocalLength));
    ui->fLength35-> setText(QString::number(result.FocalLengthIn35mm) + " mm");
    //ui->cMake-> setText(result.Make.c_str());

    //result.ExposureTime
    */
}


void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    QSqlQueryModel *modal = new QSqlQueryModel;
    QSqlQuery query;
    bool success = false;
    QString make = ui->comboBox->currentText();
    query.prepare("SELECT path FROM images WHERE make = (:make)");
    query.bindValue(":make", make);
    success = query.exec();

    if (query.exec())
    {
       if (query.next())
       {
            modal->setQuery(query);
            ui->listView_2->setModel(modal);   // it exists
       }
    }
    else if(!success){
        qDebug() << "Fetching result failed: " << query.lastError();
    }

}

void MainWindow::on_listView_2_activated(const QModelIndex &index)
{
    QString val = ui->listView_2->model()->data(index).toString();

    QPixmap pix(val);
    int w = ui->imgLabel->width ();
    int h = ui->imgLabel->height ();
    ui->imgLabel_3->setPixmap (pix.scaled (w,h,Qt::KeepAspectRatio));
    //ui->imgLabel_3->setText(val);
    QSqlQueryModel *modal = new QSqlQueryModel;
    QSqlQuery query;
    bool success = false;
    query.prepare("SELECT make, model, software, bps, width, height, description, orientation, copyright, datetime, "
                  "o_datetime, d_datetime, subsecond, exposure, f_stop, iso, s_distance, e_bias, flash, metering_mode, "
                  "focal_length, focal_length_35mm, latitude, longitude, altitude, min_focal_length, max_focal_length, "
                  "min_f_stop, max_f_stop, lens_make, lens_model FROM images WHERE path = (:path)");
    query.bindValue(":path", val);
    success = query.exec();
    if(!success){
        qDebug() << "Fetching metadata failed: " << query.lastError();
    }
/*
    if (query.exec())
    {
       if (query.next())
       {
           modal->setQuery(query);
           ui->tableView->setModel(modal);// it exists
       }
    }*/
    modal->setQuery(query);
    ui->tableView_2->setModel(modal);
}
