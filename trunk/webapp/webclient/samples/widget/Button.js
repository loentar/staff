namespace('widget');

///////////////////////////////////////////////////////////////
// class Button

widget.Button = Class.create();
widget.Button.prototype.extend(webapp.widget.Widget.prototype).extend
({
  Create: function(tParent)
  {
    if(this.tProperties.sID == null || this.tProperties.sID == "")
    {
      this.tProperties.sID = "button_" + Math.floor(Math.random() * 1000);
    }
    
    if(this.tProperties.sLabel == null || this.tProperties.sLabel == "")
    {
      this.tProperties.sLabel = this.tProperties.sID;
    }
    
    // создание YUI элемента(кнопка)
    this.tButton = new YAHOO.widget.Button
    ({
       id: this.tProperties.sID,
       label: this.tProperties.sLabel,
       container: tParent.Element != null ? tParent.Element() : tParent
    });
    
    // устанавка обработчика нажатия
    this.tButton.on("click", this._OnButtonClick.bind(this));
     
    // добавление пункта меню для виджета
    this.AddWidgetMenu(this.sName, this._OnMenuClick.bind(this));
  },
  
  _OnButtonClick: function()
  {
    webapp.view.MessageBox.ShowMessage("Нажата кнопка " + this.tProperties.sLabel, 'info');
  },

  _OnMenuClick: function()
  {
    webapp.view.MessageBox.ShowMessage("Обработчик меню для виджета " + this.tProperties.sLabel, 'info');
  },
  
  DeserializeData: function() // записать динамические свойства в контрол/обновить статические
  {
    if(this.tButton != null)
    {
      this.tButton.set('id', this.tProperties.sID);
      this.tButton.set('label', this.tProperties.sLabel);
    }
  },
  
  SerializeData: function() // получить динамические свойства из контрола
  {
//    this.tProperties.sParent = this.tButton.get('parentNode').id;
  },
  
  Destroy: function()
  {
    // pMenuItem и tButton будут удалены в родительском классе
  },
  
  IsMultiple: function()
  {
    return true;
  }
});

