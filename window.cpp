#include "window.h"
#include <QMessageBox>
#include <QDir>
#include <QGuiApplication>
#include <QByteArray>
#include <QStringList>
#include <QStandardPaths>
#include <QImageReader>
#include <QFileDialog>
#include <QMenu>
#include <QMainWindow>
#include <QMenuBar>
#include <QGridLayout>
#include <QGroupBox>
#include <QToolBar>
#include <QTimer>
#include <QScreen>
#include <QGroupBox>
#include <QBoxLayout>
#include <QDirIterator>
#include <QStringList>
#include <iostream>
#include <QStatusBar>

/**
 * @author Hashim
 * @brief Window::Window
 */
Window::Window()
{
    plgi("[Window::]");
    _fn = QString::null;
    _original_fn = QString::null;

    imageArea = new ImageArea;
    imageArea->setBackgroundRole(QPalette::Base);
    imageArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageArea);
    setCentralWidget(scrollArea);//original

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

    _tmpSvCnt = 0;
    _tmpPtr = 0;
    lgmsg = QString::null;
}

/**
 * @brief Window::~Window
 */
Window::~Window()
{
}

/**
 * @brief Window::loadFile
 * @param fileName
 * @return
 */
bool Window::loadFile(const QString &fileName)
{
    plgi("[loadFile]");
    _fn=fileName;
    QImage image(_fn);
    if (image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString());
        imageArea->setImage(QImage());
        imageArea->adjustSize();
        return false;
    }

    imageArea->setImage(image);
    scaleFactor = 1.0;

    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!fitToWindowAct->isChecked())
    {
        imageArea->adjustSize();
    }

    setWindowFilePath(fileName);
    refresh();
    return true;
}

/**
 * @brief Window::open
 */
void Window::open()
{
    plgi("[open]");
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QString pth;
    if(picturesLocations.isEmpty()) pth= QDir::currentPath() ;else pth=picturesLocations.first();
    QFileDialog dlg(this,tr("Open File..."),pth,tr("All Files (*);;JPEG/JPG (*.jpeg *.jpg);;PNG (*.png);;BitMap (*.bmp)"));
    QString fn = dlg.getOpenFileName();//! TODO:
    _original_fn = fn;
    if (!fn.isEmpty()){
        loadFile(fn);
        loadFile(fn);
    }
    imageArea->falseAll();
    tmpInit();
    tmpSave();
}

/**
 * @brief Window::_rotate
 */
void Window::_rotate()
{
    plgi("[rotate]");
    if(!imageArea->_rotate){///**TODO not safe**///
        if(imageArea->lastAction == 3){
            _undo();
        }else{
            imageArea->lastAction = 3;
        }
    }
    imageArea->h_rotate();
    if(!imageArea->_rotate)
    {
        imageArea->qpol->clear();
        imageArea->qpol = new QPolygon();
        tmpSave();
    }
    if(imageArea->_rotate){
        openAct->setEnabled(false);
        exitAct->setEnabled(false);
        zoomInAct->setEnabled(false);
        zoomOutAct->setEnabled(false);
        normalSizeAct->setEnabled(false);
        fitToWindowAct->setEnabled(false);
        aboutAct->setEnabled(false);
        saveAct->setEnabled(false);
        saveAsAct->setEnabled(false);
        helpAct->setEnabled(false);
//        rotateAct->setEnabled(false);
        scaleAct->setEnabled(false);
        cropAct->setEnabled(false);
        redoAct->setEnabled(false);
        UndoAct->setEnabled(false);
        resetAct->setEnabled(false);
    }else{
        openAct->setEnabled(true);
        exitAct->setEnabled(true);
        zoomInAct->setEnabled(true);
        zoomOutAct->setEnabled(true);
        normalSizeAct->setEnabled(true);
        fitToWindowAct->setEnabled(true);
        aboutAct->setEnabled(true);
        saveAct->setEnabled(true);
        saveAsAct->setEnabled(true);
        helpAct->setEnabled(true);
//        rotateAct->setEnabled(true);
        scaleAct->setEnabled(true);
        cropAct->setEnabled(true);
        redoAct->setEnabled(true);
        UndoAct->setEnabled(true);
        resetAct->setEnabled(true);
    }
    refresh();
}

