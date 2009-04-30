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
