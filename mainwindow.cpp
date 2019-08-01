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
#include <QSettings>
#include <QFileDialog>
#include <QLabel>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyleSheet("QWidget { background-color: rgba(100, 100, 100, 150); }"
                  "QToolTip { color:white; background-color:rgba(50, 50, 50); }");
    ui->lineEditSearch->setStyleSheet("color:white; border:1px solid gray;");
    ui->listWidget->setStyleSheet("QListWidget::Item { color:white; }"
                                  "QListWidget::Item:selected { background:#333333; border-radius:10px; }"
                                  "QListWidget::item:!selected:hover { background:#333333; border-radius:10px; }");
    ui->listWidget_kind->setStyleSheet("QListWidget::Item {color:white;}"
                                       "QListWidget::Item:selected { background:#333333; color:#6BC7FE;}");
    ui->pushButtonShutdown->setStyleSheet("QPushButton { border-image:url(:/shutdown.svg); background:transparent; }"
                                          "QPushButton::hover { border-image:url(:/shutdown_hover.svg) }"
                                          "QPushButton::focus { border-image:url(:/shutdown_hover.svg) }");
    ui->pushButtonMenu->setStyleSheet("QPushButton::menu-indicator { width:0px; }"
                                      "QPushButton { border-image:url(:/menu.svg); background:transparent; }"
                                      "QPushButton::hover { border-image:url(:/menu_hover.svg) }"
                                      "QPushButton::focus { border-image:url(:/menu_hover.svg) }");
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    move(0,QApplication::desktop()->height()-height());

    ui->listWidget_kind->addItem("全部");
    ui->listWidget_kind->addItem("网络");
    ui->listWidget_kind->addItem("社交");
    ui->listWidget_kind->addItem("音乐");
    ui->listWidget_kind->addItem("视频");
    ui->listWidget_kind->addItem("图像");
    ui->listWidget_kind->addItem("办公");
    ui->listWidget_kind->addItem("阅读");
    ui->listWidget_kind->addItem("编程");
    ui->listWidget_kind->addItem("系统");
    ui->listWidget_kind->addItem("用户");
    ui->listWidget_kind->addItem("自定义");
    for (int i=0; i<ui->listWidget_kind->count(); i++) {
        ui->listWidget_kind->item(i)->setTextAlignment(Qt::AlignCenter);
    }
    connect(ui->listWidget_kind,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(kindClicked(QListWidgetItem*)));

    action_emtpy = new QAction(this);
    action_emtpy->setIcon(QIcon(":/close.svg"));
    connect(action_emtpy, SIGNAL(triggered(bool)), this, SLOT(emptyLineEditSearch()));
    ui->lineEditSearch->addAction(action_emtpy, QLineEdit::TrailingPosition);
    action_emtpy->setVisible(false);
    connect(ui->lineEditSearch, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));
    connect(ui->lineEditSearch, SIGNAL(returnPressed()), this, SLOT(run()));
    connect(ui->listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(run(QModelIndex)));
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

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
    connect(logout, SIGNAL(triggered()), this, SLOT(logout()));
    connect(reboot, SIGNAL(triggered()), this, SLOT(reboot()));
    connect(suspend, SIGNAL(triggered()), this, SLOT(suspend()));
    connect(hibernate, SIGNAL(triggered()), this, SLOT(hibernate()));
    connect(lock, SIGNAL(triggered()), this, SLOT(lock()));
    connect(set, SIGNAL(triggered()), this, SLOT(dialogSet()));
    connect(about, SIGNAL(triggered()), this, SLOT(about()));

    listApp = genList("/usr/share/applications");
    listUser = genList(QDir::homePath() + "/.local/share/applications");
    listCustom = genList(readSettings(QDir::currentPath() + "/config.ini", "config", "CustomPath"));
    listAll = listApp + listUser + listCustom;

    ui->listWidget_kind->setCurrentRow(0);
    setList(listAll);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonShutdown_clicked()
{
    showMinimized();
    QProcess *proc = new QProcess;
    //proc->start("systemctl poweroff");
    proc->start("qdbus com.deepin.SessionManager /com/deepin/SessionManager com.deepin.SessionManager.RequestShutdown");
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
    //proc->start("systemctl reboot");
    proc->start("qdbus com.deepin.SessionManager /com/deepin/SessionManager com.deepin.SessionManager.RequestReboot");
}

void MainWindow::suspend()
{
    showMinimized();
    QProcess *proc = new QProcess;
    //proc->start("systemctl suspend");
    proc->start("qdbus com.deepin.SessionManager /com/deepin/SessionManager com.deepin.SessionManager.RequestSuspend");
}

