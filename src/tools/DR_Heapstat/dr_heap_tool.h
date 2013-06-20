/*************************************************************************
**
** 
** File: dr_heap_tool.h
** 
** Defines the base of the options window.
**
**
*************************************************************************/
#include "tools/toolbase.h"

class QGraphicsView;
class QGraphicsScene;
class QStringList;

class DR_Heapstat : public ToolBase,
					public ToolInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.DR-GUI.ToolInterface" FILE "DR_Heapstat.json")
    Q_INTERFACES(ToolInterface)

public:
    DR_Heapstat();
    QStringList toolNames() const;
    DR_Heapstat *createInstance();

private:
    QGraphicsView *graphView;
    QGraphicsScene *graphScene;
};