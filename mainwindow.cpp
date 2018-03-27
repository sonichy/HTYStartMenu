//com.deepin.SessionManager方法大全：https://github.com/linuxdeepin/dbus/blob/aabb7f4960ece3a37e29878f1b6e25a240a3d57e/docs/com_deepin_SessionManager.mkd

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QMimeDatabase>
#include <QDateTime>
#include <QFileIconProvider>
#include <QSettings>
#include <QFileDialog>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    ui->listWidget->addItem("用户");
    ui->listWidget->addItem("自定义");
    for(int i=0; i<ui->listWidget->count(); i++){
        ui->listWidget->item(i)->setTextAlignment(Qt::AlignCenter);
    }
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClick(QListWidgetItem*)));

    model = new QFileSystemModel;
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
    connect(ui->lineEditSearch,SIGNAL(textChanged(QString)),this,SLOT(nameFilter(QString)));
    connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(run(QModelIndex)));
    connect(ui->listView,SIGNAL(entered(QModelIndex)),this,SLOT(highlight(QModelIndex)));
    //connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>),model,SLOT());
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(viewContextMenu(QPoint)));

    QMenu *shutmenu = new QMenu;
    QAction *logout = new QAction("注销",this);
    QAction *reboot = new QAction("重启",this);
    QAction *suspend = new QAction("待机",this);
    QAction *hibernate = new QAction("休眠",this);
    QAction *lock = new QAction("锁定",this);
    QAction *set = new QAction("设置",this);
    QAction *about = new QAction("关于",this);
    shutmenu->addAction(logout);
    shutmenu->addAction(reboot);
    shutmenu->addAction(suspend);
    shutmenu->addAction(hibernate);
    shutmenu->addAction(lock);
    shutmenu->addAction(set);
    shutmenu->addAction(about);
    ui->pushButtonMenu->setMenu(shutmenu);
    connect(logout,SIGNAL(triggered()),this,SLOT(logout()));
    connect(reboot,SIGNAL(triggered()),this,SLOT(reboot()));
    connect(suspend,SIGNAL(triggered()),this,SLOT(suspend()));
    connect(hibernate,SIGNAL(triggered()),this,SLOT(hibernate()));
    connect(lock,SIGNAL(triggered()),this,SLOT(lock()));
    connect(set,SIGNAL(triggered()),this,SLOT(dialogSet()));
    connect(about,SIGNAL(triggered()),this,SLOT(about()));
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

void MainWindow::dialogSet()
{
    QDialog *dialog = new QDialog;
    dialog->setFixedWidth(300);
    dialog->setWindowTitle("设置");
    QGridLayout *gridLayout = new QGridLayout;
    QLabel *label = new QLabel("自定义路径");
    gridLayout->addWidget(label,0,0,1,1);
    lineEdit_custompath = new QLineEdit;
    QString customPath = readSettings(QDir::currentPath() + "/config.ini", "config", "CustomPath");
    lineEdit_custompath->setText(customPath);
    lineEdit_custompath->setCursorPosition(0);
    gridLayout->addWidget(lineEdit_custompath,0,1,1,1);
    QPushButton *pushButton_path = new QPushButton;
    pushButton_path->setObjectName("SetDialogCustomPath");
    pushButton_path->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    connect(pushButton_path,SIGNAL(pressed()),this,SLOT(chooseCustomPath()));
    gridLayout->addWidget(pushButton_path,0,2,1,1);
    dialog->setLayout(gridLayout);
    dialog->show();
}

void MainWindow::chooseCustomPath()
{
    QString path = QFileDialog::getExistingDirectory(this, "自定义路径", lineEdit_custompath->text(), QFileDialog::ShowDirsOnly |QFileDialog::DontResolveSymlinks);
    if(path != ""){
        lineEdit_custompath->setText(path);
        lineEdit_custompath->setCursorPosition(0);
        writeSettings(QDir::currentPath() + "/config.ini", "config", "CustomPath", path);
    }
}

void MainWindow::about()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰开始菜单 1.0\n一款基于 Qt 的开始菜单程序。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：https://github.com/sonichy\n\n1.0\n2018-03\n增加：用户路径，一个自定义路径，设置对话框和一个自定义路径设置。修复：非系统路径不可搜索。\n2017-05\n实现注销、锁定。\n2017-04\n实现关机、重启、待机。\n2017-03\n实现搜索\n设计界面");
    aboutMB.setIconPixmap(QPixmap(":/icon.png"));
    aboutMB.exec();
}

void MainWindow::nameFilter(QString text)
{
    QStringList filter;
    filter << "*" + text + "*";
    model->setNameFilters(filter);
}

void MainWindow::run(QModelIndex index)
{
    showMinimized();
    QString filepath = index.data(QFileSystemModel::FilePathRole).toString();
    QString MIME = QMimeDatabase().mimeTypeForFile(filepath).name();
    if (MIME == "application/x-desktop") {
        QString sexec = "";
        QFile file(filepath);
        file.open(QIODevice::ReadOnly);
        while(!file.atEnd()){
            QString sl = file.readLine().replace("\n","");
            if(sl.left(sl.indexOf("=")).toLower() == "exec"){
                sexec = sl.mid(sl.indexOf("=")+1);
                continue;
            }
        }
        qDebug() << "run" << sexec;
        QProcess *proc = new QProcess;
        proc->start(sexec);
    }
}

