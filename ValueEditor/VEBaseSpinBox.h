//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_VEBASESPINBOX_H
#define FABRICUI_VALUEEDITOR_VEBASESPINBOX_H

#include "VELineEdit.h"
#include <assert.h>
#include <FabricUI/Util/QTSignalBlocker.h>
#include <math.h>
#include <QTimer>
#include <QApplication>
#include <qevent.h>
#include <QLineEdit>
#include <QSpinBox>

namespace FabricUI {
namespace ValueEditor {
  
template<typename QT_SPINBOX, typename value_type>
class VEBaseSpinBox : public QT_SPINBOX
{
public:

  VEBaseSpinBox()
    : m_last      ( 0 )
    , m_startValue( 0 )
    , m_dragging( false )
    , m_stepping( false )
    , m_steppingTimerConnected( false )
  {
    QT_SPINBOX::setFocusPolicy(Qt::StrongFocus);
    QT_SPINBOX::lineEdit()->setAlignment( Qt::AlignRight | Qt::AlignCenter );
    QT_SPINBOX::setKeyboardTracking( false );
    m_steppingTimer.setSingleShot( true );
  }

  ~VEBaseSpinBox() {}

  virtual void mousePressEvent( QMouseEvent *event ) /*override*/
  {
    m_trackStartPos = event->pos();
    m_startValue = QT_SPINBOX::value();
    m_last       = QT_SPINBOX::value();

    static const QCursor initialOverrideCursor( Qt::SizeVerCursor );
    QApplication::setOverrideCursor( initialOverrideCursor );

    event->accept();
  }

  virtual void mouseReleaseEvent( QMouseEvent *event ) /*override*/
  {
    if ( m_dragging )
    {
      m_dragging = false;
      resetPrecision();
      emitInteractionEnd( true );

      // [FE-6014]
      QT_SPINBOX::lineEdit()->deselect();
      QT_SPINBOX::lineEdit()->clearFocus(); // [FE-6077] using this instead of QT_SPINBOX::clearFocus() makes the TAB key work.
    }
    else
    {
      beginStepping();

      QT_SPINBOX::mousePressEvent( event );
      QT_SPINBOX::mouseReleaseEvent( event );
    }

    QApplication::restoreOverrideCursor();

    event->accept();
  }

  virtual void mouseMoveEvent( QMouseEvent *event ) /*override*/
  {
    if ( event->buttons() == Qt::LeftButton )
    {
      if ( !m_dragging )
      {
        m_dragging = true;

        if ( m_stepping )
        {
          m_stepping = false;
          m_steppingTimer.stop();
        }
        else emitInteractionBegin();
      }

      QPoint trackPos = event->pos();

      int deltaX = trackPos.x() - m_trackStartPos.x();
      double deltaXInInches =
        double( deltaX ) / double( QT_SPINBOX::logicalDpiX() );

      double logBaseChangePerStep = implicitLogBaseChangePerStep();
      // Slow down movement if Ctrl is pressed
      if ( QApplication::keyboardModifiers() & Qt::ControlModifier )
        logBaseChangePerStep -= 2;
      else
        logBaseChangePerStep -= 1;

      double stepMult = updateStep( deltaXInInches, logBaseChangePerStep );

      int nSteps =
        int( round( stepMult * ( m_trackStartPos.y() - trackPos.y() ) ) );

      // While dragging, we want to do an absolute value offset,
      // so reset to start value and then increment by abs step
      {
        FabricUI::Util::QTSignalBlocker block( this );
        QT_SPINBOX::setValue( m_startValue );
      }
      QT_SPINBOX::stepBy( nSteps );
    }
    event->accept();
  }

  virtual void focusInEvent( QFocusEvent *event ) /*override*/
  {
    // [FE-5997] inspired by http://stackoverflow.com/questions/5821802/qspinbox-inside-a-qscrollarea-how-to-prevent-spin-box-from-stealing-focus-when
    QT_SPINBOX::setFocusPolicy(Qt::WheelFocus);

    if ( event->reason() != Qt::PopupFocusReason )
      m_last = QT_SPINBOX::value();
    QT_SPINBOX::focusInEvent( event );
  }

  virtual void focusOutEvent( QFocusEvent *event ) /*override*/
  {
    // [FE-5997] inspired by http://stackoverflow.com/questions/5821802/qspinbox-inside-a-qscrollarea-how-to-prevent-spin-box-from-stealing-focus-when
    QT_SPINBOX::setFocusPolicy(Qt::StrongFocus);

    QT_SPINBOX::focusOutEvent( event );
  }

  virtual void keyPressEvent( QKeyEvent *event ) /*override*/
  {
    if ( event->key() == Qt::Key_Up || event->key() == Qt::Key_Down )
    {
      beginStepping();
    }
    else if ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter )
    {
      // [FE-6025]
      endStepping();
      QT_SPINBOX::lineEdit()->deselect();
      QT_SPINBOX::clearFocus();
      event->accept();
      return;
    }
    else if ( event->key() == Qt::Key_Escape )
    {
      // [FE-6007]
      QT_SPINBOX::setValue(m_last);
      endStepping();
      QT_SPINBOX::lineEdit()->deselect();
      QT_SPINBOX::clearFocus();
      event->accept();
      return;
    }
    else
    {
      endStepping();
    }

    QT_SPINBOX::keyPressEvent( event );
  }

  virtual void wheelEvent( QWheelEvent *event ) /*override*/
  {
    // [FE-5997] inspired by http://stackoverflow.com/questions/5821802/qspinbox-inside-a-qscrollarea-how-to-prevent-spin-box-from-stealing-focus-when
    if (!this->hasFocus())
    {
      event->ignore();
      return;
    }

    beginStepping();

    QT_SPINBOX::wheelEvent( event );
  }

  virtual void leaveEvent( QEvent *event ) /*override*/
  {
    endStepping();

    QT_SPINBOX::leaveEvent( event );
  }

  virtual double implicitLogBaseChangePerStep() = 0;

  virtual double updateStep(
    double deltaXInInches,
    double logBaseChangePerStep
    ) = 0;

  virtual void resetPrecision() {}

protected:

  void beginStepping()
  {
    if ( !m_stepping )
    {
      m_stepping = true;

      if ( !m_steppingTimerConnected )
      {
        m_steppingTimerConnected = true;
        connectSteppingTimer( &m_steppingTimer );
      }

      emitInteractionBegin();

      // [pzion 20160125] Steps are bigger than dragging
      updateStep( 0.0, implicitLogBaseChangePerStep() );
    }

    assert( m_steppingTimerConnected );
    // This is finely tuned but also personal preference:
    // (pz's fav value: 600, em's fav value: 300)
    m_steppingTimer.start( 300 );
  }

  void endStepping()
  {
    if ( m_stepping )
    {
      m_stepping = false;
      m_steppingTimer.stop();
      resetPrecision();
      emitInteractionEnd( true );
    }
  }

  virtual void connectSteppingTimer( QTimer *steppingTimer ) = 0;
  virtual void emitInteractionBegin() = 0;
  virtual void emitInteractionEnd( bool commit ) = 0;

  QPoint m_trackStartPos;
  value_type m_last;
  value_type m_startValue;
  bool m_dragging;
  bool m_stepping;
  bool m_steppingTimerConnected;
  QTimer m_steppingTimer;
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_VEBASESPINBOX_H
