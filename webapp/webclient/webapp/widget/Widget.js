namespace('widget');

///////////////////////////////////////////////////////////////
// class Widget

widget.Widget = Class.create();
widget.Widget.sName = null;
widget.Widget.sClass = null;
widget.Widget.pElement = null;
widget.Widget.pMenuItem = null;
widget.Widget.tProperties = null;
widget.Widget.tOptions = null;
widget.Widget.bModified = false;
widget.Widget.prototype = 
{
  //! constructor
  initialize: function(stWidget, tOptions)
  {
    this.tOptions = tOptions;

    this.Deserialize(stWidget, true);

    if(this.Create)
    {
      this.Create();
    }

    if (this.DeserializeData)
    {
      this.DeserializeData();
    }

    this.SetModify(false);
  },
  
  //! deinitializer
  uninitialize: function()
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
    
    if(this.pElement != null)
    {
      // yui
      if(this.pElement.destroy) // assert yui
      {
        this.pElement.destroy();
      }
      else
      { // DOM
        if(this.pElement.parentElement && this.pElement.parentElement.removeChild)
        {
          this.pElement.parentElement.removeChild(this.pElement);
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
      eval("this.tProperties." + stWidget.lsProperties[i].sName + " = stWidget.lsProperties[i].tValue;");
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
      lsProperties : new Array()
    };

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
  }  
};
