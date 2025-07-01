#include "CMainWindow.h"
#include "ui_CMainWindow.h"

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CMainWindow)
{
    ui->setupUi(this);
    connect(ui->pcCom1, &CCommunicationForm::StartReceive, this, &CMainWindow::OnStartReceive);
    connect(ui->pcCom1, &CCommunicationForm::EndReceive, this, &CMainWindow::OnEndReceive);
    connect(ui->pcCom1, &CCommunicationForm::ReadyToSend, this, &CMainWindow::OnReadyToSend);

    connect(ui->pcCom2, &CCommunicationForm::StartReceive, this, &CMainWindow::OnStartReceive);
    connect(ui->pcCom2, &CCommunicationForm::EndReceive, this, &CMainWindow::OnEndReceive);
    connect(ui->pcCom2, &CCommunicationForm::ReadyToSend, this, &CMainWindow::OnReadyToSend);
}

CMainWindow::~CMainWindow()
{
    delete ui;
}
void CMainWindow::OnStartReceive()
{
    CCommunicationForm* pcForm = dynamic_cast<CCommunicationForm*>(sender());
    if( pcForm )
    {
        connect(pcForm, &CCommunicationForm::ReadyRead, this, &CMainWindow::OnReadyToRead);
    }
}
void CMainWindow::OnEndReceive()
{
    CCommunicationForm* pcForm = dynamic_cast<CCommunicationForm*>(sender());
    if( pcForm )
    {
        disconnect(pcForm, &CCommunicationForm::ReadyRead, this, &CMainWindow::OnReadyToRead);
    }
}
void CMainWindow::OnReadyToSend()
{
    CCommunicationForm* pcSend = dynamic_cast<CCommunicationForm*>(sender());
    if( pcSend )
    {
        pcSend->Send("Hello", 6);
    }
}
void CMainWindow::OnCloseToSend()
{

}
void CMainWindow::OnReadyToRead()
{
    char achBuff[64];
    CCommunicationForm* pcRecv = dynamic_cast<CCommunicationForm*>(sender());
    if( pcRecv )
    {
        int dwRet = pcRecv->Recv(achBuff, sizeof(achBuff));
        pcRecv->Send(achBuff, dwRet);
    }
}
