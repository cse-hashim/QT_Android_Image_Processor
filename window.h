#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QScrollBar>
#include <QMessageBox>
#include <QToolBar>
#include "imagearea.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

/**
 ** @author: Hashim
 ** <font color="red">cse.hashim.hossam@gmail.com</font>
 **
 **/
class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window();
    ~Window();
    bool loadFile(const QString &fileName);
    void resizeEvent(QResizeEvent* event);

private slots:
    void open();
    void save();
    void saveAs();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void about();
    void help();
    void _rotate();
    void _scale();
    void _crop();
    void _undo();
    void _redo();
    void _reset();
    void _close();

private:
    ImageArea *imageArea;
    QScrollArea *scrollArea;
    QString _fn;
    double scaleFactor;
    qint32 _tmpSvCnt;
    qint32 _tmpPtr;
    QString _original_fn;
    QString lgmsg;

    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    void tmpSave();
    void tmpOpen();
    bool tmpLoad(QString fileName);
    QString getFileNameFrom_fn(QString s);
    QString getDirFrom_fn(QString s);
    void refresh();
    void tmpInit();
    void plgi(QString s);
    void lg(QString s);
    bool removeDir(const QString & dirName);
    void createToolBars();
    void createStatusBar();

    QAction *openAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QAction *aboutAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *helpAct;
    QAction *rotateAct;
    QAction *scaleAct;
    QAction *cropAct;
    QAction *redoAct;
    QAction *UndoAct;
    QAction *resetAct;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *viewToolBar;
    QToolBar *helpToolBar;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
};

#endif // WINDOW_H
