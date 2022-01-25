#ifndef REGISTER_H
#define REGISTER_H

#include <QMainWindow>
#include <QtSql>
#include <QtDebug>
//#include <QFile>
#include <QFileInfo>
#include "mainwindow.h"
#include <QLineEdit>

namespace Ui {
class Register;
}

class Register : public QMainWindow
{
    Q_OBJECT

public:
    QSqlDatabase main_db;

    void db_conn_close(){
        main_db.close();
        main_db.removeDatabase(QSqlDatabase::defaultConnection);
    }

    bool db_conn_open(){
        main_db = QSqlDatabase::addDatabase("QSQLITE");
        //main_db.setDatabaseName("D:/Project/eLedger/main/main.db");//change path for your system

        QString db_path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("main.db");
        qDebug() << db_path;
        main_db.setDatabaseName(db_path);

        if (!main_db.open()){
            qDebug() << "Database not connected";
            return false;
        }
        else {
            qDebug() << "Database connected";
            return true;
        }
    }

    void set_icons();

    void encrypt(QString &text_to_encrypt);

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();

private slots:
    void on_registerButton_clicked();

    void on_show_hide_pwd_btn_toggled(bool checked);

    void on_login_button_clicked();

    void on_help_button_clicked();

private:
    Ui::Register *ui;
    MainWindow *showMainWindow;
};

#endif // REGISTER_H
