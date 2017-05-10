//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandException.h"
#include "KLCommandRegistry.h"
#include "KLCommand.h"
#include "KLScriptableCommand.h"
 
using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

KLCommandRegistry::KLCommandRegistry(
  Client client) 
  : CommandRegistry()
  , m_client(client)
{
  COMMAND_KL = "KL";

  try 
  {
    m_klCmdRegistry = RTVal::Create(
      m_client, 
      "CommandRegistry", 
      0, 0);

    m_klCmdRegistry = m_klCmdRegistry.callMethod(
      "CommandRegistry", 
      "getCommandRegistry", 
      0, 0);
  }

  catch(Exception &e)
  {
    CommandException::PrintOrThrow(
      "KLCommandManager_Python::KLCommandRegistry",
      "",
      e.getDesc_cstr());
  }
}

KLCommandRegistry::~KLCommandRegistry() 
{
}

Command* KLCommandRegistry::createCommand(
  const QString &cmdName) 
{  
  if(!isCommandRegistered(cmdName))
    CommandException::PrintOrThrow( 
      "KLCommandRegistry::createCommand",
      "Cannot create command '" + cmdName + "', it's not registered"
      );

  QList<QString> spec = getCommandSpecs(
    cmdName);

  return spec[1] == COMMAND_KL
    ? createKLCommand(cmdName)
    : CommandRegistry::createCommand(cmdName);
}

FabricCore::RTVal KLCommandRegistry::getKLRegistry()
{
  return m_klCmdRegistry;
}

FabricCore::Client KLCommandRegistry::getClient()
{
  return m_client;
}

void KLCommandRegistry::synchronizeKL() 
{
  try 
  {
    RTVal klCmdNameList = m_klCmdRegistry.callMethod(
      "String[]", 
      "getRegisteredCommandList", 
      0, 0);
    
    for(unsigned int i=0; i<klCmdNameList.getArraySize(); ++i)
    {
      QString cmdName(klCmdNameList.getArrayElement(i).getStringCString());
      if(!isCommandRegistered(cmdName))
        registerKLCommand(cmdName);
    }
  }

  catch(Exception &e)
  {
    CommandException::PrintOrThrow( 
      "KLCommandRegistry::synchronizeKL",
      "",
      e.getDesc_cstr()
      );
  }
}
 
void KLCommandRegistry::registerKLCommand(
  const QString &cmdName) 
{
  try 
  {
    // Ne sure the command is registered in KL. 
    RTVal args[2] = {
      RTVal::ConstructString(
        m_client, 
        cmdName.toUtf8().constData()),

      RTVal::Construct(
        m_client, 
        "Type", 
        0, 0),
    };

    bool isCmdRegistered = m_klCmdRegistry.callMethod(
      "Boolean", 
      "isCommandRegistered", 
      2, 
      args).getBoolean();

  
    if(isCmdRegistered)
    {
      // The command is already registered in KL.
      // Don't call KLCommandRegistry::commandIsRegistered.
      CommandRegistry::commandIsRegistered(
        cmdName,
        RTVal::Construct(
          m_client, 
          "String", 
          1, 
          &args[1]).getStringCString(),
        COMMAND_KL);
    }
  }

  catch(Exception &e)
  {
    CommandException::PrintOrThrow( 
      "KLCommandRegistry::registerKLCommand",
      "",
      e.getDesc_cstr()
      );
  }
}
 
Command* KLCommandRegistry::createKLCommand(
  const QString &cmdName)
{  
  try
  {
    RTVal args[2] = {
      RTVal::ConstructString(
        m_client, 
        cmdName.toUtf8().constData()),

      RTVal::ConstructString(
        m_client, 
        "")
    };

    // Creates the KL command from the KL registery. 
    // Check if it's a scriptable command
    RTVal klCmd = m_klCmdRegistry.callMethod(
      // Cast to BaseScriptableCommand and ScriptableCommand so
      // we don't have to cast the rtval in KLScriptableCommand.
      "BaseScriptableCommand", 
      "createCommand", 
      2, 
      args);

    if(!klCmd.isNullObject())
      return new KLScriptableCommand(klCmd);
    
    // if not, it's a simple command.
    else
    {
      klCmd = m_klCmdRegistry.callMethod(
        "Command", 
        "createCommand", 
        2, 
        args);

      return new KLCommand(klCmd);
    }
  }

  catch(Exception &e)
  {
    CommandException::PrintOrThrow( 
      "KLCommandRegistry::createKLCommand",
      "",
      e.getDesc_cstr()
      );
  }

  return 0;
}

void KLCommandRegistry::commandIsRegistered(
  const QString &cmdName,
  const QString &cmdType,
  const QString &implType) 
{
  try
  {
    RTVal nameVal = RTVal::ConstructString(
      m_client,
      cmdName.toUtf8().constData());

    m_klCmdRegistry.callMethod(
      "",
      "registerAppCommand",
      1,
      &nameVal);
  }

  catch(Exception &e)
  {
    CommandException::PrintOrThrow( 
      "KLCommandRegistry::commandIsRegistered",
      "",
      e.getDesc_cstr()
      );
  }
  
  CommandRegistry::commandIsRegistered(
    cmdName,
    cmdType,
    implType);
}
