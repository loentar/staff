namespace('webapp.widget');

////////////////////////////////////////////////////////////////////////////////////////////////
// class WidgetLoader

webapp.widget.WidgetLoader = Class.create();
webapp.widget.WidgetLoader.prototype = 
{
  initialize: function()
  {
  },
  
  destroy: function()
  {
  },
  
  Init: function(tOptions)
  {
    this.tOptions = tOptions;

    this.mActiveWidgets = {};
    this.mAvailableWidgets = {};
    this.aAvailableWidgets = {};

    Include
    (
      [ "webapp/clients/WidgetManager", "webapp/view/WidgetFrame" ],
      "",
      this._OnInitComplete.bind(this),
      [ "widget.WidgetManager", "webapp.view.WidgetFrame" ]
    );
  },
  
  _OnInitComplete: function()
  {
    this.pWidgetManager = new widget.WidgetManager();
    this._Open();
    this._LoadActiveWidgets();
  },
  
  NewWidgetDlg: function()
  {
    var tDlgList = new webapp.view.DlgList({ sCaption: _('Add widget'), sLabel: _('Select widget from list') + ":" });
    tDlgList.On("confirm", this._OnConfirmNewWidget, this);
    tDlgList.SetItems(this.aAvailableWidgets, { sKey: "sClass", sLabel: "sDescr", 
      fnFilter: this._FilterOutSingle, tObj: this });
    tDlgList.Show();
  },

  _OnConfirmNewWidget: function(tEvent)
  {
    var tUnits;
    var tWidgetLayout = this.GetWidgetByClass("webapp.widget.Layout");

    if(tWidgetLayout != null)
    {
      tUnits = tWidgetLayout.GetUnits();
    }
    else
    {
      tUnits = {};
      tUnits[this.tOptions.tParent.Element().id] = 
        { 
          sName: _("Main unit"),
          sId: this.tOptions.tParent.Element().id,
          tBody: this.tOptions.tParent
        };
    }

    var tDlgList = new webapp.view.DlgList({ sCaption: _('Add widget'), sLabel: _('Select layout unit') + ":" });
    tDlgList.On("confirm", this._OnConfirmUnit, this, tEvent.tItem);
    tDlgList.SetItems(tUnits, { sKey: "sId", sLabel: "sName" });
    tDlgList.Show();
  },
  
  _OnConfirmUnit: function(tEvent, tObj)
  {
    var aProperties = [];
    aProperties.push({ sName: 'sParent', tValue: tEvent.tItem.sId });
    this.NewWidget(tObj.sClass, aProperties);
  },

  RemoveWidgetDlg: function()
  {
    var tDlgList = new webapp.view.DlgList({ sCaption: _('Remove widget'), sLabel: _('Select widget from list') + ":" });
    tDlgList.On("confirm", this._OnConfirmRemoveWidget, this);
    tDlgList.SetItems(this.mActiveWidgets, { sKey: "nId", sLabel: "sDescr", fnFilter: this._FilterOutMainLayout });
    tDlgList.Show();
  },

  _OnConfirmRemoveWidget: function(tEvent)
  {
    this.RemoveWidget(tEvent.tItem.nId);
  },
    
  ConfigureWidgetDlg: function()
  {
    var tDlgList = new webapp.view.DlgList({ sCaption: _('Configure widget'), sLabel: _('Select widget from list') });
    tDlgList.On("confirm", this._OnConfirmConfigureWidget, this);
    tDlgList.SetItems(this.mActiveWidgets, 
      { sKey: "nId", sLabel: "sDescr", fnFilter: this._FilterOutNonConfigurableWidgets });
    tDlgList.Show();
  },

  _OnConfirmConfigureWidget: function(tEvent)
  {
    this.mActiveWidgets[tEvent.tItem.nId].Configure();
  },
    
  NewWidget: function(sClass, aProperties)
  {
    var stWidget = 
    {
      sClass: sClass,
      sName: webapp.ui.IdGen.Gen(sClass),
      lsProperties: aProperties || []
    };
    
    this.AddWidget(stWidget, this.tOptions);
  },
  
  GetWidgetList: function()
  {
    return this.mActiveWidgets;
  },
  
  GetWidgetById: function(nId)
  {
    return this.mActiveWidgets[nId];
  },
  
  GetWidgetByClass: function(sClass)
  {
    for(nId in this.mActiveWidgets)
    {
      if(this.mActiveWidgets[nId].sClass == sClass) 
      {
        return this.mActiveWidgets[nId];
      }
    }
  },
  
  GetWidgetByName: function(sName)
  {
    for(nId in this.mActiveWidgets)
    {
      if(this.mActiveWidgets[nId].sName == sName) 
      {
        return this.mActiveWidgets[nId];
      }
    }
  },
  
  AddWidget: function(stWidget, tOptions)
  {
    IncludeClass
    (
      stWidget.sClass,
      "",
      function()
      {
        var tNewWidget = this._CreateWidget(stWidget, tOptions);
        var nWidgetId = this.pWidgetManager.AddWidget(tNewWidget.Serialize());
        tNewWidget.nId = nWidgetId;
        this.mActiveWidgets[nWidgetId] = tNewWidget;
      }.bind(this)
    );    
  },
  
  RemoveWidget: function(nWidgetId)
  {
    this.pWidgetManager.DeleteWidget(nWidgetId);

    this.mActiveWidgets[nWidgetId].destroy();
    delete this.mActiveWidgets[nWidgetId];
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
    }
    catch(tError)
    {
      function Retry()
      {
        setTimeout(this._Open().bind(this), 10); 
        webapp.view.MessageBox.Hide(); 
      }
      
      webapp.view.MessageBox.ShowMessage(_('Cannot open widget DB') + ":<br>" + (tError.message || tError), 'error',
        [{ text: _('Retry'), handler: Retry.bind(this) }]);
    }
  },
  
  _LoadActiveWidgets: function()
  {
    try
    {
      var tWidgetList = this.pWidgetManager.GetWidgetList();
      var asWidgetScripts = [];
      for(var nWidgetId in tWidgetList)
      {
        var sClass = tWidgetList[nWidgetId].sClass;
        if(sClass != null)
        {
          asWidgetScripts.push(sClass);
        }
      }
      
      IncludeClass
      (
        asWidgetScripts,
        "",
        function()
        {
          for(var nWidgetId in tWidgetList)
          {
            if(typeof tWidgetList[nWidgetId] == 'object')
            {
              try
              {
                var tWidget = this._CreateWidget(tWidgetList[nWidgetId], this.tOptions);
                tWidget.nId = nWidgetId;
                this.mActiveWidgets[nWidgetId] = tWidget;
              }
              catch(tError)
              {
                if(!confirm(_('Cannot load widget') + ":\n" + (tError.message || tError) + 
                  "\n" + _('Load this widget next time') + "?"))
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
      webapp.view.MessageBox.ShowMessage(_('Cannot get active widget list') + ":<br>" + tError, 'error');
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
  
/*  UnloadWidgets: function()
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
  },*/
  
  
  _OnCloseWidget: function(tEvent, tWidget)
  {
    if(confirm(_('Remove widget') +  " \"" + _(tWidget.sDescr) + "\"?"))
    {
      this.RemoveWidget(tWidget.nId);
    }
  },
  
  _CreateWidget: function(stWidget, tOptions)
  {
    var tOpts = tOptions.clone();
    tOpts.sDescr = _(this.mAvailableWidgets[stWidget.sClass]);
    var tWidget = new (eval(stWidget.sClass))(stWidget, tOpts);
    tWidget.sDescr = tOpts.sDescr;
    tWidget.On("close", this._OnCloseWidget, this, tWidget);
    return tWidget;
  },

  _Dump: function()
  {
    var sDumpResult = "";
    for(nIndex in this.mActiveWidgets)
    {
      if(this.mActiveWidgets[nIndex].Dump != null)
      {
        sDumpResult += "--------------\n"  + this.mActiveWidgets[nIndex].Dump() + "--------------\n";
      }
    }
    
    return sDumpResult;
  },
  
  DumpWidgets: function()
  {
    webapp.view.MessageBox.ShowMessage(this._Dump().replace(/\n/g, '<br />'), 'info');
  },
  
  _FilterOutMainLayout: function(tItem)
  {
    return tItem.sName != 'idMainLayout';
  },
  
  _FilterOutSingle: function(tItem, tThis)
  {
    var tWidget = tThis.GetWidgetByClass(tItem.sClass);
    return tWidget == null || tWidget.IsMultiple != null && tWidget.IsMultiple();
  },
  
  _FilterOutNonConfigurableWidgets: function(tItem)
  {
    return tItem.Configure != null;
  }
}
