namespace('webapp');

webapp.Event = Class.create();
webapp.Event.prototype =
{
  AddEventHandler: function(sEvent, fnHandler, tScope, tObject)
  {
    if(this.tEvents == null)
    {
      this.tEvents = {};
    }
    
    if(this.tEvents[sEvent] == null)
    {
      this.tEvents[sEvent] = [];
    }
    
    this.tEvents[sEvent].push( { fnHandler: tScope != null ? fnHandler.bind(tScope) : fnHandler, tObject: tObject } );
  },
  
  RemoveEventHandler: function(sEvent, fnHandler)
  {
    if(this.tEvents != null)
    {
      var aEventHandlers = this.tEvents[sEvent];
      if(aEventHandlers != null)
      {
        for(var tHandler in aEventHandlers)
        {
          if (aEventHandlers[tHandler].fnHandler == fnHandler)
          {
            delete aEventHandlers[tHandler];
          }
        }
      }
    }
  },
  
  RemoveAllEventHandlers: function(sEvent)
  {
    if(this.tEvents != null && this.tEvents[sEvent] != null)
    {
      delete this.tEvents[sEvent];
    }
  },
  
  On: function(sEvent, fnHandler, tScope, tObject)
  {
    if(this.tEvents == null)
    {
      this.tEvents = {};
    }
    
    this.tEvents[sEvent] = [ { fnHandler: tScope != null ? fnHandler.bind(tScope) : fnHandler, tObject: tObject } ];
  },
  
  FireEvent: function(sEvent, tParam)
  {
    if(this.tEvents != null)
    {
      var aEventHandlers = this.tEvents[sEvent];
      if(aEventHandlers != null)
      {
        for(var tHandler in aEventHandlers)
        {
          var tEventHandler = aEventHandlers[tHandler];
          if (tEventHandler.fnHandler != null)
          {
            tEventHandler.fnHandler(tParam, tEventHandler.tObject);
          }
        }
      }
    }
  },
  
  Events: function()
  {
    return this.tEvents;
  }

};
