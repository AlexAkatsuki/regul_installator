#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QProgressBar>

#include "InstallerEngine.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

    static constexpr uint16_t MW_WIDTH = 600;
    static constexpr uint16_t MW_HEIGHT = 400;

    static constexpr uint8_t FRAME_MIN_HEIGHT = 200;

    static constexpr uint8_t MAX_ROW_SIZE = 20;

    static constexpr uint8_t MIN_SCROLL_BAR_VAL = 0;
    static constexpr uint8_t MAX_SCROLL_BAR_VAL = 100;

public:
    enum class Screen {
        Welcome,
        PackageSelection,
        Installation,
        Finished
    };

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNextClicked();
    void onBackClicked();
    void onInstallationStarted();
    void onInstallationProgress(const QString &message);
    void onInstallationFinished(bool success);
    void onInstallationError(const QString &error);

private:
    void setupUI();
    void showScreen(Screen screen);
    void setupWelcomeScreen();
    void setupPackageSelectionScreen();
    void setupInstallationScreen();

    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QStackedWidget *m_stackedWidget;

    QWidget *m_welcomeScreen;
    QWidget *m_selectionScreen;
    QWidget *m_installationScreen;

    QLabel *m_frameLabel;
    QPlainTextEdit *m_statusText;
    QProgressBar *m_progressBar;
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_backButton;
    QPushButton *m_nextButton;

    QComboBox *m_packageComboBox;

    InstallerEngine *m_installerEngine;

    Screen m_currentScreen;
};

#endif // MAINWINDOW_H
