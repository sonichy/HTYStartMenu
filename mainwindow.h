#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define DESKTOP_PATH Qt::UserRole + 1
#define DESKTOP_EXEC Qt::UserRole + 2

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
    QLineEdit *lineEdit_custompath;
    QAction *action_emtpy;
    QStandardItemModel *SIM;
    QString path;
    QList<QMap<QString,QString>> listAll, listUser, listCustom, listApp, listSystem, listMusic, listVideo, listNetwork, listGraphics, listOffice, listProgram, listRead, listChat, listSearch, listNow;
    QList<QMap<QString,QString>> genList(QString spath);
    QString readSettings(QString path, QString group, QString key);
    void writeSettings(QString path, QString group, QString key, QString value);
    void paintEvent(QPaintEvent *e);

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
    void setList(QList<QMap<QString, QString> > list);
    void emptyLineEditSearch();
    void run();
};

#endif // MAINWINDOW_H
