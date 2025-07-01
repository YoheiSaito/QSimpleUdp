#include "CCommunicationForm.h"
#include "ui_CCommunicationForm.h"
#include <QRegularExpressionValidator>
#include <QRegularExpression>
CCommunicationForm::CCommunicationForm(QWidget *pcWidget)
    : QWidget(pcWidget)
    , ui(new Ui::CCommunicationForm)
{
    ui->setupUi(this);
}

CCommunicationForm::~CCommunicationForm()
{
    delete ui;
}

int64_t CCommunicationForm::Recv(char* abyBuf, uint64_t uqwLen)
{
    int64_t qwRet = -1;
    if( m_pcRecvDevice )
    {
        qwRet = m_pcRecvDevice->readDatagram(abyBuf, uqwLen);
        if( qwRet < 0 )
        {
            ui->pcLogEdit->append(QString("error on recv %1")
                                       .arg(m_pcRecvDevice->errorString()));
        }
        else
        {
            ui->pcLogEdit->append(QString("recv %1 byte").arg(qwRet));
        }
    }
    return qwRet;
}
int64_t CCommunicationForm::Send(const char* abyBuf, uint64_t uqwLen)
{
    int64_t qwRet = -1;
    if( m_pcSendDevice )
    {
        qwRet = m_pcSendDevice->writeDatagram(abyBuf, uqwLen, m_tSendAddr, m_uwSendPort);
        if( qwRet < 0 )
        {
            ui->pcLogEdit->append(QString("error on send %1")
                                       .arg(m_pcSendDevice->errorString()));
        }
        else
        {
            ui->pcLogEdit->append(QString("send %1 byte").arg(qwRet));
        }
    }
    return qwRet;
}
void CCommunicationForm::OnReceived()
{
    emit ReadyRead();
}
void CCommunicationForm::on_pcStart_toggled(bool bChecked)
{
    static const QRegularExpression G_cIpValidator_C(
        (R"(^((25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)(\.|$)){4}$)")
    );
    static const QRegularExpressionValidator tValidator(G_cIpValidator_C);

    if( bChecked )
    {
        ui->pcLogEdit->clear();
        int iPos;
        iPos = 0;
        QString sOwnAddr = ui->pcOwnAddress->text();
        // IFを設定する
        if( tValidator.validate(sOwnAddr, iPos) )
        {
            m_tOwnAddr.setAddress(sOwnAddr);
        }
        else
        {
            m_tOwnAddr = QHostAddress::AnyIPv4;
            ui->pcLogEdit->append( sOwnAddr + u8"はipアドレスとしてマッチしません");
            sOwnAddr.clear();
        }
        // 送信先の設定
        iPos = 0;
        QString sSendAddr = ui->pcSendAddress->text();
        if( tValidator.validate(sSendAddr, iPos) )
        {
            m_tSendAddr.setAddress(sSendAddr);
        }
        else
        {
            m_tSendAddr = QHostAddress::AnyIPv4;
            ui->pcLogEdit->append( sSendAddr + u8"はipアドレスとしてマッチしません");
            sSendAddr.clear();
        }

        m_uwOwnPort = ui->pcOwnPort->value();
        m_uwSendPort = ui->pcSendPort->value();
        const bool bRecv = m_uwOwnPort != 0;
        const bool bSend = !sSendAddr.isEmpty() && m_uwSendPort != 0;
        if( bRecv )
        {
            m_pcRecvDevice.reset(new QUdpSocket);
            if( m_tSendAddr.isMulticast() )
            {
                ui->pcLogEdit->append( QString("マルチキャスト受信 %1=>%2:%3")
                                          .arg(sSendAddr)
                                          .arg(sOwnAddr)
                                          .arg(m_uwOwnPort)
                );
                if( ! m_pcRecvDevice->bind(m_tOwnAddr, m_uwOwnPort) )
                {
                    // マルチキャスト志望だがバインド失敗
                    ui->pcLogEdit->append( QString("error %1").arg(m_pcRecvDevice->errorString() ));
                    m_pcRecvDevice = nullptr;
                }
                else
                {
                    // マルチキャスト
                    m_pcRecvDevice->joinMulticastGroup(m_tSendAddr);
                    m_pcRecvDevice->setSocketOption(QUdpSocket::MulticastTtlOption, 16);
                }
            }
            else
            {
                // ユニキャスト
                if( !m_pcRecvDevice->bind(m_tOwnAddr, m_uwOwnPort) )
                {
                    // 失敗
                    m_pcRecvDevice = nullptr;
                }
            }
            if( bSend )
            {
                m_pcSendDevice = m_pcRecvDevice;
            }
        }

        if( bSend )
        {
            // 送受信を同時に行う場合は送信ソケットの実体を受信と同じものにする
            // 送信は同一ソケットでない場合のみ作成する
            if( !m_pcSendDevice )
            {
                m_pcSendDevice.reset(new QUdpSocket);
                if( m_tSendAddr.isMulticast() )
                {
                    ui->pcLogEdit->append( QString("マルチキャスト送信 %1=>%2:%3")
                                              .arg(sOwnAddr)
                                              .arg(sSendAddr)
                                              .arg(m_uwSendPort)
                                          );
                    if ( m_pcSendDevice->bind(m_tOwnAddr, 0) )
                    {
                        m_pcSendDevice->joinMulticastGroup(m_tSendAddr);
                        m_pcSendDevice->setSocketOption(QUdpSocket::MulticastTtlOption, 16);
                    }
                    else
                    {
                        ui->pcLogEdit->append( QString("error %1").arg(m_pcSendDevice->errorString() ));
                        m_pcSendDevice = nullptr;
                    }
                }
            }
        }
        if( m_pcRecvDevice )
        {
            ui->pcLogEdit->append(QString("Port(%1)で受信します").arg(m_uwOwnPort));
            connect(m_pcRecvDevice.get(), &QUdpSocket::readyRead, this, &CCommunicationForm::OnReceived);
            emit StartReceive();
        }
        else
        {
            ui->pcLogEdit->append("受信は行いません");
        }

        if( m_pcSendDevice )
        {
            ui->pcLogEdit->append(QString("%1:%2 に送信します")
                                      .arg(sSendAddr)
                                      .arg(m_uwSendPort));
            emit ReadyToSend();
        }
        else
        {
            ui->pcLogEdit->append("送信は行いません");
        }
        if( !m_pcRecvDevice && !m_pcSendDevice )
        {
            QSignalBlocker(ui->pcStart);
            bChecked = false;
            ui->pcStart->setChecked(false);
        }
    }
    else
    {
        ui->pcLogEdit->clear();
        if( m_pcSendDevice )
        {
            ui->pcLogEdit->append("Close Send Socket");
            emit CloseToSend();
            m_pcSendDevice = nullptr;
        }
        if( m_pcRecvDevice )
        {
            ui->pcLogEdit->append("Close Recv Socket");
            disconnect(m_pcRecvDevice.get(), &QUdpSocket::readyRead, this, &CCommunicationForm::OnReceived);
            emit EndReceive();
            m_pcRecvDevice = nullptr;
        }
    }
    if( bChecked )
    {
        ui->pcStart->setText( u8"通信中" );
    }
    else
    {
        ui->pcStart->setText( u8"通信開始" );
    }
    ui->pcOwnAddress->setReadOnly(bChecked);
    ui->pcSendAddress->setReadOnly(bChecked);
    ui->pcOwnPort->setReadOnly(bChecked);
    ui->pcSendPort->setReadOnly(bChecked);


}

