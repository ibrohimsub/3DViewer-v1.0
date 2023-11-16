// openglwidget.h

#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#define GL_SILENCE_DEPRECATION

#include <QColor>
#include <QColorDialog>
#include <QGesture>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QPanGesture>  // Added for pan gesture
#include <QPushButton>
#include <QWheelEvent>
#include <algorithm>
#include <cstring>

#include "mainwindow.h"

using namespace std;
extern "C" {
#include "../backend/s21_back.h"
}

class GLWidget : public QOpenGLWidget {
  Q_OBJECT

  bool event(QEvent* event) override;
  bool gestureEvent(QGestureEvent* event);

 public:
  GLWidget(QWidget* parent = nullptr);
  ~GLWidget();
  QColor backgroundColor;
  QColor lineColor;
  QColor dotColor;

  int projectionMode;
  int pointMode;
  int pointSize;
  int lineSize;
  int lineMode;

  data_t data = {0, 0, NULL, 0.0, {0, 0, 0}, NULL, NULL, 0, NULL};

  char model_file_name[S_SIZE] = {0};

  void initSettings();

 public slots:

  //  void onXTranslationChanged(int value);
  //  void onYTranslationChanged(int value);
  //  void onZTranslationChanged(int value);
  //  void onXRotationChanged(int value);
  //  void onYRotationChanged(int value);
  //  void onScaleChanged(int value);

  void setXTranslate(int step);
  void setYTranslate(int step);
  void setZTranslate(int step);

  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);

  void setScale(int scale);

 protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  // Mouse and touchpad interaction
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void pinchTriggered(QPinchGesture* gesture);
  void panTriggered(QPanGesture* gesture);  // Added for pan gesture

 private:
  const int fullRotation =
      360 * 16;  // 360 degrees represented in the fixed-point format
  const int initialRotation =
      fullRotation / 2;              // Start at 180 degrees (centered)
  const int initialTranslation = 0;  // Initial translation value
  const int initialScale = 1000;     // Initial scale value

  int xPrevTranslation = initialTranslation;
  int yPrevTranslation = initialTranslation;
  int zPrevTranslation = initialTranslation;
  int xPrevRotation = initialRotation;
  int yPrevRotation = initialRotation;
  int zPrevRotation = initialRotation;
  int prevScale = initialScale;
  int xRotation = initialRotation;
  int yRotation = initialRotation;
  int zRotation = initialRotation;
  int xTranslation = initialTranslation;
  int yTranslation = initialTranslation;
  int zTranslation = initialTranslation;
  int Vscale = initialScale;

  bool leftMousePressed;
  bool rightMousePressed;
  bool middleMousePressed;

  QPoint lastMousePos;
  QPoint lastTouchPos;   // Store the last touch position
  bool panning = false;  // A flag to indicate if panning is active

  void setupPerspective();
  void pointDrawing();
  void lineDrawing();
};

#endif  // OPENGLWIDGET_H
