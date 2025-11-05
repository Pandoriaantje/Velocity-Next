#pragma once
#include <QDialog>
#include <QMap>
#include <QFutureWatcher>
#include <memory>

QT_BEGIN_NAMESPACE
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;
class QDragEnterEvent;
class QDropEvent;
class QProgressDialog;
QT_END_NAMESPACE

namespace XboxInternals::Iso { 
    class IsoImage; 
    struct IsoEntry;
}

class IsoDialog : public QDialog {
    Q_OBJECT
public:
    explicit IsoDialog(QWidget* parent = nullptr);
    ~IsoDialog() override;

    // Load an ISO or GOD file
    void loadIsoContents(const QString& path);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

signals:
    void extractionProgressUpdate(int current, int total);

private slots:
    void onExtractSelected();
    void onExtractAll();
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onSearchTextChanged(const QString& text);
    void onExpandAll();
    void onCollapseAll();
    void onProgressUpdate(int current, int total);

private:
    struct ExtractionResult {
        bool success;
        int filesExtracted;
        QString errorMessage;
        QString outputPath;
    };

    void populateTree();
    void openFileInViewer(const XboxInternals::Iso::IsoEntry& entry);
    QTreeWidgetItem* findOrCreateFolder(const QString& path);
    void onExtractionFinished();
    
    QPushButton* extractSelectedButton_ = nullptr;
    QPushButton* extractAllButton_ = nullptr;
    QPushButton* expandAllButton_ = nullptr;
    QPushButton* collapseAllButton_ = nullptr;
    QLineEdit* searchBox_ = nullptr;
    QTreeWidget* tree_ = nullptr;
    QProgressDialog* extractionProgress_ = nullptr;
    QFutureWatcher<ExtractionResult>* extractionWatcher_ = nullptr;
    
    std::unique_ptr<XboxInternals::Iso::IsoImage> iso_;
    QString currentIsoPath_;
    QMap<QString, QTreeWidgetItem*> folderItems_;
};

