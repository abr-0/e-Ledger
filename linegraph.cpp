#include "linegraph.h"
#include "ui_linegraph.h"
//#include <algorithm>

LineGraph::LineGraph(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LineGraph)
{
    ui->setupUi(this);

    QString temp_file_path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("temp.txt");
    qDebug() << temp_file_path;
    QFile file(temp_file_path);

    if (!file.open(QFile::ReadOnly | QFile::Text)){
            qDebug() << "File not opened";
            return;
    } else {
        QTextStream in(&file);
        temp_username = in.readAll(); //temp_username has been declared in mainwindow.h
        file.close();
        file.remove();
    }

    QString months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    for (auto month: months){
        ui->select_month_menu->addItem(month);
    }

    ui->view_by_menu->addItem("Transaction amounts");
    ui->view_by_menu->addItem("No. of transaction");

    ui->select_all_btn->setVisible(0);

    QPixmap view_pixmap(":/img/img/eye.png");
    QIcon view_icon(view_pixmap);
    ui->view_btn->setIcon(view_icon);
    ui->view_btn->setIconSize(QSize(24, 24));

    QPixmap help(":/img/img/question.png");
    QIcon help_icon(help);
    ui->help_button->setIcon(help_icon);
    ui->help_button->setIconSize(QSize(32, 32));
}

LineGraph::~LineGraph()
{
    delete ui;
}

