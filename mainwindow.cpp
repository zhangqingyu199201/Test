#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qstandarditemmodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

enum {
    TABLE_TIME,
    TABLE_MONEY,
    TABLE_LEFT,
    TABLE_SD_MONEY,
    TABLE_SD_ORI,
    TABLE_SD_RATE,
    TABLE_SD_LEFT,
    TABLE_GJJ_MONEY,
    TABLE_GJJ_ORI,
    TABLE_GJJ_RATE,
    TABLE_GJJ_LEFT,
};


class moneyCalculator {
public:
    double money_;
    double rate_;
    int time_;
    int mode_;


    class moneyItem {
    public:
        int idx = 0;
        double left_total = 0.f;
        double ori_money = 0.f;
        double rate_money = 0.f;
        double total_money = 0.f;
    };


    double calMode1Summary() {
        double num = money_;
        double rate = rate_;
        double month = time_;
        return num * rate * pow(1 + rate, month) / (pow(1 + rate, month) - 1);
    }

    std::vector<moneyCalculator::moneyItem> calDetail() {
        if (mode_ == 1) {
            return calMode1Detail();
        } else {
            return calMode2Detail();
        }
    }


    std::vector<moneyCalculator::moneyItem> calMode1Detail() {

        double num = money_;
        double rate = rate_;
        double month = time_;


        double val = calMode1Summary();

        double left = num;
        std::vector<moneyCalculator::moneyItem> result;
        for (int i = 0; i < month; i++) {
            double rm = left * rate;
            double lm = val - rm;

            left = left - lm;

            moneyCalculator::moneyItem tmp;
            tmp.idx = i;
            tmp.left_total = left;
            tmp.ori_money = lm;
            tmp.rate_money = rm;
            tmp.total_money = val;
            result.push_back(tmp);
        }
        return result;
    }


    std::vector<moneyCalculator::moneyItem> calMode2Detail() {
        double num = money_;
        double rate = rate_;
        double month = time_;

        double val = money_ / time_;

        double left = num;
        std::vector<moneyCalculator::moneyItem> result;
        for (int i = 0; i < month; i++) {
            double rm = left * rate;
            double lm = val;

            left = left - val;

            moneyCalculator::moneyItem tmp;
            tmp.idx = i;
            tmp.left_total = left;
            tmp.ori_money = lm;
            tmp.rate_money = rm;
            tmp.total_money = rm + lm;
            result.push_back(tmp);
        }
        return result;
    }




};




void MainWindow::on_btn_run_clicked()
{
    moneyCalculator moneyCalSd;
    moneyCalSd.mode_ = ui->rb_mode1->isChecked() ? 1 : 2;
    moneyCalSd.money_ = ui->edt_sd_money->text().toDouble();
    moneyCalSd.rate_ = ui->edt_sd_rate->text().toDouble() / 100.f / 12;
    moneyCalSd.time_ = ui->edt_sd_time->text().toDouble();

    moneyCalculator moneyCalGjj;
    moneyCalGjj.mode_ = ui->rb_mode1->isChecked() ? 1 : 2;
    moneyCalGjj.money_ = ui->edt_gjj_money->text().toDouble();
    moneyCalGjj.rate_ = ui->edt_gjj_rate->text().toDouble() / 100.f / 12;
    moneyCalGjj.time_ = ui->edt_gjj_time->text().toDouble();

    std::vector<moneyCalculator::moneyItem> itemsSd = moneyCalSd.calDetail();
    std::vector<moneyCalculator::moneyItem> itemsGjj = moneyCalGjj.calDetail();


    QStandardItemModel *model = new QStandardItemModel();
    model->setColumnCount(11);

    model->setHeaderData(TABLE_TIME,Qt::Horizontal,QString::fromLocal8Bit("时间"));
    model->setHeaderData(TABLE_MONEY,Qt::Horizontal,QString::fromLocal8Bit("还款额"));
    model->setHeaderData(TABLE_LEFT,Qt::Horizontal,QString::fromLocal8Bit("还剩"));

    model->setHeaderData(TABLE_SD_MONEY,Qt::Horizontal,QString::fromLocal8Bit("商贷还款"));
    model->setHeaderData(TABLE_SD_ORI,Qt::Horizontal,QString::fromLocal8Bit("商贷还本"));
    model->setHeaderData(TABLE_SD_RATE,Qt::Horizontal,QString::fromLocal8Bit("商贷还息"));
    model->setHeaderData(TABLE_SD_LEFT,Qt::Horizontal,QString::fromLocal8Bit("商贷还剩"));

    model->setHeaderData(TABLE_GJJ_MONEY,Qt::Horizontal,QString::fromLocal8Bit("公积金还款"));
    model->setHeaderData(TABLE_GJJ_ORI,Qt::Horizontal,QString::fromLocal8Bit("公积金还本"));
    model->setHeaderData(TABLE_GJJ_RATE,Qt::Horizontal,QString::fromLocal8Bit("公积金还息"));
    model->setHeaderData(TABLE_GJJ_LEFT,Qt::Horizontal,QString::fromLocal8Bit("公积金还剩"));

    double sd_money = ui->edt_sd_money->text().toDouble();
    double gjj_money = ui->edt_gjj_money->text().toDouble();
    QString text = "贷款总额：" + QString::number(sd_money + gjj_money, 'f', 2);
    text += ",商贷：" + QString::number(sd_money, 'f', 2);
    text += ",公积金贷款：" + QString::number(gjj_money, 'f', 2);
    ui->txt_sumary->setText(text);

    ui->table_result->setModel(model);
    int iterSize = itemsSd.size() > itemsGjj.size() ? itemsSd.size() : itemsGjj.size();
    moneyCalculator::moneyItem itemDefault;
    for(int i = 0; i < iterSize; i++)
    {
        moneyCalculator::moneyItem itSd = i < itemsSd.size() ? itemsSd[i] : itemDefault;
        moneyCalculator::moneyItem itGjj = i < itemsGjj.size() ? itemsGjj[i] : itemDefault;
        model->setItem(i,TABLE_TIME,new QStandardItem(QString::number(i + 1)));
        model->setItem(i,TABLE_MONEY,new QStandardItem(QString::number(itSd.total_money + itGjj.total_money, 'f', 2)));
        model->setItem(i,TABLE_LEFT,new QStandardItem(QString::number(itSd.left_total + itGjj.left_total, 'f', 2)));

        model->setItem(i,TABLE_SD_MONEY,new QStandardItem(QString::number(itSd.total_money, 'f', 2)));
        model->setItem(i,TABLE_SD_ORI,new QStandardItem(QString::number(itSd.ori_money, 'f', 2)));
        model->setItem(i,TABLE_SD_RATE,new QStandardItem(QString::number(itSd.rate_money, 'f', 2)));
        model->setItem(i,TABLE_SD_LEFT,new QStandardItem(QString::number(itSd.left_total, 'f', 2)));

        model->setItem(i,TABLE_GJJ_MONEY,new QStandardItem(QString::number(itGjj.total_money, 'f', 2)));
        model->setItem(i,TABLE_GJJ_ORI,new QStandardItem(QString::number(itGjj.ori_money, 'f', 2)));
        model->setItem(i,TABLE_GJJ_RATE,new QStandardItem(QString::number(itGjj.rate_money, 'f', 2)));
        model->setItem(i,TABLE_GJJ_LEFT,new QStandardItem(QString::number(itGjj.left_total, 'f', 2)));
    }

}
