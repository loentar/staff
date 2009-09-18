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

namespace('webapp.ui');

webapp.ui.IdGen = 
{
  _tIds: {},
  
  Gen: function(sControlType)
  {
    if (!sControlType)
    {
      sControlType = 'Generic';
    }
    
    var sId = 'id_' + sControlType.replace(/\./g, '_') + '_';
    
    if (!this._tIds[sId])
    {
      this._tIds[sId] = 0;
    }

    var sResult;
    do
    {
      sResult = sId + (++this._tIds[sId]);
    }
    while (document.getElementById(sResult));

    return sResult;
  }
};

//////////////////////////////////////////////////////////////////////////
// generic control
webapp.ui.Generic = Class.create();
webapp.ui.Generic.prototype = 
{
  initialize: function(tParent, tOptions)
  {
    if (this.Create)
    {
      var tOpt = tOptions == null ? {} : tOptions;
      this.tParent = tParent;
      this.tElement = this.Create(tParent, tOpt);
      
      // id
      if (tOpt.sId == null)
      {
        if (this.sClass == null)
        {
          this.sClass = 'Generic';
        }
        
        tOpt.sId = webapp.ui.IdGen.Gen(this.sClass);
      }
      
      this.tElement.id = tOpt.sId;
      
      // css class
      if (tOpt.sClass != null)
      {
        this.tElement.className = tOpt.sClass;
      }
      
      if (tParent != null)
      {
        if (tParent.Element != null)
        {
          tParent.Element().appendChild(this.tElement);
        }
        else
        if (tParent.appendChild != null)
        {
          tParent.appendChild(this.tElement);
        }
      }
    }
  },
  
  destroy: function()
  {
    if (this.Destroy)
    {
      this.Destroy();
    }

    if(this.tElement != null && this.tElement.parentNode)
    {
      this.tElement.parentNode.removeChild(this.tElement);
      delete this.tElement;
    }
  },
  
  Element: function()
  {
    return this.tElement;
  },
  
  Focus: function()
  {
    this.tElement.focus();
  },
  
  On: function(sEvent, fnHandler, tScope, tObject)
  {
    var tSelf = this;

    addHandler
    (
      this.tElement,
      sEvent,
      function(tEvent)
      {
        fnHandler.call(tScope || tSelf, tEvent || window.event, tObject);
      }
    );
  },
  
  Fire: function(sEvent)
  {
    fireEvent(this.tElement, sEvent);
  },
  
  Show: function(bShow)
  {
    this.tElement.style.visibility = bShow !== false ? "visible" : "hidden";
  },
  
  Hide: function()
  {
    this.Show(false);
  },
  
  Visible: function()
  {
    return this.tElement.style.visibility == "visible";
  },
  
  Enable: function(bEnable)
  {
    this.tElement.disabled = bEnable === false;
  },
  
  Disable: function()
  {
    this.Enable(false);
  },

  Enabled: function()
  {
    return !this.tElement.disabled;
  },
  
  GetLeft: function()
  {
    return this.tElement.offsetLeft;
  },
  
  GetTop: function()
  {
    return this.tElement.offsetTop;
  },
  
  SetLeft: function(nLeft)
  {
    return this.tElement.style.left = nLeft;
  },
  
  SetTop: function(nTop)
  {
    return this.tElement.style.top = nTop;
  },
  
  GetWidth: function()
  {
    return this.tElement.offsetWidth;
  },
  
  GetHeight: function()
  {
    return this.tElement.offsetHeight;
  },
  
  SetWidth: function(nWidth)
  {
    return this.tElement.style.width = nWidth;
  },
  
  SetHeight: function(nHeight)
  {
    return this.tElement.style.height = nHeight;
  },
  
  GetRight: function()
  {
    return this.tElement.offsetLeft + this.tElement.offsetWidth;
  },
  
  GetBottom: function()
  {
    return this.tElement.offsetTop + this.tElement.offsetHeight;
  },
  
  SetRight: function(nRight)
  {
    return this.tElement.style.width = nRight - this.tElement.offsetLeft;
  },
  
  SetBottom: function(nBottom)
  {
    return this.tElement.style.height = nBottom - this.tElement.offsetTop;
  },
  
  GetCursor: function()
  {
    return this.tElement.style.cursor;
  },
  
  SetCursor: function(sCursor)
  {
    this.tElement.style.cursor = sCursor;
  },
  
  AppendChild: function(tChild)
  {
    this.tElement.appendChild(tChild.tElement != null ? tChild.tElement : tChild);
  },
  
  RemoveChild: function(tChild)
  {
    this.tElement.removeChild(tChild.tElement != null ? tChild.tElement : tChild);
  },
  
  GetValue: function()
  {
    return '';
  },
  
  SetValue: function(tValue)
  {
  }
};

