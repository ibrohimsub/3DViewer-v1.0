#include "openglwidget.h"

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {
  // Initialize settings for the OpenGL widget
  initSettings();

  // Initialize mouse button states to false
  leftMousePressed = false;
  rightMousePressed = false;
  middleMousePressed = false;

  // Enable touch events for the widget
  setAttribute(Qt::WA_AcceptTouchEvents);

  // Grab PanGesture events for panning
  grabGesture(Qt::PanGesture);
}

GLWidget::~GLWidget() { s21_remove_data(&data); }

void GLWidget::initSettings() {
  // Initialize various rendering and display settings
  const int defaultProjectionMode = 0;
  const int defaultPointSize = 5;
  const int defaultPointMode = 1;
  const int defaultLineSize = 1;
  const int defaultLineMode = 0;

  projectionMode = defaultProjectionMode;
  pointSize = defaultPointSize;
  pointMode = defaultPointMode;
  lineSize = defaultLineSize;
  lineMode = defaultLineMode;

  // Set background color to black and line/dot colors to white
  const int blackColorValue = 0;
  const int whiteColorValue = 255;

  backgroundColor.setRgb(blackColorValue, blackColorValue, blackColorValue);
  lineColor.setRgb(whiteColorValue, whiteColorValue, whiteColorValue);
  dotColor.setRgb(whiteColorValue, whiteColorValue, whiteColorValue);
}

void GLWidget::initializeGL() {
  // Set the initial clear color and depth settings
  const qreal initialClearColorRed = 0.0;
  const qreal initialClearColorGreen = 0.0;
  const qreal initialClearColorBlue = 0.0;
  const qreal initialClearColorAlpha = 1.0;

  glClearColor(initialClearColorRed, initialClearColorGreen,
               initialClearColorBlue, initialClearColorAlpha);

  const double initialClearDepth = 1.0;
  glClearDepth(initialClearDepth);

  // Enable depth testing for 3D rendering
  glEnable(GL_DEPTH_TEST);

  // Set up the initial perspective view
  setupPerspective();
}

void GLWidget::resizeGL(int w, int h) {
  // Set the viewport for rendering to match the widget size
  glViewport(0, 0, w, h);
}

void GLWidget::paintGL() {
  // Check if there is data to render
  if (data.array) {
    // Set the clear color to the background color
    glClearColor(backgroundColor.redF(), backgroundColor.greenF(),
                 backgroundColor.blueF(), backgroundColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setupPerspective();

    // Set up the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Define vertex data
    glVertexPointer(3, GL_DOUBLE, 0, data.array);
    glEnableClientState(GL_VERTEX_ARRAY);

    // Draw points and lines
    pointDrawing();
    lineDrawing();

    // Disable vertex array rendering
    glDisableClientState(GL_VERTEX_ARRAY);
  } else {
    // Clean up and remove data if none is available
    s21_remove_data(&data);
  }
}

void GLWidget::pointDrawing() {
  // Check if point rendering is enabled
  if (pointMode != 0) {
    // Set the point color and size
    glColor3d(dotColor.redF(), dotColor.greenF(), dotColor.blueF());
    glPointSize(pointSize);

    // Enable point smoothing if specified
    if (pointMode == 1) {
      glEnable(GL_POINT_SMOOTH);
    }

    // Draw the points using vertex data
    glDrawArrays(GL_POINTS, 1, data.count_of_vertexes);

    // Disable point smoothing if previously enabled
    if (pointMode == 1) {
      glDisable(GL_POINT_SMOOTH);
    }
  }
}

void GLWidget::lineDrawing() {
  // Set the line color
  glColor3d(lineColor.redF(), lineColor.greenF(), lineColor.blueF());

  // Set the line width
  glLineWidth(lineSize);

  // Enable line stippling if necessary
  if (lineMode == 1) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x00F0);
  }

  // Draw lines using vertex data
  glDrawElements(GL_LINES, data.sizefilds, GL_UNSIGNED_INT, data.arrayfilds);

  // Disable line stipling if it was enabled
  if (lineMode == 1) {
    glDisable(GL_LINE_STIPPLE);
  }
}

