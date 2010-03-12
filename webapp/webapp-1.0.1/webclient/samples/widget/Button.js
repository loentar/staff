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

namespace('widget');

///////////////////////////////////////////////////////////////
// class Button: example widget

widget.Button = Class.create();
widget.Button.prototype.extend(webapp.widget.Widget.prototype).extend
({
  Create: function(tParentElem)
  {
    if(!this.tProperties.sLabel)
    {
      this.tProperties.sLabel = this.sId;
    }
    
    // creating YUI button
    this.tButton = new YAHOO.widget.Button
    ({
       id: this.sId,
       label: this.tProperties.sLabel,
       container: tParentElem
    });
    
    // set "onclick" handler
    this.tButton.on('click', this._OnButtonClick.bind(this));
     
    // adding simple widget menu
    this.AddWidgetMenu(this.sId, this._OnMenuClick.bind(this));
  },
  
  _OnButtonClick: function()
  {
    webapp.view.MessageBox.ShowMessage( _('Button pressed') + ' ' + this.tProperties.sLabel, 'info');
  },

  _OnMenuClick: function()
  {
    webapp.view.MessageBox.ShowMessage(_('Menu item selected for button') + ' ' + this.tProperties.sLabel, 'info');
  },
  
  DeserializeData: function() // wirte dynamic data/update static
  {
    if(this.tButton != null)
    {
      this.tButton.set('id', this.sId);
      this.tButton.set('label', this.tProperties.sLabel);
    }
  },
  
  SerializeData: function() // get data from control
  {
  },
  
  Destroy: function()
  {
  },
  
  IsMultiple: function()
  {
    return true;
  },
  
  Configure: function()
  {
    var sResult = prompt(_('Enter new button label'), this.tProperties.sLabel);
    if (sResult)
    {
      this.tProperties.sLabel = sResult;
      this.tButton.set('label', this.tProperties.sLabel);
      this.SetModify();
    }
  }
});
