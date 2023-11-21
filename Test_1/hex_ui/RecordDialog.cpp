#include "RecordDialog.h"
#include "ui_RecordDialog.h"

#include <QFile>

RecordDialog::RecordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecordDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    // Rear File
    QFile file(RECORDS_HOME "/.setting.txt");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "not found record's .setting file";
    }
    else
    {
        QTextStream in(&file);
        QString line = in.readLine();
        auto data = line.split("##");
        file.close();

        ui->gameOn->setChecked(data[0].toInt());
        ui->gameOff->setChecked(!data[0].toInt());
        ui->eventOn->setChecked(data[1].toInt());
        ui->eventOff->setChecked(!data[1].toInt());
        ui->gameTypeConbo->setCurrentIndex(data[2].toInt());
        ui->optionEdit_1->setText(data[3]);
        ui->optionEdit_2->setText(data[4]);
        ui->optionEdit_3->setText(data[5]);
        ui->optionEdit_4->setText(data[6]);
        if (data[1].toInt()) on_eventOn_clicked();
        else on_eventOff_clicked();
    }
}

RecordDialog::~RecordDialog()
{
    delete ui;
}


void RecordDialog::on_eventOn_clicked()
{
    ui->OptionGroup->setEnabled(true);
}


void RecordDialog::on_eventOff_clicked()
{
    ui->OptionGroup->setEnabled(false);
}


void RecordDialog::on_buttonBox_accepted()
{
    // Write File
    int on_one = ui->gameOn->isChecked();
    int on_two = ui->eventOn->isChecked();
    int gmType = ui->gameTypeConbo->currentIndex();
    QString txt_1 = ui->optionEdit_1->text();
    QString txt_2 = ui->optionEdit_2->text();
    QString txt_3 = ui->optionEdit_3->text();
    QString txt_4 = ui->optionEdit_4->text();

    QFile file(RECORDS_HOME "/.setting.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        Q_ASSERT(false);
    }
    QTextStream out(&file);

    QString split = "##";

    out << on_one << split << on_two << split << gmType << split
        << txt_1  << split << txt_2  << split
        << txt_3  << split << txt_4;

    file.close();
}

