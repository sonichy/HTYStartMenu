//com.deepin.SessionManager方法大全：https://github.com/linuxdeepin/dbus/blob/aabb7f4960ece3a37e29878f1b6e25a240a3d57e/docs/com_deepin_SessionManager.mkd

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QProcess>
QStandardItemModel *SIM;
QString path;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setStyleSheet("QPushButton::menu-indicator{width:0px;}");
    ui->pushButtonMenu->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    setWindowFlags(Qt::FramelessWindowHint);
    move(0,QApplication::desktop()->height()-height());

    ui->listWidget->addItem("全部");
    ui->listWidget->addItem("网络");
    ui->listWidget->addItem("社交");
    ui->listWidget->addItem("音乐");
    ui->listWidget->addItem("视频");
    ui->listWidget->addItem("图像");
    ui->listWidget->addItem("办公");
    ui->listWidget->addItem("阅读");
    ui->listWidget->addItem("编程");
    ui->listWidget->addItem("系统");
    ui->listWidget->addItem("其他");
    for(int i=0; i<ui->listWidget->count(); i++){
        ui->listWidget->item(i)->setTextAlignment(Qt::AlignCenter);
    }
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClick(QListWidgetItem*)));

    model=new QFileSystemModel;
    // 设置model监视的目录，其下的修改会立刻signal通知view
    model->setRootPath("/usr/share/applications");
    // 没有通过过滤器的文件，true为不可用，false为隐藏
    model->setNameFilterDisables(false);
    ui->listView->setModel(model);
    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setGridSize(QSize(80,80));
    ui->listView->setWordWrap(true);
    // 设置view显示的目录
    ui->listView->setRootIndex(model->index("/usr/share/applications"));
    connect(ui->lineEditSearch,SIGNAL(textChanged(QString)),this,SLOT(namefilter(QString)));
    connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(run(QModelIndex)));
    connect(ui->listView,SIGNAL(entered(QModelIndex)),this,SLOT(highlight(QModelIndex)));
    //connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>),model,SLOT());

    QMenu *shutmenu=new QMenu;
    QAction *logout=new QAction("注销",this);
    QAction *reboot=new QAction("重启",this);
    QAction *suspend=new QAction("待机",this);
    QAction *hibernate=new QAction("休眠",this);
    QAction *lock=new QAction("锁定",this);
    QAction *about=new QAction("关于",this);
    shutmenu->addAction(logout);
    shutmenu->addAction(reboot);
    shutmenu->addAction(suspend);
    shutmenu->addAction(hibernate);
    shutmenu->addAction(lock);
    shutmenu->addAction(about);
    ui->pushButtonMenu->setMenu(shutmenu);
    connect(logout,SIGNAL(triggered()),this,SLOT(logout()));
    connect(reboot,SIGNAL(triggered()),this,SLOT(reboot()));
    connect(suspend,SIGNAL(triggered()),this,SLOT(suspend()));
    connect(hibernate,SIGNAL(triggered()),this,SLOT(hibernate()));
    connect(lock,SIGNAL(triggered()),this,SLOT(lock()));
    connect(about,SIGNAL(triggered()),this,SLOT(about()));

    path="/media/sonichy/job/HY/Linux/Qt/";
    QStandardItem *SI1,*SI2,*SI3,*SI4,*SI5,*SI6,*SI7;
    SI1=new QStandardItem(QIcon(path+"HTYEdit/icon.png"),"文本编辑器");
    SI2=new QStandardItem(QIcon(path+"HTYMP/icon.png"),"媒体播放器");
    SI3=new QStandardItem(QIcon(path+"HTYScreenshot/icon.png"),"截图");
    SI4=new QStandardItem(QIcon(path+"HTYPaint/icon.png"),"画图");
    SI5=new QStandardItem(QIcon(path+"HTYGIFPlayer/icon.png"),"GIF播放器");
    SI6=new QStandardItem(QIcon(path+"HTYDown/icon.png"),"下载");
    SI7=new QStandardItem(QIcon(path+"HTYMPAV/icon.png"),"媒体播放器QtAV");
    SIM=new QStandardItemModel(this);
    SIM->appendRow(SI1);
    SIM->appendRow(SI2);
    SIM->appendRow(SI3);
    SIM->appendRow(SI4);
    SIM->appendRow(SI5);
    SIM->appendRow(SI6);
    SIM->appendRow(SI7);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonShutdown_clicked()
{
    showMinimized();    
    QProcess *proc = new QProcess;
    proc->start("systemctl poweroff");
}

