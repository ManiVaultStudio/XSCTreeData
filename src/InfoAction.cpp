#include "QtTooltipKit.h"
#include "InfoAction.h"

#include "event/Event.h"

using namespace mv;
using namespace mv::gui;

InfoAction::InfoAction(QObject* parent, XSCTree& clusters) :
    GroupAction(parent, "Group", true),
    _clusters(&clusters),
    _treeInfoAction(this, "XSCTree info"),
    _leafInfoAction(this, "Leaf info")
{
    setText("XSCTree JSON Info");


    _treeInfoAction.setIconByName("code");
    qttooltipkit::set(_treeInfoAction, tr("Tree structure"), tr("Inspect the source JSON, including the branches and their leaf names."));
    _treeInfoAction.setDefaultWidgetFlags(StringAction::TextEdit);
    _leafInfoAction.setIconByName("leaf");
    qttooltipkit::set(_leafInfoAction, tr("Tree leaves"), tr("Read the species names stored at the tips of the reference tree."));
    _leafInfoAction.setDefaultWidgetFlags(StringAction::TextEdit);

    addAction(&_leafInfoAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto textEdit = widget->findChild<QTextEdit*>("LineEdit");

        if (textEdit)
            textEdit->setReadOnly(true);

        });
    addAction(&_treeInfoAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto textEdit = widget->findChild<QTextEdit*>("LineEdit");

        if (textEdit)
            textEdit->setReadOnly(true);

        });



    const auto updateActions = [this]() -> void {
        if (!_clusters.isValid())
            return;


        _treeInfoAction.setString(QJsonDocument(_clusters->getTreeData()).toJson());
        QString textleaf = _clusters->getTreeLeafNames().join("\n");
        _leafInfoAction.setString(textleaf);

        //qDebug() << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%";

        //qDebug() << QJsonDocument(_clusters->getTreeData()).toJson();
        //qDebug() << "Species names: " << textleaf ;
        //qDebug() << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%";

    };

    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetAdded));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataChanged));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataSelectionChanged));


    _eventListener.registerDataEventByType(XSCTreeType, [this, updateActions](mv::DatasetEvent* dataEvent) {
        if (!_clusters.isValid())
            return;

        if (dataEvent->getDataset() != _clusters)
            return;

        switch (dataEvent->getType()) {
            case EventType::DatasetAdded:
            case EventType::DatasetDataChanged:
            case EventType::DatasetDataSelectionChanged:
            {
                updateActions();
                break;
            }

            default:
                break;
        }
    });

    updateActions();
}