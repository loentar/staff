namespace('widget');

////////////////////////////////////////////////////////////////////////////////////////////////
// class WidgetLoader

widget.WidgetLoader = Class.create();
widget.WidgetLoader.prototype = 
{
  initialize: function()
  {
    this.pWidgetManager = new widget.WidgetManager();
    this.tWidgetEditDialog = new widget.WidgetEditDialog();
  },
  
  Init: function(tOptions)
  {
    this.tOptions = tOptions;

    this.mActiveWidgets = {};
    this.mAvailableWidgets = {};
    this.aAvailableWidgets = {};

    Include('WidgetFrame', 'webapp/view/',
      this._Open.bind(this),
      ['webapp.view.WidgetFrame']);
  },
  
  AddWidget: function()
  {
    try
    {
      webapp.dlgList.hide(); // если открыт диалог другого типа
      $('dlgListCaption').innerText = "Добавить сервис";
      $('dlgListLabel').innerText = "Выберите сервис из списка:";
      var pListName = $('selDlgListName');
      pListName.options.length = 0;
      pListName.options.length = this.aAvailableWidgets.length;
      for (var i = 0; i < this.aAvailableWidgets.length; i++)
      {
        pListName.options[i] = new Option(this.aAvailableWidgets[i].sDescr, i);
      }
      pListName.selectedIndex = 0;
      
      webapp.dlgList.onConfirm =
        function()
        {
          var nIndex = pListName.selectedIndex;
          var tSelectedObj = pListName.options[nIndex];
          var nWidgetID = tSelectedObj.value;
          var sWidgetDescr = tSelectedObj.text;
          var sWidgetClass = this.aAvailableWidgets[nWidgetID].sClass;

          webapp.dlgList.hide();
          
          IncludeClass // подгрузка сервиса
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

              this.tWidgetEditDialog.Show
              (
                stWidget,
                function(stWidget)
                {
                  try
                  {
                    var tNewWidget = this._CreateWidget(stWidget);
                    var nWidgetId = this.pWidgetManager.AddWidget(tNewWidget.Serialize());
                    this.mActiveWidgets[nWidgetId] = tNewWidget;
                  }
                  catch(tError)
                  {
                    webapp.MessageBox.ShowMessage(tError.message || tError, 'error');
                  }
                }.bind(this)
              );
            }.bind(this)
          );

        }.bind(this);

      webapp.dlgList.show();
    }
    catch(tError)
    {
      webapp.MessageBox.ShowMessage("Невозможно получить список доступных сервисов:<br>" + tError, 'error');
    }
  },
  
  EditWidget: function()
  {
    webapp.dlgList.hide(); // если открыт диалог другого типа
    $('dlgListCaption').innerText = "Изменить сервис";
    $('dlgListLabel').innerText = "Выберите сервис из списка:";
    var pListName = $('selDlgListName');
    pListName.options.length = 0;
    var j = 0;
    for(nIndex in this.mActiveWidgets)
    {
      if(typeof this.mActiveWidgets[nIndex] != "function") // extend, each....
      {
        pListName.options[j] = new Option(this.mActiveWidgets[nIndex].sName, nIndex);
        ++j;
      }
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
        var tWidget = this.mActiveWidgets[nWidgetID];
        var stWidget = tWidget.Serialize();
        this.tWidgetEditDialog.Show
        (
          stWidget,
          function(stWidget)
          {
            tWidget.Deserialize(stWidget);
            tWidget.SetModify(true);
            this.pWidgetManager.AlterWidget(nWidgetID, tWidget.Serialize());
          }
        );
      }.bind(this);

    webapp.dlgList.show();
  },
  
  DeleteWidget: function()
  {
    webapp.dlgList.hide(); // если открыт диалог другого типа
    $('dlgListCaption').innerText = "Выгрузить сервис";
    $('dlgListLabel').innerText = "Выберите сервис из списка:";
    var pListName = $('selDlgListName');
    pListName.options.length = 0;
    var j = 0;
    for(nIndex in this.mActiveWidgets)
      if(typeof this.mActiveWidgets[nIndex] != "function") // extend, each....
      {
        pListName.options[j] = new Option(this.mActiveWidgets[nIndex].sName, nIndex);
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

        this.pWidgetManager.DeleteWidget(nWidgetID);

        webapp.dlgList.hide();
        this.mActiveWidgets[nWidgetID].destroy();
        delete this.mActiveWidgets[nWidgetID];
      }.bind(this);

    webapp.dlgList.show();
  },
  
  _Dump: function()
  {
    var sDumpResult = "";
    for(nIndex in this.mActiveWidgets)
    {
      if(typeof this.mActiveWidgets[nIndex] != "function") // extend, each....
      {
        sDumpResult += "--------------\n"  + this.mActiveWidgets[nIndex].Dump() + "--------------\n";
      }
    }
    
    return sDumpResult;
  },
  
  DumpWidgets: function()
  {
    webapp.MessageBox.ShowMessage(this._Dump().replace(/\n/g, '<br />'), 'info');
  },
  
  GetWidgetList: function()
  {
    return this.mActiveWidgets;
  },
  
  GetWidgetByName: function(sName)
  {
    for(nId in this.mActiveWidgets)
    {
      if(typeof this.mActiveWidgets[nId] != "function" && // extend, each....
        this.mActiveWidgets[nId].sName == sName) 
      {
        return this.mActiveWidgets[nId];
      }
    }

    return null;
  },
  
  GetWidgetByClass: function(sClass)
  {
    for(nId in this.mActiveWidgets)
    {
      if(typeof this.mActiveWidgets[nId] != "function" && // extend, each....
        this.mActiveWidgets[nId].sClass == sClass) 
      {
        return this.mActiveWidgets[nId];
      }
    }

    return null;
  },
  
  GetWidgetIdByName: function(sName)
  {
    for(nId in this.mActiveWidgets)
    {
      if(typeof this.mActiveWidgets[nId] != "function" && // extend, each....
        this.mActiveWidgets[nId].sName == sName) 
      {
        return nId;
      }
    }
    
    return null;
  },
  
  LoadWidgets: function()
  {
    try
    {
      var tWidgetList = this.pWidgetManager.GetWidgetList();
      var asWidgetScripts = new Array()
      for(var nWidgetId in tWidgetList)
      {
        if(typeof tWidgetList[nWidgetId] != 'function')
        {
          asWidgetScripts.push("widget." + tWidgetList[nWidgetId].sClass);
        }
      }
      
      IncludeClass
      (
        asWidgetScripts,
        "",
        function()
        {
          this.UnloadWidgets();
          for(var nWidgetId in tWidgetList)
          {
            if(typeof tWidgetList[nWidgetId] != 'function')
            {
              try
              {
                this.mActiveWidgets[nWidgetId] = this._CreateWidget(tWidgetList[nWidgetId]);
              }
              catch(tError)
              {
                if(!confirm("Невозможно загрузить сервис:\n" + (tError.message || tError) + "\nЗагружать его в слудеющий раз?"))
                {
                  this.pWidgetManager.DeleteWidget(nWidgetId);
                }
              }

            }
          }
        }.bind(this)
      );
    }
    catch(tError)
    {
      webapp.MessageBox.ShowMessage("Невозможно получить список активных сервисов:<br>" + tError, 'error');
    }
  },
  
  SaveWidgets: function()
  {
    for(nIndex in this.mActiveWidgets)
    {
      if(typeof this.mActiveWidgets[nIndex] != "function") // extend, each....
      {
        var pWidget = this.mActiveWidgets[nIndex];
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
    for(nIndex in this.mActiveWidgets)
    {
      if(typeof this.mActiveWidgets[nIndex] != "function") // extend, each....
      {
        this.mActiveWidgets[nIndex].destroy();
        delete this.mActiveWidgets[nIndex];
      }
    }
    this.mActiveWidgets = {};
  },
  
  LoadWidget: function(stWidget, tOptions, pCallbackFunction)
  {
    IncludeClass
    (
      "widget." + stWidget.sClass,
      "",
      function()
      {
        var tNewWidget = this._CreateWidget(stWidget);
        var nWidgetId = this.pWidgetManager.AddWidget(tNewWidget.Serialize());
        this.mActiveWidgets[nWidgetId] = tNewWidget;
        if(pCallbackFunction != null)
        {
          pCallbackFunction(tNewWidget);
        }
      }.bind(this)
    );    
  },
  
  UnloadWidget: function(tWidget)
  {
    var pWidget;
    var nIndex;
    
    if(typeof tWidget != 'object')
    {
      nIndex = tWidget;
      pWidget = this.mActiveWidgets[nIndex];
    } else
    {
      pWidget = tWidget;
      nIndex = this.GetWidgetIdByName(tWidget.sName);
    }
    
    this.pWidgetManager.DeleteWidget(nIndex);

    pWidget.destroy();
    delete pWidget;
    delete this.mActiveWidgets[nIndex];
  },
  
  _Open: function()
  {
    try
    {
      this.pWidgetManager.Open(this.tOptions.sProfile);
      this.aAvailableWidgets = this.pWidgetManager.GetWidgetClassList();
      for(var nIndex = this.aAvailableWidgets.length - 1; nIndex >= 0; --nIndex)
      {
        this.mAvailableWidgets[this.aAvailableWidgets[nIndex].sClass] = this.aAvailableWidgets[nIndex].sDescr;
      }

      this.LoadWidgets();
    }
    catch(tError)
    {
      function Retry()
      { 
        setTimeout(this._Open().bind(this), 10); 
        webapp.MessageBox.Hide(); 
      }
      
      webapp.MessageBox.ShowMessage("Невозможно соединиться с сервисом сервисов:<br>" + (tError.message || tError), 'error',
        [{ text: 'Повторить', handler: Retry.bind(this) }]);
    }
  },
  
  _OnCloseWidget: function(tWidget)
  {
    if(confirm("Убрать сервис \"" + tWidget.tOptions.sDescription + "\"?"))
    {
      this.UnloadWidget(tWidget);
    }
  },
  
  _CreateWidget: function(stWidget)
  {
    var tOpts = this.tOptions.clone();
    tOpts.sDescription = this.mAvailableWidgets[stWidget.sClass];

    var tWidget = new widget[stWidget.sClass](stWidget, tOpts);
    tWidget.On("close", this._OnCloseWidget, this, tWidget);
    return tWidget;
  }

}
