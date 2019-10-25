#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_socket = new QTcpSocket(this);

    name_model->setStringList(name_list);
    ui->listView->setModel(name_model);

    connect(this, SIGNAL(add_log(QString)), m_log, SLOT(add_log(QString)));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connect_to_server(){
    if(m_connect == false){
        //m_socket->connectToHost("114.32.157.74", 19999);
        m_socket->connectToHost(QHostAddress::LocalHost, 19999);
        if(m_socket->isOpen()){
            m_connect = true;
            ui->connect_b->setText("中斷");
            emit add_log("Connected to server");
        }
        else
            emit add_log("Connection failed");

        connect(m_socket, SIGNAL(readyRead()), this, SLOT(read_msg()));
    }
    else{
        m_connect = false;
        name_list = QStringList();
        name_model->setStringList(name_list);
        ui->listView->setModel(name_model);
        ui->textEdit->clear();
        ui->connect_b->setText("連線");
        m_socket->close();
        disconnect(m_socket, SIGNAL(readyRead()), this, SLOT(read_msg()));
    }
}

void MainWindow::read_msg(){
    QString str = m_socket->readAll().data();
    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject obj = QJsonObject();

    //先檢查伺服器過來的訊息是不是正確格式
    if(doc.isObject())
        obj = doc.object();
    else{
        QString msg = QString("Error, msg format is incorrect");
        emit add_log(msg);
        return;
    }

    //去判斷這個訊息的內容是要幹啥子
    if(obj.contains("ASK")){
        //要求回復一個名稱
        QJsonObject res = QJsonObject();
        res.insert("NAME", ui->nickname_edit->text());
        QJsonDocument doc_(res);
        QString msg = QString::fromUtf8(doc_.toJson());
        //QString msg = "/NAME哈哈是我啦";
        send_to_server(msg);
    }
    else if(obj.contains("ADDPLAYER")){
        //加入新玩家
        QString data = obj["ADDPLAYER"].toString();
        add_player(data);
    }
    else if(obj.contains("ADDOLDPLAYER")){
        //加入已存在的玩家
        QString data = obj["ADDOLDPLAYER"].toString();
        add_exist_player(data);
    }
    else if(obj.contains("Ping")){
        return;
    }
    else if(obj.contains("msg")){
        QJsonObject msg = obj["msg"].toObject();
        QString name = msg["name"].toString();
        QString text = msg["text"].toString();
        QString msg_s = QString("%1  %2:  %3%4")
                .arg(QDateTime::currentDateTime().toString())
                .arg(name).arg(text)
                .arg(QString::fromUtf8(QByteArray::fromHex("0D0A")));
        ui->textEdit->append(msg_s);
    }
    else if(obj.contains("RemovePlayer")){
        QString name = obj["RemovePlayer"].toString();
        name_list.removeOne(name);
        name_model->setStringList(name_list);
        ui->listView->setModel(name_model);
    }
    else if(obj.contains("setcharacter")){
        set_character(obj["setcharacter"].toInt());
    }
    else if(obj.contains("set_start")){
        set_start(obj["set_start"].toInt());
    }
}

void MainWindow::send_msg(){

}

void MainWindow::on_pushButton_clicked()
{
    QString msg = ui->msg_edit->text();
    QJsonObject obj_1 = QJsonObject();
    obj_1.insert("name", ui->nickname_edit->text());
    obj_1.insert("text", msg);
    QJsonObject obj = QJsonObject();
    obj.insert("msg", obj_1);
    QJsonDocument doc(obj);
    QString msg_s = QString::fromUtf8(doc.toJson());
    send_to_server(msg_s);
    ui->msg_edit->clear();
}

void MainWindow::send_to_server(QString data){
    qint64 rec = m_socket->write(data.toStdString().data());

    if(rec == -1){
        emit add_log("Sending msg fail");
    }
    else
        emit add_log("Sending msg successfully");
}

void MainWindow::add_player(QString data){
    name_list.append(data);
    name_model->setStringList(name_list);
    ui->listView->setModel(name_model);
    QString msg = QString("Player %1 has joined the game").arg(data);
    emit add_log(msg);
}

void MainWindow::add_exist_player(QString data){
    //QString un_deal = data.remove(0, 13);
    //QStringList name_ = un_deal.split(",");
    QStringList name_ = data.split(",");

    foreach (QString name, name_) {
        name_list.append(name);
    }
    name_model->setStringList(name_list);
    ui->listView->setModel(name_model);
    QString msg = QString("已經載入已存在的玩家");
    emit add_log(msg);
}

void MainWindow::on_connect_b_clicked()
{
    if(ui->nickname_edit->text() != nullptr){
        connect_to_server();
    }
}

void MainWindow::set_character(int role){
    switch(role){
    case 1:{
        ui->label->setText("平民");
        break;
    }
    case 2:{
        ui->label->setText("平民");
        break;
    }
    case 3:{
        ui->label->setText("平民");
        break;
    }
    case 4:{
        ui->label->setText("狼人");
        break;
    }
    case 5:{
        ui->label->setText("狼人");
        break;
    }
    case 6:{
        ui->label->setText("狼人");
        break;
    }
    case 7:{
        ui->label->setText("預言家");
        break;
    }
    case 8:{
        ui->label->setText("女巫");
        break;
    }
    case 9:{
        ui->label->setText("獵人");
        break;
    }
    case 10:{
        ui->label->setText("平民");
        break;
    }
    }
}

void MainWindow::on_ready_b_clicked()
{
    if(!m_ready){
        QJsonObject obj = QJsonObject();
        obj.insert("ready", "true");
        QJsonDocument doc(obj);
        QString msg = QString::fromUtf8(doc.toJson());
        send_to_server(msg);
        m_ready = true;
    }
    else{
        QJsonObject obj = QJsonObject();
        obj.insert("ready", "false");
        QJsonDocument doc(obj);
        QString msg = QString::fromUtf8(doc.toJson());
        send_to_server(msg);
        m_ready = false;
    }
}

void MainWindow::set_start(int start){
    m_start = start;
    if(start){
        ui->ready_b->setEnabled(false);
    }
    else
        ui->ready_b->setEnabled(true);
}