void MainWindow::hibernate()
{
    showMinimized();
    //QProcess *proc = new QProcess;
    //proc->start("systemctl hibernate");
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
    connect(pushButton_path, SIGNAL(pressed()), this, SLOT(chooseCustomPath()));
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
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰开始菜单 3.0\n一款基于 Qt 的 Linux 开始菜单程序。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：https://github.com/sonichy\n\n3.0 2019-06\n使用QList(QMap)保存数据。\n\n2.0 2018-04\n用 QListWidget + QFileInfoList 重写，实现显示图标、分类。\n\n1.0 2017-03\nQListView + QFileSystemModel 生成列表，无法显示 desktop 图标，无法分类。");
    aboutMB.setIconPixmap(QPixmap(":/icon.png"));
    aboutMB.exec();
}

void MainWindow::search(QString text)
{
    if (text == "") {
        action_emtpy->setVisible(false);
    } else {
        action_emtpy->setVisible(true);
    }
    listSearch.clear();
    for (int i=0; i<listAll.size(); i++) {
        QMap<QString, QString> map = listAll.at(i);
        if (map.value("name").contains(text, Qt::CaseInsensitive) || map.value("exec").contains(text, Qt::CaseInsensitive)) {
            listSearch.append(map);
        }
    }
    setList(listSearch);
}

void MainWindow::run(QModelIndex index)
{
    Q_UNUSED(index);
    showMinimized();
    QMap<QString, QString> map = listNow.at(ui->listWidget->currentRow());
    QString MIME = QMimeDatabase().mimeTypeForFile(QFileInfo(map.value("filepath"))).name();
    if (MIME == "application/x-desktop") {
        QProcess *process = new QProcess;
        process->start(map.value("exec"));
    }
}

void MainWindow::kindClicked(QListWidgetItem* item)
{
    Q_UNUSED(item);
    qDebug() << "row " << ui->listWidget_kind->currentRow();
    switch (ui->listWidget_kind->currentRow()) {
    case 0:
        setList(listAll);
        break;
    case 1:
        setList(listNetwork);
        break;
    case 2:
        setList(listChat);
        break;
    case 3:
        setList(listMusic);
        break;
    case 4:
        setList(listVideo);
        break;
    case 5:
        setList(listGraphics);
        break;
    case 6:
        setList(listOffice);
        break;
    case 7:
        setList(listRead);
        break;
    case 8:
        setList(listProgram);
        break;
    case 9:
        setList(listSystem);
        break;
    case 10:
        setList(listUser);
        break;
    case 11:
        setList(listCustom);
        break;
    }
}

void MainWindow::customContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->listWidget->indexAt(pos);
    if (index.isValid()) {
        QListWidgetItem *LWI = ui->listWidget->itemAt(pos);
        QIcon icon = LWI->icon();
        QMap<QString, QString> map = listNow.at(ui->listWidget->currentRow());
        QString filepath = map.value("filepath");
        QString MIME = QMimeDatabase().mimeTypeForFile(QFileInfo(filepath)).name();
        QList<QAction*> actions;
        QAction *action_property = new QAction(this);
        action_property->setText("属性(&R)");
        actions.append(action_property);
        QAction *result_action = QMenu::exec(actions,ui->listWidget->mapToGlobal(pos));
        if (result_action == action_property) {
            QMessageBox MBox(QMessageBox::NoIcon, "属性", "文件名：\t" + QFileInfo(filepath).fileName() + "\n类型：\t" + QMimeDatabase().mimeTypeForFile(filepath).name() + "\n访问时间：\t" + QFileInfo(filepath).lastRead().toString("yyyy-MM-dd hh:mm:ss") + "\n修改时间：\t" + QFileInfo(filepath).lastModified().toString("yyyy-MM-dd hh:mm:ss"));
            if (MIME == "application/x-desktop") {
                QString sexec = map.value("exec");
                QString sname = map.value("name");
                QString spath = map.value("path");
                QString scomment = map.value("comment");;
                QString sCategories = map.value("categories");
                QString sMimeType = map.value("MimeType");
                MBox.setText("Desktop：" + filepath + "\n名称：" + sname + "\n运行：" + sexec + "\n路径：" + spath + "\n说明：" +scomment+ "\n类别：" + sCategories + "\nMimeType：" + sMimeType);
            }
            MBox.setIconPixmap(icon.pixmap(100,100));
            MBox.exec();
        }
    }
}

