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
class QPushButton;
class QTableWidget;
class QTextEdit;
class QLineEdit;
class QFile;
class QDir;

class DR_Heapstat : public ToolBase,
					public ToolInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.DR-GUI.ToolInterface" FILE "DR_Heapstat.json")
    Q_INTERFACES(ToolInterface)

public:
    DR_Heapstat();
    QStringList toolNames() const;
    DR_Heapstat *createInstance();

private slots:
	void loadResults();
	void fillCallstacksTable();
	void logDirTextChangedSlot();
	void loadSettings();
	void loadFramesTextEdit(int currentRow, int currentColumn, 
							int previousRow, int previousColumn);
private:
	void createActions();
	void createLayout();
	bool dr_checkDir(QDir dir);
	bool dr_checkFile(QFile& file);
    void readLogData();

    QGraphicsView *graphView;
    QGraphicsScene *graphScene;

    QTableWidget *callstacksTable;
    QTextEdit *framesTextEdit;
    QPushButton *prevFrameButton;
	QPushButton *nextFrameButton;

	QLineEdit *logDirLineEdit;
	bool logDirTextChanged;
    QPushButton *loadResultsButton;

    /* Dr. Heapstat variables */
    QString logDirLoc;
    struct callstackListing {
    	QStringList frameData;
        int callstackNum,
            instances, 
            bytes_asked_for,
            extra_usable,
            extra_occupied;
    };
    struct snapshotListing {
        QVector<int> assocCallstacks;
        int snapshotNum,
            tot_mallocs,
            tot_bytes_asked_for,
            tot_bytes_usable,
            tot_bytes_occupied;
    };
    QVector<struct callstackListing*> callstacks;
    QVector<struct snapshotListing*> snapshots;
};