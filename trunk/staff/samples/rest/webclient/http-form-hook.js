// most browsers does not support methods other than POST and GET
// so this script installs hook to make requests via XMLHttpRequest object

(function(){

  var oResult;

  setTimeout(function(){
    if (typeof jQuery == "undefined")
    {
      var elErr = document.createElement("span");
      elErr.style.color="red";
      elErr.innerHTML = "<br/><b>Error while loading jQuery.</b><br/> " + 
          "Please download it from <a href=\"http://jquery.com/\">http://jquery.com/</a> " + 
          "and put it as jquery.js into this \"samples/rest/webclient\" directory";
      document.body.appendChild(elErr);
    }
  }, 2000);


  // simple function to convert JSON to XML
  function jsonToXml(obj)
  {
    var result = "";
    for (var name in obj)
    {
      if (obj.hasOwnProperty(name))
      {
        var value = obj[name];
        result += "<" + name + ">";
        if (typeof value == "object")
        {
          result += jsonToXml(value);
        }
        else
        {
          result += value || "";
        }
        result += "</" + name + ">";
      }
    }
    return result;
  }

  function onFormSubmit(oEvent)
  {
    var bJson = $("#json")[0].checked;
    var oForm = $(oEvent.target);
    var sMethod = oForm.attr('method');
    var sParams = oForm.serialize();
    var sRestUrl = oForm.attr('action');
    var oRequestInfo = {
      type: sMethod,
      async: true,
      url: sRestUrl,
      dataType: bJson ? "json" : "text",
      processData: false
    };

    if (sMethod === "POST" || sMethod === "PUT")
    {
      // create JSON object in form as follows:
      // {
      //    "OpName": {
      //      param1: value1,
      //      param2: value2,
      //      ...
      //    }
      //  }
      var oRequest = {};
      var sObjName = oForm.attr("name");
      var oObjProps = oRequest[sObjName] = {};
      var oParams = sParams.split("&");
      for (var i = 0, len = oParams.length; i < len; ++i)
      {
        var oPair = oParams[i].split('=');
        oObjProps[oPair[0]] = unescape(oPair[1].replace(/\+/g, ' '));
      }

      if (bJson)
      {
        // use generated JSON as request
        oRequestInfo.data = JSON.stringify(oRequest);
        oRequestInfo.contentType = "application/json; charset=utf-8";
      }
      else
      {
        // convert JSON to XML and set it as request
        oRequestInfo.data = jsonToXml(oRequest);
        oRequestInfo.contentType = "text/xml";
      }
    }
    else // create URL with params
    {
      if (sParams.length !== 0)
      {
        oRequestInfo.url += "/" + sParams;
      }
      oRequestInfo.contentType = bJson ?
          "application/json; charset=utf-8" : oForm[0].encoding;
    }


    function onSuccess(sResult)
    {
      if (bJson)
        sResult = JSON.stringify(sResult);

      oResult.text(sResult);
      oResult.css("color", "");
    }

    function onError()
    {
      oResult.text("failed to send [" + sMethod + "] request: " + arguments[2]);
      oResult.css("color", "red");
    }

    jQuery.ajax(oRequestInfo)
      .success(onSuccess)
      .error(onError);

    oEvent.preventDefault();
  }

  $(document).ready(function(){
    // bind our handler instead default submit method
    $('form').bind("submit", onFormSubmit);

    var oBody = $(document.body);
    oBody.append("Result:<p/><div style=\"border: 1px solid silver\">" +
                            "<pre id=\"result\" style=\"word-wrap: break-word\"/></div>");
    oResult = $("#result");

    oBody.append("<p/><input type=\"checkbox\" id=\"json\"></input><label for=\"json\">use JSON</label>");
    oBody.append("<p/><a href=\"javascript:history.go(-1)\">&lt;&lt;&lt; back</a>");
  });

})();

