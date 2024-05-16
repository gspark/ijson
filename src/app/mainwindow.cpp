#include "mainwindow.h"

#include "component/aboutdialog.h"
#include "common/config.h"
#include "jsonFormatter.h"
#include "common/icondb.h"

#include <QMenuBar>
#include <QApplication>
#include <QToolBar>
#include <QLabel>
#include <QFileDialog>
#include <QStatusBar>
#include <QScreen>

MainWindow::MainWindow(QWidget* parent) : WxWindow(parent)
{
    setWindowTitle(tr("ijson"));
    this->setWindowIcon(IconDb::instance()->getIcon(IconDb::app));

    setupMenuBar();
    setupToolBar();
    setupWidgets();
    initStatusBar();

    loadWindowInfo();
}

MainWindow::~MainWindow() {
    saveWindowInfo();
    this->deleteLater();
}

void MainWindow::setupWidgets() {
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    tabWidget->setTabsClosable(true);

    tabWidget->setMovable(true); // 设置用户是否可以在选项卡栏区域内移动选项卡，默认为false
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::tabCloseRequested);

    auto* formatter = new JsonFormatter(tabWidget);
    tabWidget->addTab(formatter, tr("General"));

    connect(formatter, &JsonFormatter::message, this, &MainWindow::onMessage);
    connect(formatter, &JsonFormatter::cursorPositionChanged, this, &MainWindow::cursorPositionChanged);
    connect(formatter, &JsonFormatter::textLengthChanged, this, &MainWindow::textLengthChanged);
}

void MainWindow::setupMenuBar() {
    // file menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    QAction* openAct = new QAction(tr("&Open..."), this);
    fileMenu->addAction(openAct);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);

    QAction* exitAction = fileMenu->addAction(tr("E&xit"), qApp, &QCoreApplication::quit, Qt::QueuedConnection);
    //exitAction->setShortcuts(QKeySequence::Quit);
    //exitAction->setShortcut(Qt::CTRL | Qt::Key_Q);

    // edit menu
    QMenu* viewMenu = menuBar()->addMenu(tr("&Edit"));
    QAction* formatAct = new QAction(tr("Format Document"), this);
    viewMenu->addAction(formatAct);
    connect(formatAct, &QAction::triggered, this, &MainWindow::formatDoc);
    formatAct->setShortcut(Qt::CTRL | Qt::ALT | Qt::Key_L);

    QAction* CompressJsonAct = new QAction(tr("Compress Json"), this);
    viewMenu->addAction(CompressJsonAct);
    connect(CompressJsonAct, &QAction::triggered, this, &MainWindow::compressJson);

    QAction* clearAct = new QAction(tr("Clear Document"), this);
    viewMenu->addAction(clearAct);
    connect(clearAct, &QAction::triggered, this, &MainWindow::clearDoc);

    // help menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction* aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setToolTip(tr("Show the application's About box"));

    //tabWidget->setCornerWidget(menuBar(), Qt::TopLeftCorner);
}

void MainWindow::setupToolBar()
{
    QToolBar* toolBar = this->toolBar();
    toolBar->setContentsMargins(0, 0, 0, 0);

    QAction* openNewTabAct = toolBar->addAction(IconDb::instance()->getIcon(IconDb::newTab), tr("Open new tab"));
    connect(openNewTabAct, &QAction::triggered, this, &MainWindow::openNewTab);
    openNewTabAct->setShortcut(Qt::CTRL | Qt::Key_T);

    toolBar->addSeparator();

    QAction* formatAct = toolBar->addAction(IconDb::instance()->getIcon(IconDb::format), tr("Format"));
    connect(formatAct, &QAction::triggered, this, &MainWindow::formatDoc);

    QAction* compressJsonAct = toolBar->addAction(IconDb::instance()->getIcon(IconDb::compress), tr("Compress"));
    connect(compressJsonAct, &QAction::triggered, this, &MainWindow::compressJson);

    //QAction* clearAct = toolBar->addAction(IconDb::instance()->getIcon(IconDb::clear), tr("Clear"));
    //connect(clearAct, &QAction::triggered, this, &MainWindow::clearDoc);

    QAction* searchForAct = toolBar->addAction(IconDb::instance()->getIcon(IconDb::find), tr("Search for"));
    connect(searchForAct, &QAction::triggered, this, &MainWindow::searchForDoc);
    searchForAct->setShortcut(Qt::CTRL | Qt::Key_F);
}

