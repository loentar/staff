/******************************************************************************/
var WSRF = {
  Version :   '0.1',
  Author :    'James M Snell',
  Copyright : 'Copyright 2005, IBM Corporation',
  Revision :  '2005-11-21T16:24:00-8:00',
  URI : 'http://docs.oasis-open.org/wsrf/rp-2'
};

/******************************************************************************/
WSRF.Request = {}
WSRF.Request.QNAME_GETRESOURCEPROPERTY = 
  new WS.QName("GetResourceProperty", WSRF.URI),
WSRF.Request.QNAME_GETMULTIPLERESOURCEPROPERTIES = 
  new WS.QName("GetMultipleResourceProperties", WSRF.URI)
WSRF.Request.QNAME_RESOURCEPROPERTY = 
  new WS.QName("ResourceProperty", WSRF.URI);
WSRF.Request.GetResourceProperty = Class.create();
WSRF.Request.GetResourceProperty.prototype = {
  initialize : function(qname) {
    this.envelope = new SOAP.Envelope();
    this.set_qname(qname);
  },
  set_qname : function(qname) {
    var body = this.envelope.create_body();
    var method = body.create_child(
      WSRF.Request.QNAME_GETRESOURCEPROPERTY);
    if (!qname.namespace) qname.namespace = '';
    if (!qname.prefix) qname.prefix = 'ns';
    method.declare_namespace(qname);
    method.set_value(qname.value_of());
  }
};
WSRF.Request.GetMultipleResourceProperties = Class.create();
WSRF.Request.GetMultipleResourceProperties.prototype = {
  initialize : function(qnames) {
    this.envelope = new SOAP.Envelope();
    this.set_qnames(qnames);
  },
  set_qnames : function(qnames) {
    var body = this.envelope.create_body();
    var method = body.create_child(
      WSRF.Request.QNAME_GETMULTIPLERESOURCEPROPERTIES);
    var n = 0;
    qnames.each(
      function(qname) {
        var rp = method.create_child(
          WSRF.Request.QNAME_RESOURCEPROPERTY);
        if (!qname.namespace) qname.namespace = '';
        if (!qname.prefix) qname.prefix = 'ns' + n++;
        rp.declare_namespace(qname);
        rp.set_value(qname.value_of());
      }
    );
  }
};

/******************************************************************************/
WSRF.Resource = Class.create();
WSRF.Resource.prototype = {
  initialize : function(address, epr) {
    this.address = address;
    this.epr = epr;
  },
  get_resource_property : function(qname, callback) {
    var call = new WS.Call(this.address);
    var handler = new WSA.Handler();
    var wsactx = new WSA.MessageContext(this.epr);
    handler.set_context(wsactx);
    call.add_handler(handler);
    var req = new WSRF.Request.GetResourceProperty(qname);
    call.invoke(req.envelope, callback);
  },
  get_multiple_resource_properties : function(qnames, callback) {
    var call = new WS.Call(this.address);
    var handler = new WSA.Handler();
    var wsactx = new WSA.MessageContext(this.epr);
    handler.set_context(wsactx);
    call.add_handler(handler);
    var req = 
      new WSRF.Request.GetMultipleResourceProperties(qnames);
    call.invoke(req.envelope, callback);
  }
}
