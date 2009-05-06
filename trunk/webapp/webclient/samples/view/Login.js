namespace('webapp');

webapp.Login =
{
  Init: function()
  {
    IncludeCss('assets/pages/Login');
    
    Include
    (
      [ 'i18n', 'webappui', 'ws', 'staff', 'session', 'error', 'noselect', '../clients/Login' ], 
      'webapp/common/', webapp.Login.InitLocale,
      [ 'i18n', 'webapp.ui' ]
    );
  },
  
  InitLocale: function()
  {
    i18n.LoadLocale('webapp_sample', '', webapp.Login.InitControls.bind(webapp.Login) );
  },
  
  InitControls: function()
  {
    document.title = _('Login page');
    this.tDivMain = new webapp.ui.Div(document.body, { sClass: 'divLoginMain' });
    
    this.tCenter = document.createElement('center');
    this.tDivMain.AppendChild(this.tCenter);
    
    var tFormElem = document.createElement('form');
    tFormElem.action = "javascript:webapp.Login.Login();";
    tFormElem.method = "post";
    tFormElem.className = "formLogin";
    this.tCenter.appendChild(tFormElem);

    var tTableLayout = new webapp.ui.Table(tFormElem);

    var tCellHeader = tTableLayout.AddRowCell({ sClass: 'cellCenter' });
    tCellHeader.colSpan = 2;

    var tLabel = new webapp.ui.Label(tCellHeader, { sCaption: _('Login'), sClass: 'labelLogin' });

    var tUserLabel = new webapp.ui.Label(tTableLayout.AddRowCell(), { sCaption: _('User') + ":", sClass: 'labelHeader' });
    this.tUserEdit = new webapp.ui.Edit(tTableLayout.AddRowCell(), { sClass: 'editor' });
    tUserLabel.SetBuddy(this.tUserEdit);

    var tPasswdLabel = new webapp.ui.Label(tTableLayout.AddRowCell(), { sCaption: _('Password') + ":", sClass: 'labelHeader' });
    this.tPasswdEdit = new webapp.ui.Edit(tTableLayout.AddRowCell(), { sClass: 'editor', bIsPassword: true });
    tPasswdLabel.SetBuddy(this.tPasswdEdit);
    
    var tCellButton = tTableLayout.AddRowCell({ sClass: 'cellCenter' });
    tCellButton.colSpan = 2;
    
    var tButton = document.createElement('input');
    tButton.type = 'submit';
    tButton.value = _('Enter');
    
    tCellButton.appendChild(tButton);
    
    this.tUserEdit.On('keydown', this._OnChange, this);
    this.tPasswdEdit.On('keydown', this._OnChange, this);
    
    this.tUserEdit.Focus();
  },
    
  Login: function()
  {
    try
    {
      if(!this.tLogin)
      {
        this.tLogin = new staff.Login();
      }
      
      var sSessionID = this.tLogin.Login(this.tUserEdit.GetText(), this.tPasswdEdit.GetText());
      this.LoginSuccessed(sSessionID);
    }
    catch(tError)
    {
      if(!this.tLabelError)
      {
        this.tLabelError = new webapp.ui.Label(this.tCenter, 
          { sCaption: _('Error login') + ': ' + _(tError.message || tError.text || tError || 'Unknown error'), 
            sClass: 'labelError' });
      }
      this.tUserEdit.Focus();
    }
  },
  
  _OnChange: function()
  {
    if(this.tLabelError)
    {
      this.tLabelError.destroy();
      delete this.tLabelError;
    }
  },
    
  LoginSuccessed: function(sSessionID)
  {
    var sLocation = 'main.htm?sessid=' + sSessionID + ',profile=webapp_sample';

    if (webapp.Env.host)
    {
      sLocation += ',host=' + webapp.Env.host;
    }
    
    if (webapp.Env.port)
    {
      sLocation += ",port=" + webapp.Env.port;
    }
    
    if (webapp.Env.lang)
    {
      sLocation += ',lang=' + webapp.Env.lang;
    }
    
    document.location = sLocation;
  }
};