void MainWindow::logout()
{
    showMinimized();    
    QProcess *proc = new QProcess;
    proc->start("qdbus com.deepin.SessionManager /com/deepin/SessionManager com.deepin.SessionManager.RequestLogout");
    //proc->start("systemctl restart lightdm");
}

void MainWindow::reboot()
{
    showMinimized();    
    QProcess *proc = new QProcess;
    proc->start("systemctl reboot");
}

void MainWindow::suspend()
{
    showMinimized();    
    QProcess *proc = new QProcess;
    proc->start("systemctl suspend");
}

void MainWindow::hibernate()
{
    showMinimized();
    QProcess *proc = new QProcess;
    proc->start("systemctl hibernate");
}

void MainWindow::lock()
{
    showMinimized();    
    QProcess *proc = new QProcess;
    //proc->start("systemctl lock");
    proc->start("qdbus com.deepin.SessionManager /com/deepin/SessionManager com.deepin.SessionManager.RequestLock");
}

void MainWindow::namefilter(QString text)
{
    QStringList filter;
    filter << "*"+text+"*";
    model->setNameFilters(filter);
}

void MainWindow::run(QModelIndex index)
{
    showMinimized();
    QString filename=index.data().toString();
    //filename.truncate(filename.lastIndexOf("."));
    //system(filename.toLatin1().data());
    qDebug() << filename;
    QString cmd;
    QString str = "文本编辑器,媒体播放器,截图,画图,GIF播放器,下载,媒体播放器QtAV";
    QStringList name = str.split(",");
    switch(name.indexOf(filename)){
    case 0:
        cmd = path + "HTYEdit/HTYEdit";
        break;
    case 1:
        cmd = path + "HTYMP/HTYMP";
        break;
    case 2:
        cmd = path + "HTYScreenshot/HTYScreenshot";
        break;
    case 3:
        cmd = path + "HTYPaint/HTYPaint";
        break;
    case 4:
        cmd = path + "HTYGIFPlayer/HTYGIFPlayer";
        break;
    case 5:
        cmd = path + "HTYDown/HTYDown";
        break;
    case 6:
        cmd = path + "HTYMPAV/HTYMPAV";
        break;
    default:
        filename.truncate(filename.lastIndexOf("."));
        cmd = filename;
    }
    QProcess *proc = new QProcess;
    proc->start(cmd);
}

void MainWindow::highlight(QModelIndex index)
{
    //qDebug() << index;
    //qDebug() << "setData:Icon" << model->setData(index, QIcon("icon.png"), Qt::DecorationRole);
    //qDebug() << "setData:Text" << model->setData(index, QVariant("深度"), Qt::DisplayRole);
}

void MainWindow::about()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰开始菜单 1.0\n一款基于Qt的开始菜单程序。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：sonichy.96.lt\n\n1.0\n2017-05\n实现注销、锁定。\n2017-04\n实现关机、重启、待机。\n2017-03\n实现搜索\n设计界面");
    aboutMB.setIconPixmap(QPixmap(":/icon.png"));
    aboutMB.exec();
}

void MainWindow::itemClick(QListWidgetItem* item)
{
    Q_UNUSED(item);
    //qDebug() << "row " << ui->listWidget->currentRow();
    switch (ui->listWidget->currentRow()) {
    case 0:
        ui->listView->setModel(model);
        ui->listView->setRootIndex(model->index("/usr/share/applications"));
        break;
    case 10:
        ui->listView->setModel(SIM);
        break;
    default:
        break;
    }
}
