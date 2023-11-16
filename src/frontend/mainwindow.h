#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define _USE_MATH_DEFINES
#include <QColor>
#include <QColorDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsWidget>
#include <QLabel>
#include <QMainWindow>
#include <QSettings>
#include <QTimer>
#include <cmath>
#include <cstring>

#include "openglwidget.h"
#include "qgifimage.h"

extern "C" {
#include "../backend/s21_back.h"
}

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  int valueNormalize(int val);
  void setProjectionMode(int mode);
  void setLineMode(int mode);

  void setPointMode(int mode);
  void setVertexSize(int size);

  void setPointMode(int mode, int pointSize);

  void pickColor(QColor &colorTarget);
  void onChangeColor(QColor &targetColor, QLabel *colorLabel);
  void updateUiColors(QLabel *colorLabel, const QColor &color);
  void setUIColor(QWidget *widget, const QColor &color);

 private slots:

  void incrementXRotation();
  void incrementYRotation();
  void incrementZRotation();

  void stopRotation();

  //  void startRotation();
  //  void stopRotation();

  // Translation
  void xTranslateSliderValueChanged(int value);
  void yTranslateSliderValueChanged(int value);
  void zTranslateSliderValueChanged(int value);
  void xTranslateTextEdit();
  void yTranslateTextEdit();
  void zTranslateTextEdit();

  // Rotation
  void xRotateLineEditReturnPressed();
  void yRotateLineEditReturnPressed();
  void zRotateLineEditReturnPressed();

  void xRotateValueChanged(int value);
  void yRotateValueChanged(int value);
  void zRotateValueChanged(int value);
  void xRotateTextEdit();
  void yRotateTextEdit();
  void zRotateTextEdit();

  // Scaling
  void scaleSliderValueChanged(int value);
  void scaleTextEdit();
  //  // private slots to handle updates from GLWidget signals
  //  void updateXTranslation(int value);
  //  void updateYTranslation(int value);
  //  void updateZTranslation(int value);
  //  void updateXRotation(int value);
  //  void updateYRotation(int value);
  //  void updateScale(int value);

  // Projection
  void projectionParallel();
  void projectionCentral();

  // View Settings
  void DisableView();
  void CircleView();
  void SquareView();
  void resetValue();

  // Vertex and Line Settings
  void vertexSize(int value);
  void linesTypeSolid();
  void linesTypeDashed();
  void linesSizeSliderChanged(int value);

  // File Actions
  void on_action_triggered();
  void on_bmp_button_clicked();
  void on_gif_button_clicked();

  // Color Settings
  void onChangeLineColor();
  void onChangeBackgroundColor();
  void onChangeDotColor();

  // Timer Slot
  void slotTimer();

 private:
  int rotationIncrement = 1;
  int xRotationValue = 0;
  int yRotationValue = 0;
  int zRotationValue = 0;
  Ui::MainWindow *ui;
  QString settingFile;
  QTimer *timer;
  QGifImage *gif;
  QImage *gifImage;
  int gifTime = 0;

  QTimer rotationTimer;
  bool rotatingX = false;
  bool rotatingY = false;
  bool rotatingZ = false;

  QPushButton *rotationButton;  // Button to start/stop rotation

  void color_by_customer();
  void connectSetup();
  void sliderSetup();
  void defaultSettings();
  void saveSettings();
  void defaultSliders();
  void updateUiColors();
};
#endif  // MAINWINDOW_H