//////////////////////////////////////////////////////////////////////////
// div
webapp.ui.Div = Class.create();
webapp.ui.Div.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'Div',
  
  Create: function(tParent, tOpt)
  {
    return document.createElement('div');
  }
});

//////////////////////////////////////////////////////////////////////////
// Label
webapp.ui.Label = Class.create();
webapp.ui.Label.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'Label',
  
  Create: function(tParent, tOpt)
  {
    var tLabel = document.createElement('label');
    
    if (tOpt != null)
    {
      if (tOpt.sCaption != null)
      {
        tLabel.appendChild(document.createTextNode(tOpt.sCaption));
      }
    }

    return tLabel;
  },
  
  GetCaption: function()
  {
    return this.Element().firstChild.nodeValue;
  },
  
  SetCaption: function(sCaption)
  {
    var tNodeCaption = this.Element().firstChild;
    if (tNodeCaption != null)
    {
      tNodeCaption.nodeValue = sCaption;
    }
    else
    {
      this.AppendChild(document.createTextNode(sCaption));
    }
  },
  
  SetBuddy: function(tBuddy)
  {
    if (typeof tBuddy == 'string')
    {
      this.Element().htmlFor = tBuddy;
    }
    else
    if (typeof tBuddy == 'object')
    {
      var tElement = tBuddy.Element != null ? tBuddy.Element() : tBuddy;
      if (tElement.id == null)
      {
        tElement.id = webapp.ui.IdGen.Gen();
      }
      
      this.Element().htmlFor = tElement.id;
    }
    else
    {
      throw Error('Unable to set buddy');
    }
  }
});

//////////////////////////////////////////////////////////////////////////
// Editor
webapp.ui.Edit = Class.create();
webapp.ui.Edit.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'Edit',
  
  Create: function(tParent, tOpt)
  {
    var tInput = document.createElement('input');
    
    if (tOpt.bIsPassword === true)
    {
      tInput.type = "password";
    }
    return tInput;
  },
  
  GetText: function()
  {
    return this.Element().value;
  },
  
  SetText: function(sText)
  {
    return this.Element().value = sText;
  },
  
  GetValue: function()
  {
    return this.GetText();
  },
  
  SetValue: function(tValue)
  {
    this.SetText(tValue);
  }
});

//////////////////////////////////////////////////////////////////////////
// Button
webapp.ui.Button = Class.create();
webapp.ui.Button.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'Button',
  
  Create: function(tParent, tOpt)
  {
    var tInput = document.createElement('button');
    
    tInput.value = tOpt.sCaption || '';

    return tInput;
  },
  
  GetCaption: function()
  {
    return this.Element().value;
  },
  
  SetCaption: function(sCaption)
  {
    return this.Element().value = sCaption;
  },
  
  GetValue: function()
  {
    return this.GetCaption();
  },
  
  SetValue: function(tValue)
  {
    this.SetCaption(tValue);
  }
});