/**
 * @brief Window::refresh
 */
void Window::refresh(){
    update();
    repaint();
    imageArea->update();
    imageArea->repaint();
    scrollArea->update();
    scrollArea->repaint();
}

/**
 * @brief Window::_redo
 */
void Window::_redo()
{
    plgi("[redo]");
    if (_tmpPtr < _tmpSvCnt - 1) {
        _tmpPtr++;
    }
    tmpOpen();
    refresh();
}

/**
 * @brief Window::_undo
 */
void Window::_undo()
{
    plgi("[undo]");
    if (_tmpPtr > 0) {
        _tmpPtr--;
    }
    tmpOpen();
    refresh();
}

/**
 * @brief Window::_close
 */
void Window::_close()
{
    if(imageArea->isEdited()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Warning", "<font color=red>You have unsaved modifications.\n"
                                                       "Do you want to save them?</color>",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            if(!imageArea->_image.isNull())
            {
                plgi("[saveAs]");
                QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                                              QString(), tr("PNG (*.png);;BitMap (*.bmp);;JPEG/JPG (*.jpeg *.jpg);;All Files (*)"));
                    if (fn.isEmpty())
                        return;
                    if (! (fn.endsWith(".png", Qt::CaseInsensitive)
                           || fn.endsWith(".jpg", Qt::CaseInsensitive)
                           || fn.endsWith(".jpeg", Qt::CaseInsensitive)
                           ||fn.endsWith(".bmp", Qt::CaseInsensitive)))
                        fn += ".png"; // default

                    _fn=(fn);
                    _original_fn=fn;
                if (_fn.isEmpty())
                       return;
                if(imageArea->getImage().save(_fn)){
                    loadFile(_fn);
                    loadFile(_fn);
                }
                plgi("[saveAs]");
                imageArea->lastAction = 0;
                tmpSave();
            }
            else
            {
                plgi("Image is empty!");
            }
        } else {
        }
    }else{
    }

    //clean any created folders and delete them
    plgi("[DEL]");
    QString dir1 = getDirFrom_fn(_original_fn);//return null
    QDir dir(dir1);
    QString pth = dir.absolutePath()+"/"+tr(".qtmp_")+getFileNameFrom_fn(_original_fn);
    if(QDir(pth).exists()){
        removeDir(pth);
    }
    close();
}

/**
 * @brief Window::_reset
 */
void Window::_reset()
{
    plgi("[reset]");
    _tmpPtr=0;
    _fn=_original_fn;
    imageArea->lastAction = 0;
    tmpOpen();
    refresh();
}

/**
 * @brief Window::_crop
 */
void Window::_crop()
{
    plgi("[crop]");
    if(imageArea->_image.isNull()||imageArea->qpol->isEmpty()){
        plgi("no image or no selected area");
    }
    imageArea->h_crop();
    tmpSave();
    refresh();
}

/**
 * @brief Window::_scale
 */
void Window::_scale()
{
    plgi("[scale]");
    if(imageArea->_image.isNull()||imageArea->qpol->isEmpty()){
        plgi("no image or no selected area");
    }
    imageArea->h_scale();
    tmpSave();
    refresh();
}

/**
 * @brief Window::save
 */
void Window::save()
{
    plgi("[save]");
    tmpSave();
    if (_original_fn.isEmpty())
    {
        saveAs();
    }
    if(imageArea->getImage().save(_original_fn)){
            loadFile(_original_fn);
            loadFile(_original_fn);
    }
    imageArea->lastAction=0;
}

/**
 * @brief Window::saveAs
 */
void Window::saveAs()
{
    if(!imageArea->_image.isNull())
    {
        plgi("[saveAs]");
        QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                                      QString(), tr("PNG (*.png);;BitMap (*.bmp);;JPEG/JPG (*.jpeg *.jpg);;All Files (*)"));
            if (fn.isEmpty())
                return;
            if (! (fn.endsWith(".png", Qt::CaseInsensitive)
                   || fn.endsWith(".jpg", Qt::CaseInsensitive)
                   || fn.endsWith(".jpeg", Qt::CaseInsensitive)
                   ||fn.endsWith(".bmp", Qt::CaseInsensitive)))
                fn += ".png"; // default

            _fn=(fn);
            _original_fn=fn;

        if (_original_fn.isEmpty())
               return;
        if(imageArea->getImage().save(_original_fn)){
            loadFile(_original_fn);
            loadFile(_original_fn);
        }
        plgi("[saveAs]");
        imageArea->lastAction = 0;
        tmpSave();
    }
    else
    {
        plgi("Image is empty!");
    }
}