void GLWidget::setupPerspective() {
  if (data.max_coord < 1) {
    data.max_coord = 2;
  }

  GLdouble zNear = 0.001;
  GLdouble zFar = data.max_coord * 5;

  if (projectionMode == 0) {
    // Set up perspective projection
    GLdouble fovY = 90;
    GLdouble fH = tan(fovY / 360 * M_PI) * zNear;
    GLdouble fW = fH;

    glFrustum(-fW, fW, -fH, fH, zNear, zFar);

    // Translate the object in the negative Z-direction to center it
    glTranslatef(0, 0, -data.max_coord * 2);
  } else {
    // Set up orthographic projection
    glOrtho(-data.max_coord, data.max_coord, -data.max_coord, data.max_coord,
            -data.max_coord, zFar);

    // Translate the object in the negative Z-direction to center it
    glTranslatef(0, 0, -data.max_coord);
  }
}

void GLWidget::setScale(int scale) {
  if (scale != Vscale) {
    Vscale = scale;  // Update Vscale instead of scale
    // Scale the data based on the new scale value
    double scaleFactor = fabs(Vscale / 1000.0);  // Use Vscale
    double prevScaleFactor = fabs(prevScale / 1000.0);
    s21_scale(&data, scaleFactor, prevScaleFactor);
    prevScale = Vscale;  // Update prevScale to Vscale
    update();
  }
}

void GLWidget::setXTranslate(int step) {
  if (step != xTranslation) {
    xTranslation = step;
    // Translate data along the X-axis
    double xTranslateFactor = 0.05 * (50 - xTranslation);
    double prevXTranslateFactor = 0.05 * (50 - xPrevTranslation);
    s21_move_x(&data, xTranslateFactor, prevXTranslateFactor);
    xPrevTranslation = xTranslation;
    update();
  }
}

void GLWidget::setYTranslate(int step) {
  if (step != yTranslation) {
    yTranslation = step;
    // Translate data along the Y-axis
    double yTranslateFactor = 0.05 * (50 - yTranslation);
    double prevYTranslateFactor = 0.05 * (50 - yPrevTranslation);
    s21_move_y(&data, yTranslateFactor, prevYTranslateFactor);
    yPrevTranslation = yTranslation;
    update();
  }
}

void GLWidget::setZTranslate(int step) {
  if (step != zTranslation) {
    zTranslation = step;
    // Translate data along the Z-axis
    double zTranslateFactor = 0.05 * (50 - zTranslation);
    double prevZTranslateFactor = 0.05 * (50 - zPrevTranslation);
    s21_move_z(&data, zTranslateFactor, prevZTranslateFactor);
    zPrevTranslation = zTranslation;
    update();
  }
}

static void qNormalizeAngle(int& angle) {
  const int fullCircle =
      360 * 16;  // 360 degrees represented in the fixed-point format

  // Normalize the angle to be within the range [0, fullCircle)
  while (angle < 0) angle += fullCircle;
  while (angle >= fullCircle) angle -= fullCircle;
}

void GLWidget::setXRotation(int angle) {
  // Normalize the input angle
  qNormalizeAngle(angle);

  // Update the X rotation if the angle has changed
  if (angle != xRotation) {
    xRotation = angle;

    // Apply rotation to the data
    s21_rotate_x(&data, -(180.0 - xRotation / 16.0),
                 -(180.0 - xPrevRotation / 16.0));
    xPrevRotation = xRotation;

    // Trigger a widget update to redraw
    update();
  }
}

void GLWidget::setYRotation(int angle) {
  // Normalize the input angle
  qNormalizeAngle(angle);

  // Update the Y rotation if the angle has changed
  if (angle != yRotation) {
    yRotation = angle;

    // Apply rotation to the data
    s21_rotate_y(&data, -(180.0 - yRotation / 16.0),
                 -(180.0 - yPrevRotation / 16.0));
    yPrevRotation = yRotation;

    // Trigger a widget update to redraw
    update();
  }
}

