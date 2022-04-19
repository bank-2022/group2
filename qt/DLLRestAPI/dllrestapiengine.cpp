#include "dllrestapiengine.h"

DLLRestAPIEngine::DLLRestAPIEngine(QObject * parent) : QObject(parent)
{



}

DLLRestAPIEngine::~DLLRestAPIEngine()
{
    reply->deleteLater();
    manager->deleteLater();
}
//-----------------START OF INFO GATHERING--------------------------------------------------------
void DLLRestAPIEngine::Login(QString card, QString pin)
{
    QJsonObject jsonObj;

    jsonObj.insert("card_number",card);
    jsonObj.insert("pin",pin);

    QNetworkRequest request(base_url+"api/card/auth");

    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(loginSlot(QNetworkReply*)));

    reply = manager->post(request, QJsonDocument(jsonObj).toJson());
}

void DLLRestAPIEngine::loginSlot(QNetworkReply *reply)
{
    QByteArray response_data=reply->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonObject json_obj = json_doc.object();

    status = json_obj["status"].toString();
    resMessage = json_obj["message"].toString();

    if(status == "success"){
        auth = json_obj["token"].toString();
        auth = "Bearer "+auth;
        authByteArr = auth.toUtf8();
        qDebug()<<"LOG IN "<<status<<Qt::endl;
        GetUserInfo();
    }else if(status==NULL){
        qDebug()<<"express offline";
        reply->deleteLater();
        manager->deleteLater();
    }else if(status == "error"){
        qDebug() << this->resMessage;
        tries--;
        emit SendTriesSignal(tries);
    }else{
        qDebug()<<"Wrong pin code"<<json_obj;
    }

    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(loginSlot(QNetworkReply*)));

    emit AuthStatus(status);
}
void DLLRestAPIEngine::GetUserInfo()
{
    QNetworkRequest request(base_url+"api/user/info");

    manager = new QNetworkAccessManager(this);
    request.setRawHeader("Authorization", authByteArr);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getUserInfoSlot(QNetworkReply*)));

    reply = manager->get(request);
}

void DLLRestAPIEngine::getUserInfoSlot(QNetworkReply *reply)
{
    QByteArray response_data=reply->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonObject json_obj = json_doc.object();

    fname = json_obj["fname"].toString();
    lname = json_obj["lname"].toString();
    address = json_obj["address"].toString();
    email = json_obj["email"].toString();
    phone = json_obj["phone"].toString();
    profilePic = json_obj["picture"].toString();

    qDebug()<<"GET USERINFO "<<fname<<lname<<address<<email<<phone<<profilePic<<Qt::endl;

    GetPictureData(profilePic);
}


void DLLRestAPIEngine::GetPictureData(QString path)
{

    QNetworkRequest request(base_url+"uploads/"+path);
    manager = new QNetworkAccessManager(this);
    request.setRawHeader("Authorization", authByteArr);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getPictureDataSlot(QNetworkReply*)));

    reply = manager->get(request);
}

void DLLRestAPIEngine::getPictureDataSlot(QNetworkReply *reply)
{
    pictureData = reply->readAll();

    GetCardInfo();
}

void DLLRestAPIEngine::GetCardInfo()
{
    QNetworkRequest request(base_url+"api/card/"+card_number);
    manager = new QNetworkAccessManager(this);
    request.setRawHeader("Authorization", authByteArr);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getCardInfoSlot(QNetworkReply*)));

    reply = manager->get(request);
}

void DLLRestAPIEngine::getCardInfoSlot(QNetworkReply *reply)
{
    QByteArray response_data = reply->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonArray json_array = json_doc.array();

    foreach(const QJsonValue &value, json_array){
        QJsonObject obj = value.toObject();
        account_id_int = obj["account_ID"].toInt();
        card_type_int = obj["card_type"].toInt();
    }
    if(card_type_int == 0)
        card_type = "Debit";

    if(card_type_int == 1)
        card_type = "Credit";


    qDebug()<<"GET CARD INFO (account id) "<<account_id_int<<Qt::endl;

    GetAccountInfo();
}


void DLLRestAPIEngine::GetAccountInfo()
{

    QString requestStr = QStringLiteral("api/account/%1").arg(account_id_int);
    QNetworkRequest request(base_url+requestStr);
    request.setRawHeader("Authorization", authByteArr);
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAccountInfoSlot(QNetworkReply*)));

    reply = manager->get(request);
}


void DLLRestAPIEngine::getAccountInfoSlot(QNetworkReply *reply)
{
    QByteArray response_data = reply->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonArray json_array = json_doc.array();

    foreach(const QJsonValue &value, json_array){
        QJsonObject obj = value.toObject();
        account_name = obj["name"].toString();
        account_balance = obj["balance"].toDouble();
    }

    qDebug()<<"GET ACCOUNT INFO"<<account_balance<<account_name<<Qt::endl;

    emit dataGatheringFinished();
}

