#include "mainwindow.h"

#include "openglwidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setWindowTitle("3d_Viewer");

  // Initialize timer for GIF capturing
  timer = new QTimer;
  connect(timer, SIGNAL(timeout()), this, SLOT(slotTimer()));

  // Construct an array of QImage objects for GIF frames
  gifImage = new QImage[50]{};

  // Load settings file path
  this->settingFile = QApplication::applicationDirPath() + "/settings.conf";

  // Initialize sliders and connect signals
  sliderSetup();
  connectSetup();

  // Load default settings
  defaultSettings();
}

MainWindow::~MainWindow() {
  // Save settings before exiting
  saveSettings();

  // Clean up resources
  delete timer;
  delete[] gifImage;
  delete ui;
}

void MainWindow::sliderSetup() {
  // Translation sliders setup
  int translationMin = -500;
  int translationMax = 1000;
  int translationSingleStep = 1;

  ui->slider_translate_x->setRange(translationMin, translationMax);
  ui->slider_translate_x->setSingleStep(translationSingleStep);

  ui->slider_translate_y->setRange(translationMin, translationMax);
  ui->slider_translate_y->setSingleStep(translationSingleStep);

  ui->slider_translate_z->setRange(translationMin, translationMax);
  ui->slider_translate_z->setSingleStep(translationSingleStep);

  // Scale slider setup
  int scaleMin = 1;
  int scaleMax = 5000;
  int scaleSingleStep = 1;

  ui->scale_slider->setRange(scaleMin, scaleMax);
  ui->scale_slider->setSingleStep(scaleSingleStep);

  // Vertices slider setup
  int verticesMin = 1;
  int verticesMax = 10;
  int verticesSingleStep = 1;

  ui->slider_vertices->setRange(verticesMin, verticesMax);
  ui->slider_vertices->setSingleStep(verticesSingleStep);

  // Edges slider setup
  int edgesMin = 1;
  int edgesMax = 10;
  int edgesSingleStep = 1;

  ui->slider_edges->setRange(edgesMin, edgesMax);
  ui->slider_edges->setSingleStep(edgesSingleStep);
}

