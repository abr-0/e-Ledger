#ifndef LINEGRAPH_H
#define LINEGRAPH_H

#include <QMainWindow>
#include <QtSql>
#include <QtDebug>
#include "mainwindow.h"
#include "qcustomplot.h"

namespace Ui {
class LineGraph;
}

class LineGraph : public QMainWindow
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

    void get_min_max_value_in_vector(double &min_value, double &max_value, QVector<double> &amount_sums);
    void get_max_num_of_transaction(double &max_transaction_num, QVector<double> &num_of_transactions_list);
//    void get_min_max_for_y_axis(double &value_1, double &value_2, double &value_3, double &min_value, double &max_value);

public:
    explicit LineGraph(QWidget *parent = nullptr);
    ~LineGraph();

private slots:
    void on_view_btn_clicked();

    void on_view_by_menu_currentIndexChanged(int index);

    void on_help_button_clicked();

private:
    Ui::LineGraph *ui;
    QString temp_username;
};

#endif // LINEGRAPH_H