QString MainWindow::readSettings(QString path, QString group, QString key)
{
    QSettings setting(path, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    setting.beginGroup(group);
    QString value = setting.value(key).toString();
    return value;
}

void MainWindow::writeSettings(QString path, QString group, QString key, QString value)
{
    QSettings *settings = new QSettings(path, QSettings::IniFormat);
    settings->setIniCodec("UTF-8");
    settings->beginGroup(group);
    settings->setValue(key, value);
    settings->endGroup();
}

QList<QMap<QString, QString>> MainWindow::genList(QString spath)
{
    QList<QMap<QString, QString>> list;
    // 读取文件夹下所有文件 https://www.cnblogs.com/findumars/p/6006129.html
    QDir dir(spath);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    //dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList fileInfoList = dir.entryInfoList();   // 一句话文件信息转QList
    for (int i = 0; i < fileInfoList.size(); i++) {
        QFileInfo fileInfo = fileInfoList.at(i);
        QString MIME = QMimeDatabase().mimeTypeForFile(fileInfo.absoluteFilePath()).name();
        if (MIME == "application/x-desktop") {
            QString sname = "", spath1 = "", sexec = "",  sicon = "", scomment = "", scategories = "", sMimeType = "";
            QFile file(fileInfo.absoluteFilePath());
            file.open(QIODevice::ReadOnly);
            while(!file.atEnd()){
                QString sl = file.readLine().replace("\n","");
                //qDebug() << sl;
                if(sl.left(sl.indexOf("=")).toLower().trimmed() == "name") {
                    sname = sl.mid(sl.indexOf("=") + 1).trimmed();
                    continue;
                }
                if(sl.left(sl.indexOf("=")).toLower().trimmed() == "path"){
                    spath1 = sl.mid(sl.indexOf("=")+1).trimmed();
                    continue;
                }
                if(sl.left(sl.indexOf("=")).toLower().trimmed() == "exec"){
                    sexec = sl.mid(sl.indexOf("=") + 1).trimmed();
                    continue;
                }
                if(sl.left(sl.indexOf("=")).toLower().trimmed() == "icon"){
                    sicon = sl.mid(sl.indexOf("=")+1).trimmed();
                    continue;
                }
                if(sl.left(sl.indexOf("=")).toLower().trimmed() == "comment"){
                    scomment = sl.mid(sl.indexOf("=") + 1).trimmed();
                    continue;
                }
                if(sl.left(sl.indexOf("=")).toLower().trimmed() == "categories"){
                    scategories = sl.mid(sl.indexOf("=") + 1).trimmed();
                    continue;
                }
                if(sl.left(sl.indexOf("=")).trimmed() == "MimeType"){
                    sMimeType = sl.mid(sl.indexOf("=") + 1).trimmed();
                    continue;
                }
            }

            QMap<QString,QString> map;
            map.insert("filepath", fileInfo.absoluteFilePath());
            map.insert("name", sname);
            map.insert("path", spath1);
            map.insert("exec", sexec);
            map.insert("icon", sicon);
            map.insert("comment", scomment);
            map.insert("categories", scategories);
            map.insert("MimeType", sMimeType);
            list.append(map);

            if (scategories.contains("Music")) listMusic.append(map);
            if (scategories.contains("System")) listSystem.append(map);
            if (scategories.contains("Video")) listVideo.append(map);
            if (scategories.contains("Office")) listOffice.append(map);
            if (scategories.contains("Network")) listNetwork.append(map);
            if (scategories.contains("Program") || scategories.contains("Development") || scategories.contains("GTK")) listProgram.append(map);
            if (scategories.contains("Graphics")) listGraphics.append(map);
            if (scategories.contains("Chat")) listChat.append(map);
        }
    }
    return list;
}

void MainWindow::setList(QList<QMap<QString, QString>> list)
{
    listNow = list;
    ui->listWidget->clear();
    for (int i = 0; i < list.size(); i++) {
        QMap<QString, QString> map = list.at(i);
        QIcon icon;
        QString MIME = QMimeDatabase().mimeTypeForFile(map.value("filepath")).name();
        if (MIME == "application/x-desktop") {
            QString sicon = map.value("icon");
            if(sicon == ""){
                sicon = "applications-system-symbolic";
            }else{
                if (sicon.contains("/")) {
                    icon = QIcon(sicon);
                }else{
                    icon = QIcon::fromTheme(sicon);
                }
            }
//        }
//        else if (MIME == "inode/directory") {
//            icon = QIcon::fromTheme("folder");
        } else {
            icon = QIcon::fromTheme(MIME.replace("/", "-"));
        }
        QListWidgetItem *LWI;
        if(icon.isNull()) icon = QIcon::fromTheme("applications-system-symbolic");
        LWI = new QListWidgetItem(icon, map.value("name"));
        LWI->setSizeHint(QSize(100, 100));
        LWI->setToolTip(map.value("comment"));

        ui->listWidget->insertItem(i, LWI);
    }
    ui->listWidget->scrollToTop();
}

void MainWindow::focusOutEvent(QFocusEvent *e)
{
    Q_UNUSED(e);
    hide();
}

void MainWindow::emptyLineEditSearch()
{
    ui->lineEditSearch->setText("");
}

void MainWindow::run()
{
    QProcess *proc = new QProcess;
    proc->start(ui->lineEditSearch->text());
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    //painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.fillRect(rect(), QColor(0, 0, 0, 150));
}