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
    QFileSystemModel *model;

private:
    Ui::MainWindow *ui;
    QStandardItemModel *SIM;
    QString path;

private slots:
    void on_pushButtonShutdown_clicked();
    void logout();
    void reboot();
    void suspend();
    void hibernate();
    void lock();
    void about();
    void namefilter(QString text);
    void run(QModelIndex index);
    void highlight(QModelIndex index);
    void itemClick(QListWidgetItem* item);
    void viewContextMenu(const QPoint &position);
};

#endif // MAINWINDOW_H
