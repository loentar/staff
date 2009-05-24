function cancelEvent(tEvent)
{
  if(tEvent.preventDefault)
    tEvent.preventDefault();
  else 
    tEvent.returnValue = false;
}

function selectHandler(tEvent)
{
  var tElement = tEvent.srcElement || tEvent.target;

  if(tElement && (tElement.tagName == "INPUT" || tElement.tagName == "TEXTAREA"))
  {
    return true;
  }
  
  cancelEvent(tEvent);
  return false;
}

addHandler(document, 'selectstart', selectHandler, false);
