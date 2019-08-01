#ifndef COCO_ITEM_H
#define COCO_ITEM_H

#include <QPainter>
#include <vector>
#include <array>
#include <memory>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

struct PaintData{
    QRect rect;
    double scale;
};

struct CocoItem
{
    CocoItem(const std::string tag = "") : mTag(tag)
    {
        //qDebug()<<"create:"<<QString::fromStdString(mTag);
    }

    virtual ~CocoItem(){ clear(); }

    void paint(QPainter& painter, const PaintData& data){
        paintItself(painter, data);
        paintItems(painter, data);
    }

protected:
    std::string mTag;
    std::vector<std::shared_ptr<CocoItem>> mItems;
    inline void addItem(CocoItem* item){ mItems.push_back(std::shared_ptr<CocoItem>(item)); }

    template<typename T>
    void addItem( const QJsonValue &json_value)
    {
        if(!json_value.isUndefined())
        {
            addItem(new T(json_value));
        }
    }

    template<typename T>
    void addItem( const QJsonObject &json_object)
    {
        addItem(new T(json_object));
    }
    virtual void paintItself(QPainter& painter, const PaintData& data) {}

private:
    inline void clear(){
        //qDebug()<<"delete:"<<QString::fromStdString(mTag);
        mItems.clear();
    }

    void paintItems(QPainter& painter, const PaintData& data){
        for(const auto& item : mItems)
            item->paint(painter, data);
    }
};

struct CocoRootItem : CocoItem
{
    CocoRootItem() = default;
    CocoRootItem(const QJsonObject& json_object);
protected:
    virtual void paintItself(QPainter& painter, const PaintData& data) override {}
};

struct CocoItemInfo : CocoItem
{
    CocoItemInfo(const QJsonValue& json_value);
    std::string description;
    std::string url;
    std::string version;
    uint year;
    std::string contributor;
    std::string date_created;
};

struct CocoItemLicenses : CocoItem
{
    CocoItemLicenses(const QJsonValue& json_value);
    struct CocoLicense: CocoItem{
        std::string url;
        std::string id;
        std::string name;
    };
};

struct CocoItemImages : CocoItem
{
    CocoItemImages(const QJsonValue &json_value);
    struct CocoImage : CocoItem{
        std::string license;
        std::string file_name;
        std::string coco_url;
        std::string height;
        std::string width;
        std::string date_captured;
        std::string flickr_url;
        std::string id;
    };
};

struct CocoItemAnnotations : CocoItem
{
    CocoItemAnnotations(const QJsonValue& json_value);
    struct SegAnnotation : CocoItem{
        SegAnnotation(const QJsonObject &json_object);
        std::vector<std::vector<double>> segmentation;
        double area;
        uint iscrowd;
        uint image_id;
        std::array<double, 4> bbox;
        uint category_id;
        uint id;
    };

    struct KeyPointSegAnnotation : CocoItem{
        KeyPointSegAnnotation(const QJsonObject &json_object);
        std::vector<std::vector<double>> segmentation;
        int num_keypoints;
        double area;
        uint iscrowd;
        std::vector<uint> keypoints;
        uint image_id;
        std::array<double, 4> bbox;
        uint category_id;
        uint id;
    };

    struct RLEAnnotation : CocoItem{
        RLEAnnotation(const QJsonObject &json_object);
        std::vector<double> counts;
        std::pair<uint, uint> size;
        double area;
        uint iscrowd;
        uint image_id;
        std::array<double, 4> bbox;
        uint category_id;
        uint id;
    };
    struct ImageAnnotation : CocoItem{
        ImageAnnotation(const QJsonObject &json_object);
        uint image_id;
        uint id;
        std::string caption;
    };
    struct EyeAnnotation : CocoItem{
        EyeAnnotation(const QJsonObject &json_object);
        virtual void paintItself(QPainter& painter, const PaintData& data) override;
        uint id;
        uint team;
        uint frame;
        uint x;
        uint y;
    };
};

struct CocoItemCategories : CocoItem
{
    CocoItemCategories(const QJsonValue &json_value);
    struct SimpleCategory : CocoItem{
        std::string supercategory;
        uint id;
        std::string name;
    };

    struct KeyPointCategory{
        std::string supercategory;
        uint id;
        std::string name;
        std::vector<std::string> keypoints;
        std::vector<std::pair<uint,uint>> skeleton;
    };
};

struct CocoItemSegmentInfo : CocoItem
{
    CocoItemSegmentInfo(const QJsonValue& json_value);
    std::string information;
};




#endif // COCO_ITEM_H
