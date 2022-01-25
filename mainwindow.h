#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QtDebug>
//#include <QFile>
#include <QFileInfo>
#include "linegraph.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
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

//  Don't know what to pass
//  void view(parameters);
//  void export_as(parameters);

    void view_export(int ve); //ve = view, export; ve gets either 0 or 1

    void sort_view_export(int sve); //sve = sort, view, export; svs gets either 0 or 1

    void filter_view_export(int fve); //fve = filter, view, export; fvs gets either 0 or 1

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_save_button_clicked();

    void on_update_button_clicked();

    void on_delete_button_clicked();

    void on_view_button_clicked();

    void on_export_csv_btn_clicked();

    void on_calculate_net_amount_btn_clicked();

    void on_sort_by_drop_menu_currentIndexChanged(int index);

    void on_filter_by_drop_menu_currentIndexChanged(int index);

    void on_display_area_activated(const QModelIndex &index);

    void on_no_sort_filter_btn_clicked();

    void on_apply_sort_btn_clicked();

    void on_apply_filter_btn_clicked();

    void on_signout_button_clicked();

    void on_show_graph_btn_clicked();

    void on_select_buyer_btn_clicked();

    void on_select_seller_btn_clicked();

    void on_help_button_clicked();

    void on_buyer_or_seller_drop_menu_currentIndexChanged(int index);

private:
    QString temp_username;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
