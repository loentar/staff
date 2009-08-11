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


namespace('webapp.sample');
  
webapp.sample.Application =
{
  _GetMenuItems: function()
  {
    var aMainMenuItems =
    [
      {
        text: _('Actions'),
        submenu:
        {
          id: "MenuActions",
          itemdata:
          [
            [
              { text: "<small><b style=\'margin-left: -10px;\'>" + _('System') + ":</b></small>", disabled: true },
              { text: _('Logout'), url: "javascript:webapp.sample.Application.Logout();"  },
            ]
          ]
        }
      },
      {
        text: _('Widgets'),
        submenu:
        {
          id: "MenuWidgets",
          itemdata:
          [
            [
              { text: "<small><b style=\'margin-left: -10px;\'>" + _('Widget management') + ":</b></small>", disabled: true },
              { text: _('Save'), url: "javascript:webapp.Webapp.GetWidgetLoader().SaveWidgets();" },
              { text: _('Add'), url: "javascript:webapp.Webapp.GetWidgetLoader().NewWidgetDlg();" },
              { text: _('Configure'), url: "javascript:webapp.Webapp.GetWidgetLoader().ConfigureWidgetDlg();" },
              { text: _('Remove'), url: "javascript:webapp.Webapp.GetWidgetLoader().RemoveWidgetDlg();" },
            ],
            [
              { text: "<small><b style=\'margin-left: -10px;\'>" + _('Widget group management') + ":</b></small>", disabled: true },
              { text: _('Activate group'), url: "javascript:webapp.Webapp.GetWidgetLoader().ActivateWidgetGroupDlg();" },
              { text: _('Deactivate group'), url: "javascript:webapp.Webapp.GetWidgetLoader().DeactivateWidgetGroupDlg();" },
              { text: _('Create group'), url: "javascript:webapp.Webapp.GetWidgetLoader().CreateWidgetGroupDlg();" },
              { text: _('Edit group'), url: "javascript:webapp.Webapp.GetWidgetLoader().EditWidgetGroupDlg();" },
              { text: _('Remove group'), url: "javascript:webapp.Webapp.GetWidgetLoader().RemoveWidgetGroupDlg();" },
            ],
            [
              { 
                text: _('Active widgets'), 
                submenu:
                {
                  id: "MenuWidgetsConf",
                  itemdata:
                  [
                    [
                      { text: "<small><b style=\'margin-left: -10px;\'>" + _('Configuration') + ":</b></small>", disabled: true }
                    ]
                  ]
                }
              }
            ]
          ]
        }
      },
      {
        text: _('Help'),
        submenu:
        {
          id: "MenuHelp",
          itemdata:
          [
            { text: _('About') + '...', url: "javascript:webapp.sample.Application.OnAbout();" }
          ]
        }
      }
    ];
    return aMainMenuItems;
  },
  
  Logout: function()
  {
    try
    {
      webapp.Webapp.GetLoginService().Logout();
    }
    catch(tError)
    {
    }

    var sLocation = '';

    if (webapp.Env.host)
    {
      sLocation = (sLocation ? sLocation + "," : "?") + 'host=' + webapp.Env.host;
    }
    
    if (webapp.Env.port)
    {
      sLocation = (sLocation ? sLocation + "," : "?") + "port=" + webapp.Env.port;
    }
    
    if (webapp.Env.lang)
    {
      sLocation = (sLocation ? sLocation + "," : "?") + 'lang=' + webapp.Env.lang;
    }
    
    document.location = 'login.htm' + sLocation;
  },
  
  OnAbout: function()
  {
    webapp.view.MessageBox.ShowMessage(_('Example WEB-appplication for staff') + " 1.0<p/>&copy; 2008-2009", 'info');
  },

  ///--------------------------------------------------------------------------------------
  
  OnInit: function()
  {
    i18n.LoadLocale('webapp_sample', '', webapp.sample.Application.InitControls.bind(this));
  },
  
  InitControls: function()
  {
    document.title = _('WEB Application sample');
    
    // init menu
    this.tMenuBar = new YAHOO.widget.MenuBar('MainMenuBar', {itemdata: this._GetMenuItems()});
    this.tMenuBar.render(document.body);
    
    function onSubmenuShow()
    {
      if (this.id == "MenuActions") 
      {  // fix yui menu position
        YAHOO.util.Dom.setX(this.element, 0);
      }
    }
    
    this.tMenuBar.subscribe("show", onSubmenuShow);
    
    this.tDivMain = new webapp.ui.Div(document.body, { sId: "divMain" });
    addHandler(document.body, 'contextmenu', selectHandler);
    
    webapp.Webapp.GetWidgetLoader().Init
    (
      {
        tParent: this.tDivMain,
        tWidgetMenu: this.tMenuBar.getSubmenus()[1].getSubmenus()[0],
        tMainMenu: this.tMenuBar,
        sProfile: webapp.Env.profile
      }
    );

    var tDivLoading = $('divLoading');
    tDivLoading.parentNode.removeChild(tDivLoading);
  },

  Init: function()
  {
    IncludeCss('assets/pages/Main');
    try
    {
      webapp.Webapp.Init
      (
        {
          bEnableWidgets: true
        },
        webapp.sample.Application.OnInit,
        webapp.sample.Application
      );
    }
    catch(tError)
    {
      var sMessage = tError.message || tError.text || tError || _('Unknown error');
      try
      {
        webapp.view.MessageBox.ShowMessage(_('Error while initializing application') + ': '+ sMessage, 'error');
      }
      catch(tIgnoredError)
      {
        alert( _('Error while initializing application') + ': \n'+ sMessage);
      }
    }
  }
};

///--------------------------------------------------------------------------------------
YAHOO.util.Event.onDOMReady(webapp.sample.Application.Init);
