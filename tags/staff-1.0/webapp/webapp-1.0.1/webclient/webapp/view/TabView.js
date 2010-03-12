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

///////////////////////////////////////////////////////////////
// class webapp.TabView

webapp.view.TabView = Class.create();
webapp.view.TabView.tTabView = null;
webapp.view.TabView.prototype = 
{
  //! конструктор
  initialize: function(sParent)
  {
    this.tTabView = new YAHOO.widget.TabView(sParent);
  },
  
  GetActiveTab: function()
  {
    return this.tTabView.get('activeTab');
  },
  
  SetActiveTab: function(pTab)
  {
    return this.tTabView.set('activeTab', pTab);
  },
  
  GetTabId: function(tTab)
  {
    return tTab.get('id');
  },
  
  AddTab: function(sTabLabel, sTabName, tFrameContent)
  {
    var sFrameContent = "";
    var self = this;
    
    if(typeof tFrameContent == "string")
      sFrameContent = tFrameContent;
    
    var pTab = new YAHOO.widget.Tab
      ({
        label: sTabLabel + '&nbsp;&nbsp;<span class="yui-button yui-push-button" style="background: none; border-width: 0px;"><button type="button" style="background: url(webapp/assets/img/close.png) center center no-repeat; width: 9px; height: 9px; margin: 0; border-width: 0;"></button></span>',
        id: 'tab_' + sTabName,
        content: '<div id="tabdiv_' + sTabName + '" style="margin: 0; border-width: 0;">' + sFrameContent + '</div>', 
        active: true
      });

    var pButton = (pTab.getElementsByTagName('span')[0]).getElementsByTagName('button')[0];
    
    pTab.onRemoveTabEvent = new YAHOO.util.CustomEvent("removeTabEvent");

    YAHOO.util.Event.on
    (
      pButton, 'click', 
      function()
      {
        pTab.onRemoveTabEvent.fire();
        self.tTabView.removeTab(pTab);
      }
    );

    this.tTabView.addTab(pTab);
    
    var tDivView = $('tabdiv_' + sTabName);
    if(typeof tFrameContent == "object")
      tDivView.appendChild(tFrameContent);
    
    return pTab;
  },

  RemoveTab: function(tTab)
  {
    this.tTabView.removeTab(tTab);
  },
  
  GetTabContent: function(tTab)
  {
    return tTab.get('content');
  },
  
  GetTabElement: function(tTab)
  {
    return $("tabdiv_" + tTab.get('id').substr(4));
  },
  
  GetTabView: function()
  {
    return this.tTabView;
  }
  
}