void GLWidget::setZRotation(int angle) {
  // Normalize the input angle
  qNormalizeAngle(angle);

  // Update the Z rotation if the angle has changed
  if (angle != zRotation) {
    zRotation = angle;

    // Apply rotation to the data
    s21_rotate_z(&data, -(180.0 - zRotation / 16.0),
                 -(180.0 - zPrevRotation / 16.0));
    zPrevRotation = zRotation;

    // Trigger a widget update to redraw
    update();
  }
}

void GLWidget::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    // Left mouse button pressed, enable rotation
    leftMousePressed = true;
    lastMousePos = event->pos();
  } else if (event->button() == Qt::RightButton) {
    // Right mouse button pressed, handle specific interaction here
    rightMousePressed = true;
    lastMousePos = event->pos();
  } else if (event->button() == Qt::MiddleButton) {
    // Middle mouse button pressed, enable translation
    middleMousePressed = true;
    lastMousePos = event->pos();
  }
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
  if (leftMousePressed) {
    // Handle rotation with the left mouse button
    int dx = event->position().x() - lastMousePos.x();
    int dy = event->position().y() - lastMousePos.y();
    setXRotation(xRotation - 8 * dy);
    setYRotation(yRotation + 8 * dx);
    lastMousePos = event->pos();

    //    ui->rotate_y_lcd->setText(QString::number(yRotation + 8 * dx));
    //    ui->rotate_x_lcd->setText(QString::number(yRotation + 8 * dy));
    update();
  } else if (middleMousePressed) {
    // Handle translation with the middle mouse button
    int dx = event->position().x() - lastMousePos.x();
    int dy = event->position().y() - lastMousePos.y();
    setXTranslate(xTranslation - dx);
    setYTranslate(yTranslation + dy);
    lastMousePos = event->pos();
    update();
  } else if (rightMousePressed) {
    // Handle right mouse button interaction (e.g., zoom or pan) here
  }
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    // Left mouse button released, disable rotation
    leftMousePressed = false;
  } else if (event->button() == Qt::RightButton) {
    // Right mouse button released, handle specific interaction here
    rightMousePressed = false;
  } else if (event->button() == Qt::MiddleButton) {
    // Middle mouse button released, disable translation
    middleMousePressed = false;
  }
}

void GLWidget::wheelEvent(QWheelEvent* event) {
  const int minScaleValue = 1;
  const int maxScaleValue = 5000;

  // Handle mouse wheel scrolling for zooming
  int delta = event->angleDelta().y();
  int newScale = Vscale - delta;

  // Ensure that the new scale value is within the specified range
  if (newScale >= minScaleValue && newScale <= maxScaleValue) {
    setScale(newScale);
  }
}

void GLWidget::pinchTriggered(QPinchGesture* gesture) {
  const int minScaleValue = 1;
  const int maxScaleValue = 5000;

  // Handle pinch gesture for scaling (e.g., on touch devices)
  qreal scaleFactor = gesture->totalScaleFactor();
  int newScale = Vscale * scaleFactor;

  // Ensure that the new scale value is within the specified range
  if (newScale >= minScaleValue && newScale <= maxScaleValue) {
    setScale(newScale);
  }
}

bool GLWidget::event(QEvent* event) {
  if (event->type() == QEvent::Gesture) {
    return gestureEvent(static_cast<QGestureEvent*>(event));
  }
  return QOpenGLWidget::event(event);
}

bool GLWidget::gestureEvent(QGestureEvent* event) {
  if (QGesture* pan = event->gesture(Qt::PanGesture)) {
    event->accept(pan);
    QPanGesture* panGesture = static_cast<QPanGesture*>(pan);
    panTriggered(panGesture);
    return true;
  }
  return false;
}

void GLWidget::panTriggered(QPanGesture* gesture) {
  // Handle panning gesture (e.g., on touch devices)
  QPointF delta = gesture->delta();
  setXTranslate(xTranslation + delta.x());
  setYTranslate(yTranslation -
                delta.y());  // Invert the y-direction for panning
  update();
}
