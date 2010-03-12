
//static class webapp.MessageBox
webapp.MessageBox =
{
  Hide: function()
  {
    this.GetPanel().hide();
  },

  ShowMessage: function(sText, sType, asButtons, sCaption)
  {
    var self = this;

    this.GetPanel();
    
    if(sText != null)
      this.tMessageBoxPanel.setBody(sText);
    
    if(sCaption != null)
    {
      this.tMessageBoxPanel.setHeader(sCaption);
    } else
    if(sType == 'error')
    {
      this.tMessageBoxPanel.setHeader('Ошибка!');
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_BLOCK);
    }
    else
    if(sType == 'alarm')
    {
      this.tMessageBoxPanel.setHeader('Сигнал');
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_ALARM);
    }
    else
    if(sType == 'help')
    {
      this.tMessageBoxPanel.setHeader('Справка');
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_HTLP);
    }
    else
    if(sType == 'info')
    {
      this.tMessageBoxPanel.setHeader('Информация');
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_INFO);
    }
    else
    if(sType == 'tip')
    {
      this.tMessageBoxPanel.setHeader('Подсказка');
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_TIP);
    }
    else
    {
      this.tMessageBoxPanel.setHeader('Внимание!');
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_WARN);
    }

    if(asButtons == null)
      asButtons = [];
    asButtons.push({ text: 'Закрыть', handler: function() {self.tMessageBoxPanel.hide();}, isDefault: true, id: "btnDlgClose" });
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
            constraintoviewport: true, 
            icon: YAHOO.widget.SimpleDialog.ICON_WARN,
            buttons: [ { text: 'Закрыть' , handler: function() {self.tMessageBoxPanel.hide(); self.tResult = false;}, isDefault: true, id: 'btnDlgClose' } ]
          }
        );
      this.tMessageBoxPanel.render(document.body);
    }
    return this.tMessageBoxPanel;
  },
  
  tMessageBoxPanel: null
};