void MainWindow::highlight(QModelIndex index)
{
    //qDebug() << index;
    model->setData(index, QIcon(":/icon.png"), Qt::DecorationRole);
    model->setData(index, QString("深度"), Qt::DisplayRole);
}

void MainWindow::itemClick(QListWidgetItem* item)
{
    Q_UNUSED(item);
    //qDebug() << "row " << ui->listWidget->currentRow();
    switch (ui->listWidget->currentRow()) {
    case 0:
        model->setRootPath("/usr/share/applications");
        ui->listView->setRootIndex(model->index("/usr/share/applications"));
        break;
    case 9:{
        QProcess *process = new QProcess;
        process->start("dbus-send --print-reply --dest=com.deepin.dde.ControlCenter /com/deepin/dde/ControlCenter com.deepin.dde.ControlCenter.ShowModule \"string:systeminfo\"");
        break;}
    case 10:
        model->setRootPath(QDir::homePath() + "/.local/share/applications");
        ui->listView->setRootIndex(model->index(QDir::homePath() + "/.local/share/applications"));
        break;
    case 11:
        QString customPath = readSettings(QDir::currentPath() + "/config.ini", "config", "CustomPath");
        model->setRootPath(customPath);
        ui->listView->setRootIndex(model->index(customPath));
        break;
    }
}

void MainWindow::viewContextMenu(const QPoint &position)
{
    QModelIndex index = ui->listView->indexAt(position);
    QString filepath = index.data(QFileSystemModel::FilePathRole).toString();
    qDebug() << filepath;
    QString MIME = QMimeDatabase().mimeTypeForFile(filepath).name();
    qDebug() << MIME;
    QList<QAction *> actions;
    QAction *action_property = new QAction(this);
    action_property->setText("属性");
    actions.append(action_property);
    QAction *result_action = QMenu::exec(actions,ui->listView->mapToGlobal(position));
    if (result_action == action_property) {
        QMessageBox MBox(QMessageBox::NoIcon, "属性", "文件名：\t" + QFileInfo(filepath).fileName() + "\n类型：\t" + QMimeDatabase().mimeTypeForFile(filepath).name() + "\n访问时间：\t" + QFileInfo(filepath).lastRead().toString("yyyy-MM-dd hh:mm:ss") + "\n修改时间：\t" + QFileInfo(filepath).lastModified().toString("yyyy-MM-dd hh:mm:ss"));
        if (MIME=="application/x-desktop") {
            QString sname = "", sexec = "", spath = "", scomment = "", iconpath = "";
            QFile file(filepath);
            file.open(QIODevice::ReadOnly);
            while(!file.atEnd()){
                QString sl = file.readLine().replace("\n","");
                //qDebug() << sl;
                if(sl.left(sl.indexOf("=")).toLower() == "name") {
                    sname = sl.mid(sl.indexOf("=")+1);
                    continue;
                }
                if(sl.left(sl.indexOf("=")).toLower() == "exec"){
                    sexec = sl.mid(sl.indexOf("=")+1);
                    continue;
                }
                if(sl.left(sl.indexOf("=")).toLower() == "icon"){
                    iconpath = sl.mid(sl.indexOf("=")+1);
                    continue;
                }
                if(sl.left(sl.indexOf("=")).toLower() == "path"){
                    spath = sl.mid(sl.indexOf("=")+1);
                    continue;
                }
                if(sl.left(sl.indexOf("=")).toLower()=="comment"){
                    scomment=sl.mid(sl.indexOf("=")+1);
                    continue;
                }
            }
            MBox.setText("名称：" + sname + "\n运行：" + sexec + "\n路径：" + spath + "\n说明：" +scomment);
            MBox.setIconPixmap(QPixmap(iconpath).scaled(100,100,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        }else
        if(MIME == "inode/directory") {
            QFileInfo fileinfo(filepath);
            QFileIconProvider iconProvider;
            QIcon icon = iconProvider.icon(fileinfo);
            MBox.setIconPixmap(icon.pixmap(QSize(128,128)));
        }else{
            MBox.setIconPixmap(QPixmap("/usr/share/icons/deepin/mimetypes/128/" + MIME.replace("/","-")+".svg"));
        }
        MBox.exec();
    }
}

QString MainWindow::readSettings(QString path, QString group, QString key)
{
    QSettings setting(path, QSettings::IniFormat);
    setting.beginGroup(group);
    QString value = setting.value(key).toString();
    return value;
}

void MainWindow::writeSettings(QString path, QString group, QString key, QString value)
{
    QSettings *config = new QSettings(path, QSettings::IniFormat);
    config->beginGroup(group);
    config->setValue(key, value);
    config->endGroup();
}
