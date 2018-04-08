#include <dbmanager.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

DbManager::DbManager(const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
}

DbManager::~DbManager()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

bool DbManager::isOpen() const
{
    return m_db.isOpen();
}

bool DbManager::createTable()
{
    bool success = false;

    QSqlQuery query;
    query.prepare("CREATE TABLE images(id INTEGER PRIMARY KEY, path TEXT, make TEXT, model TEXT, software TEXT, "
                  "bps INTEGER, width INTEGER, height INTEGER, description TEXT, orientation INTEGER, copyright TEXT, "
                  "datetime TEXT, o_datetime TEXT, d_datetime TEXT, subsecond TEXT, exposure INTEGER, f_stop REAL, "
                  "iso INTEGER, s_distance REAL, e_bias REAL, flash INTEGER, metering_mode INTEGER, focal_length REAL, "
                  "focal_length_35mm REAL, latitude REAL, longitude REAL, altitude REAL, min_focal_length REAL, "
                  "max_focal_length REAL, min_f_stop REAL, max_f_stop REAL, lens_make TEXT, lens_model TEXT);");

    if (!query.exec())
    {
        qDebug() << "Couldn't create the table 'images': one might already exist.";
        success = false;
    }

    return success;
}

int DbManager::addImg(const QString& img, const QString& make, const QString& model, const QString& software, const int& bps, const int& width, const int& height, const QString& description, const int& orientation, const QString& copyright, const QString& datetime, const QString& o_datetime, const QString& d_datetime, const QString& subsecond, const int& exposure, const double& f_stop, const int& iso, const double& s_distance, const double& e_bias, const int& flash, const int& metering_mode, const double& focal_length, const double& focal_length_35mm, const double& latitude, const double& longitude, const double& altitude, const double& min_focal_length, const double& max_focal_length, const double& min_f_stop, const double& max_f_stop,const QString& lens_make, const QString& lens_model)
{
    bool success = false;

    if ((!imgExists(img)) && (!img.isEmpty()))
    {
        QSqlQuery queryAdd;
        queryAdd.prepare("INSERT INTO images (path, make, model, software, bps, width, height, description, orientation, copyright, "
                         "datetime, o_datetime, d_datetime, subsecond, exposure, f_stop, iso, s_distance, e_bias, flash, metering_mode, "
                         "focal_length, focal_length_35mm, latitude, longitude, altitude, min_focal_length, max_focal_length, min_f_stop, "
                         "max_f_stop, lens_make, lens_model) VALUES (:path, :make, :model, :software, :bps, :width, :height, :description, :orientation, :copyright, :datetime, :o_datetime, :d_datetime, :subsecond, :exposure, :f_stop, :iso, :s_distance, :e_bias, :flash, :metering_mode, :focal_length, :focal_length_35mm, :latitude, :longitude, :altitude, :min_focal_length, :max_focal_length, :min_f_stop, :max_f_stop, :lens_make, :lens_model)");
        queryAdd.bindValue(":path", img);
        queryAdd.bindValue(":make", make);
        queryAdd.bindValue(":model", model);
        queryAdd.bindValue(":software", software);
        queryAdd.bindValue(":bps", bps);
        queryAdd.bindValue(":width", width);
        queryAdd.bindValue(":height", height);
        queryAdd.bindValue(":description", description);
        queryAdd.bindValue(":orientation", orientation);
        queryAdd.bindValue(":copyright", copyright);
        queryAdd.bindValue(":datetime", datetime);
        queryAdd.bindValue(":o_datetime", o_datetime);
        queryAdd.bindValue(":d_datetime", d_datetime);
        queryAdd.bindValue(":subsecond", subsecond);
        queryAdd.bindValue(":exposure", exposure);
        queryAdd.bindValue(":f_stop", f_stop);
        queryAdd.bindValue(":iso", iso);
        queryAdd.bindValue(":s_distance", s_distance);
        queryAdd.bindValue(":e_bias", e_bias);
        queryAdd.bindValue(":flash", flash);
        queryAdd.bindValue(":metering_mode", metering_mode);
        queryAdd.bindValue(":focal_length", focal_length);
        queryAdd.bindValue(":focal_length_35mm", focal_length_35mm);
        queryAdd.bindValue(":latitude", latitude);
        queryAdd.bindValue(":longitude", longitude);
        queryAdd.bindValue(":altitude", altitude);
        queryAdd.bindValue(":min_focal_length", min_focal_length);
        queryAdd.bindValue(":max_focal_length", max_focal_length);
        queryAdd.bindValue(":min_f_stop", min_f_stop);
        queryAdd.bindValue(":max_f_stop", max_f_stop);
        queryAdd.bindValue(":lens_make", lens_make);
        queryAdd.bindValue(":lens_model", lens_model);

        if(queryAdd.exec())
        {
            success = true;
        }
        else
        {
            qDebug() << "Add image failed: " << queryAdd.lastError();
        }
    }
    else if(imgExists(img)){
        qDebug() << "Add image failed: image already exists";
    }
    else
    {
        qDebug() << "Add image failed: name cannot be empty";
    }

    return success;
}

bool DbManager::removeImg(const QString& img)
{
    bool success = false;

    if (imgExists(img))
    {
        QSqlQuery queryDelete;
        queryDelete.prepare("DELETE FROM images WHERE path = (:path)");
        queryDelete.bindValue(":path", img);
        success = queryDelete.exec();

        if(!success)
        {
            qDebug() << "Remove image failed: " << queryDelete.lastError();
        }
    }
    else
    {
        qDebug() << "Remove image failed: person doesnt exist";
    }

    return success;
}

void DbManager::printAllImages() const
{
    qDebug() << "Images in db:";
    QSqlQuery query("SELECT * FROM images");
    int idName = query.record().indexOf("path");
    while (query.next())
    {
        QString name = query.value(idName).toString();
        qDebug() << "===" << name;
    }
}

bool DbManager::imgExists(const QString& img) const
{
    bool exists = false;

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT path FROM images WHERE path = (:path)");
    checkQuery.bindValue(":path", img);

    if (checkQuery.exec())
    {
        if (checkQuery.next())
        {
            exists = true;
        }
    }
    else
    {
        qDebug() << "Image exists failed: " << checkQuery.lastError();
    }

    return exists;
}

bool DbManager::removeAllImages()
{
    bool success = false;

    QSqlQuery removeQuery;
    removeQuery.prepare("DELETE FROM images");

    if (removeQuery.exec())
    {
        success = true;
    }
    else
    {
        qDebug() << "remove all images failed: " << removeQuery.lastError();
    }

    return success;
}
