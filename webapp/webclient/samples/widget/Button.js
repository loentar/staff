
///////////////////////////////////////////////////////////////
// class Button

widget.Button = Class.create();
widget.Button.prototype.extend(widget.Widget.prototype).extend
({
  Create: function()
  {
    var self = this;
    
    // создание YUI элемента(кнопка)
    self.pElement = new YAHOO.widget.Button
    ({
       id: self.tProperties.sID,
       label: self.tProperties.sLabel,
       container: self.tOptions.pParentElement
    });
    
    // устанавка обработчика нажатия
    self.pElement.on
    (
      "click", 
      function()
      {
        webapp.MessageBox.ShowMessage("нажата кнопка " + self.tProperties.sLabel, 'info');
      }
    );
     
    // добавление пункта меню для виджета
    self.AddWidgetMenu
    (
      self.sName, 
      function()
      {
        webapp.MessageBox.ShowMessage("обработчик виджета " + self.sClass + " " + self.tProperties.sLabel, 'info');
      }
    );
  },
  
  DeserializeData: function() // записать динамические свойства в контрол/обновить статические
  {
    if(this.pElement != null)
    {
      this.pElement.set('id', this.tProperties.sID);
      this.pElement.set('label', this.tProperties.sLabel);
    }
  },
  
  SerializeData: function() // получить динамические свойства из контрола
  {
  },
  
  Destroy: function()
  {
    // pMenuItem и pElement будут удалены в родительском классе
  }
});
