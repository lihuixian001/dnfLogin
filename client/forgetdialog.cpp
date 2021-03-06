#include "forgetdialog.h"
#include "ui_forgetdialog.h"
#include "aes_cbc.h"
#include "httpaesrequest.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QDebug>

forgetDialog::forgetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::forgetDialog)
{
    ui->setupUi(this);
}

forgetDialog::~forgetDialog()
{
    delete ui;
}

void forgetDialog::on_pushButton_clicked()
{
    QByteArray postData;

    if(ui->text_passwd1->text().size() <7
            || ui->text_qq->text().size() < 7){
        QMessageBox::warning(this,tr("Error"),tr("用户名、密码、安全码长度必须大于等于7"),QMessageBox::Ok);
        return;
    }

    if(ui->text_passwd1->text() != ui->text_passwd2->text()) {
        QMessageBox::warning(this,tr("Error"),tr("两次输入的密码不一样"),QMessageBox::Ok);
        return;
    }

    if(!ui->text_passwd1->isValidity()
            && !ui->text_qq->isValidity()
            && !ui->text_username->isValidity()) {
        QMessageBox::warning(this,tr("Warning"), tr("用户名或密码不合法，只能包含a-z、A-Z、0-9"), QMessageBox::Ok);
        return;
    }

    postData.append("forget|");
    postData.append(ui->text_username->text());
    postData.append("|");
    postData.append(QCryptographicHash::hash (
                        ui->text_passwd1->text().toAscii(),
                        QCryptographicHash::Md5).toHex());
    postData.append("|");
    postData.append(ui->text_qq->text());

    HttpAesRequest *loginRequest = new HttpAesRequest();

    connect(loginRequest,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(requestFinished(QNetworkReply*)));

    loginRequest->sendPOST(postData);
}

void forgetDialog::requestFinished(QNetworkReply *reply)
{
    AES_CBC aes;
    QString response;

    response.append(aes.decrypt(reply->readAll()));
    reply->deleteLater();

    qDebug() << response;

    if(response == "success") {
        QMessageBox::about(this,tr("success"), tr("重置成功"));
        this->close();
    }else{
        QMessageBox::warning(this,tr("Warning"), tr("重置失败，请检查安全码是否正确。"), QMessageBox::Ok);
    }
}
