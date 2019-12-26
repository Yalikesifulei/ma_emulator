#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>
#include <string>
#include <ctime>
#include <QFileDialog>
#include <QFile>
#include <QTableWidgetItem>
#include "markov.h"
using namespace std;

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

void MainWindow::on_processButton_clicked()
{
    bool f0 = false, f1 = false, f2 = false, f3 = false;
    string Alphabet = (ui->Alphabet_Box->text()).toStdString();
    string Error;
    int Error_count = 0;
    int Commands_count = 0;
    vector <pair<string, string>> Cmds;
    vector <unsigned int> Ec;

    if (Alphabet.find(" ") != string::npos || Alphabet.find("~") != string::npos || Alphabet.find("!") != string::npos) {
        f0 = true;
        Error = "Alphabet mustn't contain spaces, \'~\' and \'!\' \n";                   // ERROR space or ~
        Error_count++;
    }

    for (int i=0; i<ui->Commands->rowCount(); i++) {
        if (ui->Commands->item(i,0) == 0 || ui->Commands->item(i,1) == 0) {
            Commands_count = i;
            break;
        }
        else {
            if ((ui->Commands->item(i,0)->text()).isEmpty() || (ui->Commands->item(i,1)->text()).isEmpty()){
                Commands_count = i;
                break;
            }
        }
    }

    for (int i=0; i<Commands_count; i++) {
        if (ui->Commands->item(i,0) != 0 && ui->Commands->item(i,1) != 0)
        {
            string p = (ui->Commands->item(i,0)->text()).toStdString();
            string r =(ui->Commands->item(i,1)->text()).toStdString();
            for (unsigned int j=0; j<p.size(); j++){
                if (p[j] == '!') {
                    Error += "(";
                    Error += to_string(i+1);
                    Error += " P)";
                    Error += " \'!\' can be only in the right part of rule.\n";   // ERROR ~ in rule
                    f2 = true;
                    Error_count++;
                }
                if ((Alphabet.find(p[j]) == string::npos) && (p[j]!='~') && (p[j]!='!')){
                    Error += "(";
                    Error += to_string(i+1);
                    Error += " P)";
                    Error += " symbol \'";                        // ERROR no symbol
                    Error.append(1, p[j]);
                    Error += "\' is not in alphabet.\n";
                    f1 = true;
                    Error_count++;
                }
            }
            for (unsigned int j=0; j<r.size(); j++){
                 if ((Alphabet.find(r[j]) == string::npos) && (r[j]!='~') && (r[j]!='!')){
                    Error += "(";
                    Error += to_string(i+1);
                    Error += " R)";
                    Error += " symbol \'";                        // ERROR no symbol
                    Error.append(1, r[j]);
                    Error += "\' is not in alphabet.\n";
                    f1 = true;
                    Error_count++;
                 }
             }

            if ((r.find('!')) != string::npos){
                if ((r.find('!')) == r.size()-1) {
                        Ec.push_back(i+1);
                        r.pop_back();
                    }
                else {
                    Error += "(";
                    Error += to_string(i+1);
                    Error += " R)";
                    Error += " \'!\' sign can be only in the end of rule.\n";
                    f3 = true;
                    Error_count++;
                }
            }
            Cmds.push_back(make_pair(p, r));
        }
    }

    QMessageBox Message_Box;

    if (f0 || f1 || f2 || f3){
        Message_Box.setText(QString::fromStdString(Error));
        string Title = "Errors found! ("+to_string(Error_count)+")";
        Message_Box.setWindowTitle(QString::fromStdString(Title));
        Message_Box.exec();
    }
    else {
        string Name = (ui->Name_Box->text()).toStdString();
        if (Name.size() == 0)
            Name = "Markov_" + Alphabet + "_" + to_string(Commands_count) + "_" + to_string(time(0));
        Markov temp(Name, Alphabet, Cmds, Ec);
        //temp.Export();

        bool valid = true;;
        string Input = (ui->Input_Box->text()).toStdString();
        for (char c: Input){
            if ((Alphabet.find(c)) == string::npos){
                Message_Box.setText(QString::fromStdString("Incorrect input word. Try again."));
                string Title = "Error!";
                Message_Box.setWindowTitle(QString::fromStdString(Title));
                Message_Box.exec();
                valid = false;
                break;
            }
        }
        if (valid){
            int count = 0;
            string Output = temp.Process(Input, count);
            ui->Output_Box->setText(QString::fromStdString((Output)));
            string Info = "symbols: " + to_string(Output.size()) + "\niterations: " + to_string(count);
            ui->Info_label->setText(QString::fromStdString(Info));

            ifstream f_log;
            f_log.open("exec_log.txt");
            string exec_log, temp_str;
            if (f_log.is_open()){
                if (count < 1000){
                    while (getline(f_log, temp_str)){
                        exec_log += (temp_str + "\n");
                    }
                }
                else {
                    int diff = count - 100;
                    exec_log += "Log is to large to output: only last 100 lines displayed.\nSee exec_log.txt in applications directory for full log.\n\n";
                    for (int i=0; i<diff; i++)
                        getline(f_log, temp_str);
                    while (getline(f_log, temp_str)){
                        exec_log += (temp_str + "\n");
                    }
                }
                f_log.close();
            }

            ui->Log->setText(QString::fromStdString(exec_log));
        }
        /*Message_Box.setText(QString::fromStdString("Success!"));
        Message_Box.setWindowTitle(QString::fromStdString("Success!"));
        Message_Box.exec();*/
    }
}

