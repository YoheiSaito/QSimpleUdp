#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class CMainWindow;
}
QT_END_NAMESPACE

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();
protected slots:
    void OnStartReceive();
    void OnEndReceive();
    void OnReadyToSend();
    void OnCloseToSend();
    void OnReadyToRead();
private:
    Ui::CMainWindow *ui;
};
#endif // CMAINWINDOW_H