/**
 * @brief Window::zoomIn
 */
void Window::zoomIn()
{
    plgi("[zoomIn]");

    scaleImage(1.25);
}

/**
 * @brief Window::zoomOut
 */
void Window::zoomOut()
{
    plgi("[zoomOut]");

    scaleImage(0.8);
}

/**
 * @brief Window::normalSize
 */
void Window::normalSize()
{
    plgi("[normalSize]");

    imageArea->adjustSize();
    imageArea->resetSize();
    scaleFactor = 1.0;
}

/**
 * @brief Window::resizeEvent
 * @param event
 */
void Window::resizeEvent(QResizeEvent* event)
{
    if(!imageArea->_rotate){
        plgi("[resizeEvent]");
        QMainWindow::resizeEvent(event);
        if(fitToWindowAct->isChecked()){
            fitToWindow();
        }else{
            normalSize();
        }
        updateActions();
        refresh();
        scaleImage(1.00);
    }
}

/**
 * @brief Window::fitToWindow
 */
void Window::fitToWindow()
{
    if(fitToWindowAct->isChecked()){
        QTimer::singleShot(1000, this, SLOT(showFullScreen()));
    }else{
        QTimer::singleShot(1000, this, SLOT(showNormal()));
    }
}

/**
 * @brief Window::about
 */
void Window::about()
{
    plgi("[about]");
    QMessageBox *m= new QMessageBox();
    m->setWindowOpacity(0.1);
    m->about(this, tr("About Image Processor"),
            tr("<p>The <b>Image Processor</b> a Qt Application for editting and "
               "openning an image and saving them. It support zoom in and Zoom out. "
               "Also it supports Normal size and fit to window. Supports crop to "
               "selected rectangle and resize to selected rectangle and rotate with "
               "an abgle. Supports open, save, saveAs, Quit application, undo, redo"
               "reset to original image. "
               "</p><p>Author: <font color=red>Hashim Hossam</font><br> "
               "Id:     72<br> "
               "Email:  <a href=\"mailto:cse.hashim.hossam@hossam@gmail.com\">"
               "cse.hashim.hossam@gmail.com</a></p>"
               ));
    m->setWindowOpacity(0.1);
}

/**
 * @brief Window::help
 */
void Window::help()
{
    plgi("[help]");
    //TODO: help and documentation
}

/**
 * @brief Window::createActions
 */
