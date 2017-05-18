// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <iostream>
#include "DFGPathResolver.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Commands/CommandException.h>

using namespace FabricUI;
using namespace DFG;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace PathResolvers;
 
DFGPathResolver::DFGPathResolver(
  Client client)
 : PathResolver()
 , m_client(client)
{
}

DFGPathResolver::~DFGPathResolver()
{
}

void DFGPathResolver::setBindingID(
  const QString &path,
  int bindingID)
{
  m_pathIDMap[path] = bindingID;

  m_binding = (m_client.getContext().getHost().getBindingForID(
    (unsigned)bindingID));
}

bool DFGPathResolver::knownPath(
  RTVal pathValue)
{
  bool hasPort = false;

  try 
  {
    pathValue = RTValUtil::forceToRTVal(
      pathValue);

    QString path = pathValue.maybeGetMember(
      "path").getStringCString();

    int index = path.lastIndexOf(".");
 
    DFGExec exec = m_binding.getExec().getSubExec(
      path.midRef(0, index).toUtf8().constData()
      );

    hasPort = exec.haveExecPort(
      path.midRef(index+1).toUtf8().constData()
      );
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "DFGPathResolver::knownPath",
      "",
      e.getDesc_cstr());
  }

  return hasPort;
}

QString DFGPathResolver::getType(
  RTVal pathValue)
{
  QString portType;

  try 
  {
    pathValue = RTValUtil::forceToRTVal(
      pathValue);

    QString path = pathValue.maybeGetMember(
      "path").getStringCString();
    
    int index = path.lastIndexOf(".");
 
    DFGExec exec = m_binding.getExec().getSubExec(
      path.midRef(0, index).toUtf8().constData()
      );

    portType = exec.getPortTypeSpec(
      path.midRef(index+1).toUtf8().constData()
      );
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "DFGPathResolver::getType",
      "",
      e.getDesc_cstr());
  }

  return portType;
}

void DFGPathResolver::getValue(
  RTVal pathValue)
{
  try 
  {
    pathValue = RTValUtil::forceToRTVal(
      pathValue);

    QString path = pathValue.maybeGetMember(
      "path").getStringCString();

    RTVal value = m_binding.getExec().getPortDefaultValue( 
      path.toUtf8().constData(), 
      getType(pathValue).toUtf8().constData()
      );

    pathValue.setMember(
      "value", 
      value
      );
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "DFGPathResolver::getValue",
      "",
      e.getDesc_cstr()
      );
  }
}

void DFGPathResolver::setValue(
  RTVal pathValue)
{
  try
  {
    pathValue = RTValUtil::forceToRTVal(
      pathValue);

    QString path = pathValue.maybeGetMember(
      "path").getStringCString();

    RTVal value = pathValue.maybeGetMember(
      "value");

    value = RTValUtil::forceToRTVal(
      value);

    m_binding.getExec().setPortDefaultValue( 
      path.toUtf8().constData(), 
      value, 
      false);
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "DFGPathResolver::setValue",
      "",
      e.getDesc_cstr());
  }
}
