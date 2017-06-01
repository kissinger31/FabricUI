//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <FabricCore.h>
#include "../OptionsEditorHelpers.h"
#include "CloseKLOptionsTargetEditorCommand.h"
#include <FabricUI/Commands/CommandArgHelpers.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;
using namespace OptionsEditor;

CloseKLOptionsTargetEditorCommand::CloseKLOptionsTargetEditorCommand() 
  : BaseRTValScriptableCommand()
  , m_canLog(true)
{
  try
  {
    declareRTValArg("editorID", "String");

    declareRTValArg(
      "failSilently",
      "Boolean",
      CommandArgFlags::OPTIONAL_ARG | CommandArgFlags::LOGGABLE_ARG,
      RTVal::ConstructBoolean(
        Application::FabricApplicationStates::GetAppStates()->getContext(), 
        false)
      );
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "CloseKLOptionsTargetEditorCommand::CloseKLOptionsTargetEditorCommand",
      "",
      e.what());
  }
};

CloseKLOptionsTargetEditorCommand::~CloseKLOptionsTargetEditorCommand()
{
}

bool CloseKLOptionsTargetEditorCommand::canUndo() {
  return false;
}

bool CloseKLOptionsTargetEditorCommand::canLog() {
  return m_canLog;
}

bool CloseKLOptionsTargetEditorCommand::doIt() 
{ 
  bool res = false;

  try
  {
    bool failSilently = getRTValArgValue("failSilently").getBoolean();
    QString editorID = getRTValArgValue("editorID").getStringCString();

    QWidget *dock = GetOptionsEditorDock(editorID);

    if(dock == 0)
    {
      m_canLog = false;
      res = failSilently;
    }

    else
    {
      dock->close();
      res = true;
    }
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "CloseKLOptionsTargetEditorCommand::doIt",
      "",
      e.what());
  }

  return res;
}

QString CloseKLOptionsTargetEditorCommand::getHelp()
{
  QMap<QString, QString> argsHelp;

  argsHelp["editorID"] = "Qt objectName of the option editor / ID of the KL option in the OptionsTargetRegistry";
  argsHelp["failSilently"] = "If false, throws an error if the widget has not been closed";

  return CommandArgHelpers::CreateHelpFromRTValArgs(
    "Close a Qt editor to edit a KL OptionsTarget",
    argsHelp,
    this);
}

QString CloseKLOptionsTargetEditorCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;

  argsDesc["editorID"] = getRTValArgValue(
    "editorID").getStringCString();

  argsDesc["failSilently"] = QString::number(
    getRTValArgValue("failSilently").getBoolean()
    );
 
  return CommandArgHelpers::CreateHistoryDescFromArgs(
    argsDesc,
    this);
}