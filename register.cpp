#include "register.h"
#include "ui_register.h"
#include <QMessageBox>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QWinTaskbarButton>

#include "login.h"

Register::Register(QWidget *parent) :
    QMainWindow(),//parent was passed here
    ui(new Ui::Register)
{
    ui->setupUi(this);

    QPixmap windowBG(":/img/img/bg.png");
    windowBG = windowBG.scaled(this->size(), Qt::KeepAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, windowBG);
    this->setPalette(palette);

    set_icons();

    ui->registerPasswordInput->setEchoMode(QLineEdit::Password);

    //database connection
    if (!db_conn_open())
        qDebug() << "Database connection failed.";
    else
        qDebug() << "Data connected.";
}

Register::~Register()
{
    delete ui;
    delete showMainWindow;
}

void Register::set_icons(){
    QPixmap userIcon(":/img/img/user_green.png");
    ui->registerUsernameIcon->setPixmap(userIcon.scaled(25, 25, Qt::KeepAspectRatio));
    QPixmap passwordIcon(":/img/img/lock_green_outline.png");
    ui->registerPasswordIcon->setPixmap(passwordIcon.scaled(25, 25, Qt::KeepAspectRatio));

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

void Register::on_registerButton_clicked()
{
    //if the given password is valid execute the following
    //save username and password into the database then

    QString register_username, register_password;
    register_username = ui->registerUsernameInput->text();
    register_password = ui->registerPasswordInput->text();

    QRegularExpression username_pattern("^[a-zA-Z0-9_-]{5,10}$");//only includes alphabets or digits or underscore or hyphen; 5 to 10 characters
    QRegularExpressionMatch username_valid = username_pattern.match(register_username);

    QRegularExpression password_pattern("^.*(?=.{8,})(?=.*[a-zA-Z])(?=.*\\d)(?=.*[@#$%^&-+=()])(?=\\S+$).*$");
    QRegularExpressionMatch password_valid = password_pattern.match(register_password);

    if (username_valid.hasMatch() && password_valid.hasMatch()){

        if(!db_conn_open()){
            qDebug() << "Database connection failed";
            return;
        }

        encrypt(register_password);

        QString ans_1 = ui->qs_1_input->text();
        QString ans_2 = ui->qs_2_input->text();
        encrypt(ans_1);
        encrypt(ans_2);

        db_conn_open();
        QSqlQuery register_query;
        register_query.prepare("INSERT INTO credentials (username, password, ans1, ans2) VALUES ('" + register_username + "', '" + register_password + "', '" + ans_1 + "', '" + ans_2 + "')");

        if(register_query.exec()){
            QSqlQuery add_table_b;
            add_table_b.prepare("CREATE TABLE " + register_username + "_b (`Transaction ID` INTEGER PRIMARY KEY, Date DATE, `Name` TEXT, Address TEXT, `Phone no.` INTEGER, `Total amount` DOUBLE, Received DOUBLE, Receivable DOUBLE, Remarks TEXT)");

            QSqlQuery add_table_s;
            add_table_s.prepare("CREATE TABLE " + register_username + "_s (`Transaction ID` INTEGER PRIMARY KEY, Date DATE, `Name` TEXT, Address TEXT, `Phone no.` INTEGER, `Total amount` DOUBLE, Paid DOUBLE, Payable DOUBLE, Remarks TEXT)");

            if (add_table_b.exec() && add_table_s.exec()){
                qDebug() << "New tables added";

                //QFile file("D:/Project/eLedger/main/temp.txt");//change path for your system
                QString temp_file_path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("temp.txt");
                qDebug() << temp_file_path;
                QFile file(temp_file_path);

                if (!file.open(QFile::WriteOnly | QFile::Text)){
                    qDebug() << "File not opened";
                    return;
                } else {
                    QTextStream out(&file);
                    out << register_username;
                    file.flush();
                    file.close();

                    QMessageBox::information(this, "Woo-hoo!", "Account created successfully.");

                    db_conn_close();

                    //hide();
                    close();

                    showMainWindow = new MainWindow(this);
                    showMainWindow->show();
                }
            }
        }

        else if ("UNIQUE constraint failed: credentials.username Unable to fetch row" == register_query.lastError().text()){
            //qDebug() << register_query.lastError();
            QMessageBox::warning(this, "Registration failed", "Oops, same username already exists. Specify a different one.");
        }

        else
            QMessageBox::warning(this, "Registration failed", register_query.lastError().text());
    }
    else {
        //throwing error message for different scenarios
        if (!username_valid.hasMatch() && !password_valid.hasMatch())
            QMessageBox::warning(this, "Invalid username and password", "Your username:\n  - may include letters, numbers, underscore or hyphen.\n  - needs to include 5 to 10 characters.\n\nYour password needs to:\n  - include at least one letter, number and symbol.\n  - be at least 8 characters long.");
        else if (!username_valid.hasMatch())
            QMessageBox::warning(this, "Invalid username", "Your username:\n  - may include letters, numbers, underscore or hyphen.\n  - needs to include 5 to 10 characters.");
        else if (!password_valid.hasMatch())
            QMessageBox::warning(this, "Invalid password", "Your password needs to:\n  - include at least one letter, number and symbol.\n  - be at least 8 characters long.");
    }
}

void Register::encrypt(QString &text_to_encrypt){
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

void Register::on_show_hide_pwd_btn_toggled(bool checked)
{
    if (checked){
        QPixmap hide_pwd(":/img/img/hide.png");
        QIcon hide_pwd_icon(hide_pwd);
        ui->show_hide_pwd_btn->setIcon(hide_pwd_icon);
        ui->show_hide_pwd_btn->setIconSize(QSize(24, 24));

        ui->registerPasswordInput->setEchoMode(QLineEdit::Normal);
    }
    else {
        QPixmap show_pwd(":/img/img/show.png");
        QIcon show_pwd_icon(show_pwd);
        ui->show_hide_pwd_btn->setIcon(show_pwd_icon);
        ui->show_hide_pwd_btn->setIconSize(QSize(24, 24));

        ui->registerPasswordInput->setEchoMode(QLineEdit::Password);
    }
}

void Register::on_login_button_clicked()
{
    //hide();
    close();
    Login *show_login_window = new Login(this);
    show_login_window->show();
}

void Register::on_help_button_clicked()
{
    QString path_to_manual = "file:///" + QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("manual.pdf");
    QDesktopServices::openUrl(QUrl(path_to_manual, QUrl::TolerantMode));
}
