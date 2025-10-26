#include <QApplication>
#include <QMessageBox>
#include <QFontDatabase>

#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(new QWidget(this))
    , m_mainLayout(new QVBoxLayout(m_centralWidget))
    , m_stackedWidget(new QStackedWidget(this))
    , m_welcomeScreen(nullptr)
    , m_selectionScreen(nullptr)
    , m_installationScreen(nullptr)
    , m_statusText(nullptr)
    , m_installerEngine(new InstallerEngine(this))
    , m_currentScreen(Screen::Welcome) {

    setupUI();
    showScreen(Screen::Welcome);

    if (!m_installerEngine->loadPackages())
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Не удалось загрузить информацию о пакетах"));

    connect(m_installerEngine, &InstallerEngine::installationStarted,
                                    this, &MainWindow::onInstallationStarted);
    connect(m_installerEngine, &InstallerEngine::installationProgress,
                                    this, &MainWindow::onInstallationProgress);
    connect(m_installerEngine, &InstallerEngine::installationFinished,
                                    this, &MainWindow::onInstallationFinished);
    connect(m_installerEngine, &InstallerEngine::installationError,
                                    this, &MainWindow::onInstallationError);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {

    setWindowTitle(tr("Установщик пакетов"));
    setFixedSize(MW_WIDTH, MW_HEIGHT);

    m_frameLabel = new QLabel();
    m_frameLabel->setAlignment(Qt::AlignCenter);
    m_frameLabel->setStyleSheet("QLabel { border: 2px solid gray; padding: 20px; }");
    m_frameLabel->setMinimumHeight(FRAME_MIN_HEIGHT);

    m_buttonLayout = new QHBoxLayout();
    m_backButton = new QPushButton(tr("Назад"));
    m_nextButton = new QPushButton(tr("Далее"));

    m_backButton->setVisible(false);

    m_buttonLayout->addWidget(m_backButton);
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_nextButton);

    connect(m_backButton, &QPushButton::clicked, this, &MainWindow::onBackClicked);
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::onNextClicked);

    m_mainLayout->addWidget(m_frameLabel);
    m_mainLayout->addLayout(m_buttonLayout);

    setCentralWidget(m_centralWidget);
}

void MainWindow::showScreen(Screen screen) {

    m_currentScreen = screen;

    switch (screen) {
        case Screen::Welcome:
            setupWelcomeScreen();
            break;
        case Screen::PackageSelection:
            setupPackageSelectionScreen();
            break;
        case Screen::Installation:
            setupInstallationScreen();
            break;
        case Screen::Finished:
            break;
    }
}

void MainWindow::setupWelcomeScreen() {

    if (m_mainLayout->indexOf(m_stackedWidget) != -1) {
        m_stackedWidget->setVisible(false);
        m_frameLabel->setVisible(true);
        m_mainLayout->replaceWidget(m_stackedWidget, m_frameLabel);
    }

    m_frameLabel->setText(tr("<h1>Банников</h1>"));

    m_backButton->setVisible(false);
    m_nextButton->setText(tr("Далее"));
    m_nextButton->setEnabled(true);

    m_nextButton->disconnect();
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::onNextClicked, Qt::UniqueConnection);
}

void MainWindow::setupPackageSelectionScreen() {

    if (m_selectionScreen == nullptr) {
        m_selectionScreen = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(m_selectionScreen);

        QLabel *titleLabel = new QLabel(tr("<h3>Выберите пакет для установки:</h3>"));
        titleLabel->setAlignment(Qt::AlignCenter);

        m_packageComboBox = new QComboBox();
        m_packageComboBox->addItems(m_installerEngine->getAvailablePackages());

        layout->addWidget(titleLabel);
        layout->addWidget(m_packageComboBox);
        layout->addStretch();

        m_stackedWidget->addWidget(m_selectionScreen);
    }

    if (m_mainLayout->indexOf(m_frameLabel) != -1) {
        m_frameLabel->setVisible(false);
        m_stackedWidget->setVisible(true);
        m_mainLayout->replaceWidget(m_frameLabel, m_stackedWidget);
    }

    m_stackedWidget->setCurrentWidget(m_selectionScreen);
    m_backButton->setVisible(true);
    m_nextButton->setText(tr("Установить"));
    m_nextButton->setEnabled(true);

    m_nextButton->disconnect();
    connect(m_nextButton, &QPushButton::clicked, this,
                            &MainWindow::onNextClicked, Qt::UniqueConnection);
}

