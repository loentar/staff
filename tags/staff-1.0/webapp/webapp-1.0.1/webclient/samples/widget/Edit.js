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
// class Edit

widget.Edit = Class.create();
widget.Edit.prototype.extend(webapp.widget.Widget.prototype).extend
({
  Create: function(tParent)
  {
    this.tEdit = new webapp.ui.Edit(tParent);
    this.tEdit.SetValue(this.tProperties.sValue || this.sId);

    this.AddWidgetMenu(this.sId, this._OnMenuClicked.bind(this));
    
    return this.tEdit;
  },
  
  _OnMenuClicked: function()
  {
    this.tEdit.SetValue("Hello, world!");
  },
  
  DeserializeData: function()
  {
    if (this.tProperties.sValue)
    {
      this.tEdit.SetValue(this.tProperties.sValue);
    }
  },
  
  SerializeData: function()
  {
    this.tProperties.sValue = this.tEdit.GetValue();
  },
  
  GetModifyData: function()
  {
    return (this.tProperties.sValue != this.tEdit.GetValue());
  },
  
  IsMultiple: function()
  {
    return true;
  }
});
