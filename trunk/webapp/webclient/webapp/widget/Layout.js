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

IncludeCss("webapp/assets/widgets/Layout/Layout");

///////////////////////////////////////////////////////////////
// class Layout

webapp.widget.Layout = Class.create();
webapp.widget.Layout.prototype.extend(webapp.widget.Widget.prototype).extend
({
  Create: function(tParent, tOpts)
  {
    if (!this.tProperties.Layout)
    {
      this.tProperties.Layout = this._GetDefaultLayoutUnits();
    }

    this.tLayoutUnits = this.tProperties.Layout;
        
    this.CreateControls(tParent, this.tLayoutUnits);
    
    var tParentDiv = new webapp.ui.Div(tParent);
    this.tParentElem = tParentDiv.Element();
    
    this.tLayout = new YAHOO.widget.Layout
    ( 
      this.tParentElem,
      {
        height: 300,
        width: 600,
        units: this.tLayoutUnits
      }
    );
    
    this.tLayout.on('beforeResize', this._OnBeforeResize.bind(this));
    this.tLayout.on('resize', this._OnResize.bind(this));
    this.tLayout.render();
    
    this.AddWidgetMenu(_("Configure layout"), this.Configure.bind(this));
    
    this._tParent = tParent;
    
    addHandler(window, 'resize', this._OnParentResize.bindAsEventListener(this));

    return this.tLayout;
  },

  Destroy: function()
  {
    if (this.tDiv != null)
    {
      for (var nIndex in this.tDiv)
      {
        if(this.tDiv[nIndex].destroy != null)
        {
          this.tDiv[nIndex].destroy();
        }
      }
    }
    
    var tLayoutElem = this.tLayout.get('element');
    tLayoutElem.parentNode.removeChild(tLayoutElem);
  },

  _OnParentResize: function()
  {
    this.tLayout.set('height', this._tParent.offsetHeight);
    this.tLayout.set('width', this._tParent.offsetWidth);
    this.tLayout.resize();
  },
  
  CreateControls: function(tParent, tLayoutUnits)
  {
    this.tDiv = {};
    for(var nArrIndex = 0; nArrIndex < tLayoutUnits.length; ++nArrIndex)
    {
      var tProps = tLayoutUnits[nArrIndex];
      this.tDiv[tProps.body] = new webapp.ui.Div(tParent, { sId: tProps.body });
    }
  },
  
  _Refresh: function()
  {
    //  adding/changing units
    for(var nArrIndex = 0; nArrIndex < this.tLayoutUnits.length; ++nArrIndex)
    {
      var tProps = this.tLayoutUnits[nArrIndex];
      if (!this.tDiv[tProps.body])
      {
        //  adding new unit
        this.tDiv[tProps.body] = new webapp.ui.Div(this._tParent, { sId: tProps.body });
        this.tLayout.addUnit(tProps);
      }
      else
      {
        // change
        var tUnit = this.tLayout.getUnitByPosition(tProps.position);
        if (tUnit)
        {
          for(var tIndex in tProps)
          {
            if (typeof tProps[tIndex] != 'function')
            {
              if (tUnit.get(tIndex) != tProps[tIndex])
              {
                tUnit.set(tIndex, tProps[tIndex] || false);
              }
            }
          }
        }
      }
    }
    
    // deleting old
    for(var tIndex in this.tDiv)
    {
      var tDiv = this.tDiv[tIndex];
      if (tDiv.Element)
      {
        var bFound = false;
        for(var nArrIndex = 0; nArrIndex < this.tLayoutUnits.length; ++nArrIndex)
        {
          var tProps = this.tLayoutUnits[nArrIndex];
          if (tIndex == tProps.body)
          {
            bFound = true;
            break;
          }
        }

        if (!bFound)
        {
          var tUnit = this.tLayout.getUnitById(tIndex);
          this.tLayout.removeUnit(tUnit);
          tUnit.destroy();

          this.tDiv[tIndex].destroy();
          delete this.tDiv[tIndex];
        }
      }
    }
    
    this.tLayout.refresh();
  },
  
  _GetDefaultLayoutUnits: function()
  {
    var tDefaultLayoutUnits = 
    [
      { body: 'divright' + this.sId, position: 'right', width: 515, resize: true, gutter: '5px', scroll: true },
      { body: 'divcenter' + this.sId, position: 'center', gutter: '5px 0px 5px 5px' }
    ];
    return tDefaultLayoutUnits;
  },
  
  GetFrameOptions: function()
  {
    var tOpts = 
    {
      bNoFrame: true
    };

    return tOpts;
  },
  
  GetUnits: function()
  {
    var tUnits = {};
    for(var tUnit in this.tLayoutUnits)
    {
      var sUnitId = this.tLayoutUnits[tUnit].body;
      if (sUnitId != null)
      {
        tUnits[sUnitId] = 
        {
          sName: _(this.tLayoutUnits[tUnit].position), 
          sPos: this.tLayoutUnits[tUnit].position,
          sId: sUnitId, 
          tBody: this.tDiv[sUnitId]
        };
      }
    }
    return tUnits;
  },
  
  GetLayout: function()
  {
    return this.tLayout;
  },

  _OnResize: function()
  {
    this.SetModify();
  },
  
  _OnBeforeResize: function()
  {
    if (this.tParentElem.parentNode.parentNode === document.body)
    {
      this.tLayout.set('width', YAHOO.util.Dom.getClientWidth());
      this.tLayout.set('height', YAHOO.util.Dom.getClientHeight() - (window.fullScreen ? 0 : 26));
    }
  },
  
  Configure: function()
  {
    this.tConfigureDlg = new YAHOO.widget.SimpleDialog
    (
      'LayoutConfigureDlg',
      {
        fixedcenter: true,
        visible: false,
        modal: true,
        width: '500px',
        constraintoviewport: true,
        buttons:
        [
          { text: _('OK'), handler: this._OnConfirmConfigure.bind(this), isDefault: true },
          { text: _('Cancel'), handler: this._OnCancelConfigure.bind(this) }
        ]
      }
    );
    this.tConfigureDlg.setHeader(_('Configure layout'));
    this.tConfigureDlg.render(document.body);
    
    //  init dlg
    var tBody = this.tConfigureDlg.body;
    
    // main markup table
    var tTableUnitProps = new webapp.ui.Table(tBody);
    var tRowBody = tTableUnitProps.AddRow();
    var tCellUnits = tTableUnitProps.AddCell(tRowBody);
    var tCellProps = tTableUnitProps.AddCell(tRowBody);

    // units markup
    var tDivUnits = new webapp.ui.Div(tCellUnits);
    var tLabelUnit = new webapp.ui.Label(tDivUnits, { sCaption: _('Click unit to edit')});
    var tTableUnits = new webapp.ui.Table(tDivUnits);
    // add units
    // top
    var tRowUnitTop = tTableUnits.AddRow();
    var tCellUnitTop = tTableUnits.AddCell(tRowUnitTop, { sClass: 'cellUnit', tContent: _('top') });
    tCellUnitTop.colSpan = 3;
    // left, center, right
    var tRowUnitsMid = tTableUnits.AddRow();
    var tCellUnitLeft = tTableUnits.AddCell(tRowUnitsMid, { sClass: 'cellUnit', tContent: _('left') });
    var tCellUnitCenter = tTableUnits.AddCell(tRowUnitsMid, { sClass: 'cellUnit', tContent: _('center') });
    var tCellUnitRight = tTableUnits.AddCell(tRowUnitsMid, { sClass: 'cellUnit', tContent: _('right') });
    // bottom
    var tRowUnitBottom = tTableUnits.AddRow();
    var tCellUnitBottom = tTableUnits.AddCell(tRowUnitBottom, { sClass: 'cellUnit', tContent: _('bottom') });
    tCellUnitBottom.colSpan = 3;
    
    tTableUnits.On('click', this._OnSelectUnit.bindAsEventListener(this));
    var tNoticeLabel = new webapp.ui.Label(tDivUnits, 
      { sCaption: _('After changing layout it\'s recommended to save widgets and reload page'), sClass: 'noticeLabel'});

    // properties markup
    var tDivProps = new webapp.ui.Div(tCellProps);
    var tLabelProps = new webapp.ui.Label(tDivProps, { sCaption: _('Properties')});
    var tTableProps = new webapp.ui.Table(tDivProps);
    
    this.tCells = 
    {
      top: tCellUnitTop,
      left: tCellUnitLeft,
      center: tCellUnitCenter,
      right: tCellUnitRight,
      bottom: tCellUnitBottom
    };

    this.tCtrl = {};
    this.tCtrl.enabled = new webapp.ui.LabeledCheckbox(tTableProps.AddRowCell(), { sCaption: _('Enabled'), sCheckId: 'chkLayoutCfg_enabled' });
    this.tCtrl.header = new webapp.ui.LabeledCheckbox(tTableProps.AddRowCell(), { sCaption: _('Header'), sCheckId: 'chkLayoutCfg_header', sClass: 'checkIdent' });
    this.tCtrl.valheader = new webapp.ui.Edit(tTableProps.AddRowCell(), { sClass: 'editLayoutPropLong' });
    this.tCtrl.gutter = new webapp.ui.LabeledCheckbox(tTableProps.AddRowCell(), { sCaption: _('Gutter'), sCheckId: 'chkLayoutCfg_gutter', sClass: 'checkIdent' });
    this.tCtrl.valgutter = new webapp.ui.Edit(tTableProps.AddRowCell(), { sClass: 'editLayoutProp' });
    this.tCtrl.width = new webapp.ui.LabeledCheckbox(tTableProps.AddRowCell(), { sCaption: _('Width'), sCheckId: 'chkLayoutCfg_width', sClass: 'checkIdent' });
    this.tCtrl.valwidth = new webapp.ui.Edit(tTableProps.AddRowCell(), { sClass: 'editLayoutProp' });
    this.tCtrl.height = new webapp.ui.LabeledCheckbox(tTableProps.AddRowCell(), { sCaption: _('Height'), sCheckId: 'chkLayoutCfg_height', sClass: 'checkIdent' });
    this.tCtrl.valheight = new webapp.ui.Edit(tTableProps.AddRowCell(), { sClass: 'editLayoutProp' });
    this.tCtrl.resize = new webapp.ui.LabeledCheckbox(tTableProps.AddRowCell(), { sCaption: _('Resize'), sCheckId: 'chkLayoutCfg_resize', sClass: 'checkIdent' });
    this.tCtrl.scroll = new webapp.ui.LabeledCheckbox(tTableProps.AddRowCell(), { sCaption: _('Scroll'), sCheckId: 'chkLayoutCfg_scroll', sClass: 'checkIdent' });

    this.tCtrl.enabled.On('click', this._OnConfigEnabled, this);
    this.tCtrl.header.On('click', this._OnConfigCheck, this);
    this.tCtrl.gutter.On('click', this._OnConfigCheck, this);
    this.tCtrl.width.On('click', this._OnConfigCheck, this);
    this.tCtrl.height.On('click', this._OnConfigCheck, this);

    this.tUnitRestrictValues =
    {
      top:
      {
        header: false,
        gutter: '5px',
        width: null,
        height: '250px',
        resize: true,
        scroll: false
      },

      left:
      {
        header: false,
        gutter: '5px',
        width: '100px',
        height: null,
        resize: true,
        scroll: true
      },

      center:
      {
        header: false,
        gutter: '5px',
        width: null,
        height: null,
        resize: true,
        scroll: true
      },

      right:
      {
        header: false,
        gutter: '5px',
        width: '500px',
        height: null,
        resize: true,
        scroll: true
      },

      bottom:
      {
        header: false,
        gutter: '5px',
        width: null,
        height: '250px',
        resize: true,
        scroll: false
      }
    };

    this._tLayoutUnitsCfg = this.tLayoutUnits.clone();

    this.tConfigureDlg.show();
    this._OnSelectUnit({target: tCellUnitTop});
    tCellUnitTop.focus();

    for (var itUnit in this._tLayoutUnitsCfg)
    {
      var tUnit = this._tLayoutUnitsCfg[itUnit];
      if (tUnit.position)
      {
        this.tCells[tUnit.position].className += ' cellUnitActive';
      }
    }

    this._WidgetsToRemove = {};

    addHandler(tBody, 'keydown', this._OnKeyDown.bindAsEventListener(this));
  },
  
  _OnCancelConfigure: function()
  {
    this.tConfigureDlg.hide();
    this.tConfigureDlg.destroy();
    delete this.tConfigureDlg;
    delete this.sUnit;
    delete this.tUnit;
    delete this.tCtrl;
    delete this._tLayoutUnitsCfg;
    delete this._WidgetsToRemove;
  },
  
  _OnConfirmConfigure: function()
  {
  //remove widgets
    var tWidgetLoader = window.parent.webapp.Webapp.GetWidgetLoader();
    if (tWidgetLoader)
    {
      for (var itWidget in this._WidgetsToRemove)
      {
        var tWidgets = this._WidgetsToRemove[itWidget];
        var nWidgetCount = tWidgets.length;
        if (nWidgetCount)
        {
          for (var nWidget = 0; nWidget != nWidgetCount; ++nWidget)
          {
            try
            {
              tWidgetLoader.RemoveWidget(tWidgets[nWidget].sId);
            }
            catch(tErr)
            {
            }
          }
        }
      }
    }

    this._SaveUnit(this.sUnit);
    this._tLayoutUnitsCfg.clone(this.tLayoutUnits);
    this._Refresh();
    this._OnCancelConfigure();
    this.SetModify();

    tWidgetLoader.SaveWidgets();
  },
  
  _OnSelectUnit: function(tEvent)
  {
    var tTarget = tEvent.target || tEvent.srcElement;
    if(tTarget && tTarget.tagName == "TD") // checking for td
    {
      // unselect old unit
      if(this.tUnit)
      {
        if (!this._CheckUnit(this.sUnit))
        {
          return;
        }

        this.tUnit.className = this.tUnit.className.replace(/ cellUnitSel/, '');
        this._SaveUnit(this.sUnit);
      }
      
      // save and select new unit
      this.tUnit = tTarget;
      this.tUnit.className += ' cellUnitSel';
      
      this.sUnit = '';

      // searching unit name by element
      for(var tIndex in this.tCells)
      {
        if(this.tCells[tIndex] == this.tUnit)
        {
          this.sUnit = tIndex;
          break;
        }
      }
      
      this._LoadUnit(this.sUnit);
    }
  },
  
  _OnConfigEnabled: function(tEvent)
  {
    var tRestrValue = this.tUnitRestrictValues[this.sUnit];

    var bEnable = this.tCtrl.enabled.GetChecked();
    if (!bEnable)
    {
      var tWidgetLoader = window.parent.webapp.Webapp.GetWidgetLoader();
      if (tWidgetLoader)
      {
        var aWidgets = tWidgetLoader.GetWidgetsByUnitPos(this.sUnit);
        if (aWidgets.length > 0)
        {
          if (!confirm(_("This unit contain one or more widgets.\nPress \'OK\' to remove them.")))
          {
            this.tCtrl.enabled.SetChecked();
            return;
          }
          this._WidgetsToRemove[this.sUnit] = aWidgets;
        }
      }
    }
    else
    {
      delete this._WidgetsToRemove[this.sUnit];
    }

    for(var tIndex in this.tCtrl)
    {
      var tCtrl = this.tCtrl[tIndex];
      if(tCtrl.Element)
      {
        if (tCtrl != this.tCtrl.enabled)
        {
          if (tIndex.substr(0, 3) == 'val')
          {
            var sCtrlName = tIndex.substr(3);
            var bCtrlEnable = bEnable && tRestrValue[sCtrlName] !== null && tRestrValue[sCtrlName] !== false;
            tCtrl.Enable(bCtrlEnable);
            tCtrl.SetText(bCtrlEnable ? tRestrValue[sCtrlName] : '');
          }
          else
          {
            tCtrl.Enable(bEnable && (tRestrValue[tIndex] !== null));
            tCtrl.SetChecked(tRestrValue[tIndex] !== null && tRestrValue[tIndex] !== false);
          }
        }
      }
    }

    if (bEnable)
    {
      this.tUnit.className += ' cellUnitActive';
    }
    else
    {
      this.tUnit.className = this.tUnit.className.replace(/ cellUnitActive/, '');
    }
  },
  
  _OnConfigCheck: function(tEvent)
  {
    var tTarget = tEvent.target || tEvent.srcElement;
    
    var nPos = tTarget.id.indexOf('_');
    if (nPos > 0)
    {
      var sCtrl = tTarget.id.substr(nPos + 1);
      var tCtrl = this.tCtrl[sCtrl];
      var tCtrlVal = this.tCtrl['val' + sCtrl];
      if (tCtrl && tCtrlVal)
      {
        tCtrlVal.Enable(tCtrl.GetChecked());
        if (tCtrlVal.Enabled())
        {
          tCtrlVal.Focus();
        }
      }
    }
  },

  _CheckUnit: function(sUnit)
  {
    for(var tIndex in this.tCtrl)
    {
      var tCtrl = this.tCtrl[tIndex];
      if(tCtrl && tCtrl.Element)
      {
        if(tCtrl != this.tCtrl.enabled && tCtrl.Enabled())
        {
          if(tIndex.substr(0, 3) == 'val')
          {
            var sCtlName = tIndex.substr(3);
            var sValue = tCtrl.GetValue();
            if (!sValue || ((sValue[0].search(/[^0-9]/) == 0) && (sCtlName != 'header')))
            {
              tCtrl.Focus();
              alert(_('Invalid value of field') + ' \"' + this.tCtrl[sCtlName].GetCaption() + '\"');
              return false;
            }
          }
        }
      }
    }
    return true;
  },
  
  _SaveUnit: function(sUnit)
  {
    var tUnitIndex = this._GetUnitIndexByName(sUnit);
    
    var bEnable = this.tCtrl.enabled.GetChecked();
    if (bEnable)
    {
      if (tUnitIndex == null)
      {
        tUnitIndex = this._tLayoutUnitsCfg.length;
      }

      if(!this._tLayoutUnitsCfg[tUnitIndex])
      {
        this._tLayoutUnitsCfg[tUnitIndex] = { body: 'div' + this.sUnit + this.sId, position: this.sUnit };
      }
      
      var tUnitProps = this._tLayoutUnitsCfg[tUnitIndex];
      
      for(var tIndex in this.tCtrl)
      {
        var tCtrl = this.tCtrl[tIndex];
        if(tCtrl.Element && tCtrl != this.tCtrl.enabled)
        {
          if(tIndex.substr(0, 3) != 'val')
          {
            var tCtrlVal = this.tCtrl['val' + tIndex];
            if (tCtrlVal)
            {
              if (tCtrlVal.Enabled())
              {
                tUnitProps[tIndex] = tCtrlVal.GetValue();
              }
              else
              {
                tUnitProps[tIndex] = false;
              }
            }
            else
            {
              tUnitProps[tIndex] = tCtrl.GetValue();
            }
          }
        }
      }
    }
    else
    {
      if(tUnitIndex && this._tLayoutUnitsCfg[tUnitIndex])
      {
        this._tLayoutUnitsCfg.splice(tUnitIndex, 1);
      }
    }
  },
  
  _LoadUnit: function(sUnit)
  {
    var tUnitIndex = this._GetUnitIndexByName(sUnit);
    var tUnit = tUnitIndex ? this._tLayoutUnitsCfg[tUnitIndex] : null;

    // resetting all properties controls
    for(var tIndex in this.tCtrl)
    {
      var tCtrl = this.tCtrl[tIndex];
      if(tCtrl && tCtrl.Element)
      {
        if(tCtrl != this.tCtrl.enabled)
        {
          if(tIndex.substr(0, 3) == 'val')
          {
            tCtrl.SetValue('');
            tCtrl.Disable();
          }
          else
          {
            tCtrl.SetValue(false);
            tCtrl.Enable(tUnit != null);
          }
        }
        else
        {
          if (sUnit == 'center')
          {
            tCtrl.Enable(false);
            tCtrl.SetChecked(true);
          }
          else
          {
            tCtrl.Enable(true);
          }
        }
      }
    }

    this.tCtrl.enabled.SetValue(tUnit != null);

    // if unit already exists
    if(tUnit)
    {
      // reading unit's properties
      for(var tIndex in tUnit)
      {
        var tProp = tUnit[tIndex];
        if(typeof tProp != 'function')
        {
          var tCtrl = this.tCtrl[tIndex];
          if(tCtrl) // skipping body, position, etc..
          {
            var tCtrlVal = this.tCtrl['val' + tIndex];
            if(tCtrlVal && tProp !== false)
            {
              tCtrlVal.SetValue(tProp);
              tCtrl.SetValue(true);
              tCtrlVal.Enable(true);
            }
            else
            {
              tCtrl.SetValue(tProp);
            }
          }
        }
      }
    }

    var bEnable = this.tCtrl.enabled.GetChecked();
    var tRestrValue = this.tUnitRestrictValues[sUnit];
    for(var tIndex in this.tCtrl)
    {
      var tCtrl = this.tCtrl[tIndex];
      if(tCtrl.Element)
      {
        if (tCtrl != this.tCtrl.enabled)
        {
          if (tIndex.substr(0, 3) == 'val')
          {
            var sCtrlName = tIndex.substr(3);
            tCtrl.Enable(bEnable && (tRestrValue[sCtrlName] !== null) && this.tCtrl[sCtrlName].GetChecked());
          }
          else
          {
            tCtrl.Enable(bEnable && (tRestrValue[tIndex] !== null));
          }
        }
      }
    }
  },
  
  _GetUnitIndexByName: function(sUnit)
  {
    for(var tIndex in this._tLayoutUnitsCfg)
    {
      if(this._tLayoutUnitsCfg[tIndex].position == sUnit)
      {
        return tIndex;
      }
    }
  },

  _OnKeyDown: function(tEvent)
  {
    switch (tEvent.keyCode)
    {
      case 13: // Enter
      {
        this._OnConfirmConfigure();
        break;
      }
      
      case 27: // Escape
      {
        this._OnCancelConfigure();
        break;
      }
    }
  },
  
  SerializeData: function()
  {
    var tSizes = this.tLayout.getSizes();

    for (var itUnit in this.tProperties.Layout)
    {
      var tUnit = this.tProperties.Layout[itUnit];

      if (tUnit.width != null)
      {
        tUnit.width = tSizes[tUnit.position].w;
      }
      if (tUnit.height != null)
      {
        tUnit.height = tSizes[tUnit.position].h;
      }
    }
  }

});