void MainWindow::connectSetup() {
  // Start rotation when the button is pressed
  connect(ui->pushButton_Rotate_X, &QPushButton::pressed, this, [this]() {
    if (!rotatingX) {
      rotatingX = true;
      rotationTimer.start(16);  // Adjust the timer interval as needed
    }
  });

  connect(ui->pushButton_Rotate_Y, &QPushButton::pressed, this, [this]() {
    if (!rotatingY) {
      rotatingY = true;
      rotationTimer.start(16);  // Adjust the timer interval as needed
    }
  });

  connect(ui->pushButton_Rotate_Z, &QPushButton::pressed, this, [this]() {
    if (!rotatingZ) {
      rotatingZ = true;
      rotationTimer.start(16);  // Adjust the timer interval as needed
    }
  });

  // Stop rotation when the button is released
  connect(ui->pushButton_Rotate_X, &QPushButton::released, this,
          &MainWindow::stopRotation);
  connect(ui->pushButton_Rotate_Y, &QPushButton::released, this,
          &MainWindow::stopRotation);
  connect(ui->pushButton_Rotate_Z, &QPushButton::released, this,
          &MainWindow::stopRotation);

  connect(ui->rotate_x_lcd, SIGNAL(returnPressed()), this,
          SLOT(xRotateLineEditReturnPressed()));
  connect(ui->rotate_y_lcd, SIGNAL(returnPressed()), this,
          SLOT(yRotateLineEditReturnPressed()));
  connect(ui->rotate_z_lcd, SIGNAL(returnPressed()), this,
          SLOT(zRotateLineEditReturnPressed()));

  // Connect the timer to slots for continuous rotation
  connect(&rotationTimer, &QTimer::timeout, this, [this]() {
    if (rotatingX) {
      incrementXRotation();
    }
    if (rotatingY) {
      incrementYRotation();
    }
    if (rotatingZ) {
      incrementZRotation();
    }
  });

  // Other connections for translation, scaling, and other functionality (as
  // before)

  // Translate Sliders
  connect(ui->slider_translate_x, &QScrollBar::valueChanged, ui->OGLWidget,
          &GLWidget::setXTranslate);
  connect(ui->translate_x_lcd, SIGNAL(editingFinished()), this,
          SLOT(xTranslateTextEdit()));

  connect(ui->slider_translate_y, &QScrollBar::valueChanged, ui->OGLWidget,
          &GLWidget::setYTranslate);
  connect(ui->translate_y_lcd, SIGNAL(editingFinished()), this,
          SLOT(yTranslateTextEdit()));

  connect(ui->slider_translate_z, &QScrollBar::valueChanged, ui->OGLWidget,
          &GLWidget::setZTranslate);
  connect(ui->translate_z_lcd, SIGNAL(editingFinished()), this,
          SLOT(zTranslateTextEdit()));

  // Connecting translation sliders to text input
  connect(ui->slider_translate_x, &QScrollBar::valueChanged, this,
          &MainWindow::xTranslateSliderValueChanged);
  connect(ui->slider_translate_y, &QScrollBar::valueChanged, this,
          &MainWindow::yTranslateSliderValueChanged);
  connect(ui->slider_translate_z, &QScrollBar::valueChanged, this,
          &MainWindow::zTranslateSliderValueChanged);

  // Scale Slider
  connect(ui->scale_slider, &QScrollBar::valueChanged, ui->OGLWidget,
          &GLWidget::setScale);
  connect(ui->scale_text, SIGNAL(editingFinished()), this,
          SLOT(scaleTextEdit()));

  // Connecting scale slider to text input
  connect(ui->scale_slider, &QScrollBar::valueChanged, this,
          &MainWindow::scaleSliderValueChanged);

  // Type of Projection
  connect(ui->type_of_projection_parallel_button, &QRadioButton::pressed, this,
          &MainWindow::projectionParallel);
  connect(ui->type_of_projection_central_button, &QRadioButton::pressed, this,
          &MainWindow::projectionCentral);

  // Edge Parameters
  connect(ui->edges_solid_button, &QRadioButton::pressed, this,
          &MainWindow::linesTypeSolid);
  connect(ui->dashed_solid_button, &QRadioButton::pressed, this,
          &MainWindow::linesTypeDashed);
  connect(ui->slider_edges, &QScrollBar::valueChanged, this,
          &MainWindow::linesSizeSliderChanged);
  connect(ui->color_edges_button, SIGNAL(released()), this,
          SLOT(onChangeLineColor()));

  // Vertice Parameters
  connect(ui->vertices_disable_button, &QRadioButton::pressed, this,
          &MainWindow::DisableView);
  connect(ui->vertices_circle_button, &QRadioButton::pressed, this,
          &MainWindow::CircleView);
  connect(ui->vertices_square_button, &QRadioButton::pressed, this,
          &MainWindow::SquareView);
  connect(ui->slider_vertices, &QScrollBar::valueChanged, this,
          &MainWindow::vertexSize);

  connect(ui->color_vertices_button, SIGNAL(released()), this,
          SLOT(onChangeDotColor()));
  connect(ui->background_color_button, SIGNAL(released()), this,
          SLOT(onChangeBackgroundColor()));

  // Open File
  connect(ui->open_button, &QRadioButton::pressed, this,
          &MainWindow::on_action_triggered);

  // Reset
  connect(ui->reset_values_button, SIGNAL(released()), this,
          SLOT(resetValue()));
}

void MainWindow::defaultSliders() {
  // Translation Sliders
  int defaultTranslationValue = 50;
  ui->slider_translate_x->setValue(defaultTranslationValue);
  ui->slider_translate_y->setValue(defaultTranslationValue);
  ui->slider_translate_z->setValue(defaultTranslationValue);

  ui->translate_x_lcd->setText(QString::number(0));
  ui->translate_y_lcd->setText(QString::number(0));
  ui->translate_z_lcd->setText(QString::number(0));

  ui->rotate_x_lcd->setText(QString::number(0));
  ui->rotate_y_lcd->setText(QString::number(0));
  ui->rotate_z_lcd->setText(QString::number(0));

  // Scale Slider
  int defaultScaleValue = 1;
  ui->scale_slider->setValue(defaultScaleValue);
  ui->scale_text->setText(QString::number(defaultScaleValue));
}