void MainWindow::on_clear_Alphabet_clicked()
{
    ui->Alphabet_Box->setText(QString());
}

void MainWindow::on_clear_Input_clicked()
{
    ui->Input_Box->setText(QString());
}

void MainWindow::on_clear_Commands_clicked()
{
    ui->Commands->clearContents();
}

void MainWindow::on_toolButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open algorithm"), "", tr("Markov algorithms (*.mrkv)"));

    QFile file(fileName);
    if ((file.exists())&&(file.open(QIODevice::ReadOnly))) {

        QString Name = file.readLine();
        QString Alphabet = file.readLine();
        ui->Name_Box->setText(Name.trimmed());
        ui->Alphabet_Box->setText(Alphabet.trimmed());
        int Commands_count = (file.readLine()).toInt();
        QString str="";
        ui->Commands->clearContents();
        for (int i=0; i<Commands_count; i++){
            str = (file.readLine()).trimmed();
            QTableWidgetItem *temp = new QTableWidgetItem(str);
            ui->Commands->setItem(i, 0, temp);
        }
        for (int i=0; i<Commands_count; i++){
            str = (file.readLine()).trimmed();
            QTableWidgetItem *temp = new QTableWidgetItem(str);
            ui->Commands->setItem(i, 1, temp);
        }

        file.close();
    }
}

void MainWindow::on_pushButton_clicked()
{
    ofstream f_out;
    string Name = (ui->Name_Box->text()).toStdString();
    string Alphabet = (ui->Alphabet_Box->text()).toStdString();
    int Commands_count = 0;
    for (int i=0; i<ui->Commands->rowCount(); i++) {
        if (ui->Commands->item(i,0) == 0 || ui->Commands->item(i,1) == 0) {
            Commands_count = i;
            break;
        }
        else {
            if ((ui->Commands->item(i,0)->text()).isEmpty() || (ui->Commands->item(i,1)->text()).isEmpty()){
                Commands_count = i;
                break;
            }
        }
    }

    if (Name.size() == 0)
        Name = "Markov_" + Alphabet + "_" + to_string(Commands_count) + "_" + to_string(time(0));
    f_out.open(Name+".mrkv");

    if (f_out.is_open()){
        f_out<<Name<<endl;
        f_out<<Alphabet<<endl;
        f_out<<Commands_count<<endl;
        for (int i=0; i<Commands_count; i++) {
            f_out<<(ui->Commands->item(i,0)->text()).toStdString()<<endl;
        }
        for (int i=0; i<Commands_count; i++) {
            f_out<<(ui->Commands->item(i,1)->text()).toStdString()<<endl;
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QMessageBox Message_Box;
    string Help = "Enter alphabet without spaces, ~ and ! in appropriate text box.\nEnter substitution rules into table, separating right in left parts.\nUse ~ for empty symbol and ! in right part for marking terminating rule.";
    Message_Box.setText(QString::fromStdString(Help));
    string Title = "Help";
    Message_Box.setWindowTitle(QString::fromStdString(Title));
    Message_Box.exec();
}

void MainWindow::on_pushButton_3_clicked()
{
    QMessageBox Message_Box;
    string About = "Markov algorithms emulator\n\nMade by Oleksii Galganov\nalex.galganov@gmail.com";
    Message_Box.setText(QString::fromStdString(About));
    string Title = "About";
    Message_Box.setWindowTitle(QString::fromStdString(Title));
    Message_Box.exec();
}
