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

namespace('webapp.widget.admin');
IncludeCss("webapp/assets/widgets/ProfileAdmin/ProfileAdmin");

///////////////////////////////////////////////////////////////
// class ProfileAdmin

webapp.widget.admin.ProfileAdmin = Class.create();
webapp.widget.admin.ProfileAdmin.prototype.extend(webapp.widget.Widget.prototype).extend
({
  Create: function(tParentElem, tOpts)
  {
    var tMainDiv = this._CreateProfiles(tParentElem);
    
    Include
    (
      "webapp/clients/ProfileAdmin", "",
      this._OnClientsAvailable.bind(this),
      ['webapp.admin.ProfileAdmin']
    );
    
    return tMainDiv;
  },
  
  _OnClientsAvailable: function()
  {
    this.tProfileAdmin = new webapp.admin.ProfileAdmin();
    this._ReloadProfiles();
  },

  _ReloadProfiles: function()
  {
    this.aProfiles = this.tProfileAdmin.GetProfiles();
    this.aWidgets = this.tProfileAdmin.GetWidgets();

    this.tDataTableProfiles.deleteRows(0, this.tDataTableProfiles.getRecordSet().getLength());
    this.tDataTableProfiles.addRows(this.aProfiles);
    this.tDataTableProfiles.refreshView();
  },

  _CreateProfiles: function(tParentElem)
  {
    // creating dlg components
    var tTable = new webapp.ui.Table(tParentElem, { sClass: 'tableFullWidth' });
    var tLabHeader = new webapp.ui.Label(tTable.AddRowCell(), { sCaption: _('Profiles list') });

    var tDataTableParentDiv = new webapp.ui.Div(tTable.AddRowCell(), {sClass: 'divParentDataTable' });

    //////////////////////////////////////////////////////////////////////////
    // create profiles table


    YAHOO.widget.DataTable.Formatter.fnBoolFormatter =
      function(elLiner, oRecord, oColumn, oData)
      {
        if(oRecord.getData("bIsAdmin").toLowerCase() == "true")
        {
          elLiner.innerHTML = _('Yes');
        }
        else
        {
          elLiner.innerHTML = _('No');
        }
      };

    var tProfilesColumnDefs =
    [
      { key: "sName", label: _("Name"), sortable: true, resizeable: true, width: 210 },
      { key: "bIsAdmin", label: _("Admin"), sortable: true, resizeable: true, width: 50, formatter: 'fnBoolFormatter' }
    ];

    this.tDataSourceProfiles = new YAHOO.util.DataSource(this.aProfiles);
    this.tDataSourceProfiles.responseType = YAHOO.util.DataSource.TYPE_JSARRAY;
    this.tDataSourceProfiles.responseSchema = { fields: [ "sId", "sName", "bIsAdmin" ] };

    this.tDataTableProfiles = new YAHOO.widget.DataTable(tDataTableParentDiv.Element(),
          tProfilesColumnDefs, this.tDataSourceProfiles,
          { scrollable: true, height: "15em", width: "100%", selectionMode: "single" });

    tDataTableParentDiv.Element().style.width = null; // yui override

    //////////////////////////////////////////////////////////////////////////
    // create context menu and events
    this.tDataTableProfiles.subscribe("rowMousedownEvent", this.tDataTableProfiles.onEventSelectRow);
    this.tDataTableProfiles.subscribe("cellDblclickEvent", this._OnConfigureEditProfile.bindAsEventListener(this) );

    tTable.AddRowCell({sClass: 'tdSpacer'});

    var tButtonsElem = new webapp.ui.Div(tTable.AddRowCell({sClass: "tdRight"})).Element();
    var tBtnAdd = new YAHOO.widget.Button
    ({
      container: tButtonsElem,
      label: _("Add"),
      onclick: { fn: this._OnConfigureAddProfile, scope: this }
    });
    var tBtnEdit = new YAHOO.widget.Button
    ({
      container: tButtonsElem,
      label: _("Edit"),
      onclick: { fn: this._OnConfigureEditProfile, scope: this }
    });
    var tBtnDelete = new YAHOO.widget.Button
    ({
      container: tButtonsElem,
      label: _("Delete"),
      onclick: { fn: this._OnConfigureDeleteProfile, scope: this }
    });

    return tTable;
  },
  

  _AddEditProfileDlg: function(fnConfirm, sHeader, tData)
  {
    function OnKeyDown(tEvent)
    {
      if (tEvent.keyCode == 27) // Escape
      {
        Cancel.call(this);
      }
    }

    function Confirm()
    {
      if (!tData)
      {
        tData = {};
        tData.sId = 'profile_' + new Date().getTime().toString(36);
      }
      tData.sName = tEditName.GetText();

      if (!tData.sName)
      {
        webapp.view.MessageBox.ShowMessage(_('Invalid profile name') + '.', 'error');
        return;
      }

      tData.bIsAdmin = tLabCheckbox.GetChecked();

      var tSelectedWidgets = [];
      var tSelItems = tSelSelectedWidgets.GetItems();

      for (var itItem in tSelItems)
      {
        var tItem = tSelItems[itItem];
        if (tItem.sClass)
        {
          tSelectedWidgets.push(tItem.sClass);
        }
      }
      tData.lsWidgets = tSelectedWidgets;

      // reload profiles list
      if (fnConfirm.call(this, tData))
      {
        Cancel.call(this);
        this._ReloadProfiles();
      }
    }

    function Cancel()
    {
      if(this.tDlgEditAddProfile)
      {
        this.tDlgEditAddProfile.destroy();
        delete this.tDlgEditAddProfile;
      }
    }

    function OnInclude()
    {
      tSelAvailableWidgets.MoveActiveItem(tSelSelectedWidgets);
      CheckButtons();
    }

    function OnExclude()
    {
      var sId = tSelSelectedWidgets.GetActiveItemId();

      if (sId == 'webapp.widget.Layout' ||
         ((sId == 'webapp.widget.admin.ProfileAdmin') && (tData.sId == 'admin')))
      {
        webapp.view.MessageBox.ShowMessage(_('Widget') + " \"" + tSelSelectedWidgets.GetActiveItemLabel() + "\" " + _('is requid and cannot be changed'), 'error');
        return;
      }

      tSelSelectedWidgets.MoveActiveItem(tSelAvailableWidgets);
      CheckButtons();
    }

    function CheckButtons()
    {
      tBtnExclude.Enable(tSelSelectedWidgets.GetItemCount() > 0);
      tBtnInclude.Enable(tSelAvailableWidgets.GetItemCount() > 0);
    }

    this.tDlgEditAddProfile = new YAHOO.widget.SimpleDialog
    (
      'ProfileAdminEditAddDialog',
      {
        fixedcenter: true,
        modal: true,
        width: '460px',
        constraintoviewport: true,
        close: false,
        buttons:
        [
          { text: _('OK'), handler: Confirm.bind(this), isDefault: true },
          { text: _('Cancel'), handler: Cancel.bind(this) }
        ]
      }
    );

    this.tDlgEditAddProfile.setHeader(_('Profile properties'));
    this.tDlgEditAddProfile.setBody("");
    this.tDlgEditAddProfile.render(document.body);

    addHandler(this.tDlgEditAddProfile.body, 'keydown', OnKeyDown.bindAsEventListener(this));

    // creating dlg components
    var tTable = new webapp.ui.Table(this.tDlgEditAddProfile.body, { sClass: 'tableFullWidth' });

    var tRowName = tTable.AddRow();
    var tLabName = new webapp.ui.Label(tTable.AddCell(tRowName), { sCaption: _('Profile name') + ':', sClass: 'labelProfileAdminProfileName' });
    tTable.AddCell(tRowName, { sClass: 'tdSpacer' });
    var tEditName = new webapp.ui.Edit(tTable.AddCell(tRowName));
    tLabName.SetBuddy(tEditName);

    tTable.AddRowCell({sClass: 'tdSpacer', nColSpan: 3});
    var tLabCheckbox = new webapp.ui.LabeledCheckbox(tTable.AddRowCell({ nColSpan: 3}), { sCaption: _('This profile is administrative') });
    if (tData)
    {
      tLabCheckbox.SetChecked(tData.bIsAdmin);
    }

    var tTable = new webapp.ui.Table(this.tDlgEditAddProfile.body, { sClass: 'tableFullWidth' });

    var tLabHeader = new webapp.ui.Label(tTable.AddRowCell({ nColSpan: 3 }), { sCaption: _('Widgets list') });

    tTable.AddRowCell({sClass: 'tdSpacer', nColSpan: 3});

    // selects
    var tRowSelectsHeader = tTable.AddRow();

    var tLabelSelectedWidgets = new webapp.ui.Label(tTable.AddCell(tRowSelectsHeader), { sCaption: _('Selected') });
    tTable.AddCell(tRowSelectsHeader);
    var tLabelAvailableWidgets = new webapp.ui.Label(tTable.AddCell(tRowSelectsHeader), { sCaption: _('Available') });


    var tRowSelects = tTable.AddRow();

    var tSelSelectedWidgets = new webapp.ui.Select(tTable.AddCell(tRowSelects, { sClass: 'tdProfileAdminWidgets' }), { sClass: 'selProfileAdminWidgets' });
    var tCellButtons = tTable.AddCell(tRowSelects, { sClass: 'tdProfileAdminMoveButtons' });
    var tSelAvailableWidgets = new webapp.ui.Select(tTable.AddCell(tRowSelects, { sClass: 'tdProfileAdminWidgets' }), { sClass: 'selProfileAdminWidgets' });
    tSelSelectedWidgets.SetSize(10);
    tSelAvailableWidgets.SetSize(10);

    var tBtnInclude = new webapp.ui.Button(tCellButtons, { sClass: 'btnWidgetAdmin' } );
    new webapp.ui.Br(tCellButtons);
    var tBtnExclude = new webapp.ui.Button(tCellButtons, { sClass: 'btnWidgetAdmin' } );
    new webapp.ui.Image(tBtnInclude, { sSrc: "webapp/assets/img/go-previous.png" });
    new webapp.ui.Image(tBtnExclude, { sSrc: "webapp/assets/img/go-next.png" });

    tTable.AddRowCell({sClass: 'tdSpacer', nColSpan: 3});

    tBtnInclude.On('click', OnInclude, this);
    tBtnExclude.On('click', OnExclude, this);

    tSelAvailableWidgets.On('dblclick', OnInclude, this);
    tSelSelectedWidgets.On('dblclick', OnExclude, this);


    // load data

    var asWidgets = (tData && tData.lsWidgets) || [ "webapp.widget.Layout" ];
    var aSelectedWidgets = [];
    for (var itWidget in asWidgets)
    {
      var sWidget = asWidgets[itWidget];
      if (typeof sWidget == 'string')
      {
        for (var itAvailWidget in this.aWidgets)
        {
          var tAvailWidget = this.aWidgets[itAvailWidget];
          if (tAvailWidget.sClass == sWidget)
          {
            aSelectedWidgets.push({sClass: tAvailWidget.sClass, sName: tAvailWidget.sName});
            break;
          }
        }
      }
    }

    function FilterOutAvailWidgets(tItem)
    {
      for (var itWidget in aSelectedWidgets)
      {
        var tWidget = aSelectedWidgets[itWidget];
        if (tItem.sClass == tWidget.sClass)
        {
          return false;
        }
      }

      return true;
    }

    tSelSelectedWidgets.SetItems(aSelectedWidgets, { sKey: 'sClass', sLabel: 'sName', bTranslate: true });
    tSelAvailableWidgets.SetItems(this.aWidgets, { sKey: 'sClass', sLabel: 'sName', bTranslate: true, fnFilter: FilterOutAvailWidgets });
    CheckButtons();

    this.tDlgEditAddProfile.center();
    this.tDlgEditAddProfile.bringToTop();
    this.tDlgEditAddProfile.show();

    if (tData)
    {
      tEditName.SetText(tData.sName);
    }

    this.tCurrentProfileData = tData;
    tEditName.Focus();
  },

  _OnConfigureAddProfile: function()
  {
    function Confirm(tNewData)
    {
      for (var itProfile in this.aProfiles)
      {
        var tProfile = this.aProfiles[itProfile];
        if (tNewData.sName == tProfile.sName)
        {
          webapp.view.MessageBox.ShowMessage(_("Profile") + " \"" + tData.sName + "\" " + _("already exists"), 'error');
          return;
        }
      }

      this.tProfileAdmin.SetProfile(tNewData);
      return true;
    }

    this._AddEditProfileDlg(Confirm, _("Add profile"));
  },

  _OnConfigureEditProfile: function()
  {
    function Confirm(tNewData)
    {
      this.tProfileAdmin.SetProfile(tNewData);
      return true;
    }

    var tRows = this.tDataTableProfiles.getSelectedRows();
    if(tRows.length == 0)
    {
      webapp.view.MessageBox.ShowMessage(_('Select a profile'), 'error');
      return;
    }

    var tRecord = this.tDataTableProfiles.getRecord(tRows[0]);

    var sProfileId = tRecord.getData('sId');

    var tEditProfile;
    for (var itProfile in this.aProfiles)
    {
      var tProfile = this.aProfiles[itProfile];
      if (tProfile.sId == sProfileId)
      {
        tEditProfile = tProfile;
        break;
      }
    }

    this._AddEditProfileDlg(Confirm, _("Edit profile"), tProfile);
  },

  _OnConfigureDeleteProfile: function()
  {
    var tRows = this.tDataTableProfiles.getSelectedRows();
    if (tRows.length == 0)
    {
      webapp.view.MessageBox.ShowMessage(_('Select a profile'), 'error');
      return;
    }

    var tRecord = this.tDataTableProfiles.getRecord(tRows[0]);

    var sId = tRecord.getData('sId');
    var sName = tRecord.getData('sName');

    if (sId == 'admin')
    {
      webapp.view.MessageBox.ShowMessage(_('Profile') + " \"" + sName + "\" " + _('is requid and cannot be deleted'), 'error');
      return;
    }

    if(confirm(_('Delete profile') + " \"" + sName + "\"?"))
    {
      this.tProfileAdmin.RemoveProfile(sName);

      for (var itProfile in this.aProfiles)
      {
        var tProfile = this.aProfiles[itProfile];
        if (tProfile == sName)
        {
          this.aProfiles.splice(itProfile, 1);
          this.tDataTableProfiles.deleteRow(parseInt(itProfile));
          break;
        }
      }
    }
  }
});
