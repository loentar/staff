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
// class Widget

webapp.widget.Widget = Class.create();
webapp.widget.Widget.prototype.extend(webapp.Event.prototype).extend
({
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
      }
      else
      {
        tParent = this.tOptions.tParent.Element ? this.tOptions.tParent.Element() : this.tOptions.tParent;
      }
    
      if(tParent == null)
      {
         throw Error(_('Cannot load') + " " + this.sId
            + "; <br>" + _('Parent element is missing') + ": " + this.tProperties.sParent);
      }

      this.tWidgetParent = tParent;
      
      var tFrameOpts = this.GetFrameOptions != null ? this.GetFrameOptions() : {};
      
      if(tFrameOpts.bNoFrame !== true)
      {
        if(tFrameOpts.sCaption == null)
        {
          tFrameOpts.sCaption = tOptions.sDescr;
        }
        
        if (typeof this.Configure == 'function')
        {
          tFrameOpts.bConfigure = true;
        }
        
        tFrameOpts.bCollapsed = this.tProperties.bCollapsed;
      
        this._tWidgetFrame = new webapp.view.WidgetFrame(tParent, tFrameOpts);
        this._tWidgetFrame.On('close', this._OnClose, this, stWidget);
        if (typeof this.Configure == 'function')
        {
          this._tWidgetFrame.On('configure', this.Configure, this, stWidget);
        }

        tParent = this._tWidgetFrame.GetBodyContainer().Element();
      }

      this.tOptions.pParentElement = tParent; // compat

      this.tElement = this.Create(tParent, tOptions);
    }

    if (this.DeserializeData)
    {
      this.DeserializeData();
    }

    this.SetModify(false);
  },
  
  //! deinitializer
  destroy: function()
  {
    if(this.Destroy)
    {
      this.Destroy();
    }

    // remove associated menu item
    if (this.tMenuItem != null && this.tMenuItem.destroy) // assert yui
    {
      this.tMenuItem.destroy();
    }
    
    if (this._tWidgetFrame != null)
    {
      this._tWidgetFrame.destroy();
    }
    
    if(this.tElement != null)
    {
      // yui
      if(this.tElement.destroy) // assert yui & webapp.ui
      {
        this.tElement.destroy();
      }
      else
      { // DOM
        if(this.tElement.parentNode && this.tElement.parentNode.removeChild)
        {
          this.tElement.parentNode.removeChild(this.tElement);
        }
      }
    }
  },
  
  //! restore widget state
  Deserialize: function(stWidget, bDoNotDeserializeData)
  {
    this.sId = stWidget.sId;
    this.sClass = stWidget.sClass;
    this.tdoProps = stWidget.tdoProps;
    this.tProperties = stWidget.tdoProps.Properties;
    
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

    this.tProperties.sParent = this.tWidgetParent.id;

    var stWidget = 
    {
      sId: this.sId,
      sClass: this.sClass,
      tdoProps: this.tdoProps
    };

    return stWidget;
  },
  
  //! set modification flag
  SetModify: function(bModified)
  {
    this.bModified = bModified !== false;
  },
  
  //! get modification flag
  GetModify: function()
  {
    if (this._tWidgetFrame && (this.tProperties.bCollapsed != this._tWidgetFrame.IsCollapsed()))
    {
      this.tProperties.bCollapsed = this._tWidgetFrame.IsCollapsed();
      return true;
    }
    
    if(this.bModified !== false)
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
  AddWidgetMenu: function(sLabel, fnOnClick)
  {
    this.tMenuItem = this.tOptions.tWidgetMenu.addItem({ text: sLabel }, 0);
    this.tMenuItem.subscribe("click", fnOnClick);
  },
  
  //! dump widget properties for debug
  Dump: function()
  {
    var sResult = "";
    var stWidget = this.Serialize();
    
    sResult += "sName: " + stWidget.sId + "\n";
    sResult += "sClass: " + stWidget.sClass + "\n";
    sResult += " -- properties --\n";
    sResult += YAHOO.util.Lang.dump(stWidget.tdoProps);
    
    return sResult;
  },

  _OnClose: function()
  {
    this.FireEvent("close");
  }
});
