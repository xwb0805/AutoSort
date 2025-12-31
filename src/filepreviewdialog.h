#ifndef FILEPREVIEWDIALOG_H
#define FILEPREVIEWDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QTextEdit;
class QTableWidget;
QT_END_NAMESPACE

class FilePreviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilePreviewDialog(const QFileInfo &fileInfo, QWidget *parent = nullptr);
    ~FilePreviewDialog();

private:
    void setupUI();
    void previewFile();
    void previewImage();
    void previewText();
    void previewBinary();
    void previewDirectory();
    QString formatFileSize(qint64 size) const;

    QFileInfo m_fileInfo;
    QVBoxLayout *m_mainLayout;
    QLabel *m_filePathLabel;
    QLabel *m_fileSizeLabel;
    QLabel *m_fileTypeLabel;
    QLabel *m_modifiedTimeLabel;
    QTabWidget *m_tabWidget;
    QWidget *m_imagePreviewWidget;
    QWidget *m_textPreviewWidget;
    QWidget *m_infoPreviewWidget;
    QLabel *m_imageLabel;
    QTextEdit *m_textEdit;
    QTableWidget *m_infoTable;
    QPushButton *m_closeButton;
};

#endif // FILEPREVIEWDIALOG_H