namespace('widget');

///////////////////////////////////////////////////////////////
// class Edit

widget.Edit = Class.create();
widget.Edit.prototype.extend(webapp.widget.Widget.prototype).extend
({
  Create: function()
  {
    var self = this;
    this.pElement = document.createElement("input");
    this.tOptions.tParent.AppendChild(this.pElement);

    // добавление пункта меню для виджета
    this.AddWidgetMenu
    (
      this.sName, 
      function()
      {
        self.pElement.value = "меню: " + self.tProperties.sID;
      }
    );
  },
  
  DeserializeData: function() // записать динамические свойства в контрол
  {
    this.pElement.value = this.tProperties.sValue;
  },
  
  SerializeData: function() // получить динамические свойства из контрола
  {
    this.tProperties.sValue = this.pElement.value;
  },
  
  GetModifyData: function() // получить признак модицикации динамических свойств контрола
  {
    return (this.tProperties.sValue != this.pElement.value);
  },
  
  Destroy: function()
  {
    // pMenuItem и pElement будут удалены в родительском классе
  },
  
  IsMultiple: function()
  {
    return true;
  }
});
