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

///////////////////////////////////////////////////////////////
// class ObjectAdmin

webapp.widget.ObjectAdmin = Class.create();
webapp.widget.ObjectAdmin.prototype.extend(webapp.widget.Widget.prototype).extend
({
  Create: function(tParent, tOpts)
  {
    IncludeCss("webapp/assets/widgets/ObjectAdmin/ObjectAdmin");
    
    this.tDivMain = new webapp.ui.Div(tParent, { sId: 'divObjectsAdmin' });

    Include
    (
      ["ObjectAdmin", "AccountAdmin"],
      "webapp/clients/",
      this._OnClientsAvaible.bind(this),
      ["staff.admin.ObjectAdmin", "staff.admin.AccountAdmin"]
    );
    
    return this.tDivMain;
  },

  _OnClientsAvaible: function()
  {
    this._tObjectAdminService = new staff.admin.ObjectAdmin();
    this._tAccountAdminService = new staff.admin.AccountAdmin();
    
    this._CreateLayout(this.tDivMain);

    this._LoadAccountData();
    this._LoadObjectTypes();
  },
  
  _CreateLayout: function(tParentDiv)
  {
    var tResize = new YAHOO.util.Resize(tParentDiv.Element(), { handles: ['br'], minWidth: 481, minHeight: 281 });
    
    var tDivTree = new webapp.ui.Div(tParentDiv, { sId: 'tDivTree' });
    var tDivProps = new webapp.ui.Div(tParentDiv, { sId: 'tDivProps' });
    
    var tLayout = new YAHOO.widget.Layout
    ( 
      tParentDiv.Element(),
      {
        height: 280,
        width: 480,
        units:
        [
          { body: tDivTree.Element(),   position: 'center', gutter: '5px 0px 5px 5px', scroll: true },
          { body: tDivProps.Element(),  position: 'right',  width: '275px', gutter: '5px', scroll: false }
        ]
      }
    );
    
    tResize.on
    (
      'resize', 
      function(args) 
      {
        tLayout.set('width', args.width - 1);
        tLayout.set('height', args.height - 1);
        tLayout.resize();
      }
    );
    
    tLayout.render(tParentDiv.Element());
    
    this._CreateTreeView(tDivTree);
    this._CreatePropsView(tDivProps);
    
    tDivTree.Element().parentNode.style.background = 'white';
  },
  
  _CreateTreeView: function(tDivTree)
  {
    var tYuiLoader = new YAHOO.util.YUILoader
    ({
      require: ['treeview'],
      base: 'yui/',
      onSuccess: function()
      {
        IncludeCss("webapp/assets/tree/tree");
        this._tObjectsTreeView = new YAHOO.widget.TreeView(tDivTree.Element(), {scrollable: true, width: "30em", height: "10em"});
        this._tObjectsTreeView.setDynamicLoad( this._OnTreeDataLoad.bind(this), 1 );
        this._tObjectsTreeView.subscribe("labelClick", this._ShowProps.bind(this));
        this._CreateTreeViewContextMenu();
        this._ReloadObjectsTree();
        
        tDivTree.On('dragstart', this._OnTreeNodeDragStart, this);
        tDivTree.On('dragend', this._OnTreeNodeDragEnd, this);
        tDivTree.On('dragenter', this._OnTreeNodeDragEnter, this);
        tDivTree.On('dragleave', this._OnTreeNodeDragLeave, this);
        tDivTree.On('dragover', this._OnTreeNodeDragOver, this);
      }.bind(this),
      onFailure: function(tError)
      {
        alert("error: " + YAHOO.lang.dump(tError));
      }
    });
    
    tYuiLoader.insert();
  },
  
  _CreateTreeViewContextMenu: function()
  {
    this.tContextMenuTree = new YAHOO.widget.ContextMenu(
      "tObjectAdminContextMenuTree",
      { trigger: this._tObjectsTreeView.getEl() } );

    this.tContextMenuTree.addItem(_('Add subobject'));
    this.tContextMenuTree.addItem(_('Remove'));
    
    this.tContextMenuTree.render(this.tDivMain.Element());
    this.tContextMenuTree.clickEvent.subscribe(this._OnContextMenuTreeClick, { tWidget: this });
    this.tContextMenuTree.triggerContextMenuEvent.subscribe(this._ShowMenu);
  },
  
  _CreatePropsView: function(tDivProps)
  {
    var tTableProps = new webapp.ui.Table(tDivProps, { sId: 'tTableProps', sCaption: _('Object properties') });

    var tHeadRowProps = tTableProps.AddHeadRow();
    tTableProps.AddCell(tHeadRowProps, _('Name'));
    tTableProps.AddCell(tHeadRowProps, _('Value'));
    
    // -- name -------
    var tRowName = tTableProps.AddRow();
    var tLabelName = new webapp.ui.Label(tTableProps.AddCell(tRowName), { sCaption: _('Name'), sClass: 'propLabel' });
    var tEditName = new webapp.ui.Edit(tTableProps.AddCell(tRowName), { sClass: 'propValue' });
    tLabelName.SetBuddy(tEditName);
    
    // -- type -------
    var tRowType = tTableProps.AddRow();
    var tLabelType = new webapp.ui.Label(tTableProps.AddCell(tRowType), { sCaption: _('Type'), sClass: 'propLabel' });
    var tSelectType = new webapp.ui.Select(tTableProps.AddCell(tRowType), { sClass: 'propValue' });
    tLabelType.SetBuddy(tSelectType);
    
    // -- descr -------
    var tRowDescr = tTableProps.AddRow();
    var tLabelDescr = new webapp.ui.Label(tTableProps.AddCell(tRowDescr), { sCaption: _('Description'), sClass: 'propLabel' });
    var tEditDescr = new webapp.ui.Edit(tTableProps.AddCell(tRowDescr), { sClass: 'propValue' });
    tLabelDescr.SetBuddy(tEditDescr);
    
    // -- user -------
    var tRowUser = tTableProps.AddRow();
    var tLabelUser = new webapp.ui.Label(tTableProps.AddCell(tRowUser), { sCaption: _('Owner'), sClass: 'propLabel' });
    var tSelectUser = new webapp.ui.Select(tTableProps.AddCell(tRowUser), { sClass: 'propValue' });
    tLabelUser.SetBuddy(tSelectUser);
    
    // -- group -------
    var tRowGroup = tTableProps.AddRow();
    var tLabelGroup = new webapp.ui.Label(tTableProps.AddCell(tRowGroup), { sCaption: _('Group'), sClass: 'propLabel' });
    var tSelectGroup = new webapp.ui.Select(tTableProps.AddCell(tRowGroup), { sClass: 'propValue' });
    tLabelGroup.SetBuddy(tSelectGroup);
    

    // ------- access ------------------------------------
    var tTableAccess = new webapp.ui.Table(tDivProps, { sId: 'tTableAccess', sCaption : _('Access rights') });
    
    var tHeadRowAccess = tTableAccess.AddHeadRow();
    tTableAccess.AddCell(tHeadRowAccess, {tContent: _('Owner')} );
    tTableAccess.AddCell(tHeadRowAccess, {tContent: _('Group')});
    tTableAccess.AddCell(tHeadRowAccess, {tContent: _('Others')});

    var tRowRead = tTableAccess.AddRow();
    var tLcbUserRead = new webapp.ui.LabeledCheckbox(tTableAccess.AddCell(tRowRead), { sCaption: _('Read') });
    var tLcbGroupRead = new webapp.ui.LabeledCheckbox(tTableAccess.AddCell(tRowRead), { sCaption: _('Read') });
    var tLcbOthersRead = new webapp.ui.LabeledCheckbox(tTableAccess.AddCell(tRowRead), { sCaption: _('Read') });

    var tRowWrite = tTableAccess.AddRow();
    var tLcbUserWrite = new webapp.ui.LabeledCheckbox(tTableAccess.AddCell(tRowWrite), { sCaption: _('Write') });
    var tLcbGroupWrite = new webapp.ui.LabeledCheckbox(tTableAccess.AddCell(tRowWrite), { sCaption: _('Write') });
    var tLcbOthersWrite = new webapp.ui.LabeledCheckbox(tTableAccess.AddCell(tRowWrite), { sCaption: _('Write') });

    var tRowExec = tTableAccess.AddRow();
    var tLcbUserExec = new webapp.ui.LabeledCheckbox(tTableAccess.AddCell(tRowExec), { sCaption: _('Exec') });
    var tLcbGroupExec = new webapp.ui.LabeledCheckbox(tTableAccess.AddCell(tRowExec), { sCaption: _('Exec') });
    var tLcbOthersExec = new webapp.ui.LabeledCheckbox(tTableAccess.AddCell(tRowExec), { sCaption: _('Exec') });
    
    var tDivButtons = new webapp.ui.Div(tDivProps, { sId: 'tDivButtons' });
    
    var tButtonSaveProps = new YAHOO.widget.Button
    ({
       label: _('Save'),
       id: 'tButtonSaveProps',
       container: tDivButtons.Element(),
       onclick: { fn: this._OnSaveProps, scope: this }
    });
    
    var tButtonReloadProps = new YAHOO.widget.Button
    ({
       label: _('Undo'),
       id: 'tButtonReloadProps',
       container: tDivButtons.Element(),
       onclick: { fn: this._OnReloadProps, scope: this }
    });
    
    
    // -- elements -----
    this._tPropControls = 
    {
      tEditName: tEditName,
      tSelectType: tSelectType,
      tEditDescr: tEditDescr,
      tSelectUser: tSelectUser,
      tSelectGroup: tSelectGroup,
      
      tAccess:
      {
        tUser:
        {
          tLcbRead: tLcbUserRead,
          tLcbWrite: tLcbUserWrite,
          tLcbExec: tLcbUserExec
        },
        
        tGroup: 
        {
          tLcbRead: tLcbGroupRead,
          tLcbWrite: tLcbGroupWrite,
          tLcbExec: tLcbGroupExec
        },
        
        tOthers: 
        {
          tLcbRead: tLcbOthersRead,
          tLcbWrite: tLcbOthersWrite,
          tLcbExec: tLcbOthersExec
        }
      }
    };
  },
  
  _LoadAccountData: function()
  {
    this.tUsers = this._tAccountAdminService.GetUsers();
    this.tGroups = this._tAccountAdminService.GetGroups();

    this._tPropControls.tSelectUser.SetItems(this.tUsers, { sKey: 'nId', sLabel: 'sName' });
    this._tPropControls.tSelectGroup.SetItems(this.tGroups, { sKey: 'nId', sLabel: 'sName' });
  },
  
  _LoadObjectTypes: function()
  {
    this._aObjectTypes = this._tObjectAdminService.GetObjectTypeList();
    this._tPropControls.tSelectType.SetItems(this._aObjectTypes, { sKey: 'nId', sLabel: 'sDescription' });
  },

  _ReloadObjectsTree: function()
  {
    var tRoot = this._tObjectsTreeView.getRoot();
    var tData = this._tObjectAdminService.GetObjectById(0);

    this._CreateTreeNode(tData.sObjectName, tRoot, tData);
    this._tObjectsTreeView.draw();
  },
  
  _CreateTreeNode: function(sCaption, tParentNode, tData)
  {
    var tNode = new YAHOO.widget.TextNode(sCaption, tParentNode, true);
    
    tNode.tData = tData;
    tNode.href = '#';
    
    return tNode;
  },
  
  _OnTreeNodeDragStart: function(tEvent)
  {
    this.tTreeNodeDragDrop = { };
    this.tTreeNodeDragDrop.tSrcElement = tEvent.srcElement;

    tEvent.dataTransfer.effectAllowed = "move";
    tEvent.dataTransfer.dropEffect = "move";
    tEvent.returnValue = true;

/*    this.tTreeNodeDragDrop.tDivMove = new webapp.ui.Div(this.tDivMain, { sId: 'tDivMove' });
    this.tTreeNodeDragDrop.tDivMove.Element().style.left = tEvent.x + 3;
    this.tTreeNodeDragDrop.tDivMove.Element().style.top = tEvent.y + 3;
    this.tTreeNodeDragDrop.tDivMove.Element().innerText = 'asdsadasda';*/
  },
  
  _OnTreeNodeDragEnd: function(tEvent)
  {
    if(tEvent.srcElement && tEvent.srcElement.tagName == 'A' && this.tTreeNodeDragDrop.tDstElement)
    {
      this.tTreeNodeDragDrop.tDstElement.className = this.tTreeNodeDragDrop.sClassName;

      if (confirm(_('Move object') + " \"" + this.tTreeNodeDragDrop.tSrcElement.firstChild.nodeValue + 
            "\" " + _('to') + " \"" + this.tTreeNodeDragDrop.tDstElement.firstChild.nodeValue + "\" ?"))
      {
        var tSrcNode = this._tObjectsTreeView.getNodeByElement(this.tTreeNodeDragDrop.tSrcElement);
        var tDstNode = this._tObjectsTreeView.getNodeByElement(this.tTreeNodeDragDrop.tDstElement);

        // replace parent id for service
        tSrcNode.tData.nParentObjectId = tDstNode.tData.nObjectId;

        // move in treeview
        this._CreateTreeNode(tSrcNode.label, tDstNode, tSrcNode.tData);
        this._tObjectsTreeView.removeNode(tSrcNode);
        this._tObjectsTreeView.draw();

        // commit changes in service
        this._tObjectAdminService.ReplaceObject(tSrcNode.tData);
      }
    }

//    this.tTreeNodeDragDrop.tDivMove.destroy();
    delete this.tTreeNodeDragDrop;
  },
  
  _OnTreeNodeDragEnter: function(tEvent)
  {
    if(tEvent.srcElement && tEvent.srcElement.tagName == 'A')
    {
      if (tEvent.srcElement != this.tTreeNodeDragDrop.tSrcElement)
      {
        this.tTreeNodeDragDrop.tDstElement = tEvent.srcElement;
        this.tTreeNodeDragDrop.sClassName = tEvent.srcElement.className;
        tEvent.srcElement.className = 'treeViewItemHighlight';
        tEvent.returnValue = false;
      }
    }
  },
  
  _OnTreeNodeDragLeave: function(tEvent)
  {
    if(tEvent.srcElement && tEvent.srcElement.tagName == 'A')
    {
      tEvent.srcElement.className = this.tTreeNodeDragDrop.sClassName;
      this.tTreeNodeDragDrop.tDstElement = null;
    }
  },
  
  _OnTreeNodeDragOver: function(tEvent)
  {
/*  this.tTreeNodeDragDrop.tDivMove.Element().style.left = tEvent.x + 3;
    this.tTreeNodeDragDrop.tDivMove.Element().style.top = tEvent.y + 3;
    */

    if(tEvent.srcElement && tEvent.srcElement.tagName == 'A')
    {
      tEvent.returnValue = false;
    }
  },
  
  _OnTreeDataLoad: function(tParentNode, onLoadComplete)
  {
    try
    {
      var aObjectIds = this._tObjectAdminService.GetObjectIdListWithParent(tParentNode.tData.nObjectId);
      if (aObjectIds.length > 0)
      {
        var aObjects = this._tObjectAdminService.GetObjectList(aObjectIds);
        for(var nIndex = 0; nIndex < aObjects.length; ++nIndex)
        {
          this._CreateTreeNode(aObjects[nIndex].sObjectName, tParentNode, aObjects[nIndex]);
        }
      }
    }
    catch(tError)
    {
    }

    onLoadComplete();
  },
  
  _ShowProps: function(tNode)
  {
    if (tNode)
    {
      this._tActiveNode = tNode;
    }
    
    if(!this._tActiveNode)
    {
      return;
    }
    
//!          tData.nObjectId = this._tPropControls.;
    this._tPropControls.tEditName.SetText(this._tActiveNode.tData.sObjectName);
    this._tPropControls.tSelectType.SetActiveItemById(this._tActiveNode.tData.nType);
    this._tPropControls.tEditDescr.SetText(this._tActiveNode.tData.sDescription);
    this._tPropControls.tSelectUser.SetActiveItemById(this._tActiveNode.tData.nUserId);
    this._tPropControls.tSelectGroup.SetActiveItemById(this._tActiveNode.tData.nGroupId);
//!          this._tActiveNode.tData.nParentObjectId = this._tPropControls.;

    this._tPropControls.tAccess.tUser.tLcbRead.SetChecked(this._tActiveNode.tData.stPermissions.stUser.bRead);
    this._tPropControls.tAccess.tUser.tLcbWrite.SetChecked(this._tActiveNode.tData.stPermissions.stUser.bWrite);
    this._tPropControls.tAccess.tUser.tLcbExec.SetChecked(this._tActiveNode.tData.stPermissions.stUser.bExecute);

    this._tPropControls.tAccess.tGroup.tLcbRead.SetChecked(this._tActiveNode.tData.stPermissions.stGroup.bRead);
    this._tPropControls.tAccess.tGroup.tLcbWrite.SetChecked(this._tActiveNode.tData.stPermissions.stGroup.bWrite);
    this._tPropControls.tAccess.tGroup.tLcbExec.SetChecked(this._tActiveNode.tData.stPermissions.stGroup.bExecute);

    this._tPropControls.tAccess.tOthers.tLcbRead.SetChecked(this._tActiveNode.tData.stPermissions.stOthers.bRead);
    this._tPropControls.tAccess.tOthers.tLcbWrite.SetChecked(this._tActiveNode.tData.stPermissions.stOthers.bWrite);
    this._tPropControls.tAccess.tOthers.tLcbExec.SetChecked(this._tActiveNode.tData.stPermissions.stOthers.bExecute);
    
    return false;
  },
  
  _CommitProps: function()
  {
    if (!this._tActiveNode || !this._tActiveNode.tData)
    {
      return;
    }
    
    var tData = this._tActiveNode.tData;

    //   don't change  tData.nObjectId
    tData.sObjectName = this._tPropControls.tEditName.GetText();
    tData.nType = this._tPropControls.tSelectType.GetActiveItemId();
    tData.sDescription = this._tPropControls.tEditDescr.GetText();
    tData.nUserId = this._tPropControls.tSelectUser.GetActiveItemId();
    tData.nGroupId = this._tPropControls.tSelectGroup.GetActiveItemId();
    
    // don't change tData.nParentObjectId
    //    tData.nParentObjectId = this._tActiveNode.parent.tData.nObjectId;

    tData.stPermissions.stUser.bRead = 
      this._tPropControls.tAccess.tUser.tLcbRead.GetChecked();
    tData.stPermissions.stUser.bWrite = 
      this._tPropControls.tAccess.tUser.tLcbWrite.GetChecked();
    tData.stPermissions.stUser.bExecute = 
      this._tPropControls.tAccess.tUser.tLcbExec.GetChecked();

    tData.stPermissions.stGroup.bRead = 
      this._tPropControls.tAccess.tGroup.tLcbRead.GetChecked();
    tData.stPermissions.stGroup.bWrite = 
      this._tPropControls.tAccess.tGroup.tLcbWrite.GetChecked();
    tData.stPermissions.stGroup.bExecute = 
      this._tPropControls.tAccess.tGroup.tLcbExec.GetChecked();

    tData.stPermissions.stOthers.bRead = 
      this._tPropControls.tAccess.tOthers.tLcbRead.GetChecked();
    tData.stPermissions.stOthers.bWrite = 
      this._tPropControls.tAccess.tOthers.tLcbWrite.GetChecked();
    tData.stPermissions.stOthers.bExecute = 
      this._tPropControls.tAccess.tOthers.tLcbExec.GetChecked();
    
    this._tActiveNode.getLabelEl().firstChild.nodeValue = tData.sObjectName;
    this._tActiveNode.data = tData.sObjectName;
    
    return tData;
  },
  
  _OnReloadProps: function()
  {
    this._ShowProps();
  },
  
  _OnSaveProps: function()
  {
    if (this._tActiveNode)
    {
      var tData = this._CommitProps();

      try
      {
        // new object?
        if (tData.nObjectId == -1)
        {
          tData.nObjectId = this._tObjectAdminService.AddObject(tData);
        }
        else
        {
          this._tObjectAdminService.ReplaceObject(tData);
        }
      }
      catch(tError)
      {
        webapp.view.MessageBox.ShowMessage(_('Error while saving object') 
            + ' ' + tData.sObjectName + ':\n' + tError.message, 'error');
      }
    }
  },
  
  _ShowMenu: function(sEvent, aArgs)
  {
    if(aArgs[0].srcElement.className != "ygtvlabel")
    {
      this.cancel();
    }
  },
  
  _OnContextMenuTreeClick:  function(sType, aArgs, tParams) 
  {
    var tMenuItem = aArgs[1];
    var tTargetNode = tParams.tWidget._tObjectsTreeView.getNodeByElement(this.contextEventTarget);
    if(tMenuItem && tTargetNode) 
    {
      switch(tMenuItem.index) 
      {
        case 0: // add
        {
          tParams.tWidget._OnAddObject(tTargetNode);
          tParams.tWidget._tPropControls.tEditName.Focus();
          break;
        }
        
        case 1:  // remove
        {
          tParams.tWidget._OnRemoveObject(tTargetNode);
          break;
        }
      }
    }
  },
  
  _OnAddObject: function(tParentNode)
  {
    if (tParentNode.tData.nObjectId == -1)
    {
      webapp.view.MessageBox.ShowMessage(_('Save object first') + ' ' + tParentNode.tData.sObjectName, 'error');
      return;
    }
    
    var tData = 
    {
      nObjectId: -1,
      sObjectName: 'NewObject',
      nType: 0,
      sDescription: _('New object'),
      nUserId: 0,
      nGroupId: 0,
      
      nParentObjectId: tParentNode.tData.nObjectId,

      stPermissions:
      {
        stUser:
        {
          bRead: true,
          bWrite: true,
          bExecute: true
        },

        stGroup:
        {
          bRead: true,
          bWrite: true,
          bExecute: true
        },

        stOthers:
        {
          bRead: false,
          bWrite: false,
          bExecute: false
        }
      }
    };

    var tNode = this._CreateTreeNode('*' + tData.sObjectName, tParentNode, tData);
    this._tObjectsTreeView.draw();

    this._ShowProps(tNode);
  },
  
  _OnRemoveObject: function(tNode)
  {
    if(confirm(_('Remove object') + ": " + tNode.tData.sObjectName + 
        "(" + tNode.tData.sDescription + ")\n" + _('and all childs') + "?")) 
    {
      try
      {
        // new object?
        if (tNode.tData.nObjectId != -1)
        {
          this._tObjectAdminService.RemoveObject(tNode.tData.nObjectId);
        }
        
        this._tObjectsTreeView.removeNode(tNode);
        this._tObjectsTreeView.draw();
      }
      catch(tError)
      {
        webapp.view.MessageBox.ShowMessage(_('Error while removing object') + ' ' + 
            tData.sObjectName + ':\n' + tError.message, 'error');
      }
    }
  }
});
