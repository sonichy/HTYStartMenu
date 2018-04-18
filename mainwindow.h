#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QFileSystemModel>
#include <QListWidgetItem>
#include <QStandardItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    

private:
    Ui::MainWindow *ui;
    QStandardItemModel *SIM;
    QString path;
    QString readSettings(QString path, QString group, QString key);
    void writeSettings(QString path, QString group, QString key, QString value);
    QLineEdit *lineEdit_custompath;
    QFileInfoList genList(QString spath);
    QFileInfoList listAll, listApp, listUser, listCustom, listSystem, listMusic, listVideo, listNetwork, listGraphics, listOffice, listProgram, listRead, listChat, listSearch;

protected:
    virtual void focusOutEvent(QFocusEvent *e);

private slots:
    void on_pushButtonShutdown_clicked();
    void logout();
    void reboot();
    void suspend();
    void hibernate();
    void lock();
    void dialogSet();
    void about();
    void search(QString text);
    void run(QModelIndex index);
    void kindClicked(QListWidgetItem *item);
    void customContextMenu(const QPoint &pos);
    void chooseCustomPath();
    void setList(QFileInfoList list);

};

#endif // MAINWINDOW_H
