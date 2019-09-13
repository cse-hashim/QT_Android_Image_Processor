#ifndef IMAGEAREA_H
#define IMAGEAREA_H

#include <QWidget>
#include <QPaintEvent>
#include <QSize>
#include <QPainter>
#include <QImage>

QT_BEGIN_NAMESPACE
class QPaintEvent;
QT_END_NAMESPACE

/**
 * @author Hashim
 * @brief The ImageArea class
 */
class ImageArea : public QWidget
{
    Q_OBJECT
public:
    ImageArea(QWidget *parent = 0);
    QPolygon *qpol;
    bool _rotate;
    bool _crop;
    bool _scale;
    int lastAction;
    QImage _image;
    /**
     * @brief angle in degree
     */
    qreal angle;
    qreal last_angle;
    QSize sizeHint() const {return _size;}
    void setImage(const QImage& image);
    QImage getImage();
    void mousePressEvent( QMouseEvent* evt );
    void mouseMoveEvent(QMouseEvent* evt );
    void mouseReleaseEvent(QMouseEvent* evt );
    void wheelEvent ( QWheelEvent * event );
    void rescale(qreal sx,qreal sy);
    void resetSize();
    void falseAll();
    void h_crop();
    void h_rotate();
    void h_scale();
    QPoint trpt(QPoint);
    QRect plg_2_rect(QPolygon qpol);
    QTransform doRot();
    bool isEdited();
    ////////////////////////////////////////////////////////
protected:
    void paintEvent(QPaintEvent *evt) Q_DECL_OVERRIDE;
    void paintRect();
    ////////////////////////////////////////////////////////
signals:
    void mousePressed( const QPoint& );
    void mouseReleased( const QPoint& );
    void mouseMove( const QPoint& );
    ////////////////////////////////////////////////////////
//public slots:
    ////////////////////////////////////////////////////////
private:
    QImage _tmp_image;
    QImage _OLD;
    QSize _size;
    int clkCount;
    bool ttch_scrl_mode;
    bool draw_slct_rect_mode;
    QPoint start,end;
    qreal xscale;
    qreal yscale;
    ////////////////////////////////////////////////////////
};

#endif // IMAGEAREA_H
