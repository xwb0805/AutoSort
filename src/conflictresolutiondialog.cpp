#include "conflictresolutiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QMessageBox>
#include <QFileInfo>
#include <QGroupBox>
#include <QFrame>

ConflictResolutionDialog::ConflictResolutionDialog(const QFileInfo &sourceFile, const QFileInfo &targetFile,
                                                   QWidget *parent)
    : QDialog(parent)
    , m_sourceFile(sourceFile)
    , m_targetFile(targetFile)
    , m_selectedAction(ConflictAction::Cancel)
{
    setupUI();
}

ConflictResolutionDialog::~ConflictResolutionDialog()
{
}

ConflictAction ConflictResolutionDialog::getSelectedAction() const
{
    return m_selectedAction;
}

QString ConflictResolutionDialog::getNewName() const
{
    return m_newName;
}

void ConflictResolutionDialog::onActionChanged()
{
    m_renameEdit->setEnabled(m_renameRadio->isChecked());
}

void ConflictResolutionDialog::setupUI()
{
    setWindowTitle("文件冲突");
    setMinimumSize(500, 300);

    m_mainLayout = new QVBoxLayout(this);

    // 消息标签
    m_messageLabel = new QLabel("目标位置已存在同名文件，请选择操作:", this);
    m_messageLabel->setWordWrap(true);
    m_mainLayout->addWidget(m_messageLabel);

    // 源文件信息
    m_sourceInfoLabel = new QLabel(this);
    m_sourceInfoLabel->setText(QString("源文件:\n  路径: %1\n  大小: %2\n  修改时间: %3")
        .arg(m_sourceFile.absoluteFilePath())
        .arg(formatFileSize(m_sourceFile.size()))
        .arg(m_sourceFile.lastModified().toString("yyyy-MM-dd hh:mm:ss")));
    m_sourceInfoLabel->setStyleSheet("background-color: #E3F2FD; padding: 10px; border-radius: 5px;");
    m_mainLayout->addWidget(m_sourceInfoLabel);

    // 目标文件信息
    m_targetInfoLabel = new QLabel(this);
    m_targetInfoLabel->setText(QString("目标文件:\n  路径: %1\n  大小: %2\n  修改时间: %3")
        .arg(m_targetFile.absoluteFilePath())
        .arg(formatFileSize(m_targetFile.size()))
        .arg(m_targetFile.lastModified().toString("yyyy-MM-dd hh:mm:ss")));
    m_targetInfoLabel->setStyleSheet("background-color: #FFEBEE; padding: 10px; border-radius: 5px;");
    m_mainLayout->addWidget(m_targetInfoLabel);

    // 操作选择
    m_actionGroup = new QButtonGroup(this);

    m_overwriteRadio = new QRadioButton("覆盖目标文件", this);
    m_actionGroup->addButton(m_overwriteRadio, static_cast<int>(ConflictAction::Overwrite));
    m_mainLayout->addWidget(m_overwriteRadio);

    m_skipRadio = new QRadioButton("跳过此文件", this);
    m_actionGroup->addButton(m_skipRadio, static_cast<int>(ConflictAction::Skip));
    m_skipRadio->setChecked(true);
    m_mainLayout->addWidget(m_skipRadio);

    m_renameRadio = new QRadioButton("重命名文件", this);
    m_actionGroup->addButton(m_renameRadio, static_cast<int>(ConflictAction::Rename));
    m_mainLayout->addWidget(m_renameRadio);

    // 分隔线
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    m_mainLayout->addWidget(line);

    // 批量操作
    QLabel *batchLabel = new QLabel("批量操作（将应用到所有后续冲突）:", this);
    batchLabel->setStyleSheet("font-weight: bold; color: #666;");
    m_mainLayout->addWidget(batchLabel);

    m_overwriteAllRadio = new QRadioButton("全部覆盖", this);
    m_actionGroup->addButton(m_overwriteAllRadio, static_cast<int>(ConflictAction::OverwriteAll));
    m_mainLayout->addWidget(m_overwriteAllRadio);

    m_skipAllRadio = new QRadioButton("全部跳过", this);
    m_actionGroup->addButton(m_skipAllRadio, static_cast<int>(ConflictAction::SkipAll));
    m_mainLayout->addWidget(m_skipAllRadio);

    // 重命名输入框
    QHBoxLayout *renameLayout = new QHBoxLayout();
    renameLayout->addSpacing(20);
    renameLayout->addWidget(new QLabel("新名称:", this));
    m_renameEdit = new QLineEdit(this);
    m_renameEdit->setText(m_sourceFile.baseName() + "_1." + m_sourceFile.suffix());
    m_renameEdit->setEnabled(false);
    renameLayout->addWidget(m_renameEdit);
    m_mainLayout->addLayout(renameLayout);

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_okButton = new QPushButton("确定", this);
    buttonLayout->addWidget(m_okButton);

    m_cancelButton = new QPushButton("取消", this);
    buttonLayout->addWidget(m_cancelButton);

    m_mainLayout->addLayout(buttonLayout);

    // 连接信号
    connect(m_overwriteRadio, &QRadioButton::clicked, this, &ConflictResolutionDialog::onActionChanged);
    connect(m_skipRadio, &QRadioButton::clicked, this, &ConflictResolutionDialog::onActionChanged);
    connect(m_renameRadio, &QRadioButton::clicked, this, &ConflictResolutionDialog::onActionChanged);
    connect(m_overwriteAllRadio, &QRadioButton::clicked, this, &ConflictResolutionDialog::onActionChanged);
    connect(m_skipAllRadio, &QRadioButton::clicked, this, &ConflictResolutionDialog::onActionChanged);

    connect(m_okButton, &QPushButton::clicked, [this]() {
        int selectedId = m_actionGroup->checkedId();
        m_selectedAction = static_cast<ConflictAction>(selectedId);

        if (m_selectedAction == ConflictAction::Rename) {
            m_newName = m_renameEdit->text().trimmed();
            if (m_newName.isEmpty()) {
                QMessageBox::warning(this, "错误", "请输入新文件名");
                return;
            }
        }

        accept();
    });

    connect(m_cancelButton, &QPushButton::clicked, [this]() {
        m_selectedAction = ConflictAction::Cancel;
        reject();
    });
}

QString ConflictResolutionDialog::formatFileSize(qint64 size) const
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