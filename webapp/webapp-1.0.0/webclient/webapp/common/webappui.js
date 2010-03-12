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
    
    if(this._tIds['id' + sControlType] == null)
    {
      this._tIds['id' + sControlType] = 0;
      return sControlType + '0';
    }
    else
    {
      return 'id' + sControlType + (++this._tIds['id' + sControlType]);
    }
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
  
  On: function(sEvent, fHandler, tScope)
  {
    addHandler(this.tElement, sEvent, fHandler.bindAsEventListener(tScope || this));
  },
  
  Show: function(bShow)
  {
    if (bShow == null)
    {
      bShow = true;
    }

    this.tElement.style.visibility = bShow ? "visible" : "hidden";
  },
  
  Hide: function()
  {
    this.Show(false);
  },
  
  AppendChild: function(tChild)
  {
    this.tElement.appendChild(tChild.tElement != null ? tChild.tElement : tChild);
  },
  
  RemoveChild: function(tChild)
  {
    this.tElement.removeChild(tChild.tElement != null ? tChild.tElement : tChild);
  }
};

//////////////////////////////////////////////////////////////////////////
// div
webapp.ui.Div = Class.create();
webapp.ui.Div.prototype.extend(new webapp.ui.Generic).extend
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
webapp.ui.Label.prototype.extend(new webapp.ui.Generic).extend
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
    return this.Element().firstNode.nodeValue;
  },
  
  SetCaption: function(sCaption)
  {
    var tNodeCaption = this.Element().firstNode;
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
webapp.ui.Edit.prototype.extend(new webapp.ui.Generic).extend
({
  sClass: 'Edit',
  
  Create: function(tParent, tOpt)
  {
    return document.createElement('input');
  },
  
  GetText: function()
  {
    return this.Element().value;
  },
  
  SetText: function(sText)
  {
    return this.Element().value = sText;
  }
});

//////////////////////////////////////////////////////////////////////////
// Selector
webapp.ui.Select = Class.create();
webapp.ui.Select.prototype.extend(new webapp.ui.Generic).extend
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
    
    if (tParseOpts != null && tParseOpts.sKey != null && tParseOpts.sLabel != null)
    {
      for(var nIndex = 0; nIndex < atOptions.length; ++nIndex)
      {
        var tOption = atOptions[nIndex];
        atSelOptions[nIndex] = new Option(tOption[tParseOpts.sLabel], tOption[tParseOpts.sKey]);
      }
    }
    else
    {
      for(var nIndex = 0; nIndex < atOptions.length; ++nIndex)
      {
        var tOption = atOptions[nIndex];
        atSelOptions.push(new Option(tOption[1], tOption[0]));
      }
    }
  },
  
  GetActiveItem: function()
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
  }
  
});

//////////////////////////////////////////////////////////////////////////
// Checkbox with label control
webapp.ui.LabeledCheckbox = Class.create();
webapp.ui.LabeledCheckbox.prototype.extend(new webapp.ui.Generic).extend
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
    this.tCheck.id = webapp.ui.IdGen.Gen('Checkbox');;
    tDiv.appendChild(this.tCheck);

    this.tLabel = document.createElement('label');
    this.tLabel.htmlFor = this.tCheck.id;
    this.tLabel.appendChild(document.createTextNode(tOpt.sCaption));
    tDiv.appendChild(this.tLabel);

    if (tOpt.bChecked != null)
    {
      this.SetChecked(bChecked);
    }
    
    return tDiv;
  },
  
/*  Destroy: function()
  {
    if(this.tLabel != null && this.tLabel.parentNode != null)
    {
      delete this.tLabel.parentNode.removeChild(this.tLabel);
    }

    if(this.tCheck != null && this.tCheck.parentNode != null)
    {
      delete this.tCheck.parentNode.removeChild(this.tCheck);
    }
  },*/
  
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
  }
  
});

//////////////////////////////////////////////////////////////////////////
// Table
webapp.ui.Table = Class.create();
webapp.ui.Table.prototype.extend(new webapp.ui.Generic).extend
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
  
  SetCaption: function(sCaption)
  {
    if(this.tCaptionText == null)
    {
      this.tCaption = document.createElement('caption');
      this.tTable.appendChild(this.tCaption);
      
      this.tCaptionText = document.createTextNode(sCaption)
      this.tCaption.appendChild(this.tCaptionText);    }
    else
    {
      this.tCaptionText.nodeValue = sCaption;
    }
  }
});


//////////////////////////////////////////////////////////////////////////
// Image
webapp.ui.Image = Class.create();
webapp.ui.Image.prototype.extend(new webapp.ui.Generic).extend
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
  }
});