void MainWindow::initStatusBar()
{
    message = new QLabel();
    message->setText(tr(""));

    docInfo = new QLabel();
    docInfo->setText(tr("length:%1 lines:%2").arg(0).arg(0));

    position = new QLabel();
    position->setText(tr("Ln:%1 Col:%2").arg(0).arg(0));
    position->setMinimumWidth(position->sizeHint().width() + 6);

    statusBar()->addWidget(message, 1);
    statusBar()->addWidget(docInfo, 1);
    statusBar()->addWidget(position, 0);
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open the file"));
    JsonFormatter* formatter = static_cast<JsonFormatter*>(tabWidget->currentWidget());
    formatter->loadFile(fileName);
}

// show about message
void MainWindow::about() {
    QString strProductVersion = "0.6";
    QString strFileVersion = "0.6";

    AboutDialog::show(this, tr("About") + " ijson ", "ijson", strProductVersion);
}

void MainWindow::loadWindowInfo() {
    QVariant isMaximized = ConfigIni::getInstance().iniRead(QStringLiteral("Configuration/isMaximized"), QVariant());
    if (isMaximized.isValid() && isMaximized.toBool())
    {
        this->setWindowState(Qt::WindowMaximized);
    }
    else {
        QVariant geometry = ConfigIni::getInstance().iniRead(QStringLiteral("Configuration/geometry"), QVariant());
        if (geometry.isValid()) {
            bool result = restoreGeometry(geometry.toByteArray());
        }
        else {
            // resize window
            QSize aSize = qGuiApp->primaryScreen()->availableSize();
            resize(aSize * 0.618);
        }
    }
}

void MainWindow::saveWindowInfo()
{
    if (this->windowState() == Qt::WindowMaximized)
    {
        ConfigIni::getInstance().iniWrite(QStringLiteral("Configuration/isMaximized"), true);
    }
    else {
        ConfigIni::getInstance().iniWrite(QStringLiteral("Configuration/isMaximized"), false);
        ConfigIni::getInstance().iniWrite(QStringLiteral("Configuration/geometry"), this->saveGeometry());
    }
}

void MainWindow::openNewTab()
{
    tabWidget->addTab(new JsonFormatter(tabWidget), tr("General"));
}

void MainWindow::formatDoc()
{
    JsonFormatter* formatter = (JsonFormatter*)tabWidget->currentWidget();
    formatter->formatDoc();
}

void MainWindow::compressJson()
{
    JsonFormatter* formatter = (JsonFormatter*)tabWidget->currentWidget();
    formatter->compress();
}

void MainWindow::clearDoc()
{
    JsonFormatter* formatter = (JsonFormatter*)tabWidget->currentWidget();
    formatter->clear();
}

void MainWindow::searchForDoc()
{
    JsonFormatter* formatter = (JsonFormatter*)tabWidget->currentWidget();
    formatter->searchForDoc();
}

void MainWindow::onMessage(const QString message)
{
    this->message->setText(message);
}

void MainWindow::cursorPositionChanged(intptr_t line, int column)
{
    position->setText(tr("Ln:%1 Col:%2").arg(line + 1).arg(column + 1));
}

void MainWindow::textLengthChanged(intptr_t length, intptr_t lines)
{
    docInfo->setText(tr("length:%1 lines:%2").arg(length).arg(lines));
}

void MainWindow::tabCloseRequested(int index)
{
    if (index == -1)
    {
        return;
    }
    JsonFormatter* formatter = (JsonFormatter*)tabWidget->widget(index);
    if (tabWidget->count() > 1)
    {
        disconnect(formatter, 0, 0, 0);
        delete formatter;
        tabWidget->removeTab(index);
    }
    else {
        formatter->clear();
    }
}
