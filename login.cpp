#include "login.h"
#include "ui_login.h"
#include <QPixmap>
#include <QMessageBox>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QDesktopServices>

Login::Login(QWidget *parent)
    : QMainWindow()//parent was passed here
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    QPixmap windowBG(":/img/img/bg.png");
    windowBG = windowBG.scaled(this->size(), Qt::KeepAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, windowBG);
    this->setPalette(palette);

    set_icons();

    ui->loginPasswordInput->setEchoMode(QLineEdit::Password);

    //database connection
    if (!db_conn_open())
        qDebug() << "Database connection failed.";
    else
        qDebug() << "Database connected.";
}

Login::~Login()
{
    delete ui;
    delete registerWindow;
    delete showMainWindow;
    delete forgot_password;
}

void Login::set_icons(){
    QPixmap userIcon(":/img/img/user_green.png");
    ui->loginUsernameIcon->setPixmap(userIcon.scaled(25, 25, Qt::KeepAspectRatio));
    QPixmap passwordIcon(":/img/img/lock_green_outline.png");
    ui->loginPasswordIcon->setPixmap(passwordIcon.scaled(25, 25, Qt::KeepAspectRatio));

    QPixmap show_pwd(":/img/img/show.png");
    QIcon show_pwd_icon(show_pwd);
    ui->show_hide_pwd_btn->setIcon(show_pwd_icon);
    ui->show_hide_pwd_btn->setIconSize(QSize(24, 24));

    QPixmap help(":/img/img/question.png");
    QIcon help_icon(help);
    ui->help_button->setIcon(help_icon);
    ui->help_button->setIconSize(QSize(32, 32));
}

void Login::on_createAccount_clicked()
{
    //hide();
    close();
    registerWindow = new Register(this);
    registerWindow->show();
}

void Login::on_loginButton_clicked()
{
    //if given username and password is correct then execute the following;
    QString username = ui->loginUsernameInput->text();
    QString password = ui->loginPasswordInput->text();

    if(!db_conn_open()){
        qDebug() << "Database connection failed.";
        return;
    }

    encrypt(password);

    db_conn_open();
    QSqlQuery login_query;
    login_query.prepare("SELECT * FROM credentials where username = '" + username + "' and password = '" + password + "'");

    if(login_query.exec()){
        int count = 0;
        while (login_query.next()){
            count++;
        }

        if (count == 1){
           qDebug() << "Correct username and password";

           //QFile file("D:/Project/eLedger/main/temp.txt");//enter actual path
           QString temp_file_path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("temp.txt");
           qDebug() << temp_file_path;
           QFile file(temp_file_path);

           if (!file.open(QFile::WriteOnly | QFile::Text)){
               qDebug() << "File not opened";
               return;
           } else {
               qDebug() << "File opened";

               QTextStream out(&file);
               out << username;
               file.flush();
               file.close();

               db_conn_close();

               //hide();
               close();
               showMainWindow = new MainWindow(this);
               showMainWindow->show();
           }
        }

        else{
           QMessageBox::warning(this, "Login failed", "Your username and/or password do not match. Please try again.");
           db_conn_close();
        }
    }

    else
        qDebug() << login_query.lastError();

    db_conn_close();
}

void Login::encrypt(QString &text_to_encrypt){
    //encryption
    //int k = 16;
    QString plain_text = text_to_encrypt; //making copy of text_to_encrypt
    unsigned long long k {67'83'65'65'83};
    unsigned int ascii, a26;

    for (int i = 0; i < plain_text.size(); i++)
    {
        ascii = (plain_text[i]).QCharRef::unicode();
        if (ascii >= 65 && ascii <= 90)
        {
            a26 = ((ascii + k - 65) % 26 + 65);
            text_to_encrypt[i] = static_cast<char>(a26);
        }
        else if (ascii >= 97 && ascii <= 122)
        {
            a26 = ((ascii + k - 97) % 26 + 97);
            text_to_encrypt[i] = static_cast<char>(a26);
        }
        else if (ascii >= 48 && ascii <= 57)
        {
            a26 = ((ascii + k - 48) % 10 + 48);
            text_to_encrypt[i] = static_cast<int>(a26);
        }
        else
        {
            text_to_encrypt[i] = static_cast<char>(ascii);
        }
    }
}

void Login::on_show_hide_pwd_btn_toggled(bool checked)
{
    if (checked){
        QPixmap hide_pwd(":/img/img/hide.png");
        QIcon hide_pwd_icon(hide_pwd);
        ui->show_hide_pwd_btn->setIcon(hide_pwd_icon);
        ui->show_hide_pwd_btn->setIconSize(QSize(24, 24));

        ui->loginPasswordInput->setEchoMode(QLineEdit::Normal);
    }
    else {
        QPixmap show_pwd(":/img/img/show.png");
        QIcon show_pwd_icon(show_pwd);
        ui->show_hide_pwd_btn->setIcon(show_pwd_icon);
        ui->show_hide_pwd_btn->setIconSize(QSize(24, 24));

        ui->loginPasswordInput->setEchoMode(QLineEdit::Password);
    }
}

void Login::on_forgot_password_clicked()
{
    QMessageBox::StandardButton reply_from_user = QMessageBox::question(this, "Forgot password", "Need to reset your password?", QMessageBox::Yes | QMessageBox::No);

    if (reply_from_user == QMessageBox::Yes)
    {
        //hide();
        close();
        forgot_password = new ForgotPassword(this);
        forgot_password->show();
    }

}

void Login::on_help_button_clicked()
{
    QString path_to_manual = "file:///" + QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("manual.pdf");
    QDesktopServices::openUrl(QUrl(path_to_manual, QUrl::TolerantMode));
}