void DLLRestAPIEngine::GetLogs()
{
    if(logs_curret_page <= 0){
        logs_curret_page = 1;
        return;
    }else if(logs_curret_page > logs_total_pages){
        logs_curret_page = logs_total_pages;
        return;
    }

    QString requestUrl = "api/logs/getByCardNumber/"+card_number + "/" + QString::number(logs_curret_page);
    QNetworkRequest request(base_url+ requestUrl);
    manager = new QNetworkAccessManager(this);
    request.setRawHeader("Authorization", authByteArr);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getLogsSlot(QNetworkReply*)));

    reply = manager->get(request);

}

void DLLRestAPIEngine::getLogsSlot(QNetworkReply *reply)
{
    QByteArray response_data = reply->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonObject object = json_doc.object();
    QJsonValue value = object.value("data");
    QJsonArray array = value.toArray();

    logs_total_pages = object["maxPageAmount"].toInt();
    logs_amount_list.clear();
    logs_id_list.clear();
    logs_date_list.clear();
    logs_event_list.clear();
    foreach (const QJsonValue & v, array){
        logs_amount_list << QString::number(v.toObject().value("amount").toInt());
        logs_id_list << QString::number(v.toObject().value("log_ID").toInt());
        logs_date_list << v.toObject().value("date").toString();
        logs_event_list << v.toObject().value("event").toString();
    }

    for(int i = 0; i<10; i++){
        idSignal[i] = "";
        dateSignal[i] = "";
        eventSignal[i] = "";
        amountSignal[i] = "";
    }

    for(int i = 0; i<logs_id_list.count();i++){
        idSignal[i] = logs_id_list.at(i);
        dateSignal[i] = logs_date_list.at(i);
        eventSignal[i] = logs_event_list.at(i);
        amountSignal[i] = logs_amount_list.at(i);
    }


    for(int i = 0; i < 10; i++)
        qDebug()<<"GET LOGS WITH ID: "<<idSignal[i];

    emit logsFinishedSignal();
}
//-----------------END OF INFO GATHERING--------------------------------------------------------
void DLLRestAPIEngine::CreateLog(double amount)
{
    QJsonObject jsonObj;
    jsonObj.insert("amount",amount);
    jsonObj.insert("card_number",card_number);
    manager = new QNetworkAccessManager(this);
    QNetworkRequest request(base_url+"api/logs/create");
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    request.setRawHeader("Authorization", authByteArr);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(createLogSlot(QNetworkReply*)));
    qDebug()<<jsonObj<<"request body";
    reply = manager->post(request, QJsonDocument(jsonObj).toJson());

}

void DLLRestAPIEngine::createLogSlot(QNetworkReply *reply)
{
    status = "";
    QByteArray response_data=reply->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonObject json_obj = json_doc.object();
    qDebug()<<json_doc;
    status = json_obj["status"].toString();
    if(status == "success"){
        qDebug()<<"log added success";
    }else{
        qDebug()<<"error adding log";
    }
    GetLogs();
}

void DLLRestAPIEngine::GetTries(QString card_number){
    this->card_number = card_number;
    QString requestStr = QStringLiteral("api/card/tries/%1").arg(card_number);
    QNetworkRequest request(base_url+requestStr);
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getTriesSlot(QNetworkReply*)));

    reply = manager->get(request);
    qDebug() << "Send tries query";
}

void DLLRestAPIEngine::getTriesSlot(QNetworkReply *reply){


    QByteArray response_data=reply->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonObject json_obj = json_doc.object();

    tries = json_obj["tries"].toInt();
    tries = 3 - tries;

    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getTriesSlot(QNetworkReply*)));

    emit SendTriesSignal(tries);

    qDebug() << "Got tries from api" << tries;
}


void DLLRestAPIEngine::updateBalance(int action, double amount)
{
    QString requestStr = QStringLiteral("api/account/update");
    QJsonObject jsonObj;
    jsonObj.insert("card_number",card_number);
    jsonObj.insert("amount",amount);
    jsonObj.insert("action",QString::number(action));
    lastAction = action;
    lastTransaction = amount;
    QNetworkRequest request(base_url+requestStr);
    qDebug()<<jsonObj;
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    request.setRawHeader("Authorization", authByteArr);
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(updateBalanceSlot(QNetworkReply*)));

    reply = manager->put(request, QJsonDocument(jsonObj).toJson());
}

void DLLRestAPIEngine::updateBalanceSlot(QNetworkReply *reply)
{
    QByteArray response_data = reply->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonObject obj = json_doc.object();
    QString resp;

    account_balance = obj["balance"].toDouble();
    resp = obj["message"].toString();
    if(resp == ""){
    qDebug()<<"CURRENT BALANCE "<<account_balance<<Qt::endl;
    qDebug()<<"BALANCE UPDATE RESPONSE "<<resp<<Qt::endl;
    int tempAction = 0;
        if(lastAction == 0)
            tempAction = -1;
        if(lastAction == 1)
            tempAction = 1;

    emit balanceUpdated(account_balance);
    CreateLog(lastTransaction*tempAction);
    }else{
        qDebug()<<"Error updating balance";
        qDebug()<<resp;
    }
}
