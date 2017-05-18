//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValPathValueArg.h"
#include "CommandRegistry.h"
#include "CommandException.h"
#include "ScriptableCommand.h"
#include "RTValCommandManager.h"
#include "RTValScriptableCommand.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Commands/CommandArgFlags.h>
#include <FabricUI/PathResolvers/PathResolver.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace PathResolvers;

RTValCommandManager::RTValCommandManager(
  Client client) 
  : CommandManager()
  , m_client(client)
{
  m_RTValComplexArgRegistry.registerArg(
    new RTValPathValueArg()
    );
}

RTValCommandManager::~RTValCommandManager() 
{
}

FabricCore::Client RTValCommandManager::getClient()
{
  return m_client;
}

Command* RTValCommandManager::createCommand(
  const QString &cmdName, 
  const QMap<QString, RTVal> &args, 
  bool doCmd)
{
  try 
  {  
    Command *cmd = CommandRegistry::GetCommandRegistry()->createCommand(
      cmdName);
    
    if(args.size() > 0) 
      checkCommandArgs(cmd, args);

    if(doCmd) 
      doCommand(cmd);

    return cmd;
  }

  catch(CommandException &e) 
  {
    CommandException::Throw(
      "RTValCommandManager::createCommand",
      "Cannot create command '" + cmdName + "'",
      e.what()
      );
  }
 
  return 0;
}

void RTValCommandManager::doCommand(
  Command *cmd) 
{
  try
  {
    CommandManager::doCommand(
      cmd);
  }
   
  catch(Exception &e) 
  {
    cleanupUnfinishedCommandsAndThrow(
      cmd,
      e.getDesc_cstr()
      );
  }
}

void RTValCommandManager::checkCommandArgs(
  Command *cmd,
  const QMap<QString, RTVal> &args)
{ 
  RTValScriptableCommand* rtvalScriptCmd = dynamic_cast<RTValScriptableCommand*>(cmd);
  
  if(!rtvalScriptCmd) 
    CommandException::Throw(
      "RTValCommandManager::checkCommandArgs",
      "Command '" + cmd->getName() + "' is created with args, " + 
      "but is not implementing the RTValScriptableCommand interface"
      );

  // Sets the rtval args
  QMapIterator<QString, RTVal> ite(args);
  while (ite.hasNext()) 
  {
    ite.next();

    rtvalScriptCmd->setRTValArg(
      ite.key(), 
      ite.value());
  }

  ScriptableCommand* scriptCmd = dynamic_cast<ScriptableCommand*>(cmd);
  scriptCmd->validateSetArgs();
}

void RTValCommandManager::preProcessCommandArgs(
  Command* cmd)
{
  RTValScriptableCommand* rtvalScriptCmd = dynamic_cast<RTValScriptableCommand*>(cmd);
  
  if(!rtvalScriptCmd)
    return;

  try
  {
    ScriptableCommand* scriptCmd = dynamic_cast<ScriptableCommand*>(cmd);
    
    QString key;
    foreach(key, scriptCmd->getArgKeys())
    {
      if( scriptCmd->isArg(key, CommandArgFlags::IN_ARG) ||
          scriptCmd->isArg(key, CommandArgFlags::IO_ARG) )
      {
        RTVal pathValue = rtvalScriptCmd->getRTValArg(key);
        if(PathResolver::GetPathResolver()->knownPath(pathValue))
        {
          PathResolver::GetPathResolver()->getValue(pathValue);
          rtvalScriptCmd->setRTValArg(key, pathValue);
        }
      }
    }
  }
   
  catch(Exception &e) 
  {
    CommandException::Throw(
      "RTValCommandManager::preProcessCommandArgs",
      "",
      e.getDesc_cstr()
      );
  }

  catch(CommandException &e) 
  {
    CommandException::Throw(
      "RTValCommandManager::preProcessCommandArgs",
      "",
      e.what());
  }
}

void RTValCommandManager::postProcessCommandArgs(
  Command* cmd)
{
  RTValScriptableCommand* rtvalScriptCmd = dynamic_cast<RTValScriptableCommand*>(cmd);
  
  if(!rtvalScriptCmd)
    return;

  try
  {
    ScriptableCommand* scriptCmd = dynamic_cast<ScriptableCommand*>(cmd);
    
    QString key;
    foreach(key, scriptCmd->getArgKeys())
    {
      if( scriptCmd->isArg(key, CommandArgFlags::OUT_ARG) ||
          scriptCmd->isArg(key, CommandArgFlags::IO_ARG) )
      {
        RTVal pathValue = rtvalScriptCmd->getRTValArg(key);
        if(PathResolver::GetPathResolver()->knownPath(pathValue))
          PathResolver::GetPathResolver()->setValue(
            pathValue);
      }
    }
  }
   
  catch(Exception &e) 
  {
    CommandException::Throw(
      "RTValCommandManager::postProcessCommandArgs",
      "",
      e.getDesc_cstr()
      );
  }

  catch (CommandException &e) 
  {
    CommandException::Throw(
      "RTValCommandManager::postProcessCommandArgs",
      "",
      e.what());
  }
}

RTValComplexArgRegistry& RTValCommandManager::getComplexArgRegistry()
{
  return m_RTValComplexArgRegistry;
}
