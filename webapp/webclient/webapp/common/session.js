var Session = 
{
  sHost: null,
  sPort: null,
  sID: null
};

Session.sID = GetLocParam("sessid", "");
Session.sHost = GetLocParam("host", document.location.hostname);
Session.sPort = GetLocParam("port", "");
