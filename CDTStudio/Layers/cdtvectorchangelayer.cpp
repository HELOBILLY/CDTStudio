#include "cdtvectorchangelayer.h"
#include "stable.h"
#include "cdtprojecttreeitem.h"
#include "cdtfilesystem.h"
#include "cdtvariantconverter.h"

CDTVectorChangeLayer::CDTVectorChangeLayer(QUuid uuid, QObject *parent)
    : CDTBaseLayer(uuid,parent)
{
    keyItem   = new CDTProjectTreeItem(CDTProjectTreeItem::VECTOR_CHANGE,CDTProjectTreeItem::VECTOR,QString(),this);

    QAction *actionRename = new QAction(QIcon(":/Icon/Rename.png"),
                                        tr("Rename"),this);
    QAction *actionRemoveLayer = new QAction(QIcon(":/Icon/Remove.png"),
                                        tr("Remove Vector-based change detection layer"),this);

    actions <<(QList<QAction *>()<<actionRename);
    actions <<(QList<QAction *>()<<actionRemoveLayer);

    connect(actionRename,SIGNAL(triggered()),SLOT(rename()));
    connect(actionRemoveLayer,SIGNAL(triggered()),SLOT(remove()));

    connect(this,SIGNAL(removeVectorChangeLayer(CDTVectorChangeLayer*)),this->parent(),SLOT(removeVectorChangeLayer(CDTVectorChangeLayer*)));
}

CDTVectorChangeLayer::~CDTVectorChangeLayer()
{

}

QString CDTVectorChangeLayer::name() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select name from vector_change where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

QString CDTVectorChangeLayer::shapefileID() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select shapefileid from vector_change where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

void CDTVectorChangeLayer::rename()
{
    bool ok;
    QString text = QInputDialog::getText(
                NULL, tr("Input Vector-based Change Layer Name"),
                tr("Vector-based change layer rename:"), QLineEdit::Normal,
                this->name(), &ok);
    if (ok && !text.isEmpty())
        setName(text);
}

void CDTVectorChangeLayer::remove()
{
    emit removeVectorChangeLayer(this);
}

void CDTVectorChangeLayer::setName(const QString &name)
{
    if (this->name() == name)
        return;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE vector_change set name = ? where id =?");
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    query.exec();

    keyItem->setText(name);
    emit nameChanged(name);
}

void CDTVectorChangeLayer::initVectorChangeLayer(
        const QString &name,
        const QString &shapefileID,
        const QString &clsID_T1,
        const QString &clsID_T2,
        const QVariantMap &params)
{
    QString tempShpPath;
    this->fileSystem()->getFile(shapefileID,tempShpPath);
    QgsVectorLayer *newLayer = new QgsVectorLayer(/*shpPath*/tempShpPath,QFileInfo(/*shpPath*/tempShpPath).completeBaseName(),"ogr");
    if (!newLayer->isValid())
    {
        QMessageBox::critical(NULL,tr("Error"),tr("Open shapefile ")+tempShpPath+tr(" failed!"));
        delete newLayer;
        return;
    }

    if (mapCanvasLayer)
    {
        QgsMapLayerRegistry::instance()->removeMapLayer(mapCanvasLayer->id());
        delete mapCanvasLayer;
    }
    mapCanvasLayer = newLayer;

    keyItem->setText(name);
    QgsMapLayerRegistry::instance()->addMapLayer(mapCanvasLayer);
    keyItem->setMapLayer(mapCanvasLayer);

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret ;
    ret = query.prepare("insert into vector_change VALUES(?,?,?,?,?,?)");
    query.bindValue(0,uuid.toString());
    query.bindValue(1,name);
    query.bindValue(2,shapefileID);
    query.bindValue(3,clsID_T1);
    query.bindValue(4,clsID_T2);
    query.bindValue(5,dataToVariant(params));
    query.exec();

    emit nameChanged(name);
    emit appendLayers(QList<QgsMapLayer*>()<<mapCanvasLayer);
    emit layerChanged();
}



QDataStream &operator<<(QDataStream &out, const CDTVectorChangeLayer &change)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec("select * from vector_change where id ='" + change.id().toString() +"'");
    query.next();

    out <<change.id()         //id
       <<query.value(1).toString() //name
      <<query.value(2).toString() //shapefileID
     <<query.value(3).toString() //clsID_T1
    <<query.value(4).toString() //clsID_T2
    <<query.value(5);//params
    return out;
}

QDataStream &operator>>(QDataStream &in, CDTVectorChangeLayer &change)
{
    in>>change.uuid;
    QString name,shp,cls1,cls2;
    in>>name>>shp>>cls1>>cls2;
    QVariant temp;
    in>>temp;
    QVariantMap params = variantToData<QVariantMap>(temp);
    change.initVectorChangeLayer(name,shp,cls1,cls2,params);

    return in;
}