//////////////////////////////////////////////////////////////////////////
// Selector
webapp.ui.Select = Class.create();
webapp.ui.Select.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'Select',
  
  Create: function(tParent, tOpt)
  {
    return document.createElement('select');
  },
  
  GetItems: function(tWriteOpts)
  {
    var atSelOptions = this.Element().options;
    
    if (!tWriteOpts)
    {
      tWriteOpts = this.tParseOpts;
    }
    
    if (tWriteOpts != null && tWriteOpts.sKey != null && tWriteOpts.sLabel != null)
    {
      var atOptions = [];
      for(var nIndex = 0; nIndex < atSelOptions.length; ++nIndex)
      {
        var tSelOption = atSelOptions[nIndex];
        var tOption = {};
        tOption[tWriteOpts.sLabel] = tSelOption.text;
        tOption[tWriteOpts.sKey] = tSelOption.value;
        atOptions.push(tOption);
      }
      return atOptions;
    }
    else
    {
      if (this.tParseOpts.bObjProps === true)
      {
        var tOptions = {};
        for(var nIndex = 0; nIndex < atSelOptions.length; ++nIndex)
        {
          var tSelOption = atSelOptions[nIndex];
          tOptions[tSelOption.value] = tSelOption.text;
        }
        return tOptions;
      }
      else
      {
        var atOptions = [];
        if (this.tParseOpts.bArrIndex)
        {
          for(var nIndex = 0; nIndex < atSelOptions.length; ++nIndex)
          {
            var tSelOption = atSelOptions[nIndex];
            atOptions[tSelOption.value] = tSelOption.text;
          }
        }
        else
        {
          for(var nIndex = 0; nIndex < atSelOptions.length; ++nIndex)
          {
            var tSelOption = atSelOptions[nIndex];
            var atOption = new Array();
            atOption.push(tSelOption.text);
            atOption.push(tSelOption.value);
            atOptions.push(atOption);
          }
        }
        return atOptions;
      }
    }
  },
  
  SetItems: function(atOptions, tParseOpts)
  {
    var atSelOptions = this.Element().options;
    atSelOptions.length = 0;

    if (atOptions != null)
    {
      if (tParseOpts != null && tParseOpts.sKey != null && tParseOpts.sLabel != null)
      {
        var nOptIndex = 0;
        for(var nIndex in atOptions)
        {
          var tOption = atOptions[nIndex];
          if (tOption[tParseOpts.sKey] != null && tOption[tParseOpts.sLabel] != null &&
            (tParseOpts.fnFilter == null || tParseOpts.fnFilter(tOption, tParseOpts.tObj)))
          {
            atSelOptions[nOptIndex] = new Option(tParseOpts.bTranslate ? _(tOption[tParseOpts.sLabel]) : tOption[tParseOpts.sLabel], tOption[tParseOpts.sKey]);
            ++nOptIndex;
          }
        }
        this.tParseOpts = tParseOpts;
      }
      else
      {
        if (atOptions instanceof Array)
        {
          var nOptIndex = 0;
          if(atOptions[0] instanceof Array)
          { // [ [ 1, "item1"], [ 2, "item2"], ... -> key - 1st sub array value, value - 2nd
            for(var nIndex in atOptions)
            {
              var tOption = atOptions[nIndex];
              if (tOption[0] != null && tOption[1] != null &&
                (tParseOpts == null || tParseOpts.fnFilter == null || tParseOpts.fnFilter(tOption, tParseOpts.tObj)))
              {
                atSelOptions[nOptIndex] = new Option(tParseOpts.bTranslate ? _(tOption[1]) : tOption[1], tOption[0]);
                ++nOptIndex;
              }
            }
          }
          else
          { // [ "item1", "item2", ... ] -> key by array index
            for(var nIndex in atOptions)
            {
              var tOption = atOptions[nIndex];
              if (tOption != null && typeof tOption != 'function' && 
                   (tParseOpts == null || tParseOpts.fnFilter == null || 
                      tParseOpts.fnFilter({ sKey: tIndex, sLabel: tOption }, tParseOpts.tObj)))
              {
                atSelOptions[nOptIndex] = new Option(tParseOpts.bTranslate ? _(tOption) : tOption, nIndex);
                ++nOptIndex;
              }
            }
          }
          this.tParseOpts = { bArrIndex: true };
        }
        else
        {
          var nOptIndex = 0;
          for(var tIndex in atOptions)
          {
            var tOption = atOptions[tIndex];
            if (tOption != null && typeof tOption != 'function' &&
              (tParseOpts == null || tParseOpts.fnFilter == null || 
                tParseOpts.fnFilter({ sKey: tIndex, sLabel: tOption }, tParseOpts.tObj)))
            {
              atSelOptions[nOptIndex] = new Option(tParseOpts.bTranslate ? _(tOption.toString()) : tOption.toString(), tIndex);
              ++nOptIndex;
            }
          }
          this.tParseOpts = { bObjProps: true };
        }
      }
      
      atSelOptions.selectedIndex = 0;
    }
  },
  
  GetActiveItem: function(tParseOpts)
  {
    var tActiveItem = this.GetActiveItemElement();
    var tOpts = tParseOpts || this.tParseOpts;
    if(tOpts && tOpts.sKey && tOpts.sLabel)
    {
      var tResult = {};
      tResult[tOpts.sKey] = tActiveItem.value;
      tResult[tOpts.sLabel] = tActiveItem.text;
      return tResult;
    }
    else
    {
      if (this.tParseOpts.bObjProps === true)
      {
        var tResult = {};
        tResult.sKey = tActiveItem.value;
        tResult.sLabel = tActiveItem.text;
        return tResult;
      }
      else
      {
        var aResult = [ tActiveItem.value, tActiveItem.text ];
        return aResult;
      }
    }
  },
  
  GetActiveItemElement: function()
  {
    var nIndex = this.Element().selectedIndex;
    if (nIndex != -1)
    {
      return this.Element()[nIndex];
    }
  },
  
  GetActiveItemIndex: function()
  {
    return this.Element().selectedIndex;
  },

  GetActiveItemId: function()
  {
    return this.Element()[this.Element().selectedIndex].value;
  },
  
  GetActiveItemLabel: function()
  {
    return this.Element()[this.Element().selectedIndex].text;
  },
  
  SetActiveItemByIndex: function(nIndex)
  {
    this.Element().selectedIndex = nIndex;
  },

  SetActiveItemById: function(nId)
  {
    var atSelOptions = this.Element().options;
    for(var nIndex = 0; nIndex < atSelOptions.length; ++nIndex)
    {
      if(atSelOptions[nIndex].value == nId)
      {
        atSelOptions.selectedIndex = nIndex;
        return true;
      }
    }
    
    return false;
  },
  
  SetActiveItemByLabel: function(sLabel)
  {
    var atSelOptions = this.Element().options;
    for(var nIndex = 0; nIndex < atSelOptions.length; ++nIndex)
    {
      if(atSelOptions[nIndex].text == sLabel)
      {
        atSelOptions.selectedIndex = nIndex;
        return true;
      }
    }

    return false;
  },

  GetItemIdByIndex: function(nIndex)
  {
    return this.Element()[nIndex].value;
  },

  GetItemLabelByIndex: function(nIndex)
  {
    return this.Element()[nIndex].text;
  },

  AddItem: function(tId, sLabel, bSelect)
  {
    var atSelOptions = this.Element().options;
    var nOptIndex = atSelOptions.length;
    var tNewOption = new Option(sLabel, tId);
    atSelOptions[nOptIndex] = tNewOption;
    if (bSelect)
    {
      this.Element().selectedIndex = tNewOption.index;
    }
  },

  RemoveItemByIndex: function(nIndex)
  {
    var tItemElem = this.Element()[nIndex];
    if (tItemElem)
    {
      tItemElem.parentNode.removeChild(tItemElem);
    }
  },
  
  RemoveItemById: function(nIndex)
  {
    var atSelOptions = this.Element().options;
    for (var nIndex = 0; nIndex < atSelOptions.length; ++nIndex)
    {
      var tOption = atSelOptions[nIndex];
      if (tOption.value == nId)
      {
        tOption.parentNode.removeChild(tOption);
        break;
      }
    }
  },
  
  RemoveActiveItem: function()
  {
    var tActiveItemElem = this.GetActiveItemElement();
    if (tActiveItemElem)
    {
      tActiveItemElem.parentNode.removeChild(tActiveItemElem);
    }
  },
  
  MoveActiveItem: function(tSelect)
  {
    var tSelectElem = tSelect.Element ? tSelect.Element() : tSelect;
    var nIndex = this.Element().selectedIndex;

    var tActiveItemElem = this.GetActiveItemElement();
    if (tActiveItemElem)
    {
      tSelectElem.options[tSelectElem.options.length] = new Option(tActiveItemElem.text, tActiveItemElem.value);
      tActiveItemElem.parentNode.removeChild(tActiveItemElem);
    }
    
    this.Element().selectedIndex = nIndex;
  },
  
  GetItemCount: function()
  {
    return this.Element().options.length;
  },
  
  SetSize: function(nSize)
  {
    this.Element().size = nSize;
  },
  
  GetSize: function(nSize)
  {
    return this.Element().size;
  },
  
  GetValue: function()
  {
    return this.GetActiveItemId();
  },
  
  SetValue: function(tValue)
  {
    return this.SetActiveItemById(tValue);
  }
  
});

