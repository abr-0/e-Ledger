#include "forgotpassword.h"
#include "ui_forgotpassword.h"
//#include "login.h"

#include <QMessageBox>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "login.h"

ForgotPassword::ForgotPassword(QWidget *parent) :
    QMainWindow(),//parent was passed here
    ui(new Ui::ForgotPassword)
{
    ui->setupUi(this);

    QPixmap windowBG(":/img/img/bg.png");
    windowBG = windowBG.scaled(this->size(), Qt::KeepAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, windowBG);
    this->setPalette(palette);

    set_icons();

    ui->new_password_input->setEchoMode(QLineEdit::Password);

    //database connection
    if (!db_conn_open())
        qDebug() << "Database connection failed.";
    else
        qDebug() << "Data connected.";
}

ForgotPassword::~ForgotPassword()
{
    delete ui;
    delete show_main_window;
}

void ForgotPassword::set_icons(){
    QPixmap userIcon(":/img/img/user_green.png");
    ui->username_icon->setPixmap(userIcon.scaled(25, 25, Qt::KeepAspectRatio));
    QPixmap passwordIcon(":/img/img/lock_green_outline.png");
    ui->password_icon->setPixmap(passwordIcon.scaled(25, 25, Qt::KeepAspectRatio));

    QPixmap heroIcon(":/img/img/superhero.png");
    ui->qs_1_icon->setPixmap(heroIcon.scaled(32, 32, Qt::KeepAspectRatio));
    QPixmap petIcon(":/img/img/pet.png");
    ui->qs_2_icon->setPixmap(petIcon.scaled(25, 25, Qt::KeepAspectRatio));

    QPixmap show_pwd(":/img/img/show.png");
    QIcon show_pwd_icon(show_pwd);
    ui->show_hide_pwd_btn->setIcon(show_pwd_icon);
    ui->show_hide_pwd_btn->setIconSize(QSize(24, 24));

    QPixmap help(":/img/img/question.png");
    QIcon help_icon(help);
    ui->help_button->setIcon(help_icon);
    ui->help_button->setIconSize(QSize(32, 32));
}

void ForgotPassword::on_show_hide_pwd_btn_toggled(bool checked)
{
    if (checked){
        QPixmap hide_pwd(":/img/img/hide.png");
        QIcon hide_pwd_icon(hide_pwd);
        ui->show_hide_pwd_btn->setIcon(hide_pwd_icon);
        ui->show_hide_pwd_btn->setIconSize(QSize(24, 24));

        ui->new_password_input->setEchoMode(QLineEdit::Normal);
    }
    else {
        QPixmap show_pwd(":/img/img/show.png");
        QIcon show_pwd_icon(show_pwd);
        ui->show_hide_pwd_btn->setIcon(show_pwd_icon);
        ui->show_hide_pwd_btn->setIconSize(QSize(24, 24));

        ui->new_password_input->setEchoMode(QLineEdit::Password);
    }
}

void ForgotPassword::on_reset_button_clicked()
{
        QString username=ui->username_input->text();
        QString password = ui->new_password_input->text();

//        QRegularExpression username_pattern("[a-zA-Z0-9_]");
//        QRegularExpressionMatch username_valid = username_pattern.match(username);

        QRegularExpression password_pattern("^.*(?=.{8,})(?=.*[a-zA-Z])(?=.*\\d)(?=.*[@#$%^&-+=()])(?=\\S+$).*$");
        QRegularExpressionMatch password_valid = password_pattern.match(password);

        if (password_valid.hasMatch())
            {
                if(!db_conn_open()){
                    qDebug() << "Database connection failed";
                    return;
                }

                encrypt(password);

                QString ans_1 = ui->qs_1_input->text();
                QString ans_2=ui->qs_2_input->text();
                encrypt(ans_1);
                encrypt(ans_2);

                db_conn_open();
                QSqlQuery qry;

                qry.prepare("SELECT * FROM credentials WHERE username = '" + username + "' AND `ans1` = '" + ans_1 + "' AND `ans2` = '" + ans_2 + "'");

                int count = 0;
                if (qry.exec()){
                    while (qry.next()){
                        count++;
                    }
                }

                if(count == 1)
                {
                    qry.prepare("UPDATE credentials SET password = '" + password + "' WHERE username = '" + username + "' AND `ans1` = '" + ans_1 + "' AND `ans2` = '" + ans_2 + "'");
                    qry.exec();
                    QMessageBox::information(this,tr("Woo-hoo!"),tr("Password reset successfully."));
                    db_conn_close();

                    QString temp_file_path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("temp.txt");
                    qDebug() << temp_file_path;
                    QFile file(temp_file_path);

                    if (!file.open(QFile::WriteOnly | QFile::Text)){
                        qDebug() << "File not opened";
                        return;
                    } else {
                        QTextStream out(&file);
                        out << username;
                        file.flush();
                        file.close();
                    }

                    //hide();
                    close();
                    show_main_window = new MainWindow(this);
                    show_main_window->show();
                }

                else
                {
                    QMessageBox::warning(this, "Password reset failed", "Your username or security answers do not match. Please try again.");
                    db_conn_close();
                }
            }
        else
        {
            QMessageBox::warning(this, "Invalid password", "Your password needs to:\n  - include at least one letter, number and symbol.\n  - be at least 8 characters long.");
        }
}

void ForgotPassword::encrypt(QString &text_to_encrypt){
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

void ForgotPassword::on_login_button_clicked()
{
    //hide();
    close();
    Login *show_login_window = new Login(this);
    show_login_window->show();
}

void ForgotPassword::on_help_button_clicked()
{
    QString path_to_manual = "file:///" + QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("manual.pdf");
    QDesktopServices::openUrl(QUrl(path_to_manual, QUrl::TolerantMode));
}
