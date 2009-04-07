if(typeof webapp == 'undefined')
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
var _atIncludedScripts = new Array();
var _atIncludedCss = {};

//          включить javascript файл/список файлов
/*  \param  sBaseName - базовое имя файла без расширения либо массив имен файлов без расширения. пример: "Button" или [ "Button", "Edit" ]
    \param  sBasePath - базовый путь к скрипту. пример "widgets/"
    \param  pIncludingCompleteFunction - функция, которая будет вызвана при завершении включения
    \param  pIncludeAvailableFunction - функция для проверки доступности либо массив строк, содержащий имена символов в файле
    \return null
    */
function Include( sBaseName, sBasePath, pIncludingCompleteFunction, pIncludeAvailableFunction )
{
  if(sBaseName instanceof Array)
  {
    for(var i = 0; i < sBaseName.length; ++i)
      Include(sBaseName[i], sBasePath);
    
    if(pIncludingCompleteFunction)
    {
      var nLimit = 20;
      
      function CheckIncludeMulti()
      {
        function ContinueWait()
        {
          if(--nLimit > 0)
          {
            setTimeout(CheckIncludeMulti, 100);
          }
          else
          {
            alert("can't load: " + sBaseName[i]);
          }
        }
      
        if(typeof pIncludeAvailableFunction == 'function')
        {
          for(var i = 0; i < sBaseName.length; ++i)
          {
            if(!pIncludeAvailableFunction(sBaseName[i]))
            {
              ContinueWait();
              return;
            }
          }
        }
        else
        if(pIncludeAvailableFunction instanceof Array)
        {
          for(var j = 0; j < pIncludeAvailableFunction.length; ++j)
          {
            if(typeof pIncludeAvailableFunction[j] == 'string')
            {
              try
              {
                if(eval(pIncludeAvailableFunction[j]) == null)
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
        
        pIncludingCompleteFunction();
      }
      
      if(pIncludingCompleteFunction)
      {
        setTimeout(CheckIncludeMulti, 0);
      }
    }
    
    return;
  }

  var pScript = null;
  for(var i = 0; i < _atIncludedScripts.length; ++i)
  {
    if (_atIncludedScripts[i].sBaseName == sBaseName) 
    {
      pScript = _atIncludedScripts[i];
      break;
    }
  }
  
  if (pScript == null)
  { // добавляем новый скрипт
    var tScriptBlock = null;
    var sScriptSrc = (sBasePath != null ? sBasePath : "") + sBaseName + ".js";
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
  
    var pScript = 
    {
      sBaseName: sBaseName,
      tScriptBlock: tScriptBlock
    };
    
    _atIncludedScripts.push(pScript);
  }
  
  if(pIncludingCompleteFunction)
  {
    var nLimit = 20;
    
    function CheckIncludeSingle()
    {
      if(pIncludeAvailableFunction)
        if(!pIncludeAvailableFunction(sBaseName))
        {
          if(--nLimit > 0)
            setTimeout(CheckIncludeSingle, 100);
          return;
        }
      
      pIncludingCompleteFunction();
    }
    
    if(pIncludingCompleteFunction)
      setTimeout(CheckIncludeSingle, 0);
  }
}

function IncludeClass( asFullNames, sBasePath, pIncludingCompleteFunction )
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
    pIncludingCompleteFunction,
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
function IncludeCss( sBaseName, sBasePath )
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

function addHandler(tElement, sEvent, fHandler)
{
  if(tElement.addEventListener)
  {
    tElement.addEventListener(sEvent, fHandler, null);
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

function removeHandler(tElement, sEvent, fHandler)
{
  if(tElement.removeEventListener)
  {
    tElement.removeEventListener(sEvent, fHandler);
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

function namespace(sNamespace)
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
