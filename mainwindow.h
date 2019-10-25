#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>
#include <QStringListModel>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>

#include "../Famax/MotorTest/testIO/logsystem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    bool m_connect = false;
    bool m_ready = false;
    bool m_start = false;

    int role = 0;

    logsystem *m_log = new logsystem;   //紀錄事件
    //玩家名稱
    QStringList name_list = QStringList();
    //玩家名稱model
    QStringListModel *name_model = new QStringListModel();

    QTcpSocket *m_socket;

    void connect_to_server();

    void send_to_server(QString);       //將輸入的文字傳回server
    void add_player(QString);           //當玩家加入遊戲顯示他的名字
    void add_exist_player(QString);
    void set_character(int);
    void set_start(int);

private slots:
    void read_msg();
    void send_msg();

    void on_pushButton_clicked();

    void on_connect_b_clicked();

    void on_ready_b_clicked();

signals:
    void add_log(QString);
};
#endif // MAINWINDOW_H
