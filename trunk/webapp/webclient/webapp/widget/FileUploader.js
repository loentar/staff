namespace('webapp.widget');

Include("webapp/clients/FileUploader");
IncludeCss("webapp/assets/widgets/FileUploader/FileUploader");

///////////////////////////////////////////////////////////////
// class FileUploader

webapp.widget.FileUploader = Class.create();
webapp.widget.FileUploader.prototype.extend(webapp.widget.Widget.prototype).extend
({
  Create: function(tParent, tOpts)
  {
    var tTableLayout = new webapp.ui.Table(tParent, { sClass: 'tableFileUploaderLayout' });

    this.tUploaderFrame = document.createElement('iframe');
    tTableLayout.AddRowCell().appendChild(this.tUploaderFrame);
    
    this.tUploaderFrame.id = 'idFileUploaderFrame';
    this.tUploaderFrame.frameBorder = 'none';
    this.tUploaderFrame.scrolling = 'no';
    addHandler(this.tUploaderFrame, 'load', this._OnFrameLoaded.bindAsEventListener(this));
    
    this.tLabelStatus = new webapp.ui.Label(tTableLayout.AddRowCell(), 
        { sClass: 'labFileUploaderStatus' });
    
    this.sDefaultUploadDir = tOpts.sUploadDir;
    this._CreateUpload();

    return tTableLayout;
  },
  
  _CreateUpload: function()
  {
    var tUploaderFrameDoc = (this.tUploaderFrame.contentWindow || this.tUploaderFrame.window).document;

    this.bSubmitted = false;

    tUploaderFrameDoc.open("text/html", "replace");
    tUploaderFrameDoc.write
    (
      '<html xmlns="http://www.w3.org/1999/xhtml">' +
        '<head>'+
          '<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />' +
          '<link rel="stylesheet" type="text/css" href="webapp/assets/widgets/FileUploader/FileUploaderFrame.css" />' +
          '<script type="text/javascript" src="webapp/common/tools.js"></script>' +
          '<script type="text/javascript" src="webapp/common/noselect.js"></script>' +
        '</head>' +
        '<body oncontextmenu="javascript:selectHandler(event || window.event);">' +
          '<form method="POST" action="/uploadfile" enctype="multipart/form-data" class="formUpload">' +
            '<label for="idFileUpload">' + _('Upload file') + ':</label>' +
            '<br/><input type="file" name="file" id="idFileUpload" size="20" />' +
            '<br/><label for="idPath">' + _('Save to') + ':</label>' +
            '<br/><input id="idPath" size="20" value="' + (this.sDefaultUploadDir || '/tmp/uploads') + '"/>' +
            '<br/><input type="checkbox" id="idUnpack"/>' +
            '<label for="idUnpack">' + _('Unpack file after uploading') + '</label>' +
            '<hr/>' +
            '<input type="submit" id="idBtnUpload" value="' + _('Upload') + '"/>' +
          '</form>' +
        '</body>' +
      '</html>'
    );
    tUploaderFrameDoc.close();
    
    this.tLabelStatus.SetCaption(_('Select a file'));
  },
  
  _CreateUploadFinish: function()
  {
    var tUploaderFrameDoc = (this.tUploaderFrame.contentWindow || this.tUploaderFrame.window).document;

    this.tUploaderFrame.style.visibility = "visible";
    this.bSubmitted = false;
    tUploaderFrameDoc.open("text/html", "replace");
    tUploaderFrameDoc.write
    (
      '<html xmlns="http://www.w3.org/1999/xhtml">' +
        '<head>'+
          '<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />' +
          '<link rel="stylesheet" type="text/css" href="webapp/assets/widgets/FileUploader/FileUploaderFrame.css" />' +
          '<script type="text/javascript" src="webapp/common/tools.js"></script>' +
          '<script type="text/javascript" src="webapp/common/noselect.js"></script>' +
        '</head>' +
        '<body oncontextmenu="javascript:selectHandler(event || window.event);">' +
          '<label class="' + (this.bUploadStatus ? 'labUploadSuccessed' : 'labUploadFailed') + '">' +
            _(this.bUploadStatus ? 'Upload successed' : 'Upload failed') +
          '</label>' +
          (this.bUploadStatus ? '<p/><a>' + _('File saved as') + ': <br/>' + this.sSaveTo + '/' + this.sFile + '</a>' : '') +
          '<p/><a href="javascript:void(0);" id="idLinkNewUpload">' + _('New upload') + '</a>' +
        '</body>' +
      '</html>'
    );
    tUploaderFrameDoc.close();
  },
  
  _OnFrameLoaded: function(tEvent)
  {
    var tUploaderFrameDoc = (this.tUploaderFrame.contentWindow || this.tUploaderFrame.window).document;
    if (this.bSubmitted)
    {
      var tDivSuccess = tUploaderFrameDoc.getElementById('divSuccess');
      if (tDivSuccess)
      {
        setTimeout(this._OnFileSubmitOk.bind(this), 100);
      }
      else
      {
        this.tLabelStatus.SetCaption(_('Upload failed'));
        this.bUploadStatus = false;
        setTimeout(this._CreateUploadFinish.bind(this), 100);
      }
    }
    else
    {
      var tBtnUpload = tUploaderFrameDoc.getElementById('idBtnUpload');
      if (tBtnUpload)
      {
        addHandler(tBtnUpload, 'click', this._OnCheckUpload.bindAsEventListener(this));
      }
      else
      {
        var tLinkNewUpload = tUploaderFrameDoc.getElementById('idLinkNewUpload');
        if (tLinkNewUpload)
        {
          addHandler(tLinkNewUpload, 'click', this._CreateUpload.bind(this));
        }
      }
    }
  },
  
  _OnCheckUpload: function(tEvent)
  {
    var tUploaderFrameDoc = (this.tUploaderFrame.contentWindow || this.tUploaderFrame.window).document;

    var tEditPath = tUploaderFrameDoc.getElementById('idPath');
    var tFileUpload = tUploaderFrameDoc.getElementById('idFileUpload');
    var tCheckUnpack = tUploaderFrameDoc.getElementById('idUnpack');

    this.sSaveTo = tEditPath.value;
    this.sFile = tFileUpload.value.replace(/^.*[\\\/](.*)$/g, '$1');
    this.bUnpack = tCheckUnpack.checked;

    if (!this.sSaveTo)
    {
      webapp.view.MessageBox.ShowMessage(_('Please, fill in "save to" path'), 'error');
      return cancelEvent(tEvent);
    }

    if (!this.sFile)
    {
      webapp.view.MessageBox.ShowMessage(_('Please, select a file'), 'error');
      return cancelEvent(tEvent);
    }

    this.bSubmitted = true;

    this.tUploaderFrame.style.visibility = "hidden";
    this.tLabelStatus.SetCaption(_('Uploading') + '...');
  },

  _GetFileUploader: function()
  {
    if(!this._tFileUploader)
    {
      this._tFileUploader = new webapp.FileUploader();
    }
    return this._tFileUploader;
  },

  _OnFileSubmitOk: function()
  {
    this.bSubmitted = false;
    if (this.bUnpack)
    {
      this.tLabelStatus.SetCaption(_('Upload successed') + ', ' + _('unpacking') + '...');
      this._GetFileUploader().Unpack(this.sFile, this.sSaveTo);
      setTimeout(this._CheckUnpackProcess.bind(this), 1000);
    }
    else
    {
      this.tLabelStatus.SetCaption(_('Upload successed') + ', ' + _('moving to target path') + '...');
      this._GetFileUploader().Move(this.sFile, this.sSaveTo);
      this.tLabelStatus.SetCaption(_('Success'));
      this.bUploadStatus = true;
      this._CreateUploadFinish();
    }
  },
  
  _CheckUnpackProcess: function()
  {
    var nStatus = parseInt(this._GetFileUploader().GetUnpackingStatus());
    switch (nStatus)
    {
      case -1:  // unpacking
      {
        this.sUnpackDots = (!this.sUnpackDots || (this.sUnpackDots.length > 3)) ? '.' : (this.sUnpackDots + '.');
        this.tLabelStatus.SetCaption(_('Unpacking') + this.sUnpackDots);
        setTimeout(this._CheckUnpackProcess.bind(this), 2000);
        break;
      }

      case 0:  // finished
      {
        this.tLabelStatus.SetCaption(_('Unpack finished'));
        this._GetFileUploader().Delete(this.sFile);
        this.bUploadStatus = true;
        this._CreateUploadFinish();
        break;
      }

      default:  // failed
      {
        this.tLabelStatus.SetCaption(_('Unpack failed'));
        this._GetFileUploader().Delete(this.sFile);
        this.bUploadStatus = false;
        this._CreateUploadFinish();
        break;
      }
    }
  }
});

