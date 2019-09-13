#include "imagearea.h"
#include "imagearea.h"
#include <QPainter>
#include <QPaintEvent>
#include <QImage>
#include <iostream>
#include <QPoint>
#include <math.h>
#include <QMatrix>

/**
 * @author Hashim
 * @brief ImageArea::ImageArea
 * @param parent
 */
ImageArea::ImageArea(QWidget *parent) : QWidget(parent)
{
    _image= QImage();
    _tmp_image = _image;
    _size =  QSize();
    clkCount=0;
    ttch_scrl_mode=false;
    draw_slct_rect_mode=false;
    start=QPoint();
    end=QPoint();
    qpol=new QPolygon();
    xscale=1.0;
    yscale=1.0;
    _OLD= QImage();
    _rotate = false;
    _scale = false;
    _crop =false;
    angle = 0.0;
    lastAction = 0;
    ///    this->setMinimumSize(QSize(100,100));
    ///    this->setMaximumSize(QSize(999999, 999999));
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/**
 * @brief ImageArea::h_rotate
 */
void ImageArea::h_rotate()
{
    if(_rotate){
        _rotate = false;
        draw_slct_rect_mode=true;
        _image = _image.transformed(doRot()).copy();
    }else{
        _rotate = true;
        draw_slct_rect_mode=false;
    }
    ///_image = _image.transformed(transform).copy();
}

/**
 * @brief ImageArea::trpt
 * @param pt
 * @return
 */
QPoint ImageArea::trpt(QPoint pt){
    if(size()!=_image.size()){
        if(!_rotate){
            qreal x = pt.x()*(_image.size().width()/size().width());
            qreal y = pt.y()*(_image.size().height()/size().height());
            return QPoint(x,y);
        }
        else{
            return pt;
        }
    }else{
        return pt;
    }
}

/**
 * @brief ImageArea::h_scale
 */
void ImageArea::h_scale()
{
    lastAction = 2;
    falseAll();
    _scale=true;
    if(!qpol->isEmpty()){
    _image=_image.scaled(plg_2_rect(*qpol).size());
    }
    qpol=new QPolygon();
}

/**
 * @brief ImageArea::plg_2_rect
 * @param qpol
 * @return
 */
QRect ImageArea::plg_2_rect(QPolygon qpol){
    QPoint a,b;
    bool stop=false;
    for (int i = 0; i < qpol.size(); ++i) {
         a= qpol.at(i);
        for (int j = 0; j < qpol.size(); ++i) {
             b= qpol.at(i);
            if(a.x()==b.x()||a.y()==b.y()){
                continue;
            }else{
                stop=true;
                break;
            }
        }
        if(stop) break;
    }
    return QRect(a,b);
}

/**
 * @brief ImageArea::h_crop
 */
void ImageArea::h_crop()
{
    lastAction = 1;
    falseAll();
    _crop=true;
        if(!qpol->isEmpty()){_image=_image.copy(plg_2_rect(*qpol));}
        qpol=new QPolygon();
}

/**
 * @brief ImageArea::falseAll
 */
void ImageArea::falseAll()  {
    _rotate=false;_scale=false;_crop=false;
}

/**
 * @brief ImageArea::resetSize
 */
void ImageArea::resetSize()
{
    rescale(1.00,1.00);
}

/**
 * @brief ImageArea::mousePressEvent
 * @param evt
 */
void ImageArea::mousePressEvent( QMouseEvent* evt )
{
    const QPoint p = evt->pos();
    if(clkCount==0){
        start=p;
        start = trpt(start);
        qpol=new QPolygon();
        repaint();
        update();
    }
    clkCount++;
    emit mousePressed( p );
}

/**
 * @brief ImageArea::isEdited
 * @return
 */
bool ImageArea::isEdited(){
    return lastAction!=0;
}

/**
 * @brief ImageArea::doRot
 * @return
 */
QTransform ImageArea::doRot(){
    if(angle>=360||angle<=360){angle=0;}
    angle = ((end.x()-start.x())/2);
    QTransform transform;
    if(angle!=0){
        qreal wo = _image.size().width();
        qreal ho = _image.size().height();
        qreal wc = round(wo * abs(cos(angle)) + ho * abs(sin(angle)));
        qreal hc = round(wo * abs(sin(angle)) + ho * abs(cos(angle)));
        qreal x = (wc - wo) / 2;
        qreal y = (hc - ho) / 2;
        transform.translate( wo / 2, ho / 2 );
        transform.rotate(angle);
        transform.translate(x,y);
        return transform;
    }else{
        return QTransform();
    }
}

/**
 * @brief ImageArea::mouseReleaseEvent
 * @param evt
 */
void ImageArea::mouseReleaseEvent( QMouseEvent* evt )
{
    const QPoint p = evt->pos();
    end=p;
    end = trpt(end);
    if(end!=start){
        if(clkCount==2){
            ttch_scrl_mode=true;
            draw_slct_rect_mode=false;
            this->scroll(end.x()-start.x(),end.y()-start.y());//TODO: not working at all
            clkCount=0;
        }else if(clkCount==1){
            ttch_scrl_mode=false;
            draw_slct_rect_mode=true;
            if(_rotate){
                _tmp_image = _image.transformed(doRot());
                resize(_tmp_image.size());
                update();
                repaint();
            }
                paintRect();
            clkCount=0;
        }else{
            ttch_scrl_mode=false;
            draw_slct_rect_mode=false;
            clkCount=0;
        }
    }
    update();
    repaint();
    emit mouseReleased( p );
}

/**
 * @brief ImageArea::mouseMoveEvent
 * @param evt
 */
void ImageArea::mouseMoveEvent( QMouseEvent* evt )
{
    end=evt->pos();
    end=trpt(end);
    if(end!=start){
        if(clkCount==2){
            ttch_scrl_mode=true;
            draw_slct_rect_mode=false;
        }else if(clkCount==1){
            ttch_scrl_mode=false;
            draw_slct_rect_mode=true;
            if(_rotate){
                _tmp_image = _image.transformed(doRot());
                resize(_tmp_image.size());
                update();
                repaint();
            }
            paintRect();
        }else{
            ttch_scrl_mode=false;
            draw_slct_rect_mode=false;
        }
    }
    const QPoint p = evt->pos();
    update();
    repaint();
    emit mouseMove( p );
}

/**
 * @brief ImageArea::rescale
 * @param sx
 * @param sy
 */
void ImageArea::rescale(qreal sx,qreal sy)
{
    xscale=sx;
    yscale=sy;
    repaint();
    update();
}

/**
 * @brief ImageArea::paintRect
 */
void ImageArea::paintRect()
{
    if(draw_slct_rect_mode){
        QVector < QPoint > qv = QVector < QPoint > ();
        QPoint *p1=new  QPoint(start.x(),start.y());
        QPoint *p2 =new QPoint(start.x(),end.y());
        QPoint *p3=new  QPoint(end.x(),end.y());
        QPoint *p4 =new QPoint(end.x(),start.y());
        qv.append(*p1);
        qv.append(*p2);
        qv.append(*p3);
        qv.append(*p4);
        qpol=new QPolygon(qv);
        repaint();
        update();
    }
}

/**
 * @brief ImageArea::wheelEvent
 * @param event
 */
void ImageArea::wheelEvent ( QWheelEvent * event )
{
    //TODO: not implemented
    event->isAccepted();
}

/**
 * @brief ImageArea::paintEvent
 * @param evt
 */
void ImageArea::paintEvent(QPaintEvent *evt)
{
    evt->isAccepted();//nop
    QBrush *brsh=new QBrush();
    QPen pn=QPen(*brsh,2);
//    QPen _pn=QPen();
    pn.setColor(Qt::AutoColor);
    pn.setDashOffset(0.2);
    pn.setStyle(Qt::DotLine);
    QPainter *painter =new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->save();
    painter->scale(xscale,yscale);
    if(_scale){
    }
    if (!_image.isNull())
    {
        if(_rotate){
            if(!_tmp_image.isNull()){
                painter->drawImage(QPoint(0,0), _tmp_image);
            }else{
                painter->drawImage(QPoint(0,0), _image);
            }
        }else{
            painter->drawImage(QPoint(0,0), _image);
        }
    }
    if(!qpol->isEmpty()){
        painter->setPen(pn);
        if(!_rotate){
            painter->setPen(pn);
            painter->drawPolygon(*qpol);
        }
    }
    painter->restore();
}

/**
 * @brief ImageArea::setImage set image of the image area
 * @param image
 */
void ImageArea::setImage(const QImage &image){
    _size = image.size();
    _image = image.scaled(size());//very important
    _OLD = _image;
    update();
    repaint();
}

/**
 * @brief ImageArea::getImage
 * @return
 */
QImage ImageArea::getImage()
{
    return _image;
}