void MainWindow::defaultSettings() {
  // Set default slider values
  defaultSliders();

  // Initialize data array if available
  if (ui->OGLWidget->data.array) {
    int maxVertices = (ui->OGLWidget->data.count_of_vertexes + 1) * 3;
    for (int i = 0; i < maxVertices; ++i) {
      ui->OGLWidget->data.array[i] = ui->OGLWidget->data.array_const[i];
    }
  }

  // Set default options for projection, vertices, and edges
  ui->type_of_projection_central_button->setChecked(true);
  ui->vertices_circle_button->setChecked(true);
  ui->slider_vertices->setEnabled(true);
  ui->slider_vertices->setValue(5);
  ui->edges_solid_button->setChecked(true);
  ui->slider_edges->setValue(1);

  // Initialize OpenGL widget settings
  ui->OGLWidget->initSettings();

  // Load settings from a configuration file if it exists
  if (QFile::exists(settingFile)) {
    QSettings settings(settingFile, QSettings::IniFormat);

    // Line settings
    switch (settings.value("LineSet/lineType", 1).toInt()) {
      case 0:
        ui->dashed_solid_button->setChecked(true);
        ui->OGLWidget->lineMode = 0;
        break;
      case 1:
      default:
        ui->edges_solid_button->setChecked(true);
        ui->OGLWidget->lineMode = 1;
        break;
    }

    ui->OGLWidget->lineColor = settings.value("LineSet/LineColor").toString();
    ui->slider_edges->setValue(settings.value("LineSet/value", 1).toInt());

    // Vertices settings
    switch (settings.value("Vertices/verticeType", 1).toInt()) {
      case 0:
        ui->vertices_disable_button->setChecked(true);
        ui->OGLWidget->pointMode = 0;
        break;
      case 1:
        ui->vertices_circle_button->setChecked(true);
        ui->OGLWidget->pointMode = 1;
        break;
      case 2:
        ui->vertices_square_button->setChecked(true);
        ui->OGLWidget->pointMode = 2;
        break;
      default:
        break;
    }

    ui->OGLWidget->dotColor = settings.value("Vertices/color").toString();
    ui->slider_vertices->setValue(settings.value("Vertices/size", 5).toInt());

    // Background color
    ui->OGLWidget->backgroundColor =
        settings.value("background/color").toString();
  }

  // Update UI to reflect loaded settings
  updateUiColors();
}

void MainWindow::saveSettings() {
  QSettings settings(settingFile, QSettings::IniFormat);

  // Save LineSet settings
  settings.beginGroup("LineSet");
  if (ui->edges_solid_button->isChecked()) {
    settings.setValue("lineType", 1);  // Solid
  } else if (ui->dashed_solid_button->isChecked()) {
    settings.setValue("lineType", 0);  // Dashed
  }
  settings.setValue("LineColor", ui->OGLWidget->lineColor);
  settings.setValue("value", ui->slider_edges->value());
  settings.endGroup();

  // Save Vertices settings
  settings.beginGroup("Vertices");
  if (ui->vertices_disable_button->isChecked()) {
    settings.setValue("verticeType", 0);  // Disabled
  } else if (ui->vertices_circle_button->isChecked()) {
    settings.setValue("verticeType", 1);  // Circle
  } else if (ui->vertices_square_button->isChecked()) {
    settings.setValue("verticeType", 2);  // Square
  }
  settings.setValue("color", ui->OGLWidget->dotColor);
  settings.setValue("size", ui->slider_vertices->value());
  settings.endGroup();

  // Save background color
  settings.beginGroup("background");
  settings.setValue("color", ui->OGLWidget->backgroundColor);
  settings.endGroup();
}

void MainWindow::xTranslateSliderValueChanged(int value) {
  const int translationOffset = 50;
  int translatedValue = value - translationOffset;
  ui->translate_x_lcd->setText(QString::number(translatedValue));
}

void MainWindow::yTranslateSliderValueChanged(int value) {
  const int translationOffset = 50;
  int translatedValue = value - translationOffset;
  ui->translate_y_lcd->setText(QString::number(translatedValue));
}

