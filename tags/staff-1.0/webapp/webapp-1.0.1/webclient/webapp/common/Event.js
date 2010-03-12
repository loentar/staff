/*
 *  Copyright 2009 Utkin Dmitry
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
