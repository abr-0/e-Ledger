#include "login.h"

#include <QApplication>
//#include <QSplashScreen>
#include <QTimer>
#include <QMovie>
#include <QLabel>
#include<QLayout>
#include<QBoxLayout>
#include <QStyle>
#include <QDesktopWidget>
//#include <QGuiApplication>
//#include <QStyleOptionTitleBar>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Login w;
    QMovie *movie = new QMovie(":/img/img/splash_screen.gif");
    QLabel *processLabel = new QLabel(NULL);
    processLabel->resize(740,490);
    processLabel->setMovie(movie);
    movie->start();
    processLabel->setWindowIcon(QIcon(":/img/img/ledger_small.png"));
    processLabel->setWindowFlags(Qt::FramelessWindowHint);
    processLabel->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, processLabel->size(), qApp->desktop()->availableGeometry()/*QGuiApplication::screens()*/));
    processLabel->show();

    QTimer::singleShot(2500, processLabel, SLOT(close()));
    QTimer::singleShot(2500, &w, SLOT(show()));

    return a.exec();
}
