namespace('widget');

widget.WidgetEditDialog = Class.create();
widget.WidgetEditDialog.tWidgetEditDialogPanel = null;
widget.WidgetEditDialog.prototype = 
{
  initialize: function()
  {
  },
  
  Hide: function()
  {
    webapp.MessageBox.Hide();
    webapp.MessageBox.tResult = false;
  },

  Show: function(stWidget, pOnConfirmFunction)
  {
    var self = this;
    var sBodyCode = "";
    sBodyCode += '<table cellspacing="8px">';
    sBodyCode +=   '<tr>';
    sBodyCode +=     '<td><label for="edWidgetEditClass" id="labWidgetEditClassLabel">Класс:&nbsp;</label></td>';
    sBodyCode +=     '<td><input style="width:100px" id="edWidgetEditClass" value="' + stWidget.sClass + '" readonly="readonly" /></td>';
    sBodyCode +=   '&nbsp;&nbsp;&nbsp;&nbsp;';
    sBodyCode +=     '<td><label for="edWidgetEditName" id="labWidgetEditNameLabel">Имя:&nbsp;</label></td>';
    sBodyCode +=     '<td><input style="width:100px" id="edWidgetEditName" value="' + stWidget.sName + '" /></td>';
    sBodyCode +=   '</tr>';
    sBodyCode += '</table>';
    
    sBodyCode +=   '<hr/><b>Свойства</b><p/><p/>';

    sBodyCode +=   '<div id="divEditWidgetTable"></div>';
    this.formatButton = 
      function(elCell, oRecord, oColumn, oData) 
      {
        elCell.innerHTML = 
          '<span class="yui-button yui-push-button">' +
            '<span class="first-child">' +
              '<button name="btnWidgetPropDel" type="button" style="background: url(webapp/assets/img/del.gif) center center no-repeat; text-indent: -4em; height: 2em; width: 2em; *margin-left: 4em;"></button>' +
            '</span>' +
          '</span>';
     };
       
    this.formatTextBox = function(elCell, oRecord, oColumn, oData)
    {
      YAHOO.widget.DataTable.formatTextbox(elCell, oRecord, oColumn, oData);
      elCell.firstChild.style.width = "128px";
      addHandler
      ( 
        elCell.firstChild, 
        "blur", 
        function(tEvent) // save data
        {
          if(tEvent.originalTarget != null) // firefox
            oRecord.getData()[oColumn.key] = tEvent.originalTarget.value;
          else
          if(tEvent.srcElement != null) // IE
            oRecord.getData()[oColumn.key] = tEvent.srcElement.value;
        },
        false
      );
    }

    var myColumnDefs = 
    [
      {key: "sName", formatter: this.formatTextBox, label: "Имя"},
      {key: "tValue", formatter: this.formatTextBox, label: "Значение"},
      {key: "tActions", formatter: this.formatButton, label: "Действия"}
    ];

    this.myDataSource = new YAHOO.util.DataSource(stWidget);
    this.myDataSource.responseType = YAHOO.util.DataSource.TYPE_JSON;
    this.myDataSource.responseSchema = 
    {
      resultsList: "lsProperties",
      fields: [ {key: "sName"}, "tValue" ]
    };
  
    var tCenterElement = document.createElement('center');

    var tDataTableElement = document.createElement('div');
    var tButtonsElement = document.createElement('div');

    tButtonsElement.id = "tButtonsElement";

    tCenterElement.appendChild(tDataTableElement);
    tCenterElement.appendChild(tButtonsElement);
    
    var tMBPanel = webapp.MessageBox.GetPanel();
    tMBPanel.setBody(sBodyCode);
    tMBPanel.appendToBody(tCenterElement);
    
    tButtonsElement.style.textAlign = "right";
    tButtonsElement.style.width =  "380px";
    tButtonsElement.style.marginTop = "8px";

    var btnAdd = new YAHOO.widget.Button
    ({
       label: '<img src="webapp/assets/img/add.gif" style="margin-top: 2px" align="left"></img>Добавить',
       container: "tButtonsElement",
       id: "btnAdd"
    });
    
    // устанавка обработчика нажатия
    btnAdd.on
    (
      "click", 
      function()
      {
        self.myDataTable.addRow({sName: "name", tValue: "value"});
      }
    );

    this.myDataTable = new YAHOO.widget.DataTable(tDataTableElement, myColumnDefs, this.myDataSource);
    this.myDataTable.render();
     
    this.myDataTable.subscribe
      (
        "buttonClickEvent", 
        function(oArgs)
        {
          if(oArgs.target.name == "btnWidgetPropDel")
          {
            var tRecordData = this.getRecord(oArgs.target).getData();
            var tRecordSet = this.getRecordSet();
            
            for(var i = 0; i < tRecordSet.getLength(); ++i)
            {
              if(tRecordSet.getRecord(i).getData() == tRecordData)
              {
                self.myDataTable.deleteRow(i);
                break;
              }
            }
          }
        }
      );
  
    function SaveChanges()
    {
      stWidget.sName = $('edWidgetEditName').value;
      stWidget.lsProperties.length = 0;
      
      var tRecordSet = self.myDataTable.getRecordSet();
      for(var i = 0; i < tRecordSet.getLength(); ++i)
        stWidget.lsProperties.push(tRecordSet.getRecord(i).getData());
      
      webapp.MessageBox.Hide();
      webapp.MessageBox.tResult = true;
      if(pOnConfirmFunction)
        pOnConfirmFunction(stWidget);
    }

    webapp.MessageBox.ShowMessage
    (
      null, 
      '',
      [ { text: 'Сохранить', handler: SaveChanges, id: 'btnDlgSave' } ],
      'Свойства виджета'
    );
  }
};