void Window::createActions()
{
    plgi("[createActions]");

    openAct = new QAction(QIcon(":/icons/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/icons/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save modification to the file"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));


    saveAsAct = new QAction(QIcon(":/icons/saveAs.png"), tr("&Save As..."), this);
    saveAsAct->setShortcut(tr("Ctrl+Shift+S"));
    saveAsAct->setStatusTip(tr("Save modification to another choosed file"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    cropAct = new QAction(QIcon(":/icons/crop.png"), tr("Cro&p"), this);
    cropAct->setShortcut(tr("Ctrl+p"));
    cropAct->setStatusTip(tr("Crop the selected area by the rectabgle"));
    connect(cropAct, SIGNAL(triggered()), this, SLOT(_crop()));

    rotateAct = new QAction(QIcon(":/icons/rotate.png"), tr("Ro&tate"), this);
    rotateAct->setShortcut(tr("Ctrl+r"));
    rotateAct->setStatusTip(tr("Start Rotate Mode"));
    connect(rotateAct, SIGNAL(triggered()), this, SLOT(_rotate()));

    scaleAct = new QAction(QIcon(":/icons/scale.png"), tr("Sca&le"), this);
    scaleAct->setShortcut(tr("Ctrl+l"));
    scaleAct->setStatusTip(tr("Resize image dimensions (scale)"));
    connect(scaleAct, SIGNAL(triggered()), this, SLOT(_scale()));

    exitAct = new QAction(QIcon(":/icons/exit.png"), tr("&Exit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit Application Good bye:)"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(_close()));

    zoomInAct = new QAction(QIcon(":/icons/zoomIn.png"), tr("Zoom &In"), this);
    zoomInAct->setShortcut(tr("Ctrl+="));
    zoomInAct->setEnabled(false);
    zoomInAct->setStatusTip(tr("Zoom In by 25% up of the image"));
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(QIcon(":/icons/zoomOut.png"), tr("Zoom O&ut"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    zoomOutAct->setStatusTip(tr("Zoom out by 25% of down the image"));
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(QIcon(":/icons/normalSize.png"), tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+N"));
    normalSizeAct->setEnabled(false);
    normalSizeAct->setStatusTip(tr("Reset zooming to 100%"));
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAct = new QAction(QIcon(":/icons/fitToWindow.png"), tr("&Fit To Window"), this);
    fitToWindowAct->setEnabled(true);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
    fitToWindowAct->setStatusTip(tr("fit image to window size"));
    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    redoAct = new QAction(QIcon(":/icons/redo.png"), tr("&Redo"), this);
    redoAct->setShortcut(tr("Ctrl+y"));
    redoAct->setStatusTip(tr("Redo changes"));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(_redo()));

    UndoAct = new QAction(QIcon(":/icons/undo.png"), tr("&Undo"), this);
    UndoAct->setShortcut(tr("Ctrl+z"));
    UndoAct->setStatusTip(tr("Undo Changes"));
    connect(UndoAct, SIGNAL(triggered()), this, SLOT(_undo()));

    resetAct = new QAction(QIcon(":/icons/reset.png"), tr("R&eset"), this);
    resetAct->setShortcut(tr("Ctrl+0"));
    resetAct->setStatusTip(tr("Reset to original image discarding modifications"));
    connect(resetAct, SIGNAL(triggered()), this, SLOT(_reset()));

    aboutAct = new QAction(QIcon(":/icons/about.png"), tr("&About"), this);
    aboutAct->setStatusTip(tr("About me"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    helpAct = new QAction(QIcon(":/icons/help.png"), tr("&Help"), this);
    helpAct->setStatusTip(tr("Help and hints"));
    connect(helpAct, SIGNAL(triggered()), this, SLOT(help()));
}

/**
 * @brief Window::createMenus
 */
void Window::createMenus()
{
    plgi("[createMenus]");
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(UndoAct);
    editMenu->addAction(redoAct);
    editMenu->addAction(resetAct);
    editMenu->addSeparator();
    editMenu->addAction(rotateAct);
    editMenu->addAction(scaleAct);
    editMenu->addAction(cropAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addSeparator();
    viewMenu->addAction(fitToWindowAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(helpAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}

/**
 * @brief Window::getDirFrom_fn
 * @param s
 * @return
 */
QString Window::getDirFrom_fn(QString s){
    QString str = tr("");
    int max=s.lastIndexOf("/");
    for (int i = 0; i < max; ++i) {
        str.append((s.at(i)));
    }
    return str;
}

/**
 * @brief Window::getFileNameFrom_fn
 * @param s
 * @return
 */
QString Window::getFileNameFrom_fn(QString s){
    QString str = QString::null;
    int max=s.lastIndexOf("/");
    for (int i = max+1; i<s.size(); ++i) {
        str.append((s.at(i)));
    }
    return str;
}

/**
 * @brief Window::createToolBars
 */
void Window::createToolBars(){
    fileToolBar=addToolBar(tr("File"));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(saveAsAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(exitAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(UndoAct);
    editToolBar->addAction(redoAct);
    editToolBar->addAction(resetAct);
    editToolBar->addSeparator();
    editToolBar->addAction(rotateAct);
    editToolBar->addAction(scaleAct);
    editToolBar->addAction(cropAct);

    viewToolBar = addToolBar(tr("View"));
    viewToolBar->addAction(zoomInAct);
    viewToolBar->addAction(zoomOutAct);
    viewToolBar->addAction(normalSizeAct);
    viewToolBar->addSeparator();
    viewToolBar->addAction(fitToWindowAct);
}

/**
 * @brief Window::createStatusBar
 */
void Window::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

/**
 * @brief Window::removeDir
 * @param dirName
 * @return
 */
bool Window::removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

/**
 * @brief Window::tmpInit
 */
void Window::tmpInit(){
    plgi("[tmpInit]");
    QString dir1 = getDirFrom_fn(_original_fn);//return null
    QDir dir(dir1);
     QString pth = dir.absolutePath()+"/"+tr(".qtmp_")+getFileNameFrom_fn(_original_fn);
    if(QDir(pth).exists()){
        removeDir(pth);
        dir.mkdir(pth);
        _tmpSvCnt = 0;
        _tmpPtr = 0;
    }else{
        dir.mkdir(pth);
        _tmpSvCnt = 0;
        _tmpPtr = 0;
    }
}

/**
 * @brief Window::plgi
 * @param s
 */
void Window::plgi(QString s){
//    std::cout<<s.toStdString()<<std::endl;
    statusBar()->showMessage(s);

}

/**
 * @brief Window::lg
 * @param s
 */
void Window::lg(QString s){
    s.append("not used");
}

/**
 * @brief Window::tmpSave
 */
void Window::tmpSave(){
    plgi("[tmpSave]");
    QString dir1 = getDirFrom_fn(_original_fn);//return null
    QDir dir(dir1);
     QString pth = dir.absolutePath()+"/"+tr(".qtmp_")+getFileNameFrom_fn(_original_fn);
    if(QDir(pth).exists()){

    }else{
        dir.mkdir(pth);
        _tmpSvCnt = 0;
        _tmpPtr = 0;
    }
    dir.cd(tr(".qtmp_")+getFileNameFrom_fn(_original_fn));
    char num[10];
//    sprintf(num,"%d\0",_tmpSvCnt++);
    sprintf(num,"%d",_tmpSvCnt++);
    pth=dir.absolutePath()+tr("/")+(tr(num))+tr("_")+getFileNameFrom_fn(_original_fn);
    _tmpPtr = _tmpSvCnt - 1;
//    imageArea->_save();
    imageArea->getImage().save(pth);
    tmpOpen();
}

/**
 * @brief Window::tmpLoad
 * @param fileName
 * @return
 */
bool Window::tmpLoad(QString fileName){
    plgi("[loadFile]");
    _fn=fileName;
    QImage image(_fn);
    imageArea->setImage(image);
    fitToWindowAct->setEnabled(true);
    updateActions();
    if (!fitToWindowAct->isChecked())
    {
        imageArea->adjustSize();
    }
    setWindowFilePath(_original_fn);
    refresh();
    return true;
}

/**
 * @brief Window::tmpOpen
 */
void Window::tmpOpen(){
    plgi("[tmpOpen]");
    QDir dir(getDirFrom_fn(_original_fn));
    QString pth = dir.absolutePath()+"/"+tr(".qtmp_")+getFileNameFrom_fn(_original_fn);
    if(QDir(pth).exists()){
        dir.cd(tr(".qtmp_")+getFileNameFrom_fn(_original_fn));
        char num[10];
//        sprintf(num,"%d\0",_tmpPtr);
        sprintf(num,"%d",_tmpPtr);
        pth=dir.absolutePath()+tr("/")+tr(num)+tr("_")+getFileNameFrom_fn(_original_fn);
        tmpLoad(pth);
        tmpLoad(pth);
    }
}

/**
 * @brief Window::updateActions
 */
void Window::updateActions()
{
    if(!imageArea->_rotate){
        zoomInAct->setEnabled(!fitToWindowAct->isChecked());
        zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
        normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
    }
}

/**
 * @brief Window::scaleImage
 * @param factor
 */
void Window::scaleImage(double factor)
{
    plgi("[scaleImage]");
    scaleFactor *= factor;
    QSize sz=imageArea->sizeHint();
    imageArea->resize(scaleFactor * sz);
    imageArea->rescale(scaleFactor ,scaleFactor );

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 10.0);
    zoomOutAct->setEnabled(scaleFactor > 0.01);
}

/**
 * @brief Window::adjustScrollBar
 * @param scrollBar
 * @param factor
 */
void Window::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    plgi("[adjustScrollBar]");
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
