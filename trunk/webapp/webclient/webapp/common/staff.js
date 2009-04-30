////////////////////////////////////////////////////////////////////////////////////////////////
// namespace staff
namespace('staff');

////////////////////////////////////////////////////////////////////////////////////////////////
// class client
staff.Client = Class.create();
staff.Client.prototype = 
{
  initialize: function(sServiceName, sHostName, sHostPort, sID)
  {
    if(sHostName == null) sHostName = Session.sHost;
    if(sHostPort == null) sHostPort = Session.sPort;
    if(sID == null) sID = Session.sID;
    
    this.sServiceUri = webapp.Env.protocol + sHostName;
    if(sHostPort != '')
      this.sServiceUri += ':' + sHostPort;
    this.sServiceUri += '/axis2/services/' + sServiceName;
    this.sID = sID;
  },
  
  SetID: function(sID)
  {
    this.sID = sID;
  },
  
  GetServiceUri: function()
  {
    return this.sServiceUri;
  },
  
  IsInit: function()
  {
    return this.sServiceUri != '';
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
      tSidElm = tHeader.create_child(new WS.QName("SessionId"));
      tSidElm.set_value(this.sID);
    }
    
    var tRequestOptions = 
    {
      method:         'post',
      asynchronous:   false,
      parameters:     '',
      postBody:       tEnvelope.asElement().ownerDocument,
      requestHeaders: [ 'Content-Type', 'application/xml', 'SOAPAction', '""' ]
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
      if(tOperation.IsFault())
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

      throw Error(_('Failed to invoke service') + " " + this.sServiceUri + ": <b>" + _(sMessage) + "</b> <br/>" 
          + "(" + _(tAjaxRequest.transport.statusText || GetErrorStr(tAjaxRequest.transport.status)) + ")");
    }
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////
// class Operation

staff.Operation = Class.create();
staff.Operation.prototype = 
{
  //! sName - имя операции
  initialize: function(sName, sServiceUri)
  {
    this.sName = sName;
    this.tRequestEnvelope = new SOAP.Envelope();
    this.tRequestElement = this.tRequestEnvelope.create_body().create_child(new WS.QName(this.sName, sServiceUri, "staff"));
  },
  
  SetName: function(sName)
  {
    this.sName = sNameInit;
  },
  
  GetName: function()
  {
    return this.sName;
  },
  
  AddParameter: function(sName, tValue, tNode)
  {
    var tParentElm = tNode == null ? this.tRequestElement : tNode;
    var tCreatedElm = tParentElm.create_child(new WS.QName(sName));
    
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
  
  AddDataParameter: function(sName, tData, tNode)
  {
    var tParentElm = tNode == null ? this.tRequestElement : tNode;
    var tCreatedElm = tParentElm.create_child(new WS.QName(sName));
    
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

  ResultEnvelope: function()
  {
    return this.tResultEnvelope;
  },

  ResultElement: function()
  {
    return this.tResultElement;
  },
  
  FaultElement: function()
  {
    return this.tFaultElement;
  },
  
  IsFault: function()
  {
    return this.tFaultElement != null;
  },
  
  SetFaultString: function(sFaultString)
  {
    if(this.tResultEnvelope == null)
    {
      this.tResultEnvelope = new SOAP.Envelope();
      this.tResultEnvelope.create_body();
    }
    
    var tFaultNode = this.tResultEnvelope.get_body().create_child(new WS.QName('Fault'));
    
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
    this.tResultEnvelope = tEnvelope;
    this.tResultElement = null;
    this.tFaultElement = null;
    if(tEnvelope == null)
    {
      return;
    }

    if (this.tResultEnvelope.element.childNodes.length == 0) // empty envelope
    {
      this.SetFault(_('Failed to invoke service'));
    } else // 
    {
      // пытаемся получить результат
      var tRes = this.SubNode('staff:' + this.sName + 'Result', this.tResultEnvelope.get_body().element);

      if(tRes != null)
      {
        this.tResultElement = tRes;
      } 
      else
      {
        tRes = this.tResultEnvelope.get_body().get_children(new WS.QName('Fault', SOAP.URI, 'soapenv'));
        if(tRes.length != 0)
        {
          this.tFaultElement = tRes[0].element;
        } 
        else
        {
          throw Error(_('Failed to get') +' ' + this.sName + 'Result');
        }
      }
    }
  },
  
  SubNode: function(sName, tNode)
  {
    if (tNode == null)
    {
      tNode = this.ResultElement();
    }

    if (tNode != null)
    {
      for (var i = 0; i < tNode.childNodes.length; i++)
      {
        if (tNode.childNodes[i].nodeName == sName)
        {
          return tNode.childNodes[i];
        }
      }
    }
    
    return null;
  },
  
  SubNodeNoCase: function(sName, tNode)
  {
    if (tNode == null)
    {
      tNode = this.ResultElement();
    }

    if (tNode != null && tNode.childNodes != null)
    {
      var sNameLocase = sName.toLowerCase();
      for (var i = 0; i < tNode.childNodes.length; i++)
      {
        if (tNode.childNodes[i].nodeName.toLowerCase() == sNameLocase)
          return tNode.childNodes[i];
      }
    }
    
    return null;
  },
  
  
  SubNodeText: function (sName, tNode)
  {
    var tFindNode = this.SubNode(sName, tNode);
    if (tFindNode != null)
    {
      if(tFindNode.firstChild == null || tFindNode.firstChild.nodeValue == null)
      {
        return "";
      }
      
      return tFindNode.firstChild.nodeValue;
    }
    
    return null;
  },
  
  SubNodeNoCaseText: function (sName, tNode)
  {
    var tFindNode = this.SubNodeNoCase(sName, tNode);
    if (tFindNode != null)
    {
      if(tFindNode.firstChild == null || tFindNode.firstChild.nodeValue == null)
      {
        return "";
      }
      
      return tFindNode.firstChild.nodeValue;
    }
    
    return null;
  },
  
  SetValue: function(sValue, tNode)
  {
    if (tNode == null)
    {
      tNode = this.ResultElement();
    }
    
    tNode.element.firstChild.nodeValue = sValue;
  },
  
  SetData: function(tData, tNode)
  {
    if (tNode == null)
    {
      tNode = this.ResultElement();
    }
    
    tNode.add_child(tData);
  }
};

