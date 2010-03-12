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

// i18n: internationalization object

i18n = 
{
  tLocales: {},
  tLocale: {},
  sLocale: '',
  
  LoadLocale: function(sNamespace, sPath, fnComplete, tOptions)
  {
    var tOpts = tOptions || {};
    if (tOpts.sLocale == null)
    {
      tOpts.sLocale = webapp.Env.lang || navigator.userLanguage || navigator.language;
    }
    
    if (tOpts.sLocale == 'en' || tOpts.sLocale == 'en-US')
    { // do not load default language
      LoadingDone();
      return;
    }
    
    function OnLoaded()
    {
      if(i18n.tLocales[tOpts.sLocale] == null)
      {
        i18n.tLocales[tOpts.sLocale] = i18n[sNamespace];
      }
      else
      {
        var tLoc = i18n.tLocales[tOpts.sLocale];
        var tNewLoc = i18n[sNamespace];
        for(var nIndex in tNewLoc)
        {
          tLoc[nIndex] = tNewLoc[nIndex];
        }
      }

      LoadingDone();      
    }
    
    function OnError()
    {
      alert("can't load locale: \"" + tOpts.sLocale + "\" for " + sNamespace);
      try
      {
        LoadingDone();      
      }
      catch(tErrorIgnored)
      {
      }
    }
    
    function LoadingDone()
    {
      if (tOpts.bSwitchTo !== false)
      {
        i18n.SetLocale(tOpts.sLocale);
      }
      fnComplete && fnComplete();
    }

    Include(tOpts.sLocale, (sPath != '' ? sPath + '/' : '') + 'assets/i18n/', OnLoaded, [ 'i18n.' + sNamespace ], OnError);
  },
  
  SetLocale: function(sLocale)
  {
    this.tLocale = this.tLocales[sLocale] || {};
    this.sLocale = sLocale;
  },
  
  GetLocale: function()
  {
    return sLocale;
  },
  
  Translate: function(sText)
  {
    return (i18n.tLocale && i18n.tLocale[sText]) || sText;
  }
};

// shortcut for translation function
_ = i18n.Translate;