void LineGraph::on_view_btn_clicked()
{
    ui->customPlot->xAxis->setLabel("Days");//set name of the x-axis

    //checking if the user selected buyer or seller
    QString buyer_or_seller, received_or_paid, receivable_or_payable;
    if (ui->select_buyer_btn->isChecked()){
        buyer_or_seller = temp_username + "_b";
        received_or_paid = "Received";
        receivable_or_payable = "Receivable";
    }
    else if (ui->select_seller_btn->isChecked()){
        buyer_or_seller = temp_username + "_s";
        received_or_paid = "Paid";
        receivable_or_payable = "Payable";
    }

    // to get number of days according to the selected month
    int month_number = ui->select_month_menu->currentIndex() + 1; //to get month no.
    int last_day;//stores no. of day in a month
    QVector<double> x_day_numbers;//stores 0 to last_day, collection of x-coordinate values
    if (month_number == 1 || month_number == 3 || month_number == 5 || month_number == 7 || month_number == 8 || month_number == 10 || month_number == 12){
        for(int i = 0; i <= 31; i++ ){ //storing 0 to 31 in x_days_numbers
            x_day_numbers.push_back(i);//adding day number one by one, i.e. 0, 1, 2, ..., 31
        }

        last_day = 31;
        ui->customPlot->xAxis->setRange(0, 31);//x-axis contains value from 0 to 31
    }
    else if (month_number == 4 || month_number == 6 || month_number == 9 || month_number == 11){
        for(int i = 0; i <= 30; i++ ){
            x_day_numbers.push_back(i);
        }

        last_day = 30;
        ui->customPlot->xAxis->setRange(0, 30);//30 days
    }
    else if (month_number == 2){
        //February, leap year (29 days in February) not included
        for(int i = 0; i <= 28; i++ ){
            x_day_numbers.push_back(i);
        }

        last_day = 28;
        ui->customPlot->xAxis->setRange(0, 28);
    }

    if (!ui->view_by_menu->currentIndex()){ //to view graph on the basis of Transaction Amounts whose index in comboBox is 0, so !0 = 1
        //-----------------------------To get values from the database---------------------
        db_conn_open();

        QSqlQuery *qry = new QSqlQuery(main_db);

        QSqlQueryModel *model = new QSqlQueryModel();

        //----------------------------------===================------------------------------//
        //amount1 = Total amount, amount2 = Received or Paid, amount3 = Receivable or Payable//
        //----------------------------------===================------------------------------//

        //collections of y-coordinate values; all are associated with the y-axis values
        QVector<double> y_amount1_sums, y_amount2_sums, y_amount3_sums;

        //to get the origin i.e. (x, y) = (0, 0)
        y_amount1_sums.push_back(0.0);
        y_amount2_sums.push_back(0.0);
        y_amount3_sums.push_back(0.0);

        //to store sum of amount1, amount2 and net amount in y_amount1_sums, y_amount2_sums and y_amount3_sums respectively
        QString amount_types[3] = {"`Total amount`", received_or_paid, receivable_or_payable};
        for (int i = 0; i < 3; i++) { //to loop through amount_types array
            for (int day = 1; day <= last_day; day++ ){//to loop through days
                QString month_number_text, day_number_text;
                if (day < 10)//if day number is 1, 2, ..., 9, day_number_text would be 01, 02, ..., 09
                    day_number_text = "0" + QString::number(day);
                else if (day >= 10 && day <= 31)//else simply 10, 11, ..., 31
                    day_number_text = QString::number(day);

                if (month_number < 10)//if day number is 1, 2, ..., 9, day_number_text would be 01, 02, ..., 09
                    month_number_text = "0" + QString::number(month_number);
                else if (month_number >= 10 && month_number <= 12)//else simply 10, 11, 12
                    month_number_text = QString::number(month_number);

                // to get date in YYYY-MM-DD format; year is not counted, only month and day are counted
                QString date = "'____-" + month_number_text + "-" + day_number_text + "'";

                //to get sum of amount1 or amount2 or net amount for the given date
                qry->prepare("SELECT SUM (" + amount_types[i] + ") FROM " + buyer_or_seller + " WHERE Date LIKE " + date);
                qry->exec();
                model->setQuery(*qry);
                double sum = model->data(model->index(0,0)).toDouble();//returns value at index(0,0) i.e. first row, first column

                //adding sum of amounts per day to the corresponding vector
                if (i == 0)//for amount1
                    y_amount1_sums.push_back(sum);
                else if (i == 1)//for amount2
                    y_amount2_sums.push_back(sum);
                else if (i == 2)//for net amount
                    y_amount3_sums.push_back(sum);
            }
        }

        for (auto a: y_amount1_sums)
            qDebug() << a;

        db_conn_close();

    //==========================================To get minimum and maximum values==============================

        double amount1_sums_max = y_amount1_sums[0];
        double amount1_sums_min = y_amount1_sums[0];
        get_min_max_value_in_vector(amount1_sums_min, amount1_sums_max, y_amount1_sums);//to get minimum and maximum values in y_amount1_sums

        double amount2_sums_max = y_amount2_sums[0];
        double amount2_sums_min = y_amount2_sums[0];
        get_min_max_value_in_vector(amount2_sums_min, amount2_sums_max, y_amount2_sums);//to get minimum and maximum values in y_amount2_sums

        double net_amount_sums_max = y_amount3_sums[0];
        double net_amount_sums_min = y_amount3_sums[0];
        get_min_max_value_in_vector(net_amount_sums_min, net_amount_sums_max, y_amount3_sums);//to get minimum and maximum values in y_amount3_sums

        //to get maximum value among amount1_sums_max, amount2_sums_max and net_amount_sums_max
        double y_max_value;
        if(amount1_sums_max > amount2_sums_max && amount1_sums_max > net_amount_sums_max)
            y_max_value = amount1_sums_max;
        else if (amount2_sums_max > amount1_sums_max && amount2_sums_max > net_amount_sums_max)
            y_max_value = amount2_sums_max;
        else
            y_max_value = net_amount_sums_max;

        //to get minimum value among amount1_sums_min, amount2_sums_min and net_amount_sums_min
        double y_min_value;
        if(amount1_sums_min < amount2_sums_min && amount1_sums_min < net_amount_sums_min)
            y_min_value = amount1_sums_min;
        else if (amount2_sums_min < amount1_sums_min && amount2_sums_min < net_amount_sums_min)
            y_min_value = amount2_sums_min;
        else
            y_min_value = net_amount_sums_min;

        //deletes graph objects and their correspending data
        ui->customPlot->clearPlottables();

        // set title of the graph:
        ui->plot_title->setText("Graph of transaction amounts vs days");

        // set name of y-axis
        ui->customPlot->yAxis->setLabel("Transaction amounts");

        //set range of y-coordinate values from y_min_value to y_max_value
        ui->customPlot->yAxis->setRange(y_min_value, y_max_value);

        //add graphs and graph data
        ui->customPlot->addGraph();//addding graph to customPlot, which is an instance of QCustomPlot
        ui->customPlot->graph(0)->setPen(QPen(Qt::green));//set color of graph
        ui->customPlot->graph(0)->setName("Total amount");//set name of graph

        ui->customPlot->addGraph();
        ui->customPlot->graph(1)->setPen(QPen(Qt::red));
        ui->customPlot->graph(1)->setName(received_or_paid);

        ui->customPlot->addGraph();
        ui->customPlot->graph(2)->setPen(QPen(Qt::blue));
        ui->customPlot->graph(2)->setName(receivable_or_payable);

        //setting points to plot graphs
        ui->customPlot->graph(0)->setData(x_day_numbers, y_amount1_sums); //amount1; green
        ui->customPlot->graph(1)->setData(x_day_numbers, y_amount2_sums); //amount2; red
        ui->customPlot->graph(2)->setData(x_day_numbers, y_amount3_sums); //net amount; blue

        //to display legend/index
        ui->customPlot->legend->setVisible(true);

        //to display graph(s)
        ui->customPlot->replot();
    }
    else if (ui->view_by_menu->currentIndex()){//to view graph on the basis of No. of transaction whose index in comboBox is 1
        db_conn_open();

        QSqlQuery *qry = new QSqlQuery(main_db);

        QSqlQueryModel *model = new QSqlQueryModel();

        QVector<double> num_of_transactions_list;//it is associated with the y-axis values
        num_of_transactions_list.push_back(0.0);//first element is 0 to get the origin (x, y) = (0, 0)

        //to add no. of transaction per day to num_of_transactions_list
        for (int day = 1; day <= last_day; day++ ){
            QString month_number_text, day_number_text;
            if (day < 10)//if day number is 1, 2, ..., 9, day_number_text would be 01, 02, ..., 09
                day_number_text = "0" + QString::number(day);
            else if (day >= 10 && day <= 31)//else simply 10, 11, ..., 31
                day_number_text = QString::number(day);

            if (month_number < 10)//if month number is 1, 2, ..., 9, month_number_text would be 01, 02, ..., 09
                month_number_text = "0" + QString::number(month_number);
            else if (month_number >= 10 && month_number <= 12)//else simply 10, 11, 12
                month_number_text = QString::number(month_number);

            // to get date in YYYY-MM-DD format; year is not counted, only month and day are counted
            QString date = "'____-" + month_number_text + "-" + day_number_text + "'";

            // to get no. of transaction per day
            if(ui->select_all_btn->isChecked()){//if All is selected

                //buyer_count = no. of tranaction with buyers, seller_count = no. of transaction with sellers
                double buyer_count = 0.0, seller_count = 0.0;

                qry->prepare("SELECT COUNT (`Transaction ID`) FROM " + temp_username + "_b WHERE Date LIKE " + date);//counting no. of row where given date matches
                qry->exec();
                model->setQuery(*qry);
                buyer_count = model->data(model->index(0,0)).toDouble();//returns value at index(0,0) i.e. first row, first column

                qry->prepare("SELECT COUNT (`Transaction ID`) FROM " + temp_username + "_s WHERE Date LIKE " + date);//counting no. of row where given date matches
                qry->exec();
                model->setQuery(*qry);
                seller_count = model->data(model->index(0,0)).toDouble();

                num_of_transactions_list.push_back(buyer_count + seller_count);//number of transaction including both buyers and sellers
            }
            else if (ui->select_buyer_btn->isChecked() || ui->select_seller_btn->isChecked()){//if Buyer or Seller is selected
                qry->prepare("SELECT COUNT (`Transaction ID`) FROM " + buyer_or_seller + " WHERE Date LIKE " + date); //add date

                qry->exec();
                model->setQuery(*qry);

                double count = model->data(model->index(0,0)).toDouble();
                num_of_transactions_list.push_back(count);//adding no. of rows/transaction to num_of_transactions_list
            }
        }

        db_conn_close();

        //to get maximum no. of transaction in a month
        double y_max_transaction_num = num_of_transactions_list[0];
        get_max_num_of_transaction(y_max_transaction_num, num_of_transactions_list);

        //deletes graph objects and their corresponding data
        ui->customPlot->clearPlottables();

        // set title of graph
        ui->plot_title->setText("Graph of no. of transaction vs days");

        //add graph to customPlot object
        ui->customPlot->addGraph();
        ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
        ui->customPlot->graph(0)->setName("No. of transaction");//set name of the graph

        // set name of the y-axis
        ui->customPlot->yAxis->setLabel("No. of transaction");

        //set range of y-coordinate values from 0 to y_max_transaction_num
        ui->customPlot->yAxis->setRange(0, y_max_transaction_num);

        //set points to plot graph
        ui->customPlot->graph(0)->setData(x_day_numbers, num_of_transactions_list);

        //to display legend/index
        ui->customPlot->legend->setVisible(true);

        //to display graph(s)
        ui->customPlot->replot();
    }
}

void LineGraph::get_min_max_value_in_vector(double &min_value, double &max_value, QVector<double> &amount_sums){
    //returns minimum and maximum values
    for(const double &each_sum: amount_sums)
    {
       if (max_value < each_sum)
           max_value = each_sum;
       if (min_value > each_sum)
           min_value = each_sum;
    }
}

void LineGraph::on_view_by_menu_currentIndexChanged(int index)
{
    if (!index){ //only option for Buyer and Seller is shown, option for All is not shown
        ui->select_all_btn->setVisible(0);
    }
    else if (index){ //option of Buyer, Seller and All is shown
        ui->select_all_btn->setVisible(1);
    }
}

void LineGraph::get_max_num_of_transaction(double &max_transaction_num, QVector<double> &num_of_transactions_list){
    //returns maximum no. of transaction in a month
    for (const double &transaction_per_day: num_of_transactions_list){
           if (max_transaction_num < transaction_per_day)
               max_transaction_num = transaction_per_day;
    }
}

void LineGraph::on_help_button_clicked()
{
    QString path_to_manual = "file:///" + QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("manual.pdf");
    QDesktopServices::openUrl(QUrl(path_to_manual, QUrl::TolerantMode));
}