//////////////////////////////////////////////////////////////////////////
// Checkbox with label control
webapp.ui.LabeledCheckbox = Class.create();
webapp.ui.LabeledCheckbox.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'LabeledCheckbox',
  
  Create: function(tParent, tOpt)
  {
    if (tOpt.sCaption == null)
    {
      tOpt.sCaption = '';
    }
    
    var tDiv = document.createElement('div');
    
    this.tCheck = document.createElement('input');
    this.tCheck.type = 'checkbox';
    this.tCheck.id = tOpt.sCheckId || webapp.ui.IdGen.Gen('Checkbox');;
    this.tCheck.style.verticalAlign = 'bottom';
    tDiv.appendChild(this.tCheck);

    this.tLabel = document.createElement('label');
    this.tLabel.htmlFor = this.tCheck.id;
    this.tLabel.appendChild(document.createTextNode(tOpt.sCaption));
    this.tLabel.style.verticalAlign = 'bottom';
    tDiv.appendChild(this.tLabel);

    if (tOpt.bChecked != null)
    {
      this.SetChecked(bChecked);
    }
    
    return tDiv;
  },
  
  GetCaption: function()
  {
    return this.tLabel.firstChild.nodeValue;
  },
  
  SetCaption: function(sCaption)
  {
    this.tLabel.firstChild.nodeValue = sCaption;
  },

  SetChecked: function(bChecked)
  {
    if(bChecked == null)
    {
      bChecked = true;
    }
    
    if (typeof bChecked == 'string')
    {
      this.tCheck.checked = (bChecked.toLowerCase() == 'true');
    }
    else
    {
      this.tCheck.checked = bChecked;
    }
  },
  
  GetChecked: function()
  {
    return this.tCheck.checked;
  },
  
  On: function(sEvent, fnHandler, tScope)
  {
    addHandler(this.tCheck, sEvent, fnHandler.bindAsEventListener(tScope || this));
  },
  
  Fire: function(sEvent)
  {
    fireEvent(this.tCheck, sEvent);
  },
  
  GetValue: function()
  {
    return this.GetChecked();
  },
  
  SetValue: function(tValue)
  {
    this.SetChecked(tValue);
  },
  
  Enable: function(bEnable)
  {
    this.tCheck.disabled = bEnable === false;
    this.tLabel.disabled = bEnable === false;
  },
  
  Enabled: function()
  {
    return !this.tCheck.disabled;
  }
});

