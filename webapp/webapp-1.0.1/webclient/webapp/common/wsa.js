/******************************************************************************/
var WSA = {
  Version :   '0.1',
  Author :    'James M Snell',
  Copyright : 'Copyright 2005, IBM Corporation',
  Revision :  '2005-11-21T14:00:00-8:00',
  URI : 'http://www.w3.org/2005/08/addressing',
  ANONYMOUS : 'http://www.w3.org/2005/08/addressing/anonymous'
};

/******************************************************************************/
WSA.EndpointReference = Class.create();
WSA.EndpointReference.QNAME = new WS.QName('EndpointReference', WSA.URI);
WSA.EndpointReference.QNAME_ADDRESS = new WS.QName('Address', WSA.URI);
WSA.EndpointReference.QNAME_REFERENCEPARAMETERS = new WS.QName('ReferenceParameters', WSA.URI);
WSA.EndpointReference.QNAME_ACTION = new WS.QName('Action', WSA.URI);
WSA.EndpointReference.QNAME_TO = new WS.QName('To', WSA.URI);
WSA.EndpointReference.QNAME_FROM = new WS.QName('From', WSA.URI);
WSA.EndpointReference.QNAME_REPLYTO = new WS.QName('ReplyTo', WSA.URI);
WSA.EndpointReference.QNAME_FAULTTO = new WS.QName('FaultTo', WSA.URI);
WSA.EndpointReference.QNAME_MESSAGEID = new WS.QName('MessageID', WSA.URI);
WSA.EndpointReference.QNAME_RELATESTO = new WS.QName('RelatesTo', WSA.URI);
WSA.EndpointReference.prototype = (new SOAP.Element()).extend({
  initialize : function(address) {
    var document = 
      XML.createDocumentQName(WSA.EndpointReference.QNAME)
    element = document.documentElement;
    this.initialize_internal(element);
    this.set_address(address);
  },
  set_value : null,
  get_value : null,
  equals : function(epr) {
    if (!(epr instanceof WSA.EndpointReference)) return false;
    return epr.get_address().get_value() == this.get_address().get_value();
  },
  set_address : function(address) {
    if (!this.has_address()) {
      var doc = this.element.ownerDocument;
      var el = XML.createElementQName(doc, WSA.EndpointReference.QNAME_ADDRESS);
      el.appendChild(doc.createTextNode(address));
      this.element.appendChild(el);
    } else {
      this.get_address().set_value(address);
    }
  },
  get_address : function() {
    var val = null;
    for (var n = 0; n < this.element.childNodes.length; n++) {
      if (this.element.childNodes[n].nodeType == 1) {
        var el = this.element.childNodes[n];
        if (WSA.EndpointReference.QNAME_ADDRESS.equals(WS.QName.fromElement.bind2(el)())) {
          val = new WSA.EndpointReference.Address(el);
          break;
        }
      }
    }
    return val;
  },
  has_address : function() {
    var val = null;
    for (var n = 0; n < this.element.childNodes.length; n++) {
      if (this.element.childNodes[n].nodeType == 1) {
        var el = this.element.childNodes[n];
        if (WSA.EndpointReference.QNAME_ADDRESS.equals(WS.QName.fromElement.bind2(el)())) {
          val = true;
          break;
        }
      }
    }
    return val;
  },
  create_reference_parameters : function() {
    if (!this.has_reference_parameters()) {
      var doc = this.element.ownerDocument;
      var el = XML.createElementQName(doc, WSA.EndpointReference.QNAME_REFERENCEPARAMETERS);
      this.element.appendChild(el);
      var ret = new WSA.EndpointReference.ReferenceParameters(el);
      return ret;
    } else {
      return this.get_reference_parameters();
    }
  },
  get_reference_parameters : function() {
    var val = null;
    for (var n = 0; n < this.element.childNodes.length; n++) {
      if (this.element.childNodes[n].nodeType == 1) {
        var el = this.element.childNodes[n];
        if (WSA.EndpointReference.QNAME_REFERENCEPARAMETERS.equals(WS.QName.fromElement.bind2(el)())) {
          val = new WSA.EndpointReference.ReferenceParameters(el);
          break;
        }
      }
    }
    return val;
  },
  has_reference_parameters : function() {
    var val = null;
    for (var n = 0; n < this.element.childNodes.length; n++) {
      if (this.element.childNodes[n].nodeType == 1) {
        var el = this.element.childNodes[n];
        if (WSA.EndpointReference.QNAME_REFERENCEPARAMETERS.equals(WS.QName.fromElement.bind2(el)())) {
          val = true;
          break;
        }
      }
    }
    return val;
  }
});

