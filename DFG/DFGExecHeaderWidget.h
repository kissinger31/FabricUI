// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGExecHeaderWidget__
#define __UI_DFG_DFGExecHeaderWidget__

#include <FabricCore.h>

#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>

#include <FTL/StrRef.h>

#include <FabricUI/GraphView/GraphConfig.h>

class QLabel;

namespace FabricUI
{
  namespace DFG
  {
    class DFGController;

    class DFGExecHeaderWidget : public QFrame
    {
      Q_OBJECT

    public:

      DFGExecHeaderWidget(
        QWidget * parent,
        DFGController *dfgController,
        const GraphView::GraphConfig & config = GraphView::GraphConfig()
        );
      virtual ~DFGExecHeaderWidget();
      
      void refreshTitle( FTL::CStrRef title );
      void refreshExtDeps( FTL::CStrRef extDeps );

      // return true if the req. exts QLineEdit
      // widget has the keyboard focus..
      bool reqExtLineEditWidgetHasFocus() const;

      // discard the changes made in the req. exts
      // QLineEdit widget and remove the keyboard focus.
      // returns true on success.
      bool reqExtLineEditWidgetClearFocus();

    signals:

      void goUpPressed();
      void reloadPressed();
      void savePressed();
      void extensionLoaded();

    public slots:

      void refresh();
      void onExecChanged();

    protected:

      FTL::CStrRef getExecPath();
      FabricCore::DFGExec &getExec();

      virtual void paintEvent(QPaintEvent * event);

    protected slots:

      void reqExtEditingFinished();
      void reqExtResizeToContent();
      void onSplitFromPresetClicked();

    private:

      DFGController *m_dfgController;
      QLabel *m_execPathLabel;
      QLabel *m_presetNameLabel;
      QLabel *m_presetPathSep;
      QLabel *m_reqExtLabel;
      QLineEdit *m_reqExtLineEdit;
      QPushButton * m_backButton;
      QPushButton * m_saveButton;
      QPushButton * m_reloadButton;
      QColor m_backgroundColor;
      QPen m_pen;
      QWidget *m_presetSplitWidget;

    };

  };

};

#endif // __UI_DFG_DFGExecHeaderWidget__
