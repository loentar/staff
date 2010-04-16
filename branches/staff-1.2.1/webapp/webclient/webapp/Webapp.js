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
// namespace webapp
namespace('webapp');

webapp.Webapp =
{
  Init: function(tOptions, fnOnComplete, tScope)
  {
    var tOpts = tOptions || {};
      
    var asYuiComponents = [
      "reset", "fonts", "reset-fonts-grids", //css
      "button", "datatable", "container", "tabview", "menu", "layout", "resize"]; // js
      
    var tYuiLoader = new YAHOO.util.YUILoader
    ({
      require: asYuiComponents,
      base: 'yui/',
      onSuccess: LoadLocale,
      onFailure: function(e)
      {
        alert("error: " + YAHOO.lang.dump(e));
      }
     });
    tYuiLoader.insert();
    
    function LoadLocale()
    {
      YAHOO.widget.DataTable.MSG_EMPTY = "";
      YAHOO.widget.DataTable.MSG_LOADING = "";

      Include
      (
        ["i18n"], 
        "webapp/common/",
        SetLocale,
        ["i18n"]
      );
    }
    
    function SetLocale()
    {
      i18n.LoadLocale('webapp', 'webapp', LoadBase);
    }

    function LoadBase()
    {
      Include
      (
        ["ws", "error", "noselect", "webappui"], 
        "webapp/common/",
        LoadStaff,
        ["WS", "GetErrorStr", "webapp.ui"]
      );
    }
    
    function LoadStaff()
    {
      Include
      (
        ["staff", "Event"],
        "webapp/common/",
        LoadView,
        ["staff", "webapp.Event"]
      );
    }
    
    function LoadView()
    {
      var asScripts = ["webapp.view.MessageBox", "webapp.view.WidgetFrame", "webapp.view.DlgList"];
      IncludeClass
      (
        asScripts,
        "",
        LoadClients
      );
    }
    
    function LoadClients()
    {
      Include
      (
        ["Login"],
        "webapp/clients/",
        LoadWidget,
        ["staff.Login"]
      );
    }
    
    function LoadWidget()
    {
      try
      {
        IncludeClass
        (
          ["webapp.widget.Widget", "webapp.widget.WidgetLoader"],
          "",
          function()
          {
            webapp.Webapp.tWidgetLoader = new webapp.widget.WidgetLoader(tOpts.tWidgetInitInfo);
            fnOnComplete.bind(tScope)();
          }
        );
      }
      catch(tError)
      {
        webapp.view.MessageBox.ShowMessage('Ошибка при загрузке системы виджетов: ' + (tError.message || tError), 'error');
      }
    }
  },
  
  GetWidgetLoader: function()
  {
    return this.tWidgetLoader;
  },
  
  GetLoginService: function()
  {
    if(this.tLoginService == null)
    {
      this.tLoginService = new staff.Login();
    }
    
    return this.tLoginService;
  },
  
  SetMainMenu: function(tMainMenu)
  {
    this.tMainMenu = tMainMenu;
  },
  
  GetMainMenu: function()
  {
    return this.tMainMenu;
  }
};