//////////////////////////////////////////////////////////////////////////
// Table
webapp.ui.Table = Class.create();
webapp.ui.Table.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'Table',
  
  Create: function(tParent, tOpt)
  {
    this.tTable = document.createElement('table');
    
    this.tTBody = document.createElement('tbody');
    this.tTable.appendChild(this.tTBody);
    
    if (tOpt.sCaption != null)
    {
      this.SetCaption(tOpt.sCaption);
    }
    
    return this.tTable;
  },
  
  Head: function()
  {
    if (this.tTHeader == null)
    {
      this.tTHead = document.createElement('thead');
      this.tTable.appendChild(this.tTHead);
    }
    
    return this.tTHead;
  },
  
  AddHeadRow: function(tOpt)
  {
    var tTr = document.createElement('tr');
    if (tOpt != null)
    {
      if (tOpt.sClass != null)
      {
        tTr.className = tOpt.sClass;
      }
    }
    this.Head().appendChild(tTr);
    return tTr;
  },
  
  AddRow: function(tOpt)
  {
    var tTr = document.createElement('tr');
    if (tOpt != null)
    {
      if (tOpt.sClass != null)
      {
        tTr.className = tOpt.sClass;
      }
    }
    this.tTBody.appendChild(tTr);
    return tTr;
  },
  
  AddCell: function(tTr, tOpt)
  {
    var tTd = document.createElement('td');
    tTr.appendChild(tTd);
    
    if (tOpt != null)
    {
      if (tOpt.tContent != null)
      {
        if (typeof tOpt.tContent == 'string')
        {
          tTd.appendChild(document.createTextNode(tOpt.tContent));
        }
        else
        if (typeof tOpt.tContent == 'object')
        {
          if (tContent.Element == null)
          {
            tTd.appendChild(tOpt.tContent);
          }
          else
          {
            tTd.appendChild(tOpt.tContent.Element());
          }
        }
      }

      if (tOpt.sClass != null)
      {
        tTd.className = tOpt.sClass;
      }

      if (tOpt.nColSpan != null)
      {
        tTd.colSpan = tOpt.nColSpan;
      }

      if (tOpt.nRowSpan != null)
      {
        tTd.rowSpan = tOpt.nRowSpan;
      }
    }
    
    return tTd;
  },
  
  AddRowCell: function(tOpt)
  {
    return this.AddCell(this.AddRow(), tOpt);
  },
  
  SetCaption: function(sCaption)
  {
    if(this.tCaptionText == null)
    {
      this.tCaption = document.createElement('caption');
      this.tTable.appendChild(this.tCaption);
      
      this.tCaptionText = document.createTextNode(sCaption)
      this.tCaption.appendChild(this.tCaptionText);    }
    else
    {
      this.tCaptionText.nodeValue = sCaption;
    }
  },

  RemoveAllRows: function()
  {
    while(this.tTBody.lastChild)
    {
      this.tTBody.removeChild(this.tTBody.lastChild);
    }
  }
});


