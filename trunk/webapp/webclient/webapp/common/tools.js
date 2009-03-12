if(typeof webapp == 'undefined')
{
  webapp = {};
}

webapp.Env = {};
webapp.Env.Get = 
  function(sName, sDefault)
  {
    var tRet = eval('webapp.Env.' + sName);
    return tRet == null ? sDefault : tRet;
  };

if(document.location.search.length > 1)
{
  var _asLocationParams = document.location.search.substr(1);
  var _asLocationParamsFlat = _asLocationParams.split(',');

  for(var i = 0; i < _asLocationParamsFlat.length; ++i)
  {
    var asParam = _asLocationParamsFlat[i].split('=');
    eval('webapp.Env.' + asParam[0] + '=\'' + asParam[1] + '\';');
  }
}

webapp.Env.protocol = document.location.protocol + '//';
var _atIncludedScripts = new Array();

//!         включить javascript файл/список файлов
/*! \param  sBaseName - базовое имя файла без расширения либо массив имен файлов без расширения. пример: "Button" или [ "Button", "Edit" ]
    \param  sBasePath - базовый путь к скрипту. пример "widgets/"
    \param  pIncludingCompleteFunction - функция, которая будет вызвана при завершении включения
    \return null
    */
function Include( sBaseName, sBasePath, pIncludingCompleteFunction, pIncludeAvaibleFunction )
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
        if(pIncludeAvaibleFunction)
        {
          for(var i = 0; i < sBaseName.length; ++i)
          {
            if(!pIncludeAvaibleFunction(sBaseName[i]))
            {
              if(--nLimit > 0)
              {
                setTimeout(CheckIncludeMulti, 100);
              }
              else
              {
                alert("can't load: " + sBaseName[i]);
              }
              return;
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
      if(pIncludeAvaibleFunction)
        if(!pIncludeAvaibleFunction(sBaseName))
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

//!         включить css файл/список файлов
/*! \param  sBaseName - базовое имя файла без расширения либо массив имен файлов без расширения. пример: "Button" или [ "Button", "Edit" ]
    \param  sBasePath - базовый путь к скрипту. пример "css/"
    */
function IncludeCss( sBaseName, sBasePath )
{
  if(sBaseName instanceof Array)
  {
    for(var i = 0; i < sBaseName.length; ++i)
      IncludeCss(sBaseName[i], sBasePath);
    
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
    var sScriptSrc = (sBasePath != null ? sBasePath : "") + sBaseName + ".css";
    var pHead = document.getElementsByTagName("head")[0];

    // ищем в документе
    for(tScriptBlock = pHead.firstChild; tScriptBlock != null; tScriptBlock = tScriptBlock.nextSibling)
      if(tScriptBlock.tagName == "LINK" && tScriptBlock.src == sScriptSrc)
        break;

    if (tScriptBlock == null) // нет в документе, загружаем
    {
      tScriptBlock = document.createElement('link');
      tScriptBlock.rel = 'stylesheet';
      tScriptBlock.type = 'text/css';
      tScriptBlock.href = sScriptSrc;
      pHead.appendChild(tScriptBlock);
    }
  
    var pScript = 
    {
      sBaseName: sBaseName,
      tScriptBlock: tScriptBlock
    };
    
    _atIncludedScripts.push(pScript);
  }
}

function addHandler(element,event,action,param)
{
  if(document.addEventListener)
    element.addEventListener(event,action,param);
  else 
    if(document.attachEvent)
      element.attachEvent('on'+event,action);
    else 
      element['on'+event]=action;
}

function removeHandler(element,event,action,param)
{
  if(document.addEventListener)
    element.removeEventListener(event,action,param);
  else 
    if(document.attachEvent)
      element.detachEvent('on'+event,action);
    else 
      element['on'+event]=returnFalse;
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