void MainWindow::zTranslateSliderValueChanged(int value) {
  const int translationOffset = 50;
  int translatedValue = value - translationOffset;
  ui->translate_z_lcd->setText(QString::number(translatedValue));
}

void MainWindow::xTranslateTextEdit() {
  const int translationOffset = 50;
  int inputValue = ui->translate_x_lcd->text().toInt();
  int translatedValue = inputValue + translationOffset;
  ui->slider_translate_x->setValue(translatedValue);
}

void MainWindow::yTranslateTextEdit() {
  const int translationOffset = 50;
  int inputValue = ui->translate_y_lcd->text().toInt();
  int translatedValue = inputValue + translationOffset;
  ui->slider_translate_y->setValue(translatedValue);
}

void MainWindow::zTranslateTextEdit() {
  const int translationOffset = 50;
  int inputValue = ui->translate_z_lcd->text().toInt();
  int translatedValue = inputValue + translationOffset;
  ui->slider_translate_z->setValue(translatedValue);
}

// Rotate

void MainWindow::xRotateLineEditReturnPressed() {
  int inputValue = ui->rotate_x_lcd->text().toInt();
  int normalizedValue = valueNormalize(inputValue);
  ui->OGLWidget->setXRotation((normalizedValue + 180) * 16);
  xRotateValueChanged((normalizedValue + 180) * 16);
}

void MainWindow::yRotateLineEditReturnPressed() {
  int inputValue = ui->rotate_y_lcd->text().toInt();
  int normalizedValue = valueNormalize(inputValue);
  ui->OGLWidget->setYRotation((normalizedValue + 180) * 16);
  yRotateValueChanged((normalizedValue + 180) * 16);
}

void MainWindow::zRotateLineEditReturnPressed() {
  int inputValue = ui->rotate_z_lcd->text().toInt();
  int normalizedValue = valueNormalize(inputValue);
  ui->OGLWidget->setZRotation((normalizedValue + 180) * 16);
  zRotateValueChanged((normalizedValue + 180) * 16);
}

void MainWindow::xRotateValueChanged(int value) {
  // Normalize the value from [0, 360 * 16] to [-180, 180]
  int normalizedValue = (value - 180 * 16) / 16;
  ui->rotate_x_lcd->setText(QString::number(normalizedValue));
}

void MainWindow::yRotateValueChanged(int value) {
  // Normalize the value from [0, 360 * 16] to [-180, 180]
  int normalizedValue = (value - 180 * 16) / 16;
  ui->rotate_y_lcd->setText(QString::number(normalizedValue));
}

void MainWindow::zRotateValueChanged(int value) {
  // Normalize the value from [0, 360 * 16] to [-180, 180]
  int normalizedValue = (value - 180 * 16) / 16;
  ui->rotate_z_lcd->setText(QString::number(normalizedValue));
}

// Normalize the value to the range [-180, 180]
int MainWindow::valueNormalize(int val) {
  return (val > 180) ? (val - 360) : ((val < -180) ? (val + 360) : val);
}

void MainWindow::xRotateTextEdit() {
  int val = ui->rotate_x_lcd->text().toInt();
  val = valueNormalize(val);
  int normalizedValue = (val + 180) * 16;
  ui->OGLWidget->setXRotation(normalizedValue);
}

void MainWindow::yRotateTextEdit() {
  int val = ui->rotate_y_lcd->text().toInt();
  val = valueNormalize(val);
  int normalizedValue = (val + 180) * 16;
  ui->OGLWidget->setYRotation(normalizedValue);
}

void MainWindow::zRotateTextEdit() {
  int val = ui->rotate_z_lcd->text().toInt();
  val = valueNormalize(val);
  int normalizedValue = (val + 180) * 16;
  ui->OGLWidget->setZRotation(normalizedValue);
}

// Scale
void MainWindow::scaleSliderValueChanged(int value) {
  ui->scale_text->setText(QString::number(value));
}

void MainWindow::scaleTextEdit() {
  int val = ui->scale_text->text().toInt();
  ui->scale_slider->setValue(val);
}

