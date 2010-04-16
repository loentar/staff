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

namespace('webapp.view');

//static class webapp.view.MessageBox
webapp.view.MessageBox =
{
  Hide: function()
  {
    this.GetPanel().hide();
  },

  ShowMessage: function(sText, sType, asButtons, sCaption)
  {
    this.GetPanel();
    
    if(sText != null)
    {
      this.tMessageBoxPanel.setBody(sText);
    }
    
    if(sCaption != null)
    {
      this.tMessageBoxPanel.setHeader(sCaption);
    } else
    if(sType == 'error')
    {
      this.tMessageBoxPanel.setHeader(_('Error') + '!');
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_BLOCK);
    }
    else
    if(sType == 'alarm')
    {
      this.tMessageBoxPanel.setHeader(_('Alarm'));
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_ALARM);
    }
    else
    if(sType == 'help')
    {
      this.tMessageBoxPanel.setHeader(_('Help'));
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_HELP);
    }
    else
    if(sType == 'info')
    {
      this.tMessageBoxPanel.setHeader(_('Information'));
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_INFO);
    }
    else
    if(sType == 'tip')
    {
      this.tMessageBoxPanel.setHeader(_('Hint'));
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_TIP);
    }
    else
    {
      this.tMessageBoxPanel.setHeader(_('Warning') + '!');
      this.tMessageBoxPanel.cfg.setProperty('icon', YAHOO.widget.SimpleDialog.ICON_WARN);
    }

    if(asButtons == null)
    {
      asButtons = [];
    }
    asButtons.push({ text: _('Close'), handler: this.tMessageBoxPanel.hide, isDefault: true, id: "btnDlgClose" });

    this.tMessageBoxPanel.cfg.setProperty('buttons', asButtons);
    this.tMessageBoxPanel.bringToTop();
    this.tMessageBoxPanel.show();
    this.tMessageBoxPanel.body.focus();
  },
  
  GetPanel: function()
  {
    if(this.tMessageBoxPanel == null)
    {
      this.tMessageBoxPanel = new YAHOO.widget.SimpleDialog
        (
          'alert', 
          {
            fixedcenter: true,
            visible: false,
            modal: true,
            width: '400px',
            constraintoviewport: true
          }
        );
      this.tMessageBoxPanel.render(document.body);
      addHandler(this.tMessageBoxPanel.body, 'keydown', this._OnKeyDown.bindAsEventListener(this));
    }
    return this.tMessageBoxPanel;
  },
  
  _OnKeyDown: function(tEvent)
  {
    switch (tEvent.keyCode)
    {
      case 13: // Enter
      case 27: // Escape
      {
        this.tMessageBoxPanel.hide();
        break;
      }
    }
  },
  
  tMessageBoxPanel: null
};
