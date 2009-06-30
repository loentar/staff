////////////////////////////////////////////////////////////////////////////////////////////////
// namespace staff
namespace('staff');

////////////////////////////////////////////////////////////////////////////////////////////////
// class client
staff.Client = Class.create();
staff.Client.prototype = 
{
  initialize: function(sServiceUri, sSessionId)
  {
    this.sServiceUri = sServiceUri;
    this.sSessionId = sSessionId;
  },
  
  SetSessionId: function(sSessionId)
  {
    this.sSessionId = sSessionId;
  },
  
  GetSessionId: function(sSessionId)
  {
    return this.sSessionId;
  },
  
  SetServiceUri: function(sServiceUri)
  {
    this.sServiceUri = sServiceUri;
  },
  
  GetServiceUri: function()
  {
    return this.sServiceUri;
  },
  
  IsInit: function()
  {
    return !!this.sServiceUri;
  },
  
  InvokeOperation: function(tOperation, pOnComplete, pOnError)
  {
    var tEnvelope = tOperation.RequestEnvelope();
    var tHeader = tEnvelope.get_header();

    if(tHeader == null)
    {
      tHeader = tEnvelope.create_header();
    }
      
    var tSidElm = tOperation.SubNode("SessionId", tHeader.element);
    if(tSidElm == null)
    {
      tSidElm = tHeader.create_child(new WS.QName("SessionId", "http://tempui.org/staff/sessionid", "sid"));
      tSidElm.set_value(this.sSessionId);
    }
    
    var tRequestOptions = 
    {
      method:         'post',
      asynchronous:   false,
      parameters:     '',
      postBody:       tEnvelope.asElement().ownerDocument,
      requestHeaders: 
      [ 
        'Content-Type', 'application/xml',
        'SOAPAction',   tOperation.GetSoapAction()
      ]
    };
    
    //asynchronous request
    if (typeof pOnComplete == "function")
    {
      tRequestOptions.asynchronous = true;
      var tAjaxRequest = null;
      
      function OnAsyncResponse()
      {
        try
        {
          this._ParseResponse(tAjaxRequest, tOperation);
          pOnComplete(tOperation);
        }
        catch(tError)
        {
          // error
          if(pOnError != null)
          {
            tOperation.SetFaultString(tError.message);
            pOnError(tOperation);
          }
        }
      }
      
      tRequestOptions.onComplete = OnAsyncResponse.bindAsEventListener(this);
      
      tAjaxRequest = new Ajax.Request(this.sServiceUri, tRequestOptions);
    }
    else
    {
      var tAjaxRequest = new Ajax.Request(this.sServiceUri, tRequestOptions);
      
      return this._ParseResponse(tAjaxRequest, tOperation);
    }
  },
  
  _ParseResponse: function(tAjaxRequest, tOperation)
  {
    if(tAjaxRequest.transport.status == 200)
    {
      var tResponseXml = tAjaxRequest.transport.responseXML;
      if (tResponseXml == null || tResponseXml.documentElement == null)
      {
        throw Error(_('Error while parsing response') + "!");
      }
      
      tOperation.SetResultEvenlope(new SOAP.Envelope(tResponseXml.documentElement));
      if (tOperation.IsFault())
      {
        throw Error(_('Failed to invoke service') + " " + this.sServiceUri + ": " + tOperation.GetFaultString());
      }
    }
    else
    {
      var sMessage = '';
      if (tAjaxRequest.transport.responseXML != null && tAjaxRequest.transport.responseXML.documentElement != null)
      { //remote exception
        tOperation.SetResultEvenlope(new SOAP.Envelope(tAjaxRequest.transport.responseXML.documentElement));
        sMessage = tOperation.GetFaultString();
      }
      
      throw Error("<b>" + _(sMessage) + "</b><br/>" + 
          _('Failed to invoke service') + " <u style='color: blue'>" + this.sServiceUri + "</u><br/>" 
          + "<sub>(" + _(tAjaxRequest.transport.statusText || GetErrorStr(tAjaxRequest.transport.status)) + ")</sub>");
    }
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////
// class Operation

staff.Operation = Class.create();
staff.Operation.prototype = 
{
  initialize: function(sName, sTargetNamespace, sResponseName, sResultName)
  {
    this.sName = sName;
    this.sResponseName = sResponseName || (sName + "Result");
    this.sResultName = sResultName;
    this.sTargetNamespace = sTargetNamespace;

    this.tRequestEnvelope = new SOAP.Envelope();
    this.tRequestElement = this.tRequestEnvelope.create_body().create_child(new WS.QName(this.sName, this.sTargetNamespace));
  },
  
  SetName: function(sName)
  {
    this.sName = sNameInit;
  },
  
  GetName: function()
  {
    return this.sName;
  },
  
  SetResponseName: function(sResponseName)
  {
    this.sResponseName = sResponseName;
  },
  
  GetResponseName: function()
  {
    return this.sResponseName;
  },
  
  SetResultName: function(sResultName)
  {
    this.sResultName = sResultName;
  },
  
  GetResultName: function()
  {
    return this.sResultName;
  },
  
  SetSoapAction: function(sSoapAction)
  {
    this.sSoapAction = sSoapAction;
  },
  
  GetSoapAction: function()
  {
    return this.sSoapAction;
  },
  
  AddParameter: function(sName, tValue, tNode)
  {
    var tParentElm = tNode == null ? this.tRequestElement : tNode;
    var tCreatedElm = tParentElm.create_child(new WS.QName(sName, this.sTargetNamespace));
    
    if (tValue == null)
    {
      tCreatedElm.set_value('');
    }
    else
    {
      if (typeof tValue != 'string')
      {
        tCreatedElm.set_value(tValue.toString());
      }
      else
      {
        tCreatedElm.set_value(tValue);
      }
    }
    
    return tCreatedElm;
  },
  
  AddDataObjectParameter: function(sName, tData, tNode)
  {
    var tParentElm = tNode == null ? this.tRequestElement : tNode;
    var tCreatedElm = tParentElm.create_child(new WS.QName(sName, this.sTargetNamespace));
    
    tCreatedElm.add_child(tData.ToElement());
    
    return tCreatedElm;
  },
  
  AddXmlParameter: function(sName, tData, tNode)
  {
    var tParentElm = tNode == null ? this.tRequestElement : tNode;
    var tCreatedElm = tParentElm.create_child(new WS.QName(sName, this.sTargetNamespace));
    
    tCreatedElm.add_child(tData);
    
    return tCreatedElm;
  },
  
  RequestEnvelope: function()
  {
    return this.tRequestEnvelope;
  },

  RequestElement: function()
  {
    return this.tRequestElement;
  },

  ResponseEnvelope: function()
  {
    return this.tResponseEnvelope;
  },

  ResponseElement: function()
  {
    return this.tResponseElement;
  },
  
  ResultElement: function()
  {
    return this.tResultElement;
  },
  
  ResultValue: function()
  {
    return !this.tResultElement.firstChild ? "" : this.tResultElement.firstChild.nodeValue;
  },
  
  FaultElement: function()
  {
    return this.tFaultElement;
  },
  
  IsFault: function()
  {
    return !!this.tFaultElement;
  },
  
  SetFaultString: function(sFaultString)
  {
    if(this.tResponseEnvelope == null)
    {
      this.tResponseEnvelope = new SOAP.Envelope();
      this.tResponseEnvelope.create_body();
    }
    
    var tFaultNode = this.tResponseEnvelope.get_body().create_child(new WS.QName('Fault'));
    
    this.tFaultElement = tFaultNode.element;
    tFaultNode.create_child(new WS.QName('FaultString')).set_value(sFaultString);
    this.tResultElement = this.tFaultElement;
  },
  
  GetFaultString: function()
  {
    return this.SubNodeNoCaseText('FaultString', this.tFaultElement);
  },
  
  SetResultEvenlope: function(tEnvelope)
  {
    this.tResponseEnvelope = tEnvelope;
    this.tResponseElement = null;
    this.tResultElement = null;
    this.tFaultElement = null;

    if (!tEnvelope)
    {
      throw Error(_("Envelope is not defined"));
    }
    
    var tBodyElement = this.tResponseEnvelope.get_body().element;

    // trying to retrive result
    this.tResponseElement = this.SubNode(this.sResponseName, tBodyElement);

    if (!this.tResponseElement) // soap:Fault?
    {
      var tFaultElement = this.SubNode("Fault", tBodyElement);
      if (!tFaultElement)
      {
        throw Error(_('Failed to get') + ' ResponseElement: ' + this.sResponseName);
      }

      this.tFaultElement = tFaultElement;
    }

    if (!this.sResultName)
    {
      this.tResultElement = this.tResponseElement;
    }
    else
    {
      this.tResultElement = this.SubNode(this.sResultName, this.tResponseElement);
    }
  },
  
  SubNode: function(sName, tNode)
  {
    if (!tNode)
    {
      tNode = this.ResultElement();
    }

    if (tNode && tNode.childNodes)
    {
      for (var i = 0; i < tNode.childNodes.length; i++)
      {
        if (tNode.childNodes[i].localName == sName)
        {
          return tNode.childNodes[i];
        }
      }
    }
  },
  
  SubNodeNoCase: function(sName, tNode)
  {
    if (!tNode)
    {
      tNode = this.ResultElement();
    }

    if (tNode && tNode.childNodes)
    {
      var sNameLocase = sName.toLowerCase();
      for (var i = 0; i < tNode.childNodes.length; i++)
      {
        if (tNode.childNodes[i].localName.toLowerCase() == sNameLocase)
        {
          return tNode.childNodes[i];
        }
      }
    }
  },
  
  
  SubNodeText: function (sName, tNode)
  {
    var tFindNode = this.SubNode(sName, tNode);
    if (tFindNode)
    {
      if(!tFindNode.firstChild || !tFindNode.firstChild.nodeValue)
      {
        return "";
      }
      
      return tFindNode.firstChild.nodeValue;
    }
  },
  
  SubNodeNoCaseText: function (sName, tNode)
  {
    var tFindNode = this.SubNodeNoCase(sName, tNode);
    if (tFindNode)
    {
      if(!tFindNode.firstChild || !tFindNode.firstChild.nodeValue)
      {
        return "";
      }
      
      return tFindNode.firstChild.nodeValue;
    }
  },
  
  SetValue: function(sValue, tNode)
  {
    if (!tNode)
    {
      tNode = this.ResultElement();
    }
    
    tNode.element.firstChild.nodeValue = sValue;
  },
  
  SetDataObject: function(tDataObject, tNode)
  {
    if (!tNode)
    {
      tNode = this.ResultElement();
    }
    
    tNode.add_child(tDataObject.ToElement());
  }
};


////////////////////////////////////////////////////////////////////////////////////////////////
// class DataObject

staff.DataObject = Class.create();
staff.DataObject.prototype = 
{
  initialize: function(tElement)
  {
    if (tElement != null)
    {
      this.FromElement(tElement);
    }
  },
  
  Erase: function()
  {
    for (var tIndex in this)
    {
      if (typeof this[tIndex] != 'function')
      {
        delete this[tIndex];
      }
    }
  },
  
  FromElement: function(tElement)
  {
    this.Erase();
    this.extend(this._FromElement(tElement));
  },
  
  _FromElement: function(tElem)
  {
    // detect type

    // node?
    if (!tElem.firstChild)
    {
      //empty string
      return tElem.nodeType == 1 ? {} : '';
    }

    // is object/array?
    {
      var bArray = null;
      // has subnodes?
      var tNode = tElem.firstChild;
      for (; tNode; tNode = tNode.nextSibling)
      {
        if (tNode.nodeType == 1) // filter out text nodes
        {
          bArray = tNode.localName == 'ArrayItem';
          break;
        }
      }
      
      if (tNode && bArray !== null)
      {
        if (bArray)
        { // array
          var aData = [];
          for (; tNode; tNode = tNode.nextSibling)
          {
            if (tNode.nodeType == 1) // filter out text nodes
            {
              aData.push(this._FromElement(tNode));
            }
          }

          return aData;
        }
        else
        { // object
          var oData = {};
          for (; tNode; tNode = tNode.nextSibling)
          {
            if (tNode.nodeType == 1) // filter out text nodes
            {
              oData[tNode.localName] = this._FromElement(tNode);
            }
          }

          return oData;
        }
      }
    }

    // is boolean?
    var sVal = tElem.firstChild.nodeValue.toLowerCase();
    if (sVal == 'true' || sVal == 'false')
    {
      return sVal == 'true';
    }

    // is number? (including exp form)
    if (sVal.search(/^([-+]|)([\d]+|)(\.|)([\d]+|)([eE][-+\d][\d]+|)$/g) == 0)
    {
      return parseFloat(sVal);
    }
    
    return tElem.firstChild.nodeValue;
  },
  
  ToElement: function(tElem)
  {
    for (var tIndex in this)
    {
      if (typeof this[tIndex] != 'function')
      {
        return this._ToElement(tElem, this[tIndex], tIndex);
      }
    }
  },
  
  _ToElement: function(tParentElem, tObj, sName)
  {
    var tElem = tParentElem.create_child(new WS.QName(sName));

    if (typeof tObj == 'object')
    {
      if (tObj instanceof Array)
      {
        for(var nIndex = 0; nIndex < tObj.length; ++nIndex)
        {
          this._ToElement(tElem, tObj[nIndex], 'ArrayItem');
        }
      }
      else
      {
        // deny to serialize HTML elements
        if (tObj.document === document)
        {
          throw Error('Unable to serialize HTML elements');
        }
        
        for(var tIndex in tObj)
        {
          var tSubObj = tObj[tIndex];
          if (typeof tSubObj != 'function')
          {
            this._ToElement(tElem, tObj[tIndex], tIndex);
          }
        }
      }
    }
    else
    {
      tElem.set_value(tObj.toString());
    }

    return tElem;
  }
};
