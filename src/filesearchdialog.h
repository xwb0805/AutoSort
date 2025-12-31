#ifndef FILESEARCHDIALOG_H
#define FILESEARCHDIALOG_H

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
class QTableWidget;
class QProgressBar;
class QGroupBox;
QT_END_NAMESPACE

class FileSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileSearchDialog(QWidget *parent = nullptr);
    ~FileSearchDialog();

private slots:
    void browsePath();
    void startSearch();
    void stopSearch();
    void openSelectedFile();
    void showSelectedFileInfo();

private:
    void setupUI();
    void searchFiles(const QString &path, const QString &namePattern, const QString &extension,
                    qint64 minSize, qint64 maxSize, bool includeSubdirs);
    void addSearchResult(const QFileInfo &fileInfo);
    QString formatFileSize(qint64 size) const;
    void updateProgress(int current, int total);

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_pathLayout;
    QHBoxLayout *m_searchLayout;
    QHBoxLayout *m_filterLayout;
    QHBoxLayout *m_buttonLayout;

    QLineEdit *m_pathEdit;
    QPushButton *m_browseButton;
    QLineEdit *m_nameEdit;
    QLineEdit *m_extensionEdit;
    QComboBox *m_sizeOperatorCombo;
    QLineEdit *m_sizeEdit;
    QComboBox *m_sizeUnitCombo;
    QCheckBox *m_includeSubdirsCheckBox;
    QCheckBox *m_caseSensitiveCheckBox;

    QPushButton *m_searchButton;
    QPushButton *m_stopButton;
    QPushButton *m_openButton;
    QPushButton *m_infoButton;
    QPushButton *m_closeButton;

    QTableWidget *m_resultsTable;
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;

    bool m_isSearching;
    bool m_stopRequested;
};

#endif // FILESEARCHDIALOG_H