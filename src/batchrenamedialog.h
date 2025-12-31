#ifndef BATCHRENAMEDIALOG_H
#define BATCHRENAMEDIALOG_H

#include <QDialog>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QCheckBox;
class QSpinBox;
class QTableWidget;
class QGroupBox;
QT_END_NAMESPACE

class BatchRenameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BatchRenameDialog(QWidget *parent = nullptr);
    ~BatchRenameDialog();

private slots:
    void browsePath();
    void loadFiles();
    void addFiles();
    void removeFiles();
    void clearFiles();
    void previewRename();
    void executeRename();
    void onPatternChanged();
    void onCounterStartChanged();

private:
    void setupUI();
    void updatePreview();
    QString generateNewName(const QString &oldName, int index);
    QString formatFileSize(qint64 size) const;

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_pathLayout;
    QGridLayout *m_renameLayout;
    QHBoxLayout *m_buttonLayout;
    QHBoxLayout *m_actionLayout;

    QLineEdit *m_pathEdit;
    QPushButton *m_browseButton;
    QPushButton *m_loadButton;

    QTableWidget *m_filesTable;

    QGroupBox *m_renameGroup;
    QComboBox *m_renameModeCombo;
    QLineEdit *m_prefixEdit;
    QLineEdit *m_suffixEdit;
    QLineEdit *m_patternEdit;
    QSpinBox *m_counterStartSpinBox;
    QLineEdit *m_replaceOldEdit;
    QLineEdit *m_replaceNewEdit;
    QCheckBox *m_caseSensitiveCheckBox;

    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_clearButton;
    QPushButton *m_previewButton;
    QPushButton *m_executeButton;
    QPushButton *m_closeButton;

    QList<QFileInfo> m_fileList;
};

#endif // BATCHRENAMEDIALOG_H