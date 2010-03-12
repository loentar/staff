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

namespace('webapp.view');
IncludeCss('webapp/assets/view/DlgWidgetGroup/DlgWidgetGroup');

webapp.view.DlgWidgetGroup = Class.create();
webapp.view.DlgWidgetGroup.prototype.extend(webapp.Event.prototype).extend
({
  initialize: function(tOptions)
  {
    var tOpts = tOptions || {};

    this.tDialog = new YAHOO.widget.Dialog
    (
      "idDlgWidgetGroupProperties",
      {
        width: "450px",
        fixedcenter: true,
        visible: false,
        modal: true,
        constraintoviewport: true,
        buttons: 
        [
          { text: _('OK'), handler: this._OnConfirm.bind(this), isDefault: true },
          { text: _('Cancel'), handler: this._OnCancel.bind(this) } 
        ]
      }
    );
    
    this.tDialog.setHeader(tOpts.sCaption || _('Widget group properties'));
    this.tDialog.setBody("");
    this.tDialog.render(document.body);
    
    this.sGroupId = tOpts.sGroupId;

    // header
    var tDivDlgWidgetGroupHeader = new webapp.ui.Div(this.tDialog.body, { sClass: 'divDlgWidgetGroupHeader' });
    var tTableHeader = new webapp.ui.Table(tDivDlgWidgetGroupHeader, { sClass: 'tableDlgWidgetGroupHeader' });
    var tRowTableHeader = tTableHeader.AddRow();
    
    this.tLabelWidgetGroupDescr = new webapp.ui.Label(tTableHeader.AddCell(tRowTableHeader), 
        { sCaption: _('Widget group description') + ':'});
    tTableHeader.AddCell(tRowTableHeader, { sClass: 'tdDlgWidgetGroupSpace' });
    this.tEditWidgetGroupDescr = new webapp.ui.Edit(tTableHeader.AddCell(tRowTableHeader));
    this.tEditWidgetGroupDescr.SetText(tOpts.sGroupDescr);
    
    new webapp.ui.Hr(this.tDialog.body);
    
    // layout
    var tTableLayout = new webapp.ui.Table(this.tDialog.body);
    var tRowHeader = tTableLayout.AddRow();

    // body header
    var tRowBodyHeader = tTableLayout.AddRow();

    this.tLabelGroups = new webapp.ui.Label(tTableLayout.AddCell(tRowBodyHeader),
      { sCaption: _('Widgets in group') + ':'});
    tTableLayout.AddCell(tRowBodyHeader);
    this.tLabelWidgets = new webapp.ui.Label(tTableLayout.AddCell(tRowBodyHeader),
      { sCaption: _('Active widgets') + ':'});


    // body    
    var tRowBody = tTableLayout.AddRow();

    // widget groups
    var tCellBodyGroups = tTableLayout.AddCell(tRowBody);

    this.tSelectGroups = new webapp.ui.Select(tCellBodyGroups);
    this.tSelectGroups.SetSize(7);
    this.tSelectGroups.SetWidth(195);
    this.tSelectGroups.On("dblclick", this._OnMoveToGroups, this);
    this.tLabelGroups.SetBuddy(this.tSelectGroups);
    this.tSelectGroups.SetItems(tOpts.aWidgetsInGroup || [], { sKey: 'sKey', sLabel: 'sLabel' });
    

    // include/exclude buttons
    var tCellBodyButtons = tTableLayout.AddCell(tRowBody);

    this.tBtnInclude = new YAHOO.widget.Button
    ({
      id: 'idDlgWidgetGroupInclude',
      container: tCellBodyButtons,
      onclick: { fn: this._OnMoveToGroups, scope: this }
    });
    new webapp.ui.Br(tCellBodyButtons);
    this.tBtnExclude = new YAHOO.widget.Button
    ({
      id: 'idDlgWidgetGroupExclude',
      container: tCellBodyButtons,
      onclick: { fn: this._OnMoveToWidgets, scope: this }
    });

    // widgets
    var tCellBodyWidgets = tTableLayout.AddCell(tRowBody);

    this.tSelectWidgets = new webapp.ui.Select(tCellBodyWidgets);
    this.tSelectWidgets.SetSize(7);
    this.tSelectWidgets.SetWidth(195);
    this.tSelectWidgets.On("dblclick", this._OnMoveToWidgets, this);
    this.tLabelWidgets.SetBuddy(this.tSelectWidgets);

    this.tSelectWidgets.SetItems(tOpts.aActiveWidgets || [], { sKey: 'sKey', sLabel: 'sLabel' });
    
    this._UpdateButtons();
    this.tDialog.sizeUnderlay();
  },
  
  destroy: function()
  {
    this.tSelectGroups.destroy();
    this.tLabelGroups.destroy();
    this.tSelectWidgets.destroy();
    this.tLabelWidgets.destroy();
    this.tDialog.destroy();
  },
  
  Show: function()
  {
    this.tDialog.bringToTop();
    this.tDialog.show();
  },
  
  _OnConfirm: function()
  {
    if (!this.tSelectGroups.GetItemCount())
    {
      webapp.view.MessageBox.ShowMessage(_('Group can\'t be empty'), 'error');
      return;
    }

    var tEventObj =
    {
      sGroupId: this.sGroupId,
      sGroupDescr: this.tEditWidgetGroupDescr.GetText(),
      aActiveWidgets: this.tSelectWidgets.GetItems(),
      aWidgetsInGroup: this.tSelectGroups.GetItems()
    };
    this.FireEvent('confirm', tEventObj);
    this.destroy();
  },
  
  _OnCancel: function()
  {
    this.destroy();
  },
  
  _OnMoveToGroups: function()
  {
    this.tSelectGroups.MoveActiveItem(this.tSelectWidgets);
    this._UpdateButtons();
  },
  
  _OnMoveToWidgets: function()
  {
    this.tSelectWidgets.MoveActiveItem(this.tSelectGroups);
    this._UpdateButtons();
  },
  
  _UpdateButtons: function()
  {
    this.tBtnInclude.set('disabled', !this.tSelectGroups.GetItemCount());
    this.tBtnExclude.set('disabled', !this.tSelectWidgets.GetItemCount());
  }
  
});
