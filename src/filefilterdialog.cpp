#include "filefilterdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QInputDialog>
#include <QDate>

FileFilterDialog::FileFilterDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    loadFilters();
}

FileFilterDialog::~FileFilterDialog()
{
}

void FileFilterDialog::setupUI()
{
    setWindowTitle("文件过滤规则");
    setMinimumSize(800, 600);

    m_mainLayout = new QVBoxLayout(this);

    // 过滤规则表格
    m_filtersTable = new QTableWidget(this);
    m_filtersTable->setColumnCount(7);
    m_filtersTable->setHorizontalHeaderLabels(QStringList() << "启用" << "名称" << "扩展名" << "最小大小" << "最大大小" << "最早日期" << "最晚日期");
    m_filtersTable->horizontalHeader()->setStretchLastSection(true);
    m_filtersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_filtersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_mainLayout->addWidget(m_filtersTable);

    // 操作按钮
    m_buttonLayout = new QHBoxLayout();

    m_addButton = new QPushButton("添加规则", this);
    m_buttonLayout->addWidget(m_addButton);

    m_removeButton = new QPushButton("删除规则", this);
    m_buttonLayout->addWidget(m_removeButton);

    m_clearButton = new QPushButton("清空所有", this);
    m_buttonLayout->addWidget(m_clearButton);

    m_buttonLayout->addStretch();

    m_applyButton = new QPushButton("应用", this);
    m_applyButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; }");
    m_buttonLayout->addWidget(m_applyButton);

    m_closeButton = new QPushButton("关闭", this);
    m_buttonLayout->addWidget(m_closeButton);

    m_mainLayout->addLayout(m_buttonLayout);

    // 连接信号
    connect(m_addButton, &QPushButton::clicked, this, &FileFilterDialog::addFilter);
    connect(m_removeButton, &QPushButton::clicked, this, &FileFilterDialog::removeFilter);
    connect(m_clearButton, &QPushButton::clicked, this, &FileFilterDialog::clearAll);
    connect(m_applyButton, &QPushButton::clicked, this, &FileFilterDialog::applyFilters);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

QList<FilterRule> FileFilterDialog::getFilters() const
{
    return m_filters;
}

void FileFilterDialog::addFilter()
{
    FilterRule rule;
    rule.name = QString("规则 %1").arg(m_filters.size() + 1);

    // 简单对话框输入规则
    bool ok;
    QString extensions = QInputDialog::getText(this, "添加过滤规则", "文件扩展名（用逗号分隔）:", QLineEdit::Normal, "", &ok);
    if (!ok || extensions.isEmpty()) {
        return;
    }

    rule.extension = extensions;

    QString minSizeStr = QInputDialog::getText(this, "添加过滤规则", "最小大小（KB，留空表示无限制）:", QLineEdit::Normal, "", &ok);
    if (ok && !minSizeStr.isEmpty()) {
        rule.minSize = minSizeStr.toLongLong() * 1024;
    }

    QString maxSizeStr = QInputDialog::getText(this, "添加过滤规则", "最大大小（KB，留空表示无限制）:", QLineEdit::Normal, "", &ok);
    if (ok && !maxSizeStr.isEmpty()) {
        rule.maxSize = maxSizeStr.toLongLong() * 1024;
    }

    m_filters.append(rule);
    updateFilterTable();
    saveFilters();
}

void FileFilterDialog::removeFilter()
{
    int currentRow = m_filtersTable->currentRow();
    if (currentRow >= 0) {
        m_filters.removeAt(currentRow);
        updateFilterTable();
        saveFilters();
    }
}

