#ifndef FILEFILTERDIALOG_H
#define FILEFILTERDIALOG_H

#include <QDialog>
#include <QDate>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QCheckBox;
class QDateEdit;
class QTableWidget;
class QGroupBox;
QT_END_NAMESPACE

struct FilterRule {
    QString name;
    QString extension;
    qint64 minSize;
    qint64 maxSize;
    QDate minDate;
    QDate maxDate;
    bool enabled;

    FilterRule()
        : minSize(0)
        , maxSize(-1)
        , enabled(true)
    {}
};

class FileFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileFilterDialog(QWidget *parent = nullptr);
    ~FileFilterDialog();

    QList<FilterRule> getFilters() const;

private slots:
    void addFilter();
    void removeFilter();
    void clearAll();
    void applyFilters();
    void onFilterChanged();

private:
    void setupUI();
    void updateFilterTable();
    void loadFilters();
    void saveFilters();
    QString formatFileSize(qint64 size) const;

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonLayout;

    QTableWidget *m_filtersTable;

    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_clearButton;
    QPushButton *m_applyButton;
    QPushButton *m_closeButton;

    QList<FilterRule> m_filters;
};

#endif // FILEFILTERDIALOG_H