// Type of projection
void MainWindow::setProjectionMode(int mode) {
  ui->OGLWidget->projectionMode = mode;
  ui->OGLWidget->update();
}

void MainWindow::projectionCentral() {
  setProjectionMode(0);  // Central Projection
}

void MainWindow::projectionParallel() {
  setProjectionMode(1);  // Parallel Projection
}

// Edge parameters
void MainWindow::setLineMode(int mode) {
  ui->OGLWidget->lineMode = mode;
  ui->OGLWidget->update();
}

void MainWindow::linesTypeSolid() {
  setLineMode(0);  // Solid Lines
}

void MainWindow::linesTypeDashed() {
  setLineMode(1);  // Dashed Lines
}

void MainWindow::linesSizeSliderChanged(int value) {
  ui->OGLWidget->lineSize = value;
  ui->OGLWidget->update();
}

// Function to disable point rendering
void MainWindow::DisableView() {
  ui->OGLWidget->pointMode = 0;            // Set point mode to None
  ui->slider_vertices->setValue(0);        // Reset vertex slider
  ui->slider_vertices->setEnabled(false);  // Disable vertex slider
  ui->OGLWidget->update();                 // Update OpenGL widget
}

// Function to render circles
void MainWindow::CircleView() {
  ui->OGLWidget->pointMode = 1;           // Set point mode to Circle
  ui->slider_vertices->setEnabled(true);  // Enable vertex slider
  ui->OGLWidget->update();                // Update OpenGL widget
}

// Function to render squares
void MainWindow::SquareView() {
  ui->OGLWidget->pointMode = 2;           // Set point mode to Square
  ui->slider_vertices->setEnabled(true);  // Enable vertex slider
  ui->OGLWidget->update();                // Update OpenGL widget
}

// Function to set the size of vertices
void MainWindow::vertexSize(int value) {
  ui->OGLWidget->pointSize = value;  // Set point size
  ui->OGLWidget->update();           // Update OpenGL widget
}

// Helper function to set UI element colors
void setUIColor(QWidget* widget, const QColor& color) {
  widget->setAutoFillBackground(true);
  widget->setStyleSheet(
      QString("border-style: solid; border-width: 1px; border-color: black; "
              "padding-top: 3px; background-color: %1;")
          .arg(color.name()));
}

// Function to change the background color
void MainWindow::onChangeBackgroundColor() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Select Color",
                                        QColorDialog::DontUseNativeDialog);
  if (color.isValid()) {
    ui->OGLWidget->backgroundColor = color;
    updateUiColors();  // Update UI elements to reflect the color change
  }
}

// Function to change the line color
void MainWindow::onChangeLineColor() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Select Color",
                                        QColorDialog::DontUseNativeDialog);
  if (color.isValid()) {
    ui->OGLWidget->lineColor = color;
    updateUiColors();  // Update UI elements to reflect the color change
  }
}

// Function to change the dot (vertex) color
void MainWindow::onChangeDotColor() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Select Color",
                                        QColorDialog::DontUseNativeDialog);
  if (color.isValid()) {
    ui->OGLWidget->dotColor = color;
    updateUiColors();  // Update UI elements to reflect the color change
  }
}

// Function to update UI elements to display selected colors
void MainWindow::updateUiColors() {
  // Set background color on the UI
  setUIColor(ui->graphics_background_color, ui->OGLWidget->backgroundColor);

  // Set line color on the UI
  setUIColor(ui->graphics_color_edges, ui->OGLWidget->lineColor);

  // Set vertex color on the UI
  setUIColor(ui->graphics_vertices_color, ui->OGLWidget->dotColor);

  ui->OGLWidget->update();  // Update OpenGL widget
}

// Helper function to set UI element colors
void MainWindow::setUIColor(QWidget* widget, const QColor& color) {
  widget->setAutoFillBackground(true);
  widget->setStyleSheet(
      QString("border-style: solid; border-width: 1px; border-color: black; "
              "padding-top: 3px; background-color: %1;")
          .arg(color.name()));
}

// Reset to default values
void MainWindow::resetValue() {
  // Check if the settings file exists and remove it
  if (QFile::exists(settingFile)) {
    QFile::remove(settingFile);
  }

  // Reset to default settings
  defaultSettings();

  // Update the OpenGL widget
  ui->OGLWidget->update();
}

