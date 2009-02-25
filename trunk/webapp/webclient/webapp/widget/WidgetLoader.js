namespace('widget');

////////////////////////////////////////////////////////////////////////////////////////////////
// class WidgetLoader

widget.WidgetLoader = Class.create();
widget.WidgetLoader.pWidgetManager = null;
widget.WidgetLoader.tOptions = null;
widget.WidgetLoader.aWidgetList = null;
widget.WidgetLoader.pCallbackFunction = null;
widget.WidgetLoader.tWidgetEditDialog = null;
widget.WidgetLoader.prototype = 
{
  initialize: function()
  {
    if(GetLocParam("debug", "none") != "none")
      throw Error("Debug");
    var self = this;
    this.pWidgetManager = new widget.WidgetManager();
    this.aWidgetList = new Array();
    this.tWidgetEditDialog = new widget.WidgetEditDialog();
  },
  
  Init: function(tOptions)
  {
    this.tOptions = tOptions;
    this._Open();
  },
  
  AddWidget: function()
  {
    var self = this;
    
    try
    {
      var tWidgetClassList = this.pWidgetManager.GetWidgetClassList();

      webapp.dlgList.hide(); // если открыт диалог другого типа
      $('dlgListCaption').innerText = "Добавить виджет";
      $('dlgListLabel').innerText = "Выберите виджет из списка:";
      var pListName = $('selDlgListName');
      pListName.options.length = 0;
      pListName.options.length = tWidgetClassList.length;
      for (var i = 0; i < tWidgetClassList.length; i++)
        pListName.options[i] = new Option(tWidgetClassList[i].sDescr, i);
      pListName.selectedIndex = 0;
      
      webapp.dlgList.onConfirm =
        function()
        {
          var nIndex = pListName.selectedIndex;
          var tSelectedObj = pListName.options[nIndex];
          var nWidgetID = tSelectedObj.value;
          var sWidgetClass = tWidgetClassList[nWidgetID].sClass;

          webapp.dlgList.hide();
          
          IncludeClass // подгрузка виджета
          (
            "widget." + sWidgetClass,
            "",
            function()
            {
              var stWidget = 
              {
                sClass: sWidgetClass, 
                sName: 'id' + sWidgetClass, 
                lsProperties: []
              };

              self.tWidgetEditDialog.Show
              (
                stWidget,
                function(stWidget)
                {
                  var nKey = 0;
                    
                  for(nIndex in self.aWidgetList)
                  {
                    if(typeof self.aWidgetList[nIndex] == "object" && nKey <= nIndex) // extend, each....
                    {
                      nKey = nIndex + 1;
                    }
                  }
                      
                  try
                  {
                    var tNewWidget = eval('new widget.' + sWidgetClass + '(stWidget, self.tOptions)');
                    self.pWidgetManager.AddWidget(tNewWidget.Serialize());
                    self.aWidgetList[nKey] = tNewWidget;
                  }
                  catch(tError)
                  {
                     webapp.MessageBox.ShowMessage(tError.text, 'error');
                  }
                }
              );
            }
          );
          
        }

        webapp.dlgList.show();
      }
      catch(tError)
      {
        webapp.MessageBox.ShowMessage("Невозможно получить список доступных виджетов:<br>" + tError, 'error');
      }
  },
  
  EditWidget: function()
  {
    var self = this;

    webapp.dlgList.hide(); // если открыт диалог другого типа
    $('dlgListCaption').innerText = "Изменить виджет";
    $('dlgListLabel').innerText = "Выберите виджет из списка:";
    var pListName = $('selDlgListName');
    pListName.options.length = 0;
    var j = 0;
    for(nIndex in this.aWidgetList)
      if(typeof this.aWidgetList[nIndex] == "object") // extend, each....
      {
        pListName.options[j] = new Option(this.aWidgetList[nIndex].sName, nIndex);
        ++j;
      }

    pListName.options.length = j; // opera fix?
    pListName.selectedIndex = 0;
    
    webapp.dlgList.onConfirm =
      function()
      {
        var nIndex = pListName.selectedIndex;
        var tSelectedObj = pListName.options[nIndex];
        var nWidgetID = tSelectedObj.value;
        var sWidgetName = tSelectedObj.text;

        webapp.dlgList.hide();
        var tWidget = self.aWidgetList[nWidgetID];
        var stWidget = tWidget.Serialize();
        self.tWidgetEditDialog.Show
        (
          stWidget,
          function(stWidget)
          {
            tWidget.Deserialize(stWidget);
            tWidget.SetModify(true);
            self.pWidgetManager.AlterWidget(nWidgetID, tWidget.Serialize());
          }
        );
      }

    webapp.dlgList.show();
  },
  
  DeleteWidget: function()
  {
    var self = this;

    webapp.dlgList.hide(); // если открыт диалог другого типа
    $('dlgListCaption').innerText = "Выгрузить виджет";
    $('dlgListLabel').innerText = "Выберите виджет из списка:";
    var pListName = $('selDlgListName');
    pListName.options.length = 0;
    var j = 0;
    for(nIndex in this.aWidgetList)
      if(typeof this.aWidgetList[nIndex] == "object") // extend, each....
      {
        pListName.options[j] = new Option(this.aWidgetList[nIndex].sName, nIndex);
        ++j;
      }

    pListName.options.length = j; // opera fix?
    pListName.selectedIndex = 0;
    
    webapp.dlgList.onConfirm =
      function()
      {
        var nIndex = pListName.selectedIndex;
        var tSelectedObj = pListName.options[nIndex];
        var nWidgetID = tSelectedObj.value;
        var sWidgetName = tSelectedObj.text;

        self.pWidgetManager.DeleteWidget(nWidgetID);

        webapp.dlgList.hide();
        var tWidgetToDel = self.aWidgetList[nWidgetID];
        tWidgetToDel.uninitialize();
        delete tWidgetToDel;
        delete self.aWidgetList[nWidgetID];
      }

    webapp.dlgList.show();
  },
  
  _Dump: function()
  {
    var sDumpResult = "";
    for(nIndex in this.aWidgetList)
      if(typeof this.aWidgetList[nIndex] == "object") // extend, each....
        sDumpResult += "--------------\n"  + this.aWidgetList[nIndex].Dump() + "--------------\n";
    
    return sDumpResult;
  },
  
  DumpWidgets: function()
  {
    webapp.MessageBox.ShowMessage(this._Dump().replace(/\n/g, '<br />'), 'info');
  },
  
  GetWidgetList: function()
  {
    return this.aWidgetList;
  },
  
  GetWidgetByName: function(sName)
  {
    for(nId in this.aWidgetList)
    {
      if(typeof this.aWidgetList[nId] == "object" && // extend, each....
        this.aWidgetList[nId].sName == sName) 
      {
        return this.aWidgetList[nId];
      }
    }

    return null;
  },
  
  GetWidgetByClass: function(sClass)
  {
    for(nId in this.aWidgetList)
    {
      if(typeof this.aWidgetList[nId] == "object" && // extend, each....
        this.aWidgetList[nId].sClass == sClass) 
      {
        return this.aWidgetList[nId];
      }
    }

    return null;
  },
  
  GetWidgetIdByName: function(sName)
  {
    for(nId in this.aWidgetList)
    {
      if(typeof this.aWidgetList[nId] == "object" && // extend, each....
        this.aWidgetList[nId].sName == sName) 
      {
        return nId;
      }
    }
    
    return null;
  },
  
  LoadWidgets: function()
  {
    var self = this;
    try
    {
      var tWidgetList = this.pWidgetManager.GetWidgetList();
      var asWidgetScripts = new Array()
      for(var i = 0; i < tWidgetList.length; ++i)
        asWidgetScripts.push("widget." + tWidgetList[i].value.sClass);
      
      IncludeClass
      (
        asWidgetScripts,
        "",
        function()
        {
          self.UnloadWidgets();
          for(var i = 0; i < tWidgetList.length; ++i)
          {
            var nKey = parseInt(tWidgetList[i].key);
            self.aWidgetList[nKey] = eval('new widget.' + tWidgetList[i].value.sClass + '(tWidgetList[' + i + '].value, self.tOptions)');
          }
        }
      );
    }
    catch(tError)
    {
      webapp.MessageBox.ShowMessage("Невозможно получить список активных виджетов:<br>" + tError, 'error');
    }
  },
  
  SaveWidgets: function()
  {
    for(nIndex in this.aWidgetList)
    {
      if(typeof this.aWidgetList[nIndex] == "object") // extend, each....
      {
        var pWidget = this.aWidgetList[nIndex];
        if(pWidget.GetModify())
        {
          this.pWidgetManager.AlterWidget(nIndex, pWidget.Serialize());
          pWidget.SetModify(false);
        }
      }
    }

    this.pWidgetManager.Commit();
  },
  
  UnloadWidgets: function()
  {
    for(nIndex in this.aWidgetList)
    {
      if(typeof this.aWidgetList[nIndex] == "object") // extend, each....
      {
        var pWidget = this.aWidgetList[nIndex];
        pWidget.uninitialize();
        delete pWidget;
        delete this.aWidgetList[nIndex];
      }
    }
    this.aWidgetList.length = 0;
  },
  
  LoadWidget: function(stWidget, tOptions, pCallbackFunction)
  {
    var self = this;
    IncludeClass
    (
      "widget." + stWidget.sClass,
      "",
      function()
      {
        var nKey = self.aWidgetList.length;
        var tNewWidget = eval('new widget.' + stWidget.sClass + '(stWidget, tOptions)');

        self.pWidgetManager.AddWidget(tNewWidget.Serialize());

        self.aWidgetList[nKey] = tNewWidget;
        if(pCallbackFunction != null)
          pCallbackFunction(self.aWidgetList[nKey]);
      }
    );    
  },
  
  UnloadWidget: function(tWidget)
  {
    var pWidget;
    var nIndex;
    
    if(typeof tWidget != 'object')
    {
      nIndex = tWidget;
      pWidget = this.aWidgetList[nIndex];
    } else
    {
      pWidget = tWidget;
      nIndex = this.GetWidgetIndexByName(tWidget);
    }
    
    this.pWidgetManager.DeleteWidget(nIndex);

    pWidget.uninitialize();
    delete pWidget;
    this.aWidgetList.splice(nIndex, 1);
  },
  
  _Open: function()
  {
    var self = this;
    try
    {
      this.pWidgetManager.Open(this.tOptions.sWidgetDbName);
      this.LoadWidgets();
    }
    catch(tError)
    {
      webapp.MessageBox.ShowMessage("Невозможно соединиться с сервисом виджетов:<br>" + tError.message, 'error',
        [{ text: 'Повторить', handler: function() { setTimeout(function(){self._Open();}, 10); webapp.MessageBox.Hide(); } }]);
    }
  }

}
