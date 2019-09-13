#include <QApplication>
#include "window.h"
/**
 * @author Hashim
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Window w;
    w.showFullScreen();

    return a.exec();
}