// Open and parse an .obj file
void MainWindow::on_action_triggered() {
  QFileDialog fileDialog(this);
  fileDialog.setWindowTitle(tr("Choose .obj-file"));
  fileDialog.setNameFilter(tr("(*.obj)"));
  fileDialog.setViewMode(QFileDialog::Detail);
  fileDialog.setFileMode(QFileDialog::ExistingFile);

  if (fileDialog.exec()) {
    QStringList fileNames = fileDialog.selectedFiles();

    if (!fileNames.isEmpty()) {
      QString fileName = fileNames.first();
      QByteArray ba = fileName.toLocal8Bit();
      char* input = ba.data();

      // Remove existing data
      s21_remove_data(&ui->OGLWidget->data);

      if (s21_parsing(&ui->OGLWidget->data, input)) {
        // Successful parsing
        setWindowTitle(fileName);
        ui->statusBar->showMessage("Path: " + fileName);
        ui->number_of_facets->setText(
            QString::number(ui->OGLWidget->data.count_of_facets));
        ui->number_of_vertices->setText(
            QString::number(ui->OGLWidget->data.count_of_vertexes));
        defaultSliders();
      } else {
        // Error during parsing
        s21_remove_data(&ui->OGLWidget->data);
      }
    }
  }
}

// Save a screenshot as BMP or JPEG
void MainWindow::on_bmp_button_clicked() {
  saveSettings();

  // Create a unique filename based on the current date and time
  QDateTime dateTime = QDateTime::currentDateTime();
  QString currentDateTime = dateTime.toString("dd.MM.yy_HH.mm.ss_zzz");
  QString name_photo = QFileDialog::getSaveFileName(
      this, "Save as...", "Screenshot_" + currentDateTime,
      "BMP (*.bmp);; JPEG (*.jpeg)");

  // Capture and save the OpenGL widget's framebuffer
  ui->OGLWidget->grabFramebuffer().save(name_photo);
}

// Save a GIF by capturing frames at regular intervals
void MainWindow::on_gif_button_clicked() {
  ui->gif_button->setEnabled(false);
  saveSettings();
  timer->start(100);
}

// Slot for the timer to capture frames for GIF
void MainWindow::slotTimer() {
  if (gifTime < 50) {
    gifImage[gifTime] = ui->OGLWidget->grab().toImage();
    ++gifTime;
  } else {
    gif = new QGifImage;

    for (int i = 0; i < gifTime; ++i) {
      gif->addFrame(gifImage[i], 0);
    }

    // Create a unique filename based on the current date and time
    QDateTime dateTime = QDateTime::currentDateTime();
    QString currentDateTime = dateTime.toString("dd.MM.yy_HH.mm.ss_zzz");
    QString fileName = QFileDialog::getSaveFileName(
        this, "Save GIF", "GIF_" + currentDateTime, "GIF (*.gif)");

    // Save the GIF
    gif->save(fileName);
    ui->gif_button->setEnabled(true);
    timer->stop();
    delete gif;
    gifTime = 0;
  }
}

void MainWindow::incrementXRotation() {
  xRotationValue = (xRotationValue + rotationIncrement) % 360;
  ui->OGLWidget->setXRotation(xRotationValue * 16);
  xRotateValueChanged(xRotationValue * 16);
}

// Slot for incrementing Y rotation
void MainWindow::incrementYRotation() {
  yRotationValue = (yRotationValue + rotationIncrement) % 360;
  ui->OGLWidget->setYRotation(yRotationValue * 16);
  yRotateValueChanged(yRotationValue * 16);
}

// Slot for incrementing Z rotation
void MainWindow::incrementZRotation() {
  zRotationValue = (zRotationValue + rotationIncrement) % 360;
  ui->OGLWidget->setZRotation(zRotationValue * 16);
  zRotateValueChanged(zRotationValue * 16);
}
// Stop rotation when the button is released
void MainWindow::stopRotation() {
  rotatingX = false;
  rotatingY = false;
  rotatingZ = false;
  rotationTimer.stop();
}