//////////////////////////////////////////////////////////////////////////
// Image
webapp.ui.Image = Class.create();
webapp.ui.Image.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'Image',
  
  Create: function(tParent, tOpt)
  {
    var tImage = new Image();
    
    if (tOpt != null)
    {
      if (tOpt.sSrc != null)
      {
        tImage.src = tOpt.sSrc;
      }
    }
    
    return tImage;
  },
  
  SetSrc: function(sUrl)
  {
    this.Element().src = sUrl;
  },
  
  GetSrc: function()
  {
    return this.Element().src;
  },
  
  GetValue: function()
  {
    return this.GetSrc();
  },
  
  SetValue: function(tValue)
  {
    this.SetSrc(tValue);
  }
});

//////////////////////////////////////////////////////////////////////////
// Link
webapp.ui.Link = Class.create();
webapp.ui.Link.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'Link',
  
  Create: function(tParent, tOpt)
  {
    var tElem = document.createElement('a');
    tElem.href = tOpt.sUrl || '#';
    tElem.appendChild(document.createTextNode(tOpt.sCaption || '...'));
    return tElem;
  }
});

//////////////////////////////////////////////////////////////////////////
// Horizontal rule
webapp.ui.Hr = Class.create();
webapp.ui.Hr.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'Hr',
  
  Create: function(tParent, tOpt)
  {
    return document.createElement('hr');
  }
});

//////////////////////////////////////////////////////////////////////////
// Line break
webapp.ui.Br = Class.create();
webapp.ui.Br.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'Br',
  
  Create: function(tParent, tOpt)
  {
    return document.createElement('br');
  }
});

//////////////////////////////////////////////////////////////////////////
// Paragraph
webapp.ui.P = Class.create();
webapp.ui.P.prototype.extend(webapp.ui.Generic.prototype).extend
({
  sClass: 'P',
  
  Create: function(tParent, tOpt)
  {
    return document.createElement('p');
  }
});

