//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BooleanCheckBoxViewItem.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"

#include <QVariant>
#include <QCheckBox>

using namespace FabricUI::ValueEditor;

BooleanCheckBoxViewItem::BooleanCheckBoxViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
{
  m_checkBox = new QCheckBox;
  m_checkBox->setObjectName( "BooleanItem" );
  m_checkBox->setContentsMargins( 0, 0, 0, 0 );
  connect(
    m_checkBox, SIGNAL( stateChanged( int ) ),
    this, SLOT( onStateChanged( int ) )
    );

  onModelValueChanged( value );
}

BooleanCheckBoxViewItem::~BooleanCheckBoxViewItem()
{
}

QWidget *BooleanCheckBoxViewItem::getWidget()
{
  return m_checkBox;
}

void BooleanCheckBoxViewItem::onModelValueChanged( QVariant const &v )
{
  m_checkBox->setChecked( getQVariantRTValValue<bool>(v) );
}

void BooleanCheckBoxViewItem::onStateChanged( int value )
{
  emit viewValueChanged( QVariant::fromValue( bool( value ) ) );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem *BooleanCheckBoxViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
{
  if (RTVariant::isType<bool>(value))
  {
    BooleanCheckBoxViewItem* item =
      new BooleanCheckBoxViewItem( name, value, metadata );
    return item;
  }
  return NULL;
}

const int BooleanCheckBoxViewItem::Priority = 3;
