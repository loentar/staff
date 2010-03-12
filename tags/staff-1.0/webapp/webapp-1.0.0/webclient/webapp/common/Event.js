namespace('webapp');

webapp.Event = Class.create();
webapp.Event.prototype =
{
  initialize: function()
  {
    this.tEvents = {};
  },
  
  AddEventHandler: function(sEvent, fHandler, tScope, tObject)
  {
    var aEventHandlers = this.tEvents[sEvent];
    if(aEventHandlers == null)
    {
      aEventHandlers = new Array();
      this.tEvents[sEvent] = aEventHandlers;
    }
    
    aEventHandlers.push( { fHandler: tScope != null ? fHandler.bind(tScope) : fHandler, tObject: tObject } );
  },
  
  RemoveEventHandler: function(sEvent, fHandler)
  {
    var aEventHandlers = this.tEvents[sEvent];
    if(aEventHandlers != null)
    {
      for(var tHandler in aEventHandlers)
      {
        if (aEventHandlers[tHandler].fHandler == fHandler)
        {
          delete aEventHandlers[tHandler];
        }
      }
    }
  },
  
  FireEvent: function(sEvent, tParam)
  {
    var aEventHandlers = this.tEvents[sEvent];
    if(aEventHandlers != null)
    {
      for(var tHandler in aEventHandlers)
      {
        var tEventHandler = aEventHandlers[tHandler];
        if (tEventHandler.fHandler != null)
        {
          tEventHandler.fHandler(tEventHandler.tObject, tParam);
        }
      }
    }
  },
  
  Events: function()
  {
    return this.tEvents;
  }

};
