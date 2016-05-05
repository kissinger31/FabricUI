// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeLabel__
#define __UI_GraphView_NodeLabel__

#include "TextContainer.h"

namespace FabricUI
{

  namespace GraphView
  {

    class NodeLabel : public TextContainer
    {
      Q_OBJECT

    public:

      NodeLabel(
        QGraphicsWidget * parent,
        QString const &text,
        QColor color,
        QColor highlightColor,
        QFont font
        );

    };

  };

};

#endif // __UI_GraphView_NodeLabel__
