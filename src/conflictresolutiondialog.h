#ifndef CONFLICTRESOLUTIONDIALOG_H
#define CONFLICTRESOLUTIONDIALOG_H

#include "common.h"
#include <QDialog>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QPushButton;
class QRadioButton;
class QButtonGroup;
class QLineEdit;
QT_END_NAMESPACE

class ConflictResolutionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConflictResolutionDialog(const QFileInfo &sourceFile, const QFileInfo &targetFile,
                                     QWidget *parent = nullptr);
    ~ConflictResolutionDialog();

    ConflictAction getSelectedAction() const;
    QString getNewName() const;

private slots:
    void onActionChanged();

private:
    void setupUI();
    QString formatFileSize(qint64 size) const;

    QFileInfo m_sourceFile;
    QFileInfo m_targetFile;
    ConflictAction m_selectedAction;
    QString m_newName;

    QVBoxLayout *m_mainLayout;
    QLabel *m_messageLabel;
    QLabel *m_sourceInfoLabel;
    QLabel *m_targetInfoLabel;
    QButtonGroup *m_actionGroup;
    QRadioButton *m_overwriteRadio;
    QRadioButton *m_skipRadio;
    QRadioButton *m_renameRadio;
    QRadioButton *m_overwriteAllRadio;
    QRadioButton *m_skipAllRadio;
    QLineEdit *m_renameEdit;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
};

#endif // CONFLICTRESOLUTIONDIALOG_H