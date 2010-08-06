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

(function(){

if (typeof webapp == 'undefined')
{
  webapp = {};
}

webapp.Env = {};
webapp.Env.Get = 
  function(sName, sDefault)
  {
    return webapp.Env[sName] || sDefault;
  };

if(document.location.search.length > 1)
{
  var _asLocationParamsFlat = document.location.search.substr(1).split(',');

  for(var i = 0; i < _asLocationParamsFlat.length; ++i)
  {
    var asParam = _asLocationParamsFlat[i].split('=');
    webapp.Env[asParam[0]] = asParam[1];
  }
}

webapp.Env.protocol = document.location.protocol + '//';
var _tIncludedScripts = new Array();
var _atIncludedCss = {};

//          включить javascript файл/список файлов
/*  \param  sBaseName - базовое имя файла без расширения либо массив имен файлов без расширения. пример: "Button" или [ "Button", "Edit" ]
    \param  sBasePath - базовый путь к скрипту. пример "widgets/"
    \param  fnComplete - функция, которая будет вызвана при завершении включения
    \param  fnCheckAvail - функция для проверки доступности либо массив строк, содержащий имена символов в файле
    \return null
    */
Include = function( sBaseName, sBasePath, fnComplete, fnCheckAvail, fnError )
{
  if(sBaseName instanceof Array)
  {
    for(var i = 0; i < sBaseName.length; ++i)
    {
      Include(sBaseName[i], sBasePath);
    }
    
    if(fnComplete)
    {
      var nLimit = 30;
      
      function CheckIncludeMulti()
      {
        function ContinueWait(i)
        {
          if(--nLimit > 0)
          {
            setTimeout(CheckIncludeMulti, 200);
          }
          else
          {
            if (fnError != null)
            {
              fnError(sBaseName, sBasePath);
            }
            else
            {
              alert("can't load" + ": " + sBaseName[i]);
            }
          }
        }
      
        if(typeof fnCheckAvail == 'function')
        {
          for(var i = 0; i < sBaseName.length; ++i)
          {
            if(!fnCheckAvail(sBaseName[i]))
            {
              ContinueWait(i);
              return;
            }
          }
        }
        else
        if(fnCheckAvail instanceof Array)
        {
          for(var j = 0; j < fnCheckAvail.length; ++j)
          {
            if(typeof fnCheckAvail[j] == 'string')
            {
              try
              {
                if(eval(fnCheckAvail[j]) == null)
                {
                  ContinueWait(j);
                  return;
                }
              }
              catch(tError)
              {
                ContinueWait(j);
                return;
              }
            }
          }
        }
        
        try
        {
          fnComplete();
        }
        catch(tError)
        {
          if (fnError != null)
          {
            fnError(sBaseName, sBasePath, (tError.message || tError.text || tError));
          }
          else
          {
            alert(tError.message || tError.text || tError);
          }
        }
      }
      
      if(fnComplete)
      {
        setTimeout(CheckIncludeMulti, 0);
      }
    }
    
    return;
  }

  var sScriptSrc = (sBasePath || '') + sBaseName + '.js';
  
  if (_tIncludedScripts[sScriptSrc] == null)
  { // добавляем новый скрипт
    var tScriptBlock = null;
    var pHead = document.getElementsByTagName("head")[0];

    // ищем в документе
    for(tScriptBlock = pHead.firstChild; tScriptBlock != null; tScriptBlock = tScriptBlock.nextSibling)
    {
      if(tScriptBlock.tagName == "SCRIPT" && tScriptBlock.src == sScriptSrc)
      {
        break;
      }
    }

    if (tScriptBlock == null) // нет в документе, загружаем
    {
      tScriptBlock = document.createElement('script');
      tScriptBlock.type = 'text/javascript';
      tScriptBlock.src = sScriptSrc;
      tScriptBlock.defer = 'defer';
      pHead.appendChild(tScriptBlock);
    }
  
    _tIncludedScripts[sScriptSrc] = tScriptBlock;
  }
  
  if(fnComplete)
  {
    var nLimit = 30;
    
    function CheckIncludeSingle()
    {
      function ContinueWait()
      {
        if(--nLimit > 0)
        {
          setTimeout(CheckIncludeSingle, 200);
        }
        else
        {
          if (fnError != null)
          {
            fnError(sBaseName, sBasePath);
          }
          else
          {
            alert("can't load" + ": " + sBaseName[i]);
          }
        }
      }
    

      if(fnCheckAvail)
      {
        if(typeof fnCheckAvail == 'function')
        {
          if(!fnCheckAvail(sBaseName))
          {
            ContinueWait();
            return;
          }
        }
        else
        if(fnCheckAvail instanceof Array)
        {
          for(var j = 0; j < fnCheckAvail.length; ++j)
          {
            if(typeof fnCheckAvail[j] == 'string')
            {
              try
              {
                if(eval(fnCheckAvail[j]) == null)
                {
                  ContinueWait();
                  return;
                }
              }
              catch(tError)
              {
                ContinueWait();
                return;
              }
            }
          }
        }
      }
      
      fnComplete();
    }
    
    if(fnComplete)
      setTimeout(CheckIncludeSingle, 0);
  }
}

IncludeClass = function( asFullNames, sBasePath, fnComplete )
{
  var asBaseNames = new Array();

  if(asFullNames instanceof Array)
  {
    for(var i = 0; i < asFullNames.length; ++i)
    {
      asBaseNames[i] = asFullNames[i].replace(/\./g, '/');
    }
  }
  else
  {
    asBaseNames[0] = asFullNames.replace(/\./g, '/');
  }

  Include
  (
    asBaseNames,
    sBasePath,
    fnComplete,
    function(sBaseName)
    {
      try
      {
        if (eval('typeof ' + sBaseName.replace(/\//g, '.')) == 'undefined')
        {
          return false;
        }
      }
      catch(e)
      {
        return false;
      }

      return true;
    }
  );
}

//          включить css файл/список файлов
/*  \param  sBaseName - базовое имя файла без расширения либо массив имен файлов без расширения. пример: "Button" или [ "Button", "Edit" ]
    \param  sBasePath - базовый путь к скрипту. пример "css/"
    */
IncludeCss = function( sBaseName, sBasePath )
{
  if(sBaseName instanceof Array)
  {
    for(var i = 0; i < sBaseName.length; ++i)
    {
      IncludeCss(sBaseName[i], sBasePath);
    }
    
    return;
  }
  
  var sCssSrc = (sBasePath || '') + sBaseName + ".css";
  
  if (_atIncludedCss[sCssSrc] == null)
  { // добавляем новый скрипт
    var tCssBlock = null;
    var pHead = document.getElementsByTagName("head")[0];

    // ищем в документе
    for(tCssBlock = pHead.firstChild; tCssBlock != null; tCssBlock = tCssBlock.nextSibling)
    {
      if(tCssBlock.tagName == "LINK" && tCssBlock.src == sCssSrc)
      {
        break;
      }
    }

    if (tCssBlock == null) // нет в документе, загружаем
    {
      tCssBlock = document.createElement('link');
      tCssBlock.rel = 'stylesheet';
      tCssBlock.type = 'text/css';
      tCssBlock.href = sCssSrc;
      pHead.appendChild(tCssBlock);
    }
  
    _atIncludedCss[sCssSrc] = tCssBlock;
  }
}

addHandler = function(tElement, sEvent, fHandler)
{
  if(tElement.addEventListener)
  {
    tElement.addEventListener(sEvent, fHandler, false);
  }
  else 
  {
    if(tElement.attachEvent)
    {
      tElement.attachEvent('on' + sEvent, fHandler);
    }
    else 
    {
      tElement['on' + sEvent] = fHandler;
    }
  }
}

removeHandler = function(tElement, sEvent, fHandler)
{
  if(tElement.removeEventListener)
  {
    tElement.removeEventListener(sEvent, fHandler, false);
  }
  else 
  {
    if(tElement.detachEvent)
    {
      tElement.detachEvent('on' + sEvent, fHandler);
    }
    else 
    {
      tElement['on' + sEvent] = 
        function()
        {
          return false;
        };
    }
  }
}

fireEvent = function(tElement, sEvent)
{
  if (document.createEventObject)
  {
    // IE
    var tEventObj = document.createEventObject();
    return tElement.fireEvent('on' + sEvent, tEventObj)
  }
  else
  {
    // firefox & others
    var tEventObj = document.createEvent("HTMLEvents");
    tEventObj.initEvent(sEvent, true, true );
    return !tElement.dispatchEvent(tEventObj);
  }
}

namespace = function(sNamespace)
{
  var aNamespace = sNamespace.split('.');
  var sCurrentNamespace = '';
  for(var nNs = 0; nNs != aNamespace.length; ++nNs)
  {
    if (nNs != 0) 
    {
      sCurrentNamespace += '.';
    }
    sCurrentNamespace += aNamespace[nNs];
    
    eval('if (typeof ' + sCurrentNamespace + ' == \'undefined\') { ' + sCurrentNamespace + ' = {}; };');
  }
}

Object.prototype.clone = function(tCloneTo)
{
  if (tCloneTo)
  {
    if(this instanceof Array)
    {
      tCloneTo.splice(0, tCloneTo.length);
    }
    else
    {
      for(var tProperty in tCloneTo)
      {
        if (tProperty != 'clone')
        {
          delete tCloneTo[tProperty];
        }
      }
    }

    for (tProperty in this)
    {
      if (tProperty != 'clone')
      {
        tCloneTo[tProperty] = this[tProperty];
      }
    }
    return tCloneTo;
  }
  else
  {
    var tClone;
    if(this instanceof Array)
    {
      tClone = [];
    }
    else
    {
      tClone = {};
    }

    for (tProperty in this)
    {
      if (tProperty != 'clone')
      {
        tClone[tProperty] = this[tProperty];
      }
    }
    return tClone;
  }
}

})()
