namespace('widget');

///////////////////////////////////////////////////////////////
// class Button: example widget

widget.Button = Class.create();
widget.Button.prototype.extend(webapp.widget.Widget.prototype).extend
({
  Create: function(tParentElem)
  {
    if(!this.tProperties.sLabel)
    {
      this.tProperties.sLabel = this.sId;
    }
    
    // creating YUI button
    this.tButton = new YAHOO.widget.Button
    ({
       id: this.sId,
       label: this.tProperties.sLabel,
       container: tParentElem
    });
    
    // set "onclick" handler
    this.tButton.on('click', this._OnButtonClick.bind(this));
     
    // adding simple widget menu
    this.AddWidgetMenu(this.sId, this._OnMenuClick.bind(this));
  },
  
  _OnButtonClick: function()
  {
    webapp.view.MessageBox.ShowMessage( _('Button pressed') + ' ' + this.tProperties.sLabel, 'info');
  },

  _OnMenuClick: function()
  {
    webapp.view.MessageBox.ShowMessage(_('Menu item selected for button') + ' ' + this.tProperties.sLabel, 'info');
  },
  
  DeserializeData: function() // wirte dynamic data/update static
  {
    if(this.tButton != null)
    {
      this.tButton.set('id', this.sId);
      this.tButton.set('label', this.tProperties.sLabel);
    }
  },
  
  SerializeData: function() // get data from control
  {
  },
  
  Destroy: function()
  {
  },
  
  IsMultiple: function()
  {
    return true;
  },
  
  Configure: function()
  {
    var sResult = prompt(_('Enter new button label'), this.tProperties.sLabel);
    if (sResult)
    {
      this.tProperties.sLabel = sResult;
      this.tButton.set('label', this.tProperties.sLabel);
      this.SetModify();
    }
  }
});
