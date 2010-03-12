namespace('widget');

///////////////////////////////////////////////////////////////
// class Widget

widget.Widget = Class.create();
widget.Widget.prototype = 
{
  //! constructor
  initialize: function(stWidget, tOptions)
  {
    this.tOptions = tOptions;

    this.Deserialize(stWidget, true);

    if(this.Create)
    {
      var tParent;
      
      if(this.tProperties.sParent)
      {
        tParent = document.getElementById(this.tProperties.sParent);
        delete this.tProperties.sParent; // compat
      }
      else
      {
        tParent = this.tOptions.pParentElement;
      }
    
      if(tParent == null)
      {
         throw Error("Невозможно восстановить " + this.sName 
            + "; <br>Отсутствует родительский элемент: " + this.tProperties.sParent);
      }

      this.tWidgetParent = tParent;
      
      var tFrameOpts = this.GetFrameOptions != null ? this.GetFrameOptions() : {};
      
      if(tFrameOpts.bNoFrame !== true)
      {
        if(tFrameOpts.sCaption == null)
        {
          tFrameOpts.sCaption = tOptions.sDescription;
        }
      
        this._tWidgetFrame = new webapp.view.WidgetFrame(tParent, tFrameOpts);
        this._tWidgetFrame.On('close', this._OnClose, this, stWidget);
        tParent = this._tWidgetFrame.GetBodyContainer();
      }

      this.tOptions.pParentElement = tParent.Element != null ? tParent.Element() : tParent; // compat

      this.tElement = this.Create(tParent, tOptions);
/*      if (tElement != null) // compat
      {
        this.pElement = tElement;
      }*/
    }

    if (this.DeserializeData)
    {
      this.DeserializeData();
    }

    this.SetModify(false);
    this.tEvents = {};
  },
  
  //! deinitializer
  destroy: function()
  {
    if(this.Destroy)
    {
      this.Destroy();
    }

    // remove associated menu item
    if (this.pMenuItem != null && this.pMenuItem.destroy) // assert yui
    {
      this.pMenuItem.destroy();
    }
    
    if (this._tWidgetFrame != null)
    {
      this._tWidgetFrame.destroy();
    }
    
    if(this.pElement != null)
    {
      // yui
      if(this.pElement.destroy) // assert yui
      {
        this.pElement.destroy();
      }
      else
      { // DOM
        if(this.pElement.parentNode && this.pElement.parentNode.removeChild)
        {
          this.pElement.parentNode.removeChild(this.pElement);
        }
      }
    }
  },
  
  //! restore widget state
  Deserialize: function(stWidget, bDoNotDeserializeData)
  {
    this.sName = stWidget.sName;
    this.sClass = stWidget.sClass;
    if(this.tProperties != null)
    {
      delete this.tProperties;
    }
    this.tProperties = {};
    for(var i = 0; i < stWidget.lsProperties.length; ++i)
    {
      this.tProperties[stWidget.lsProperties[i].sName] = stWidget.lsProperties[i].tValue;
    }

    // deserialize data by default
    if (bDoNotDeserializeData != true && this.DeserializeData)
    {
      this.DeserializeData();
    }
  },
  
  //! store widget state
  Serialize: function()
  {
    if (this.SerializeData)
    {
      this.SerializeData();
    }

    var stWidget = 
    {
      sName: this.sName,
      sClass: this.sClass,
      lsProperties: new Array()
    };

    this.tProperties.sParent = this.tWidgetParent.id;
    for(tProperty in this.tProperties)
    {
      if(typeof this.tProperties[tProperty] != "function")
      {
        var tCurrProperty =
        {
          sName: tProperty,
          tValue: this.tProperties[tProperty]
        };
        
        stWidget.lsProperties.push(tCurrProperty);
      }
    }
      
    return stWidget;
  },
  
  //! set modification flag
  SetModify: function(bModified)
  {
    if(bModified == null)
    {
      this.bModified = true;
    }
    else
    {
      this.bModified = bModified;
    }
  },
  
  //! get modification flag
  GetModify: function()
  {
    if(this.bModified)
    {
      return true;
    }
    
    if(this.GetModifyData)
    {
      this.bModified = this.GetModifyData();
      return this.bModified;
    }
    
    return false;
  },
  
  //! add menu item for widget
  AddWidgetMenu: function(sLabel, pFunction)
  {
    this.pMenuItem = this.tOptions.pWidgetMenu.addItem({ text: sLabel }, 1);
    this.pMenuItem.subscribe("click", pFunction);
  },
  
  //! dump widget properties for debug
  Dump: function()
  {
    var sResult = "";
    var stWidget = this.Serialize();
    
    sResult += "sName: " + stWidget.sName + "\n";
    sResult += "sClass: " + stWidget.sClass + "\n";
    sResult += " -- properties --\n";
    for(var i = 0; i < stWidget.lsProperties.length; ++i)
    {
      sResult += stWidget.lsProperties[i].sName + ":" + stWidget.lsProperties[i].tValue + "\n";
    }
    
    return sResult;
  },

  On: function(sEvent, fHandler, tScope, tObject)
  {
    this.tEvents[sEvent] = 
    {
      fHandler: tScope != null ? fHandler.bind(tScope) : fHandler,
      tObject: tObject
    };
  },
  
  FireEvent: function(sEvent)
  {
    var tEventHandler = this.tEvents[sEvent];
    if (tEventHandler != null)
    {
      tEventHandler.fHandler(tEventHandler.tObject);
    }
  },
  
  _OnClose: function()
  {
    this.FireEvent("close");
  }
};
