/*
 *  Copyright 2009 Utkin Dmitry
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
 
/* 
 *  This file is part of the WSF Staff project.
 *  Please, visit http://code.google.com/p/staff for more information.
 */

namespace('webapp.widget');
Include('webapp/view/DlgWidgetGroup');

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
    this.tWidgetManager = new widget.WidgetManager();
    if(this._Open())
    {
      this._LoadActiveWidgets();
      this._LoadActiveWidgetGroups();
    }
  },
  
  NewWidgetDlg: function()
  {
    var tDlgList = new webapp.view.DlgList({ sCaption: _('Add widget'), sLabel: _('Select widget from list') + ":" });
    tDlgList.On("confirm", this._OnConfirmNewWidget, this);
    tDlgList.SetItems(this.mAvailableWidgets, { fnFilter: this._FilterOutSingle, tObj: this, bTranslate: true });
    tDlgList.Show();
  },

  _OnConfirmNewWidget: function(tEvent)
  {
    var tUnits = {};
    var tWidgetLayout = this.GetWidgetByClass("webapp.widget.Layout");

    if(tWidgetLayout != null)
    {
      var tLayoutUnits = tWidgetLayout.GetUnits();
      for (var itUnit in tLayoutUnits)
      {
        var tUnit = tLayoutUnits[itUnit];
        if (tUnit.sId)
        {
          var bFound = false;
          for (var itWidget in this.mActiveWidgets)
          {
            var tWidget = this.mActiveWidgets[itWidget];
            if ( ((tWidget.sClass == tEvent.tItem.sKey) || (tWidget.IsFullUnit && tWidget.IsFullUnit())) &&
                 (tWidget.tWidgetParent == tUnit.tBody.tElement) )
            {
              bFound = true;
            }
          }

          if (!bFound)
          {
            tUnits[itUnit] = tUnit;
          }
        }
      }
    }
    else
    {
      tUnits[this.tOptions.tParent.Element().id] = 
        { 
          sName: _("Main unit"),
          sId: this.tOptions.tParent.Element().id,
          tBody: this.tOptions.tParent
        };
    }

    var tDlgList = new webapp.view.DlgList({ sCaption: _('Add widget'), sLabel: _('Select layout unit') + ":" });
    tDlgList.On("confirm", this._OnConfirmUnit, this, tEvent.tItem);
    tDlgList.SetItems(tUnits, { sKey: "sId", sLabel: "sName", bTranslate: true });
    tDlgList.Show();
  },
  
  _OnConfirmUnit: function(tEvent, tObj)
  {
    var tdoProps = new staff.DataObject();
    
    tdoProps.Properties = {};
    tdoProps.Properties.sParent = tEvent.tItem.sId;

    this.NewWidget(tObj.sKey, tdoProps);
  },

  RemoveWidgetDlg: function()
  {
    var tDlgList = new webapp.view.DlgList({ sCaption: _('Remove widget'), sLabel: _('Select widget from list') + ":" });
    tDlgList.On("confirm", this._OnConfirmRemoveWidget, this);
    tDlgList.SetItems(this.mActiveWidgets, { sKey: "sId", sLabel: "sDescr", fnFilter: this._FilterOutMainLayout, bTranslate: true });
    tDlgList.Show();
  },

  _OnConfirmRemoveWidget: function(tEvent)
  {
    this.RemoveWidget(tEvent.tItem.sId);
  },
    
  ConfigureWidgetDlg: function()
  {
    var tDlgList = new webapp.view.DlgList({ sCaption: _('Configure widget'), sLabel: _('Select widget from list') });
    tDlgList.On("confirm", this._OnConfirmConfigureWidget, this);
    tDlgList.SetItems(this.mActiveWidgets, 
      { sKey: "sId", sLabel: "sDescr", fnFilter: this._FilterOutNonConfigurableWidgets, bTranslate: true });
    tDlgList.Show();
  },

  _OnConfirmConfigureWidget: function(tEvent)
  {
    this.mActiveWidgets[tEvent.tItem.sId].Configure();
  },
    
  NewWidget: function(sClass, tProps)
  {
    var stWidget = 
    {
      sClass: sClass,
      sId: this._GenWidgetId(sClass),
      tdoProps: tProps || new staff.DataObject()
    };

    if (!stWidget.tdoProps.Properties)
    {
      stWidget.tdoProps.Properties = {};
    }
    
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
    if (sClass)
    {
      for(var nId in this.mActiveWidgets)
      {
        if(this.mActiveWidgets[nId].sClass == sClass) 
        {
          return this.mActiveWidgets[nId];
        }
      }
    }
  },
  
  GetWidgetByName: function(sName)
  {
    for(var nId in this.mActiveWidgets)
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
        this.tWidgetManager.AddWidget(tNewWidget.Serialize());
        this.mActiveWidgets[tNewWidget.sId] = tNewWidget;
      }.bind(this)
    );    
  },
  
  RemoveWidget: function(sWidgetId)
  {
    this.tWidgetManager.DeleteWidget(sWidgetId);

    this.mActiveWidgets[sWidgetId].destroy();
    delete this.mActiveWidgets[sWidgetId];
  },
  
  _Open: function()
  {
    try
    {
      this.tWidgetManager.Open(this.tOptions.sProfile);
      this.mAvailableWidgets = this.tWidgetManager.GetWidgetClasses();
      this.mAvailableWidgetGroups = this.tWidgetManager.GetAvailableWidgetGroups();
      return true;
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
      var aActiveWidgets = this.tWidgetManager.GetActiveWidgets();
      this.LoadWidgets(aActiveWidgets, this.mActiveWidgets);
    }
    catch(tError)
    {
      webapp.view.MessageBox.ShowMessage(_('Cannot get active widget list') + ":<br>" + tError, 'error');
    }
  },
  
  LoadWidgets: function(mWidgets, mLoadedWidgets)
  {
    var asWidgetScripts = [];
    for(var sWidgetId in mWidgets)
    {
      var sClass = mWidgets[sWidgetId].sClass;
      if (sClass)
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
        for(var sWidgetId in mWidgets)
        {
          if(typeof mWidgets[sWidgetId] == 'object')
          {
            try
            {
              mLoadedWidgets[sWidgetId] = this._CreateWidget(mWidgets[sWidgetId], this.tOptions);
            }
            catch(tError)
            {
              if (sWidgetId.charAt(0) == '_')
              {
                webapp.view.MessageBox.ShowMessage(_('Error while loading main Layout') 
                    + ":<br>" + (tError.message || tError), 'error');
                return;
              }
              else
              {
                if(!confirm(_('Cannot load widget') + ":\n" + (tError.message || tError) + 
                  "\n" + _('Load this widget next time') + "?"))
                {
                  this.tWidgetManager.DeleteWidget(sWidgetId);
                }
              }
            }
          } // if typeof == object
        } // for
      }.bind(this)
    );
  },
  
  SaveWidgets: function()
  {
    for (var sIndex in this.mActiveWidgets)
    {
      var tWidget = this.mActiveWidgets[sIndex];
      if (tWidget.GetModify && tWidget.GetModify())
      {
        this.tWidgetManager.AlterWidget(tWidget.Serialize());
        tWidget.SetModify(false);
      }
    }

    for(var sWidgetGroupId in this.mActiveWidgetGroups)
    {
      var mWidgets = this.mActiveWidgetGroups[sWidgetGroupId];
      if (typeof mWidgets == 'object')
      {
        var bGroupModified = false;

        for (var sIndex in mWidgets)
        {
          var tWidget = mWidgets[sIndex];
          if (tWidget.GetModify && tWidget.GetModify())
          {
            bGroupModified = true;
            break;
          }
        }

        if (bGroupModified)
        {
          var tWidgetGroupData = 
          {
            sId: sWidgetGroupId,
            sDescr: this.mAvailableWidgetGroups[sWidgetGroupId],
            mWidgets: {}
          };
          
          var mNewWidgetGroup = {};
          
          var aWidgets = [];
          for(var sIndex in mWidgets)
          {
            var tWidget = mWidgets[sIndex]
            if (tWidget.Serialize)
            {
              tWidgetGroupData.mWidgets[sIndex] = tWidget.Serialize();
              tWidget.SetModify(false);
            }
          }
          
          this.tWidgetManager.AlterWidgetGroup(tWidgetGroupData);
        }
      }
    }

    this.tWidgetManager.Commit();
  },
  
  UnloadWidgets: function(mWidgets)
  {
    for(nIndex in mWidgets)
    {
      if(typeof mWidgets[nIndex] != "function") // extend, each....
      {
        mWidgets[nIndex].destroy();
        delete mWidgets[nIndex];
      }
    }
  },
  
  
  _OnCloseWidget: function(tEvent, tWidget)
  {
    if(confirm(_('Remove widget') +  " \"" + _(tWidget.sDescr) + "\"?"))
    {
      this.RemoveWidget(tWidget.sId);
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
  
  _GenWidgetId: function(sClass)
  {
    return 'id_' + sClass.replace(/\./g, '_') + '_' + (new Date()).getTime().toString(36);
  },
  
  
  _LoadActiveWidgetGroups: function()
  {
    this.mActiveWidgetGroups = {};
    this.aActiveWidgetGroupList = this.tWidgetManager.GetActiveWidgetGroups();
    var mstActiveWidgetGroups = this.tWidgetManager.GetWidgetGroups(this.aActiveWidgetGroupList);
    this.LoadWidgetGroups(mstActiveWidgetGroups);
  },
  
  LoadWidgetGroups: function(mstWidgetGroups)
  {
    for(var sWidgetGroupId in mstWidgetGroups)
    {
      var stWidgetGroup = mstWidgetGroups[sWidgetGroupId];
      if (typeof stWidgetGroup == 'object')
      {
        this.LoadWidgetGroup(stWidgetGroup);
      }
    }
  },
  
  LoadWidgetGroup: function(mWidgetGroup)
  {
    this.mActiveWidgetGroups[mWidgetGroup.sId] = {};
    this.LoadWidgets(mWidgetGroup.mWidgets, this.mActiveWidgetGroups[mWidgetGroup.sId]);
  },
  
  UnloadWidgetGroup: function(sWidgetGroupId)
  {
    this.UnloadWidgets(this.mActiveWidgetGroups[sWidgetGroupId]);
    delete this.mActiveWidgetGroups[sWidgetGroupId];
  },
  
  ActivateWidgetGroupDlg: function()
  {
    var tDlgList = new webapp.view.DlgList({ sCaption: _('Activate widget group'), sLabel: _('Select widget group from list') });
    tDlgList.On("confirm", this._OnConfirmActivateWidgetGroup, this);
    tDlgList.SetItems(this.mAvailableWidgetGroups, { fnFilter: this._FilterOutActiveWidgetGroups, tObj: this, bTranslate: true });
    tDlgList.Show();
  },

  _OnConfirmActivateWidgetGroup: function(tEvent)
  {
    var mstWidgetGroups = this.tWidgetManager.GetWidgetGroups([tEvent.tItem.sKey]);
    this.LoadWidgetGroups(mstWidgetGroups);
    this.aActiveWidgetGroupList.push(tEvent.tItem.sKey);
    this.tWidgetManager.SetActiveWidgetGroups(this.aActiveWidgetGroupList);
  },
  
  DeactivateWidgetGroupDlg: function()
  {
    var tDlgList = new webapp.view.DlgList({ sCaption: _('Deactivate widget group'), sLabel: _('Select widget group from list') });
    tDlgList.On("confirm", this._OnConfirmDeactivateWidgetGroup, this);
    
    var mActiveWidgets = {};
    for (var nIndex = 0; nIndex != this.aActiveWidgetGroupList.length; ++nIndex)
    {
      var sGroupId = this.aActiveWidgetGroupList[nIndex];
      mActiveWidgets[sGroupId] = 
      {
        sGroupId: sGroupId,
        sDescr: this.mAvailableWidgetGroups[sGroupId] + '(' + sGroupId + ')'
      };
    }
    
    tDlgList.SetItems(mActiveWidgets, { sKey: "sGroupId", sLabel: "sDescr", bTranslate: true });
    tDlgList.Show();
  },

  _OnConfirmDeactivateWidgetGroup: function(tEvent)
  {
    for (var nIndex = 0; nIndex < this.aActiveWidgetGroupList.length; ++nIndex)
    {
      if (this.aActiveWidgetGroupList[nIndex] == tEvent.tItem.sGroupId)
      {
        this.UnloadWidgetGroup(tEvent.tItem.sGroupId);
        this.aActiveWidgetGroupList.splice(nIndex, 1);
        this.tWidgetManager.SetActiveWidgetGroups(this.aActiveWidgetGroupList);
        break;
      }
    }
  },
  
  CreateWidgetGroupDlg: function()
  {
    var tOpts = 
    {
      sGroupId: this._GenWidgetId('WidgetGroup'),
      sGroupDescr: '',
      aActiveWidgets: [],
      aWidgetsInGroup: []
    };
    
    tOpts.sGroupDescr = tOpts.sGroupId;
    
    for(var tIndex in this.mActiveWidgets)
    {
      var tWidget = this.mActiveWidgets[tIndex]
      if (tWidget.sId && tWidget.sId.charAt(0) != '_')
      {
        var tItem = 
        {
          sKey: tWidget.sId,
          sLabel: this.mAvailableWidgets[tWidget.sClass]
        };

        tOpts.aWidgetsInGroup.push(tItem);
      }
    }
    
    if (!tOpts.aWidgetsInGroup.length)
    {
      webapp.view.MessageBox.ShowMessage(_('Cannot create group') + ': <br/>' + _('add some wigets first'), 'error');
      return;
    }
    
    var tWidgetGroupDlg = new webapp.view.DlgWidgetGroup(tOpts);
    tWidgetGroupDlg.On('confirm', this._OnCreateWidgetGroupDlgConfirm, this);
    tWidgetGroupDlg.Show();
  },
  
  _OnCreateWidgetGroupDlgConfirm: function(tEvent)
  {
    var tNewWidgetGroupData = 
    {
      sId: tEvent.sGroupId,
      sDescr: tEvent.sGroupDescr,
      mWidgets: {}
    };
    
    var mNewWidgetGroup = {};
    
    var aWidgetsToDel = [];
    
    for (var tWidgetIndex in tEvent.aWidgetsInGroup)
    {
      var sWidgetId = tEvent.aWidgetsInGroup[tWidgetIndex].sKey;
      if (sWidgetId && sWidgetId.charAt(0) != '_')
      {
        var tWidget = this.mActiveWidgets[sWidgetId];
        if (tWidget)
        {
          aWidgetsToDel.push(tWidget.sId);
          tNewWidgetGroupData.mWidgets[sWidgetId] = tWidget.Serialize();
          mNewWidgetGroup[tWidget.sId] = tWidget;
          delete this.mActiveWidgets[sWidgetId];
        }
      }
    }
    
    this.tWidgetManager.DeleteWidgets(aWidgetsToDel);
    this.tWidgetManager.AddWidgetGroup(tNewWidgetGroupData);
    this.aActiveWidgetGroupList.push(tEvent.sGroupId);
    this.tWidgetManager.SetActiveWidgetGroups(this.aActiveWidgetGroupList);
    // move current widgets to group
    this.mActiveWidgetGroups[tEvent.sGroupId] = mNewWidgetGroup;
    this.mAvailableWidgetGroups[tEvent.sGroupId] = tEvent.sGroupDescr;
  },
  
  EditWidgetGroupDlg: function()
  {
    var tDlgList = new webapp.view.DlgList({ sCaption: _('Edit widget group'), sLabel: _('Select widget group from list') });
    tDlgList.On("confirm", this._OnEditWidgetGroupDlgSelect, this);

    var tActiveWidgetGroups = {};
    for (tIndex in this.mActiveWidgetGroups)
    {
      var tGroup = this.mActiveWidgetGroups[tIndex];
      if (typeof tGroup == 'object')
      {
        tActiveWidgetGroups[tIndex] = this.mAvailableWidgetGroups[tIndex];
      }
    }

    tDlgList.SetItems(tActiveWidgetGroups, {bTranslate: true});
    tDlgList.Show();
  },

  _OnEditWidgetGroupDlgSelect: function(tEvent)
  {
    var sWidgetGroupId = tEvent.tItem.sKey;
    var tWidgetGroup = this.mActiveWidgetGroups[sWidgetGroupId];

    var tOpts = 
    {
      sGroupId: sWidgetGroupId,
      sGroupDescr: tEvent.tItem.sLabel,
      aActiveWidgets: [],
      aWidgetsInGroup: []
    };
    
    for(var tIndex in tWidgetGroup)
    {
      var tWidget = tWidgetGroup[tIndex];
      if (typeof tWidget == 'object')
      {
        var tItem = 
        {
          sKey: tWidget.sId,
          sLabel: this.mAvailableWidgets[tWidget.sClass]
        };

        tOpts.aWidgetsInGroup.push(tItem);
      }
    }
    
    for(var tIndex in this.mActiveWidgets)
    {
      var tWidget = this.mActiveWidgets[tIndex]
      if (tWidget.sId && tWidget.sId.charAt(0) != '_')
      {
        var tItem = 
        {
          sKey: tWidget.sId,
          sLabel: this.mAvailableWidgets[tWidget.sClass]
        };

        tOpts.aActiveWidgets.push(tItem);
      }
    }
    
    var tWidgetGroupDlg = new webapp.view.DlgWidgetGroup(tOpts);
    tWidgetGroupDlg.On('confirm', this._OnEditWidgetGroupDlgConfirm, this);
    tWidgetGroupDlg.Show();
  },
  
  _OnEditWidgetGroupDlgConfirm: function(tEvent)
  {
    var tWidgetGroup = this.mActiveWidgetGroups[tEvent.sGroupId];

    var tWidgetGroupData = 
    {
      sId: tEvent.sGroupId,
      sDescr: tEvent.sGroupDescr,
      mWidgets: {}
    };
    
    var aWidgetsToDel = [];
    // include into group
    for (var tWidgetIndex in tEvent.aWidgetsInGroup)
    {
      var sWidgetId = tEvent.aWidgetsInGroup[tWidgetIndex].sKey;
      if (sWidgetId && sWidgetId.charAt(0) != '_')
      {
        var tWidgetInGroup = tWidgetGroup[sWidgetId];
        if (!tWidgetInGroup)
        {
          var tActiveWidget = this.mActiveWidgets[sWidgetId];
          aWidgetsToDel.push(sWidgetId);
          tWidgetGroupData.mWidgets[sWidgetId] = tActiveWidget.Serialize();
          tWidgetGroup[sWidgetId] = tActiveWidget;
          delete this.mActiveWidgets[sWidgetId];
        }
        else
        {
          tWidgetGroupData.mWidgets[sWidgetId] = tWidgetInGroup.Serialize();
        }
      }
    }
    
    var aWidgetsToAdd = [];
    // exclude from group
    for (var tWidgetIndex in tEvent.aActiveWidgets)
    {
      var sWidgetId = tEvent.aActiveWidgets[tWidgetIndex].sKey;
      if (sWidgetId && sWidgetId.charAt(0) != '_')
      {
        var tActiveWidget = this.mActiveWidgets[sWidgetId];
        if (!tActiveWidget)
        {
          var tWidgetInGroup = tWidgetGroup[sWidgetId];
          aWidgetsToAdd.push(tWidgetInGroup.Serialize());
          this.mActiveWidgets[sWidgetId] = tWidgetInGroup;
          delete tWidgetGroup[sWidgetId];
        }
      }
    }
    
    if (aWidgetsToAdd.length)
    {
      this.tWidgetManager.AddWidgets(aWidgetsToAdd);
    }
    if (aWidgetsToDel.length)
    {
      this.tWidgetManager.DeleteWidgets(aWidgetsToDel);
    }
  
    this.tWidgetManager.AlterWidgetGroup(tWidgetGroupData);
    this.mAvailableWidgetGroups[tEvent.sGroupId] = tEvent.sGroupDescr;
  },
  
  RemoveWidgetGroupDlg: function()
  {
    var tDlgList = new webapp.view.DlgList({ sCaption: _('Delete widget group'), sLabel: _('Select widget group from list') });
    tDlgList.On("confirm", this._OnConfirmRemoveWidgetGroup, this);
    tDlgList.SetItems(this.mAvailableWidgetGroups, {bTranslate: true});
    tDlgList.Show();
  },

  _OnConfirmRemoveWidgetGroup: function(tEvent)
  {
    var sWidgetGroupId = tEvent.tItem.sKey;
    // deactivate widget
    for (var nIndex = 0; nIndex < this.aActiveWidgetGroupList.length; ++nIndex)
    {
      if (this.aActiveWidgetGroupList[nIndex] == sWidgetGroupId)
      {
        this.UnloadWidgetGroup(sWidgetGroupId);
        this.aActiveWidgetGroupList.splice(nIndex, 1);
        this.tWidgetManager.SetActiveWidgetGroups(this.aActiveWidgetGroupList);
        break;
      }
    }
    
    this.tWidgetManager.DeleteWidgetGroup(sWidgetGroupId);
    delete this.mAvailableWidgetGroups[sWidgetGroupId];
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
    return tItem.sId.charAt(0) != '_';
  },
  
  _FilterOutSingle: function(tItem, tThis)
  {
    var tWidget = tThis.GetWidgetByClass(tItem.sKey);
    return tWidget == null || tWidget.IsMultiple != null && tWidget.IsMultiple();
  },
  
  _FilterOutNonConfigurableWidgets: function(tItem)
  {
    return tItem.Configure != null;
  },
  
  _FilterOutActiveWidgetGroups: function(tItem, tObj)
  {
    return !tObj.mActiveWidgetGroups[tItem.sKey];
  }
};