void MainWindow::setupInstallationScreen() {

    if (m_installationScreen == nullptr) {
        m_installationScreen = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(m_installationScreen);

        QLabel *titleLabel = new QLabel(tr("<h3>Установка пакета...</h3>"));
        titleLabel->setAlignment(Qt::AlignCenter);

        m_statusText = new QPlainTextEdit();
        m_statusText->setReadOnly(true);
        m_statusText->setMaximumBlockCount(MAX_ROW_SIZE);

        QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        m_statusText->setFont(font);

        m_progressBar = new QProgressBar();
        m_progressBar->setRange(MIN_SCROLL_BAR_VAL, MIN_SCROLL_BAR_VAL);

        layout->addWidget(titleLabel);
        layout->addWidget(m_statusText);
        layout->addWidget(m_progressBar);

        m_stackedWidget->addWidget(m_installationScreen);
    }

    m_statusText->clear();
    m_progressBar->setValue(MIN_SCROLL_BAR_VAL);

    m_stackedWidget->setCurrentWidget(m_installationScreen);
    m_backButton->setVisible(false);
    m_nextButton->setVisible(false);

    QString selectedPackage = m_packageComboBox->currentText();
    m_statusText->appendPlainText(tr("Выбран пакет: %1").arg(selectedPackage));
    m_installerEngine->installPackage(selectedPackage);
}

void MainWindow::onNextClicked() {

    switch (m_currentScreen) {
        case Screen::Welcome:
            showScreen(Screen::PackageSelection);
            break;
        case Screen::PackageSelection:
            showScreen(Screen::Installation);
            break;
        default:
            break;
    }
}

void MainWindow::onBackClicked() {

    switch (m_currentScreen) {
        case Screen::PackageSelection:
            showScreen(Screen::Welcome);
            break;
        case Screen::Installation:
            showScreen(Screen::PackageSelection);
            break;
        default:
            break;
    }
}

void MainWindow::onInstallationStarted() {
    m_statusText->appendPlainText(tr("Начало установки..."));
}

void MainWindow::onInstallationProgress(const QString &message) {

    m_statusText->appendPlainText(message);

    QTextCursor cursor = m_statusText->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_statusText->setTextCursor(cursor);
    m_statusText->ensureCursorVisible();
}

void MainWindow::onInstallationFinished(bool success) {

    m_progressBar->setRange(MIN_SCROLL_BAR_VAL, MAX_SCROLL_BAR_VAL);
    m_progressBar->setValue(MAX_SCROLL_BAR_VAL);

    if (success)
        m_statusText->appendPlainText(tr("Установка завершена!"));
    else
        m_statusText->appendPlainText(tr("Установка не удалась"));

    m_backButton->setVisible(true);
    m_nextButton->setVisible(true);
    m_nextButton->setText(tr("Завершить"));
    m_nextButton->setEnabled(true);

    m_nextButton->disconnect();
    connect(m_nextButton, &QPushButton::clicked, this, []() {
        QApplication::quit();
    });

    m_backButton->disconnect();
    connect(m_backButton, &QPushButton::clicked, this,
                            &MainWindow::onBackClicked, Qt::UniqueConnection);
}

void MainWindow::onInstallationError(const QString &error) {

    m_statusText->appendPlainText(tr("Ошибка: %1").arg(error));

    m_backButton->setVisible(true);
    m_nextButton->setVisible(true);
    m_nextButton->setText(tr("Повторить"));
    m_nextButton->setEnabled(true);

    m_nextButton->disconnect();
    connect(m_nextButton, &QPushButton::clicked, this,
                            &MainWindow::onNextClicked, Qt::UniqueConnection);

    m_backButton->disconnect();
    connect(m_backButton, &QPushButton::clicked, this,
                            &MainWindow::onBackClicked, Qt::UniqueConnection);
}
