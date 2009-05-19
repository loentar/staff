namespace('widget');

///////////////////////////////////////////////////////////////
// class Edit

widget.Edit = Class.create();
widget.Edit.prototype.extend(webapp.widget.Widget.prototype).extend
({
  Create: function(tParent)
  {
    this.tEdit = new webapp.ui.Edit(tParent);
    this.tEdit.SetValue(this.tProperties.sValue || this.sId);

    this.AddWidgetMenu(this.sId, this._OnMenuClicked.bind(this));
    
    return this.tEdit;
  },
  
  _OnMenuClicked: function()
  {
    this.tEdit.SetValue("Hello, world!");
  },
  
  DeserializeData: function()
  {
    if (this.tProperties.sValue)
    {
      this.tEdit.SetValue(this.tProperties.sValue);
    }
  },
  
  SerializeData: function()
  {
    this.tProperties.sValue = this.tEdit.GetValue();
  },
  
  GetModifyData: function()
  {
    return (this.tProperties.sValue != this.tEdit.GetValue());
  },
  
  IsMultiple: function()
  {
    return true;
  }
});
