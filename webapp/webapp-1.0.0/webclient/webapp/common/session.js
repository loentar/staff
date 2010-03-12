var Session = 
{
  sHost: null,
  sPort: null,
  sID: null
};

Session.sID = webapp.Env.Get('sessid', '');
Session.sHost = webapp.Env.Get('host', document.location.hostname);
Session.sPort = webapp.Env.Get('port', '');
