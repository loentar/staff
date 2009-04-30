namespace('webapp.widget');

///////////////////////////////////////////////////////////////
// class Layout

webapp.widget.Layout = Class.create();
webapp.widget.Layout.prototype.extend(webapp.widget.Widget.prototype).extend
({
  Create: function(tParent, tOpts)
  {
    this.tLayoutUnits = 
      this.tProperties.sLayout ? this.DeserializeProps(this.tProperties.sLayout) : this._GetDefaultLayoutUnits();
        
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
    this.tLayout.render();
    
    this.AddWidgetMenu(_("Configure layout"), this.Configure.bind(this));
    
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
  
  CreateControls: function(tParent, tLayoutUnits)
  {
    this.tDiv = {};
    for(var nArrIndex = 0; nArrIndex < tLayoutUnits.length; ++nArrIndex)
    {
      var tProps = tLayoutUnits[nArrIndex];
      this.tDiv[tProps.body] = new webapp.ui.Div(tParent, { sId: tProps.body });
    }
  },
  
  _GetDefaultLayoutUnits: function()
  {
    var tDefaultLayoutUnits = 
    [
      { body: 'divright' + this.sName, position: 'right', width: 515, resize: true, gutter: '5px', scroll: true },
      { body: 'divcenter' + this.sName, position: 'center', gutter: '5px 0px 5px 5px' }
    ];
    return tDefaultLayoutUnits;
  },
  
  DeserializeData: function() // записать динамические свойства в контрол
  {
    if (this.tProperties.sLayout)
    {
      this.tLayoutUnits = this.DeserializeProps(this.tProperties.sLayout);
    }
  },
  
  SerializeData: function() // получить динамические свойства из контрола
  {
    if(this.tLayoutUnits)
    {
      this.tProperties.sLayout = this.SerializeProps(this.tLayoutUnits);
    }
  },
  
  DeserializeProps: function(sProps)
  {
    var aProps = [];
    var aPropsArr = sProps.split('|');
    
    for(var nArrIndex = 0; nArrIndex < aPropsArr.length - 1; ++nArrIndex)
    {
      var tProps = {};
      var aPropsKeys = aPropsArr[nArrIndex].split(';');
      for(var nIndex = 0; nIndex < aPropsKeys.length - 1; ++nIndex)
      {
        var aPair = aPropsKeys[nIndex].split(':');
        if (aPair[1].substr(0, 1) === "'")
        {
          tProps[aPair[0]] = aPair[1].substr(1, aPair[1].length - 2);
        }
        else
        {
          tProps[aPair[0]] = aPair[1];
        }
      }
      aProps[aProps.length] = tProps;
    }
    
    return aProps;
  },
  
  SerializeProps: function(aProps)
  {
    var sProps = "";
    
    for(var nArrIndex = 0; nArrIndex < aProps.length; ++nArrIndex)
    {
      var tProps = aProps[nArrIndex];
      for(nIndex in tProps)
      {
        var sType = typeof tProps[nIndex];
        if (sType == "string")
        {
          sProps += nIndex + ':\'' + tProps[nIndex] + '\';';
        }
        else
        if (sType == "number" || sType == "boolean")
        {
          sProps += nIndex + ':' + tProps[nIndex] + ';';
        }
      }
      sProps += "|";
    }
    
    return sProps;
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
  
  _OnBeforeResize: function()
  {
    if (this.tParentElem.parentNode.parentNode === document.body)
    {
      this.tLayout.set('width', YAHOO.util.Dom.getClientWidth());
      this.tLayout.set('height', YAHOO.util.Dom.getClientHeight() - 26);
    }
  },
  
  Configure: function()
  {
    // TODO: Implement
    alert("configure");
  }
});
