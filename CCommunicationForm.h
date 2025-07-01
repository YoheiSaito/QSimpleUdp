#ifndef CCOMMUNICATIONFORM_H
#define CCOMMUNICATIONFORM_H

#include <QWidget>
#include <QUdpSocket>
#include <memory>
namespace Ui {
class CCommunicationForm;
}

class CCommunicationForm : public QWidget
{
    Q_OBJECT

public:
    explicit CCommunicationForm(QWidget *parent = nullptr);
    ~CCommunicationForm();
    int64_t Recv(char* abyBuf, uint64_t uqwLen);
    int64_t Send(const char* abyBuf, uint64_t uqwLen);
signals:
    void StartReceive();
    void EndReceive();
    void ReadyToSend();
    void CloseToSend();
    void ReadyRead();

private slots:
    void OnReceived();
    void on_pcStart_toggled(bool checked);

private:
    Ui::CCommunicationForm *ui;

    QHostAddress m_tOwnAddr;
    QHostAddress m_tSendAddr;
    uint16_t     m_uwOwnPort;
    uint16_t     m_uwSendPort;
    std::shared_ptr<QUdpSocket> m_pcSendDevice;
    std::shared_ptr<QUdpSocket> m_pcRecvDevice;
};

#endif // CCOMMUNICATIONFORM_H
