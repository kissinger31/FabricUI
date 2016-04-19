//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/ModelItems/RootModelItem.h>

namespace FabricUI {
namespace SceneHub {

//////////////////////////////////////////////////////////////////////////
class SGObjectModelItem : public ModelItems::RootModelItem
{
  Q_OBJECT
  
  private:

    FabricCore::Client m_client;
    FabricCore::RTVal m_rtVal;

    FabricCore::RTVal m_lastStructureVersionRtVal;
    FabricCore::RTVal m_lastValuesVersionRtVal;
    FabricCore::RTVal m_isValidRtVal;
    FabricCore::RTVal m_structureChangedRtVal;
    FabricCore::RTVal m_valueChangedRtVal;

    std::string m_name;
    FabricCore::RTVal m_propertiesRtVal;
    std::map<std::string, unsigned int> m_propertyNameMap;

  public:

    SGObjectModelItem(
      FabricCore::Client client,
      FabricCore::RTVal rtVal
      );
    virtual ~SGObjectModelItem();

    BaseModelItem *createChild( FTL::CStrRef name ) /*override*/;

    virtual int getNumChildren() /*override*/;

    virtual FTL::CStrRef getChildName( int i ) /*override*/;

    virtual void onStructureChanged();

    const FabricCore::RTVal& getSGObject() { return m_rtVal; }

    // Detects potential scene changes for this SGObject.
    // If structureChanged, this object must be recreated
    // (incremental update is not supported right now). Otherwise, property values will 
    // be updated if required.
    void updateFromScene( const FabricCore::RTVal& newSGObject, bool& isValid, bool& structureChanged );

    /////////////////////////////////////////////////////////////////////////
    // Name
    /////////////////////////////////////////////////////////////////////////

    virtual FTL::CStrRef getName() /*override*/;

    virtual bool canRename() /*override*/;

    virtual void rename( FTL::CStrRef newName ) /*override*/;

    virtual void onRenamed(
      FTL::CStrRef oldName,
      FTL::CStrRef newName
      ) /*override*/;

    /////////////////////////////////////////////////////////////////////////
    // Value
    /////////////////////////////////////////////////////////////////////////
    virtual QVariant getValue() /*override*/;

    /////////////////////////////////////////////////////////////////////////
    // Metadata
    /////////////////////////////////////////////////////////////////////////

    virtual FabricUI::ValueEditor::ItemMetadata* getMetadata() /*override*/;

    virtual void setMetadataImp( const char* key, 
                              const char* value, 
                              bool canUndo )/*override*/;


  public slots:
    void onSynchronizeCommands();


  signals:
    void propertyItemInserted( FabricUI::ValueEditor::BaseModelItem * item );

    void synchronizeCommands();


  protected:
    virtual void setValue(
      QVariant var,
      bool commit,
      QVariant valueAtInteractionBegin
      ) /*override*/;

    void ensurePropertiesRTVal();
};

} // namespace SceneHub
} // namespace FabricUI

