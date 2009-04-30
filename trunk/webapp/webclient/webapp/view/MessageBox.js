namespace('webapp.view');

//static class webapp.view.MessageBox
webapp.view.MessageBox =
{
  Hide: function()
  {
    this.GetPanel().hide();
  },

  ShowMessage: function(sText, sType, asButtons, sCaption)
  {
    this.GetPanel();
    
    if(sText != null)
    {
      this.tMessageBoxPanel.setBody(sText);
    }
    
    if(sCaption != null)
    {
      this.tMessageBoxPanel.setHeader(sCaption);
    } else
    if(sType == 'error')
    {
      this.tMessageBoxPanel.setHeader(_('Error') + '!');
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_BLOCK);
    }
    else
    if(sType == 'alarm')
    {
      this.tMessageBoxPanel.setHeader(_('Alarm'));
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_ALARM);
    }
    else
    if(sType == 'help')
    {
      this.tMessageBoxPanel.setHeader(_('Help'));
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_HELP);
    }
    else
    if(sType == 'info')
    {
      this.tMessageBoxPanel.setHeader(_('Information'));
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_INFO);
    }
    else
    if(sType == 'tip')
    {
      this.tMessageBoxPanel.setHeader(_('Hint'));
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_TIP);
    }
    else
    {
      this.tMessageBoxPanel.setHeader(_('Warning') + '!');
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_WARN);
    }

    if(asButtons == null)
    {
      asButtons = [];
    }
    asButtons.push({ text: _('Close'), handler: this.tMessageBoxPanel.hide, isDefault: true, id: "btnDlgClose" });

    this.tMessageBoxPanel.cfg.setProperty('buttons', asButtons);
    this.tMessageBoxPanel.bringToTop();
    this.tMessageBoxPanel.show();
  },
  
  GetPanel: function()
  {
    if(this.tMessageBoxPanel == null)
    {
      this.tMessageBoxPanel = new YAHOO.widget.SimpleDialog
        (
          'alert', 
          {
            fixedcenter: true,
            visible: false,
            modal: true,
            width: '400px',
            constraintoviewport: true
          }
        );
      this.tMessageBoxPanel.render(document.body);
    }
    return this.tMessageBoxPanel;
  },
  
  tMessageBoxPanel: null
};
