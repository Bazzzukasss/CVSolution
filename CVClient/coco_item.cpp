#include "coco_item.h"
#include "app_constants.h"
#include <QDebug>

CocoRootItem::CocoRootItem(const QJsonObject &json_object)
    : CocoItem("CocoRootItem")
{
    //addItem<CocoItemInfo>("info", json_object);
    //addItem<CocoItemLicenses>("licenses", json_object);
    //addItem<CocoItemImages>("images", json_object);
    //addItem<CocoItemCategories>("categories", json_object);
    addItem<CocoItemAnnotations>(json_object["annotations"]);
}

CocoItemInfo::CocoItemInfo(const QJsonValue &json_value)
    : CocoItem("CocoItemInfo")
{

}

CocoItemLicenses::CocoItemLicenses(const QJsonValue &json_value)
    : CocoItem("CocoItemLicenses")
{

}

CocoItemImages::CocoItemImages(const QJsonValue& json_value)
    : CocoItem("CocoItemImages")
{

}

CocoItemAnnotations::CocoItemAnnotations(const QJsonValue &json_value)
    : CocoItem("CocoItemAnnotations")
{
    QJsonArray array = json_value.toArray();
    for(QJsonValue it : array)
    {
        //qDebug()<<it.toObject();

        if(!it.toObject()["x"].isNull())
        {
            addItem<CocoItemAnnotations::EyeAnnotation>(it.toObject());
            continue;
        }

        if(!it.toObject()["caption"].isNull())
        {
            addItem<CocoItemAnnotations::ImageAnnotation>(it.toObject());
            continue;
        }

        if(!it.toObject()["num_keypoints"].isNull())
        {
            addItem<CocoItemAnnotations::KeyPointSegAnnotation>(it.toObject());
            continue;
        }

        if(!it.toObject()["segmentation"].isNull())
        {
            if(it.toObject()["segmentation"].isArray())
                addItem<CocoItemAnnotations::SegAnnotation>(it.toObject());
            else
                addItem<CocoItemAnnotations::RLEAnnotation>(it.toObject());
            continue;
        }
    }
}

CocoItemCategories::CocoItemCategories(const QJsonValue& json_value)
    : CocoItem("CocoItemCategories")
{

}

CocoItemSegmentInfo::CocoItemSegmentInfo(const QJsonValue &json_value)
    : CocoItem("CocoItemSegmentInfo")
{

}

CocoItemAnnotations::ImageAnnotation::ImageAnnotation(const QJsonObject &json_object)
{
    qDebug()<<"ImageAnnotation";
    image_id = json_object["image_id"].toInt();
    id = json_object["id"].toInt();
    caption = json_object["caption"].toString().toStdString();
}

CocoItemAnnotations::RLEAnnotation::RLEAnnotation(const QJsonObject &json_object)
{
    qDebug()<<"RLEAnnotation";

    auto seg_object = json_object["segmentation"].toObject();
    for(QJsonValue length : seg_object["counts"].toArray())
        counts.push_back(length.toInt());

    size.first = seg_object["size"].toArray()[0].toInt();
    size.second = seg_object["size"].toArray()[1].toInt();

    int count{};
    for(QJsonValue coord : json_object["bbox"].toArray())
        bbox[count++] = coord.toDouble();

    area = json_object["area"].toDouble();
    iscrowd = json_object["iscrowd"].toInt();
    image_id = json_object["image_id"].toInt();
    category_id = json_object["category_id"].toInt();
    id  = json_object["id"].toInt();
}

CocoItemAnnotations::KeyPointSegAnnotation::KeyPointSegAnnotation(const QJsonObject &json_object)
{
    qDebug()<<"KeyPointSegAnnotation";
    for(QJsonValue poligon : json_object["segmentation"].toArray())
    {
        std::vector<double> coordinates;
        for(QJsonValue coord : poligon.toArray())
        {
            coordinates.push_back(coord.toDouble());
        }
        segmentation.push_back(coordinates);
    }

    int count{0};
    for(QJsonValue coord : json_object["bbox"].toArray())
        bbox[count++] = coord.toDouble();

    for(QJsonValue point : json_object["keypoints"].toArray())
        keypoints.push_back(point.toInt());

    num_keypoints = json_object["num_keypoints"].toDouble();
    area = json_object["area"].toDouble();
    iscrowd = json_object["iscrowd"].toInt();
    image_id = json_object["image_id"].toInt();
    category_id = json_object["category_id"].toInt();
    id  = json_object["id"].toInt();
}

CocoItemAnnotations::SegAnnotation::SegAnnotation(const QJsonObject &json_object)
{
    qDebug()<<"SegAnnotation";
    for(QJsonValue poligon : json_object["segmentation"].toArray())
    {
        std::vector<double> coordinates;
        for(QJsonValue coord : poligon.toArray())
        {
            coordinates.push_back(coord.toDouble());
        }
        segmentation.push_back(coordinates);
    }

    int count{};
    for(QJsonValue coord : json_object["bbox"].toArray())
        bbox[count++] = coord.toDouble();

    area = json_object["area"].toDouble();
    iscrowd = json_object["iscrowd"].toInt();
    image_id = json_object["image_id"].toInt();
    category_id = json_object["category_id"].toInt();
    id  = json_object["id"].toInt();
}

CocoItemAnnotations::EyeAnnotation::EyeAnnotation(const QJsonObject &json_object)
{
    id = json_object["id"].toInt();
    team = json_object["team"].toInt();
    frame = json_object["frame"].toInt();
    x = json_object["x"].toInt();
    y = json_object["y"].toInt();
}

void CocoItemAnnotations::EyeAnnotation::paintItself(QPainter &painter, const PaintData &data)
{
    painter.drawEllipse(data.rect.x() + x * data.scale, data.rect.y() + y * data.scale, app_config::kPointRadius  * data.scale, app_config::kPointRadius * data.scale);
}
