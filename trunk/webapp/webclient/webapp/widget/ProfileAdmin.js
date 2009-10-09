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
IncludeCss("webapp/assets/widgets/ProfileAdmin/ProfileAdmin");

///////////////////////////////////////////////////////////////
// class ProfileAdmin

webapp.widget.ProfileAdmin = Class.create();
webapp.widget.ProfileAdmin.prototype.extend(webapp.widget.Widget.prototype).extend
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
    this.aProfiles = this.tProfileAdmin.GetProfileList();
    var aProfiles = [];
    for (var itProfile in this.aProfiles)
    {
      var tProfile = this.aProfiles[itProfile];
      if (typeof tProfile == 'string')
      {
        aProfiles.push({sName: tProfile});
      }
    }

    this.tDataTableProfiles.deleteRows(0, this.tDataTableProfiles.getRecordSet().getLength());
    this.tDataTableProfiles.addRows(aProfiles);
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
    var tProfilesColumnDefs =
    [
      { key: "sName", label: _("Name"), sortable: true, resizeable: true, width: 180 },
    ];

    this.tDataSourceProfiles = new YAHOO.util.DataSource(this.aProfiles);
    this.tDataSourceProfiles.responseType = YAHOO.util.DataSource.TYPE_JSARRAY;
    this.tDataSourceProfiles.responseSchema = { fields: [ "sName" ] };

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
      tData.sName = tEditName.GetText();

      if (!tData.sName || (tData.sName.match(/\W/) != null))
      {
        webapp.view.MessageBox.ShowMessage(_("Invalid profile name") + ".<br/>" + _("Profile name must contain only latin letters, digits and underline sign") + '.', 'error');
        return;
      }

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

    this.tDlgEditAddProfile = new YAHOO.widget.SimpleDialog
    (
      'ProfileAdminEditAddDialog',
      {
        fixedcenter: true,
        modal: true,
        width: '410px',
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

    var tCellName = tTable.AddRowCell();
    var tLabName = new webapp.ui.Label(tCellName, { sCaption: _('Profile name') });
    var tEditName = new webapp.ui.Edit(tCellName);
    tLabName.SetBuddy(tEditName);

    var tCellName = tTable.AddRowCell({sClass: 'tdSpacer'});
    var tLabHeader = new webapp.ui.Label(tTable.AddRowCell(), { sCaption: _('Widgets list') });

    var tDataTableParentDiv = new webapp.ui.Div(tTable.AddRowCell(), {sClass: 'divParentDataTableWidgets' });

    //////////////////////////////////////////////////////////////////////////
    // create profiles table
    var tWidgetsColumnDefs =
    [
      { key: "sName", label: _("Name"), sortable: true, resizeable: true },
      { key: "sDescr", label: _("Description"), sortable: true, resizeable: true }
    ];

    var tWidgets = (tData && tData.lsWidgets) || [{ sName: "webapp.widget.Layout", sDescr: "Layout" }];

    this.tDataSourceWidgets = new YAHOO.util.DataSource(tWidgets);
    this.tDataSourceWidgets.responseType = YAHOO.util.DataSource.TYPE_JSARRAY;
    this.tDataSourceWidgets.responseSchema = { fields: [ "sName", "sDescr" ] };

    this.tDataTableWidgets = new YAHOO.widget.DataTable(tDataTableParentDiv.Element(),
          tWidgetsColumnDefs, this.tDataSourceWidgets,
          { scrollable: true, height: "15em", width: "100%", selectionMode: "single" });

    tDataTableParentDiv.Element().style.width = null; // yui override

    //////////////////////////////////////////////////////////////////////////
    // create context menu and events
    this.tDataTableWidgets.subscribe("rowMousedownEvent", this.tDataTableWidgets.onEventSelectRow);
    this.tDataTableWidgets.subscribe("cellDblclickEvent", this._OnConfigureEditWidget.bindAsEventListener(this) );

    tTable.AddRowCell({sClass: 'tdSpacer'});
    var tButtonsElem = new webapp.ui.Div(tTable.AddRowCell({sClass: "tdRight"})).Element();
    var tBtnAdd = new YAHOO.widget.Button
    ({
      container: tButtonsElem,
      label: _("Add"),
      onclick: { fn: this._OnConfigureAddWidget, scope: this }
    });
    var tBtnEdit = new YAHOO.widget.Button
    ({
      container: tButtonsElem,
      label: _("Edit"),
      onclick: { fn: this._OnConfigureEditWidget, scope: this }
    });
    var tBtnDelete = new YAHOO.widget.Button
    ({
      container: tButtonsElem,
      label: _("Delete"),
      onclick: { fn: this._OnConfigureDeleteWidget, scope: this }
    });

    this.tDlgEditAddProfile.bringToTop();
    this.tDlgEditAddProfile.show();

    if (tData)
    {
      tEditName.Disable();
      tEditName.SetText(tData.sName);
    }
    else
    {
      tEditName.Focus();
      tData =
      {
        sName: "",
        lsWidgets: []
      };
    }

    this.tCurrentProfileData = tData;
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

    var sProfileId = tRecord.getData('sName');

    var tProfile = this.tProfileAdmin.GetProfile(sProfileId);

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

    var sName = tRecord.getData('sName');

    if (sName == 'admin' || sName == 'default')
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
  },

  _OnCancelProperties: function()
  {
    this.tEditDiv.firstChild.nodeValue = this.sEditValue;
    this.tEditProp.destroy();
    delete this.tEditProp;
  },

  _OnConfigureEditWidget: function(tItem)
  {
    function OnConfirm(tEditedWidget)
    {
      for (var itWidget in this.tCurrentProfileData.lsWidgets)
      {
        var tWidget = this.tCurrentProfileData.lsWidgets[itWidget];
        if (tWidget.sName == tEditData.sName)
        {
          this.tEditRecord.setData('sName', tEditedWidget.sName);
          this.tEditRecord.setData('sDescr', tEditedWidget.sDescr);
          tWidget.sName = tEditedWidget.sName;
          tWidget.sDescr = tEditedWidget.sDescr;
          this.tDataTableWidgets.refreshView();
        }
      }

      return true;
    }

    var tTarget = tItem.target;
    if (!tTarget || this.tEditProp)
    {
      return;
    }

    var tRows = this.tDataTableWidgets.getSelectedRows();
    if(tRows.length == 0)
    {
      return;
    }

    this.tEditRecord = this.tDataTableWidgets.getRecord(tRows[0]);
    var tEditData = this.tEditRecord.getData();

    if (tEditData.sName == 'webapp.widget.Layout' ||
        ((tEditData.sName == 'webapp.widget.ProfileAdmin') && (this.tCurrentProfileData.sName == 'admin')))
    {
      webapp.view.MessageBox.ShowMessage(_('Widget') + " \"" + tEditData.sName + "\" " + _('is requid and cannot be changed'), 'error');
      return;
    }

    this._EditWidgetDlg(OnConfirm, _("Edit widget"), tEditData);
  },

  _OnConfigureAddWidget: function()
  {
    function OnConfirm(tNewWidget)
    {
      for (var itWidget in this.tCurrentProfileData.lsWidgets)
      {
        var tWidget = this.tCurrentProfileData.lsWidgets[itWidget];
        if (tWidget.sName == tNewWidget.sName)
        {
          webapp.view.MessageBox.ShowMessage(_("Widget with class") + " \"" + tNewWidget.sName + "\" " + _("already exists"), 'error');
          return;
        }
      }

      this.tDataTableWidgets.addRow(tNewWidget);
      this.tCurrentProfileData.lsWidgets.push(tNewWidget);
      return true;
    }

    this._EditWidgetDlg(OnConfirm, _("Add widget"));
  },

  _EditWidgetDlg: function(fnConfirm, sHeader, tData)
  {
    function OnConfirm()
    {
      var sName = tEditName.GetText();
      if (!sName || (sName.match(/[^A-Za-z0-9_\.]/) != null))
      {
        webapp.view.MessageBox.ShowMessage(_("Invalid class name") + ".<br/>" + _("Class name must contain only latin letters, digits, dots and underline sign") + '.', 'error');
        return;
      }

      var sDescr = tEditDescr.GetText();
      if (!sDescr)
      {
        webapp.view.MessageBox.ShowMessage(_("Widget description cannot be empty"), 'error');
        return;
      }

      var tWidget =
      {
        sName: sName,
        sDescr: sDescr,
      };

      if (fnConfirm)
      {
        fnConfirm.call(this, tWidget);
      }
      OnCancel.call(this);
    }

    function OnCancel()
    {
      this.tDialogWidgetEditAdd.hide();
      this.tDialogWidgetEditAdd.destroy();
      delete this.tDialogWidgetEditAdd;
      this.tDataTableProfiles.getBody().focus();
    }

    function OnKeyDown(tEvent)
    {
      if (tEvent.keyCode == 27) // Escape
      {
        OnCancel.call(this);
      }
      else
      if (tEvent.keyCode == 13) // Enter
      {
        OnConfirm.call(this);
      }
    }

    this.tDialogWidgetEditAdd = new YAHOO.widget.SimpleDialog
    (
      'WidgetConfAddEditDialog',
      {
        fixedcenter: true,
        modal: true,
        width: '300px',
        constraintoviewport: true,
        close: false,
        buttons:
        [
          { text: _('OK'), handler: OnConfirm.bind(this), isDefault: true },
          { text: _('Cancel'), handler: OnCancel.bind(this) }
        ]
      }
    );

    this.tDialogWidgetEditAdd.setHeader(sHeader);
    this.tDialogWidgetEditAdd.setBody("");
    this.tDialogWidgetEditAdd.render(document.body);

    addHandler(this.tDialogWidgetEditAdd.body, 'keydown', OnKeyDown.bind(this));

    // creating dlg components
    var tTable = new webapp.ui.Table(this.tDialogWidgetEditAdd.body, { sClass: 'tableFullWidth' });

    var tRowName = tTable.AddRow();
    var tLabName = new webapp.ui.Label(tTable.AddCell(tRowName), { sCaption: _('Class name') });
    var tEditName = new webapp.ui.Edit(tTable.AddCell(tRowName));
    tLabName.SetBuddy(tEditName);

    var tRowDescr = tTable.AddRow();
    var tLabDescr = new webapp.ui.Label(tTable.AddCell(tRowDescr), { sCaption: _('Description') });
    var tEditDescr = new webapp.ui.Edit(tTable.AddCell(tRowDescr));
    tLabDescr.SetBuddy(tEditDescr);

    if (tData)
    {
      tEditName.SetText(tData.sName);
      tEditDescr.SetText(tData.sDescr);
    }

    this.tDialogWidgetEditAdd.bringToTop();
    this.tDialogWidgetEditAdd.show();

    tEditName.Focus();
  },

  _OnConfigureDeleteWidget: function()
  {
    var tRows = this.tDataTableWidgets.getSelectedRows();
    if(tRows.length == 0)
    {
      webapp.view.MessageBox.ShowMessage(_('Select a widget'), 'error');
      return;
    }

    var tRecord = this.tDataTableWidgets.getRecord(tRows[0]);

    var sName = tRecord.getData('sName');

    if (sName == 'webapp.widget.Layout' ||
        ((sName == 'webapp.widget.ProfileAdmin') && (this.tCurrentProfileData.sName == 'admin')))
    {
      webapp.view.MessageBox.ShowMessage(_('Widget') + " \"" + sName + "\" " + _('is requid and cannot be deleted'), 'error');
      return;
    }

    if(confirm(_('Delete widget') + " \"" + tRecord.getData('sDescr') + "\"?"))
    {
      for (var itWidget in this.tCurrentProfileData.lsWidgets)
      {
        var tWidget = this.tCurrentProfileData.lsWidgets[itWidget];
        if (tWidget.sName == sName)
        {
          this.tCurrentProfileData.lsWidgets.splice(itWidget, 1);
          this.tDataTableWidgets.deleteRow(parseInt(itWidget));
          break;
        }
      }
    }
  }
});
