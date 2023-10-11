#include "StartWidget.h"
#include "./ui_StartWidget.h"

#include <QFile>
#include <QTextStream>

#include "MainWindow.h"

StartWidget::StartWidget(MainWindow *parent)
    : QWidget(parent)
    , ui(new Ui::StartWidget)
{
    ui->setupUi(this);
    connect(this, &StartWidget::start_game, parent, &MainWindow::StartGame);

    // Rear File
    QFile file(CONFIG_FILE);
    if (!file.open(QIODevice::ReadOnly))
    {
        Q_ASSERT(false);
    }
    QTextStream in(&file);
    QString line = in.readLine();
    auto data = line.split(" ");
    file.close();

    auto _vs = [](int vss)
    {
        switch(vss)
        {
        case 1:
            return 0;
        case 5:
            return 1;
        case 7:
            return 2;
        }
        return -1;
    };

    ui->orderComboBox->setCurrentIndex(data[0].toInt() - 5);
    ui->modeComboBox->setCurrentIndex(data[1].toInt());
    ui->BlackRadio->setChecked(data[2].toInt());
    ui->WhiteRadio->setChecked(!data[2].toInt());
    ui->b_versionCombo->setCurrentIndex(_vs(data[3].toInt()));
    ui->b_doubleSpin->setValue(data[4].toDouble());
    ui->b_Time_SpinBox->setValue(data[5].toInt());
    ui->b_ParallelizedRadio->setChecked(data[6].toInt());
    ui->b_SingleRadio->setChecked(!data[6].toInt());
    ui->w_versionCombo->setCurrentIndex(_vs(data[7].toInt()));
    ui->w_doubleSpin->setValue(data[8].toDouble());
    ui->w_Time_SpinBox->setValue(data[9].toInt());
    ui->w_ParallelizedRadio->setChecked(data[10].toInt());
    ui->w_SingleRadio->setChecked(!data[10].toInt());
}

StartWidget::~StartWidget()
{
    delete ui;
}

void StartWidget::on_startButton_clicked()
{
    // Write File
    int vss[] = {1, 5, 7}; // version

    int order = ui->orderComboBox->currentIndex() + 5;
    int mode = ui->modeComboBox->currentIndex();
    int attacker = ui->BlackRadio->isChecked();
    int blackVersion = vss[ui->b_versionCombo->currentIndex()];
    double blackECF = ui->b_doubleSpin->value();
    int blackTime = ui->b_Time_SpinBox->value();
    int blackParallelized = ui->b_ParallelizedRadio->isChecked();
    int whiteVersion = vss[ui->w_versionCombo->currentIndex()];
    double whiteECF = ui->w_doubleSpin->value();
    int whiteTime = ui->w_Time_SpinBox->value();
    int whiteParallelized = ui->w_ParallelizedRadio->isChecked();

    QFile file(CONFIG_FILE);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        Q_ASSERT(false);
    }
    QTextStream out(&file);

    out << order << " " << mode << " " << attacker << " "
        << blackVersion << " " << blackECF << " "
        << blackTime << " " << blackParallelized << " "
        << whiteVersion << " " << whiteECF << " "
        << whiteTime << " " << whiteParallelized;

    file.close();

    emit start_game(order, attacker, mode);
}


void StartWidget::on_modeComboBox_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0: // PvP
        ui->attackerGroup->setEnabled(true);
        ui->blackAIGroup->setEnabled(false);
        ui->whiteAIGroup->setEnabled(false);
        break;
    case 1: // PvE
        ui->attackerGroup->setEnabled(true);
        if (ui->BlackRadio->isChecked())
        {
            ui->blackAIGroup->setEnabled(false);
            ui->whiteAIGroup->setEnabled(true);
        }
        else
        {
            ui->blackAIGroup->setEnabled(true);
            ui->whiteAIGroup->setEnabled(false);
        }
        break;
    case 2: // EvE
    case 3: // Debug
        ui->attackerGroup->setEnabled(false);
        ui->blackAIGroup->setEnabled(true);
        ui->whiteAIGroup->setEnabled(true);
        break;
    }
}

void StartWidget::on_WhiteRadio_clicked()
{
    if (ui->modeComboBox->currentIndex() == 1)
    {
        ui->blackAIGroup->setEnabled(true);
        ui->whiteAIGroup->setEnabled(false);
    }
}

void StartWidget::on_BlackRadio_clicked()
{
    if (ui->modeComboBox->currentIndex() == 1)
    {
        ui->blackAIGroup->setEnabled(false);
        ui->whiteAIGroup->setEnabled(true);
    }
}

