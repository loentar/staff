/*
 *  Copyright 2011 Utkin Dmitry
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

  // utils
  function createObject(sNs)
  {
    var oNext;
    var oCurr = window;
    var oNs = sNs.split('.');
    var nNsLen = oNs.length;
    for (var nIndex = 0; nIndex != nNsLen; ++nIndex)
    {
      oNext = oCurr[oNs[nIndex]];
      oCurr = (oNext !== undefined) ? oNext : (oCurr[oNs[nIndex]] = {});
    }
    return oCurr;
  }

  createObject('staff');

  staff.namespace = createObject;
  staff.object = createObject;


  // xml builder
  staff.Element = function(sName, sPrefix, sUri, sText)
  {
    var _this = this;
    _this._sName = sName;
    _this._sText = sText || "";
    _this._aChilds = [];
    _this._oAttrs = {};
    _this._oNss = {};
    _this._oNs = {};

    _this.appendChild = function(oChild)
    {
      return _this._aChilds.push(oChild);
    }

    _this.createChild = function(sName, sPrefix, sUri, sText)
    {
      var oChild = new staff.Element(sName, sPrefix, sUri, sText);
      _this.appendChild(oChild);
      return oChild;
    }

    _this.removeAllChildren = function()
    {
      _this._aChilds = [];
    }

    _this.addAttr = function(sAttrName, sAttrText)
    {
      _this._oAttrs[sAttrName] = sAttrText;
    }

    _this.addNs = function(sPrefix, sUri)
    {
      _this._oNss[sPrefix] = sUri;
    }

    _this.setNs = function(sPrefix, sUri)
    {
      _this._oNs = {sPrefix: (sPrefix || ""), sUri: sUri};
      if (_this._oNs.sUri)
      {
        _this.addNs(_this._oNs.sPrefix, _this._oNs.sUri);
      }
    }

    _this.setText = function(sText)
    {
      _this._sText = sText || "";
    }

    _this.toString = function()
    {
      var sResult = "<" + (_this._oNs.sPrefix ? (_this._oNs.sPrefix + ":") : "") + _this._sName;
      for (var itNs in _this._oNss)
      {
        if (!_this._oNss.hasOwnProperty || _this._oNss.hasOwnProperty(itNs))
        {
          sResult += " xmlns" + (itNs ? (":" + itNs) : "") + "=\"" + _this._oNss[itNs] + "\"";
        }
      }
      for (var itAttr in _this._oAttrs)
      {
        if (!_this._oAttrs.hasOwnProperty || _this._oAttrs.hasOwnProperty(itAttr))
        {
          sResult += " " + itAttr + "=\"" + _this._oAttrs[itAttr] + "\"";
        }
      }
      sResult += ">";

      var nChildSize = _this._aChilds.length;
      for (var nChild = 0; nChild < nChildSize; ++nChild)
      {
        sResult += _this._aChilds[nChild].toString()
      }

      sResult += _this._sText + "</" + (_this._oNs.sPrefix ? (_this._oNs.sPrefix + ":") : "") + _this._sName + ">";
      return sResult;
    }

    _this.setNs(sPrefix, sUri);
  }

  // SOAP Envelope
  staff.SoapEnvelope = function(sSoapVersion)
  {
    var _this = this;
    _this._oEnvelope = new staff.Element("Envelope");
    _this._oHead = null;
    _this._oBody = _this._oEnvelope.createChild("Body", "soapenv");

    _this.setSoapVersion = function(sSoapVersion)
    {
      _this._oEnvelope.setNs("soapenv", sSoapVersion == "1.1" ?
          "http://schemas.xmlsoap.org/soap/envelope/" :
          "http://www.w3.org/2003/05/soap-envelope");
    }

    _this.setSoapVersionUri = function(sUri)
    {
      _this._oEnvelope.setNs("soapenv", sUri);
    }

    _this.addHeader = function(oElement)
    {
      if (!_this._oHead)
      {
        _this._oHead = _this._oEnvelope.createChild("Head", "soapenv");
      }
      _this._oHead.appendChild(oElement);
    }

    _this.addBodyChild = function(oElement)
    {
      _this._oBody.appendChild(oElement);
    }

    _this.clearHeader = function()
    {
      _this._oHead = null;
    }

    _this.clearBody = function()
    {
      _this._oBody.removeAllChildren();
    }

    _this.toString = function()
    {
      return _this._oEnvelope.toString();
    }

    _this.setSoapVersion(sSoapVersion);
  }

  // SOAP client
  staff.SoapClient = function(sAddress, sSessionId, sInstanceId)
  {
    var _this = this;

    _this._sAddress = sAddress || "";
    _this._sCharset = "utf-8";
    _this._sSoapAction = null;
    _this._sSoapVersion = null;
    _this._aHttpHeaders = {};
    _this._oSoapEnvelope = new staff.SoapEnvelope();

    _this.setAddress = function(sAddress)
    {
      _this._sAddress = sAddress;
    }

    _this.setContentType = function(sContentType)
    {
      _this._sContentType = sContentType;
    }

    _this.setCharset = function(sCharset)
    {
      _this._sCharset = sCharset;
    }

    _this.setSoapAction = function(sSoapAction)
    {
      _this._sSoapAction = sSoapAction;
    }

    // 1.1 or 1.2
    _this.setSoapVersion = function(sVersion)
    {
      _this._sSoapVersion = sVersion;
      _this._oSoapEnvelope.setSoapVersion(sVersion);
    }

    _this.setSoapVersionUri = function(sUri)
    {
      _this._oSoapEnvelope.setSoapVersionUri(sUri);
    }

    _this.setHttpHeader = function(sName, sValue)
    {
      _this._aHttpHeaders[sName] = sValue;
    }

    _this.getSoapEnvelope = function()
    {
      return _this._oSoapEnvelope;
    }

    _this.setSessionId = function(sSessionId)
    {
      if (!_this._oSessionIdElem)
      {
        _this._oSessionIdElem =
            new staff.Element("SessionId", "sid", "http://tempui.org/staff/sessionid", sSessionId);
        _this._oSoapEnvelope.addHeader(_this._oSessionIdElem);
      }
      else
      {
        _this._oSessionIdElem.setText(sSessionId);
      }
    }

    _this.setInstanceId = function(sInstanceId)
    {
      if (!_this._oInstanceIdElem)
      {
        _this._oInstanceIdElem =
            new staff.Element("InstanceId", "iid", "http://tempui.org/staff/instanceid", sInstanceId);
        _this._oSoapEnvelope.addHeader(_this._oInstanceIdElem);
      }
      else
      {
        _this._oInstanceIdElem.setText(sInstanceId);
      }
    }

    _this.setTimeout = function(nTimeout)
    {
      _this._nTimeout = nTimeout;
    }

    _this.setHttpMethod = function(sHttpMethod)
    {
      _this._sHttpMethod = sHttpMethod;
    }

    _this.invoke = function(oRequestElement, fnCallback, fnError)
    {
      function getBodyElement(oData)
      {
        // data.responseXML is not set in IE
        if (oData.responseXML == undefined && jQuery.browser.msie)
        {
          oData.responseXML = new ActiveXObject("Microsoft.XMLDOM");
          oData.responseXML.loadXML(oData.responseText);
        }

        var oEnvelopeElem = oData.responseXML.firstChild;
        if (!oEnvelopeElem || staff.getElementLocalName(oEnvelopeElem) != "Envelope")
        {
          throw new Error("Invalid response");
        }

        var oBodyElem = staff.getChildElement(oEnvelopeElem, "Body");
        if (!oBodyElem)
        {
          throw new Error("Can't get body");
        }

        return oBodyElem;
      }

      function onResponse(oDoc, sStatus, oData)
      {
        if (typeof(fnCallback) == "function")
        {
          fnCallback(getBodyElement(oData));
        }
      }

      function onBeforeSend(oAjaxRequest)
      {
        if (_this._sSoapVersion == "1.1")
        {
          oAjaxRequest.setRequestHeader("SOAPAction", "\"" + _this._sSoapAction + "\"");
        }

        for (var sHeader in _this._aHttpHeaders)
        {
          if (!_this._aHttpHeaders.hasOwnProperty || _this._aHttpHeaders.hasOwnProperty(sHeader))
          {
            oAjaxRequest.setRequestHeader(sHeader, _this._aHttpHeaders[sHeader]);
          }
        }
      }

      _this._oSoapEnvelope.clearBody();
      _this._oSoapEnvelope.addBodyChild(oRequestElement);

      var sContentType = (_this._sSoapVersion == "1.1") ?
            ("text/xml; charset=\"" + _this._sCharset + "\"") :
            ("application/soap+xml; charset=\"" + _this._sCharset + "\"; action=\"" + _this._sSoapAction + "\"");

      var oRequestSettings =
      {
        type: _this._sHttpMethod || "POST",
        async: !!fnCallback,
        url: _this._sAddress,
        dataType: "xml",
        processData: false,
        data: _this._oSoapEnvelope.toString(),
        success: onResponse,
        contentType: sContentType,
        beforeSend: onBeforeSend
      };
      if (_this._nTimeout != undefined)
      {
        oRequestSettings.timeout = _this._nTimeout;
      }

      var oData = jQuery.ajax(oRequestSettings);
      if (fnError)
      {
        oData.error(fnError);
      }
      else
      {
        oData.error(function(oError){
          throw oError;
        });
      }

      if (!fnCallback)
      {
        return getBodyElement(oData);
      }
    }

    if (sSessionId)
    {
      _this.setSessionId(sSessionId);
    }
    if (sInstanceId)
    {
      _this.setInstanceId(sInstanceId);
    }

  }

  // detect ability of using localName
  if (document.firstChild.localName != undefined)
  {
    staff.getElementLocalName = function(oElement)
    {
      return oElement.localName;
    }

    // iterate child elements, return false to break
    staff.eachChildElement = function(oElement, fnCallback)
    {
      for (var oChild = oElement.firstElementChild; oChild; oChild = oChild.nextElementSibling)
      {
        if (fnCallback(oChild) === false)
        {
          break;
        }
      }
    }

    // iterate child elements with given name, return false to break
    staff.eachChildElementName = function(oElement, sName, fnCallback)
    {
      for (var oChild = oElement.firstElementChild; oChild; oChild = oChild.nextElementSibling)
      {
        if (oChild.localName == sName)
        {
          if (fnCallback(oChild) === false)
          {
            break;
          }
        }
      }
    }

    staff.getChildElement = function(oElement, sName)
    {
      for (var oChild = oElement.firstElementChild; oChild; oChild = oChild.nextElementSibling)
      {
        if (oChild.localName == sName)
        {
          return oChild;
        }
      }
    }
  }
  else // ie
  {
    staff.getElementLocalName = function(oElement)
    {
      return oElement.nodeName.replace(/^(.*:)/,'');
    }

    // iterate child elements, return false to break
    staff.eachChildElement = function(oElement, fnCallback)
    {
      for (var oChild = oElement.firstChild; oChild; oChild = oChild.nextSibling)
      {
        if (oChild.nodeType == 1)
        {
          if (fnCallback(oChild) === false)
          {
            break;
          }
        }
      }
    }

    // iterate child elements with given name, return false to break
    staff.eachChildElementName = function(oElement, sName, fnCallback)
    {
      for (var oChild = oElement.firstChild; oChild; oChild = oChild.nextSibling)
      {
        if (oChild.nodeType == 1 && oChild.nodeName.replace(/^(.*:)/,'') == sName)
        {
          if (fnCallback(oChild) === false)
          {
            break;
          }
        }
      }
    }

    staff.getChildElement = function(oElement, sName)
    {
      for (var oChild = oElement.firstChild; oChild; oChild = oChild.nextSibling)
      {
        if (oChild.nodeType == 1 && oChild.nodeName.replace(/^(.*:)/,'') == sName)
        {
          return oChild;
        }
      }
    }
  }

  staff.getElementValue = function(oElement)
  {
    return oElement.firstChild.nodeValue;
  }

})();