WSA.EndpointReference.Address = Class.create();
WSA.EndpointReference.Address.prototype = (new SOAP.Element()).extend({
  initialize : function(element) {
    this.initialize_internal(element);
  }
});

WSA.EndpointReference.ReferenceParameters = Class.create();
WSA.EndpointReference.ReferenceParameters.prototype = (new SOAP.Element()).extend({
  initialize : function(element) {
    this.initialize_internal(element);
  },
  set_value : function() {},
  get_value : function() {}
});


/******************************************************************************/
WSA.EndpointReference.Binder = Class.create();
WSA.EndpointReference.Binder.prototype = (new WS.Binder()).extend({
  initialize : function(qname) {
    if (qname == null) qname = WSA.EndpointReference.QNAME;
    this.qname = qname;
  },
  to_soap_element : function(value,envelope) {
    var doc = envelope.element.ownerDocument;
    var el = XML.createElementQName(doc, this.qname);
    var ret = null;
    if (value instanceof WSA.EndpointReference) {
      var vel = value.element;
      var nodes = vel.childNodes;
      for (var n = 0; n < nodes.length; n++) {
        var nel = doc.importNode(nodes[n], true);
        el.appendChild(nel);
      }
      ret = new SOAP.Element(el);
    } else {
      var epr = new WSA.EndpointReference(value);
      ret = this.to_soap_element(epr,envelope);
    }
    return ret;
  },
  to_value_object : function(element) {
    var epr = new WSA.EndpointReference('');
    epr.element = (element instanceof SOAP.Element) ? element.element : element;
    return epr;
  }
});

/******************************************************************************/
WSA.MessageContext = function(
  to,        // WSA.EndpointReference
  from,      // WSA.EndpointReference
  replyto,   // WSA.EndpointReference
  faultto,   // WSA.EndpointReference
  action,    // URI
  messageid, // String
  relatesto  // Array of String pairs [relationship, uri],
  ) {
    this.to = to;
    this.from = from;
    this.replyto = replyto;
    this.faultto = faultto;
    this.action = action;
    this.messageid = messageid;
    this.relatesto = relatesto;
}

/******************************************************************************/
WSA.Handler = Class.create();
WSA.Handler.prototype = (new WS.Handler()).extend({
  set_context : function(context) {
    if (!(context instanceof WSA.MessageContext)) {
      throw 'Context must be an instance of WSA.MessageContext'
    }
    this.context = context;
  },
  on_request : function(call, envelope) {
    if (this.context == null) return;
    var header = envelope.create_header();
    if (this.context.to) 
      header.create_child(
        WSA.EndpointReference.QNAME_TO).
          set_value(this.context.to.get_address().get_value());
      refparams = this.context.to.get_reference_parameters();
      if (refparams) {
        refparams.get_all_children().each(
          function(value) {
            header.add_child(value);
          }
        );
      }
    if (this.context.action) 
      header.create_child(
        WSA.EndpointReference.QNAME_ACTION).
          set_value(this.context.action);
    if (this.context.messageid) 
      header.create_child(
        WSA.EndpointReference.QNAME_MESSAGEID).
          set_value(this.context.messageid);
    var serepr = function(qname, epr) {
      var epr_b = new WSA.EndpointReference.Binder(qname);
      var soapel = epr_b.to_soap_element(epr, envelope);
      return soapel;
    }
    if (this.context.from) { 
      header.add_child(
        serepr(
          WSA.EndpointReference.QNAME_FROM, 
          this.context.from)) }
    if (this.context.replyto) { 
      header.add_child(
        serepr(
          WSA.EndpointReference.QNAME_REPLYTO, 
          this.context.replyto)) }
    if (this.context.faultto) { 
      header.add_child(
        serepr(
          WSA.EndpointReference.QNAME_FAULTTO, 
          this.context.faultto)) }
    if (this.context.relatesto) {
      this.context.relatesto.each(function(value) {
        var h = header.create_child(
          WSA.EndpointReference.QNAME_RELATESTO);
        h.set_attribute(
          new WS.QName(
            'RelationshipType', 
            WSA.URI), value[1]);
        h.set_value(value[0]);
      });
    }
  },
  on_response : function(call, envelope) {},
  on_error : function(call, envelope) {}
});