void FileFilterDialog::clearAll()
{
    if (QMessageBox::question(this, "确认", "确定要清空所有过滤规则吗？",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        m_filters.clear();
        updateFilterTable();
        saveFilters();
    }
}

void FileFilterDialog::applyFilters()
{
    saveFilters();
    QMessageBox::information(this, "成功", "过滤规则已应用");
}

void FileFilterDialog::onFilterChanged()
{
    updateFilterTable();
}

void FileFilterDialog::updateFilterTable()
{
    m_filtersTable->setRowCount(0);

    for (int i = 0; i < m_filters.size(); ++i) {
        const FilterRule &rule = m_filters[i];

        int row = m_filtersTable->rowCount();
        m_filtersTable->insertRow(row);

        // 启用复选框
        QTableWidgetItem *enabledItem = new QTableWidgetItem();
        enabledItem->setCheckState(rule.enabled ? Qt::Checked : Qt::Unchecked);
        m_filtersTable->setItem(row, 0, enabledItem);

        m_filtersTable->setItem(row, 1, new QTableWidgetItem(rule.name));
        m_filtersTable->setItem(row, 2, new QTableWidgetItem(rule.extension));
        m_filtersTable->setItem(row, 3, new QTableWidgetItem(rule.minSize > 0 ? formatFileSize(rule.minSize) : "无限制"));
        m_filtersTable->setItem(row, 4, new QTableWidgetItem(rule.maxSize >= 0 ? formatFileSize(rule.maxSize) : "无限制"));
        m_filtersTable->setItem(row, 5, new QTableWidgetItem(rule.minDate.isValid() ? rule.minDate.toString("yyyy-MM-dd") : "无限制"));
        m_filtersTable->setItem(row, 6, new QTableWidgetItem(rule.maxDate.isValid() ? rule.maxDate.toString("yyyy-MM-dd") : "无限制"));
    }
}

void FileFilterDialog::loadFilters()
{
    QString filterPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(filterPath);
    filterPath += "/filters.json";

    QFile file(filterPath);
    if (!file.exists()) {
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open filters file:" << filterPath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        qWarning() << "Invalid filters file format";
        return;
    }

    QJsonArray filterArray = doc.array();
    m_filters.clear();

    for (const QJsonValue &value : filterArray) {
        QJsonObject filterObj = value.toObject();

        FilterRule rule;
        rule.name = filterObj.value("name").toString();
        rule.extension = filterObj.value("extension").toString();
        rule.minSize = filterObj.value("minSize").toVariant().toLongLong();
        rule.maxSize = filterObj.value("maxSize").toVariant().toLongLong();
        rule.enabled = filterObj.value("enabled").toBool(true);

        QString minDateStr = filterObj.value("minDate").toString();
        if (!minDateStr.isEmpty()) {
            rule.minDate = QDate::fromString(minDateStr, "yyyy-MM-dd");
        }

        QString maxDateStr = filterObj.value("maxDate").toString();
        if (!maxDateStr.isEmpty()) {
            rule.maxDate = QDate::fromString(maxDateStr, "yyyy-MM-dd");
        }

        m_filters.append(rule);
    }

    updateFilterTable();
}

void FileFilterDialog::saveFilters()
{
    QString filterPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(filterPath);
    filterPath += "/filters.json";

    QJsonArray filterArray;

    for (const FilterRule &rule : m_filters) {
        QJsonObject filterObj;
        filterObj["name"] = rule.name;
        filterObj["extension"] = rule.extension;
        filterObj["minSize"] = QJsonValue::fromVariant(rule.minSize);
        filterObj["maxSize"] = QJsonValue::fromVariant(rule.maxSize);
        filterObj["enabled"] = rule.enabled;

        if (rule.minDate.isValid()) {
            filterObj["minDate"] = rule.minDate.toString("yyyy-MM-dd");
        }

        if (rule.maxDate.isValid()) {
            filterObj["maxDate"] = rule.maxDate.toString("yyyy-MM-dd");
        }

        filterArray.append(filterObj);
    }

    QJsonDocument doc(filterArray);

    QFile file(filterPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot write filters file:" << filterPath;
        return;
    }

    file.write(doc.toJson());
    file.close();
}

QString FileFilterDialog::formatFileSize(qint64 size) const
{
    const qint64 KB = 1024;
    const qint64 MB = 1024 * KB;
    const qint64 GB = 1024 * MB;

    if (size < KB) {
        return QString("%1 B").arg(size);
    } else if (size < MB) {
        return QString("%1 KB").arg(size / (double)KB, 0, 'f', 2);
    } else if (size < GB) {
        return QString("%1 MB").arg(size / (double)MB, 0, 'f', 2);
    } else {
        return QString("%1 GB").arg(size / (double)GB, 0, 'f', 2);
    }
}