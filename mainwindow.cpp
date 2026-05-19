#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <exception>

#include "BaseParser.h"
#include "BaseFormatter.h"
#include "BigFraction.h"

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

void MainWindow::on_convertButton_clicked()
{
    ui->outputLog->clear(); // очищаем поле вывода перед новой конвертацией

    std::string inputStr = ui->inputLineEdit->text().toStdString();
    int p = ui->pSpinBox->value();
    int q = ui->qSpinBox->value();

    if (inputStr.empty()) {
        ui->outputLog->setTextColor(Qt::red);
        ui->outputLog->setText("Ошибка: Введите число!");
        return;
    }

    try {
        ui->outputLog->setTextColor(Qt::white);

        // бэк
        BigFraction fraction = BaseParser::parse(inputStr, p);
        bool is_trimmed = false;

        // стринг без кью
        std::string result = BaseFormatter::format(fraction, q, is_trimmed);

        // стринг -> кью стринг
        ui->outputLog->setText(QString::fromStdString(result));

        // если обрезка
        if (is_trimmed) {
            ui->outputLog->append("\nВнимание: Полная запись результата слишком длинная. Показаны первые 100000 символов.");
        }

    } catch (const std::exception& e) {
        ui->outputLog->setTextColor(Qt::red);
        ui->outputLog->setText(QString("Ошибка: ") + e.what());
    }
}

void MainWindow::on_btnLoadFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        if (!in.atEnd()) ui->pSpinBox->setValue(in.readLine().toInt());
        if (!in.atEnd()) ui->qSpinBox->setValue(in.readLine().toInt());
        if (!in.atEnd()) ui->inputLineEdit->setText(in.readLine());
        file.close();
        ui->outputLog->clear();
    }
}

void MainWindow::on_btnSaveFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить результат", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << ui->outputLog->toPlainText();
        file.close();
    }
}
