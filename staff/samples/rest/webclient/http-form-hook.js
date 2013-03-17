// most browsers does not support methods other than POST and GET
// so this script installs hook to make requests via XMLHttpRequest object

(function(){

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
    var oForm = $(oEvent.target);
    var sMethod = oForm.attr('method');
    var sParams = oForm.serialize();
    var sRestUrl = oForm.attr('action');
    var oRequestInfo = {
      type: sMethod,
      async: true,
      url: sRestUrl,
      dataType: "text",
      processData: false
    };

    if (sMethod == "POST")
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
      var sOpName = sRestUrl.substr(sRestUrl.lastIndexOf('/') + 1);
      var oOpParams = oRequest[sOpName] = {};
      var oParams = sParams.split("&");
      for (var i = 0, len = oParams.length; i < len; ++i)
      {
        var oPair = oParams[i].split('=');
        oOpParams[oPair[0]] = oPair[1];
      }

      // convert JSON to XML and set it as request
      oRequestInfo.data = jsonToXml(oRequest);
      oRequestInfo.contentType = "text/xml";
    }
    else // create URL with params
    {
      sRestUrl += sParams;
      oRequestInfo.contentType = oForm[0].encoding;
    }

    function onSuccess(sResult)
    {
      alert(sResult);
    }

    function onError()
    {
      alert("failed to send [" + sMethod + "] request: " + arguments[2]);
    }

    jQuery.ajax(oRequestInfo)
      .success(onSuccess)
      .error(onError);

    oEvent.preventDefault();
  }

  $(document).ready(function(){
    // bind our handler instead default submit method
    $('form').bind("submit", onFormSubmit);
  });

})()

