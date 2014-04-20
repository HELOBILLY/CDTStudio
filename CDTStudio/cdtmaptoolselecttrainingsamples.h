#ifndef CDTMAPTOOLSELECTTRAININGSAMPLES_H
#define CDTMAPTOOLSELECTTRAININGSAMPLES_H

#include <qgsmaptool.h>
#include "cdtsegmentationlayer.h"

class QgsRubberBand;
class CDTMapToolSelectTrainingSamples : public QgsMapTool
{
    Q_OBJECT
public:
    explicit CDTMapToolSelectTrainingSamples(QgsMapCanvas *canvas,bool isReadOnly);
    ~CDTMapToolSelectTrainingSamples();

    void canvasMoveEvent( QMouseEvent * e );
    void canvasReleaseEvent( QMouseEvent * e );
signals:

public slots:
    void setSampleID(QUuid layerID);
    void setReadOnly(bool readOnly);
    void clearRubberBand();
private:
    QUuid sampleID;
    QgsMapCanvas *mapCanvas;
    QMultiMap<QString,qint64> samples;
    QList<QgsRubberBand*> rubberBands;
    bool mDragging;
    bool mReadOnly;

    void updateRubber();
    void addSingleSample(qint64 id);
};

#endif // CDTMAPTOOLSELECTTRAININGSAMPLES_H
