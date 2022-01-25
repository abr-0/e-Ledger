#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QtSql>
#include <QDebug>
//#include <QFile>
#include <QFileInfo>
#include "register.h"
#include "mainwindow.h"
#include "forgotpassword.h"
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QMainWindow
{
    Q_OBJECT

public:
    QSqlDatabase main_db;

    void db_conn_close()
    {
        main_db.close();
        main_db.removeDatabase(QSqlDatabase::defaultConnection);
    }

    bool db_conn_open()
    {
        main_db = QSqlDatabase::addDatabase("QSQLITE");
        //main_db.setDatabaseName(path_to_db);//change path for your system

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
    Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_createAccount_clicked();

    void on_loginButton_clicked();

    void on_show_hide_pwd_btn_toggled(bool checked);

    void on_forgot_password_clicked();

    void on_help_button_clicked();

private:
    Ui::Login *ui;
    Register *registerWindow;
    MainWindow *showMainWindow;
    ForgotPassword *forgot_password;
};
#endif // LOGIN_H
