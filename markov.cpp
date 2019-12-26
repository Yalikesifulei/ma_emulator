#include "markov.h"
#include <QMainWindow>
#include <QMessageBox>

void Markov::Export()
{
    ofstream f_out;
    f_out.open(Name+".mrkv");
    if (f_out.is_open()){
            // export in readable form
            /*f_out<<"# Algorithm name: "<<Name<<endl;
            f_out<<"# Alphabet: "<<Alphabet<<endl;
            f_out<<"# Command list:"<<endl;
            for (unsigned int i=0; i<Commands.size(); i++) {
                f_out<<"# "<<i+1<<". "<<Commands[i].first<<" => "<<Commands[i].second;
                if (find(End_Commands.begin(), End_Commands.end(), (i+1)) != End_Commands.end()) f_out<<" (end)";
                f_out<<endl;
            }*/
        f_out<<Name<<endl;
        f_out<<Alphabet<<endl;
        f_out<<Commands.size()<<endl;
    for (unsigned int i=0; i<Commands.size(); i++) {
        f_out<<Commands[i].first<<endl;
    }
    for (unsigned int i=0; i<Commands.size(); i++) {
        f_out<<Commands[i].second;
        if (find(End_Commands.begin(), End_Commands.end(), (i+1)) != End_Commands.end()) f_out<<"!";
        //if (i == Commands.size()-1) f_out<<".";
        f_out<<endl;
        }
    }
}

string Markov::Process(string Input, int &count)
{
    ofstream f_log;
    f_log.open("exec_log.txt");
    Input.insert(Input.begin(), '~'); Input.push_back('~');
    f_log<<"[0] "<<Input<<endl;
    //string::iterator iter = Input.begin();
    bool t = true; unsigned int i = 0; //int count = 0;
    Search:
    while (t){

        for (i=0; i<Commands.size(); i++) {
            if (Input.find(Commands[i].first) != string::npos){
                                                                //cout<<"found command "<<i+1<<" in "<<Input<<endl;
                string temp;
                size_t before = Input.find(Commands[i].first);
                size_t after = before + Commands[i].first.size();
                                                                //cout<<before<<";"<<after<<" ";
                for (unsigned int j=0; j<before; j++)
                    temp.push_back(Input[j]);
                for (unsigned int j=0; j<Commands[i].second.size(); j++)
                    temp.push_back(Commands[i].second[j]);
                for (unsigned int j=after; j<Input.size(); j++)
                    temp.push_back(Input[j]);

                Input = temp;
                if (Input[0]!='~') Input.insert(Input.begin(), '~');
                if (*(Input.end()-1)!='~') Input.push_back('~');

                count++;
                f_log<<"["<<count<<"] "<<Input<<endl;
                if (count>=10000){
                    QMessageBox Message_Box;
                    Message_Box.setText("Too many iterations or empty command in list.");
                    string Title = "Error!";
                    Message_Box.setWindowTitle(QString::fromStdString(Title));
                    Message_Box.exec();

                    Input.clear();
                    t = false; goto Search;
                }

                                                                //cout<<Input<<endl; // DEBUG OUTPUT
                if (find(End_Commands.begin(), End_Commands.end(), (i+1)) != End_Commands.end()){
                    t = false; goto Search;
                }
                break;
            }
        }
        if (i==Commands.size()){
            f_log<<"No command found!"<<endl;
            break;
            QMessageBox Message_Box;
            Message_Box.setText("No command found.");
            string Title = "Error!";
            Message_Box.setWindowTitle(QString::fromStdString(Title));
            Message_Box.exec();
        }
    }

    string Output;
    Output.reserve(Input.size());
    for (size_t i=0; i<Input.size(); i++)
        if (Input[i] != '~') Output += Input[i];
    
    return Output;
}
