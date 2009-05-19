namespace('webapp.view');

webapp.view.DlgList = Class.create();
webapp.view.DlgList.prototype.extend(webapp.Event.prototype).extend
({
  initialize: function(tOptions)
  {
    var tOpts = tOptions || {};

    this.tDialog = new YAHOO.widget.Dialog
    (
      webapp.ui.IdGen.Gen("DlgList"),
      {
        width: "270px",
        fixedcenter: true,
        visible: false, 
        constraintoviewport: true,
        buttons: 
        [ 
          { text: _('OK'), handler: this._OnConfirm.bind(this), isDefault: true },
          { text: _('Cancel'), handler: this._OnCancel.bind(this) } 
        ]
      }
    );
    
    this.tDialog.setHeader(tOpts.sCaption || _('List'));
    this.tDialog.setBody("");
    
    this.tLabel = new webapp.ui.Label(this.tDialog.body, { sCaption: (tOpts.sLabel || (_('Select a item') + ':'))});
    
    this.tSelect = new webapp.ui.Select(this.tDialog.body);
    this.tSelect.SetSize(7);
    this.tSelect.SetWidth(240);
    this.tSelect.On("dblclick", this._OnConfirm, this);
    
    this.tLabel.SetBuddy(this.tSelect);
    
    if (tOpts.aItems)
    {
      this.SetItems(tOpts.aItems, tOpts.tItemOpts);
    }
    
    this.bDestructiveClose = tOpts.bDestructiveClose;

    this.tDialog.render(document.body);
    
    addHandler(this.tDialog.body, 'keydown', this._OnKeyDown.bindAsEventListener(this));
  },
  
  destroy: function()
  {
    this.tSelect.destroy();
    this.tLabel.destroy();
    this.tDialog.destroy();
    delete this.tSelect;
    delete this.tLabel;
    delete this.tDialog;
  },
  
  SetCaption: function(sCaption)
  {
    this.tDialog.setHeader(sCaption || _('List'));
  },
  
  SetLabel: function(sLabel)
  {
    this.tLabel.SetCaption(sLabel || (_('Select a item') + ':'));
  },
  
  Show: function()
  {
    this.FireEvent("show");
    this.tDialog.show();
    this.tSelect.Focus();
  },
  
  Close: function()
  {
    this.FireEvent("close");
    this.tDialog.hide();
    if (this.bDestructiveClose !== false)
    {
      this.destroy();
    }
  },
  
  SetItems: function(aItems, tOptions)
  {
    this.tSelect.SetItems(aItems, tOptions);
  },
  
  GetSelect: function()
  {
    this.tSelect;
  },
  
  _OnConfirm: function()
  {
    var tParams = {};
    tParams.tItem = this.tSelect.GetActiveItem();

    this.FireEvent("confirm", tParams);

    if (tParams.bClose !== false)
    {
      this.Close();
    }
  },
  
  _OnCancel: function()
  {
    var tParams = {};
    this.FireEvent("cancel");

    if (tParams.bClose !== false)
    {
      this.Close();
    }
  },
  
  _OnKeyDown: function(tEvent)
  {
    switch (tEvent.keyCode)
    {
      case 13: // Enter
      {
        this._OnConfirm();
        break;
      }
      
      case 27: // Escape
      {
        this._OnCancel();
        break;
      }
    }
  }
  
});

