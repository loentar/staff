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
  
  On: function(sEvent, fnHandler, tScope)
  {
    addHandler(this.tElement, sEvent, fnHandler.bindAsEventListener(tScope || this));
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
    return this.tElement.style.width = nHeight;
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
    var atOptions = new Array();
    var atSelOptions = this.Element().options;
    
    if (tWriteOpts != null && tWriteOpts.sKey != null && tWriteOpts.sLabel != null)
    {
      for(var nIndex = 0; nIndex < atSelOptions.length; ++nIndex)
      {
        var tSelOption = atSelOptions[nIndex];
        var tOption = {};
        tOption[tWriteOpts.sLabel] = tSelOption.text;
        tOption[tWriteOpts.sKey] = tSelOption.value;
        atOptions.push(tOption);
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
            atSelOptions[nOptIndex] = new Option(_(tOption[tParseOpts.sLabel]), tOption[tParseOpts.sKey]);
            ++nOptIndex;
          }
        }
        this.tParseOpts = tParseOpts;
      }
      else
      {
        var nOptIndex = 0;
        for(var nIndex in atOptions)
        {
          var tOption = atOptions[nIndex];
          if (tOption[0] != null && tOption[1] != null &&
            (tParseOpts == null || tParseOpts.fnFilter == null || tParseOpts.fnFilter(tOption, tParseOpts.tObj)))
          {
            atSelOptions[nOptIndex] = new Option(_(tOption[1]), tOption[0]);
            ++nOptIndex;
          }
        }
      }
      
      atSelOptions.selectedIndex = 0;
    }
  },
  
  GetActiveItem: function(tParseOpts)
  {
    var tActiveItem = this.GetActiveItemElement();
    var tOpts = tParseOpts || this.tParseOpts;
    if(tOpts)
    {
      var tResult = {};
      tResult[tOpts.sKey] = tActiveItem.value;
      tResult[tOpts.sLabel] = tActiveItem.text;
      return tResult;
    }
    else
    {
      var aResult = [ tActiveItem.value, tActiveItem.text ];
      return aResult;
    }
  },
  
  GetActiveItemElement: function()
  {
    return this.Element()[this.Element().selectedIndex];
  },
  
  GetActiveItemId: function()
  {
    return this.Element()[this.Element().selectedIndex].value;
  },
  
  GetActiveItemLabel: function()
  {
    return this.Element()[this.Element().selectedIndex].text;
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
  
  AddHeadRow: function()
  {
    var tTr = document.createElement('tr');
    this.Head().appendChild(tTr);
    return tTr;
  },
  
  AddRow: function()
  {
    var tTr = document.createElement('tr');
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

