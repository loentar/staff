function cancelEvent(e)
{
  if(e.preventDefault)
    e.preventDefault();
  else 
    e.returnValue = false;
}

function selectHandler(e)
{
  if(e.srcElement.tagName == "INPUT" || e.srcElement.tagName == "TEXTAREA")
    return true;
  
  cancelEvent(e);
  return false;
}

addHandler(document, 'selectstart', selectHandler, false);
