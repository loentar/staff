// most browsers does not support methods other than POST and GET
// so this script installs hook to make requests via XMLHttpRequest object

(function(){

  setTimeout(function(){
    if (typeof jQuery == "undefined")
    {
      var elErr = document.createElement("span");
      elErr.style.color="red";
      elErr.innerHTML = "<br/><b>Error while loading jQuery.</b><br/> Please download it from <a href=\"http://jquery.com/\">http://jquery.com/</a> " + 
      "and put it as jquery.js into this \"samples/rest/webclient\" directory";
      document.body.appendChild(elErr);
    }
  }, 2000);


  function onFormSubmit(oEvent)
  {
    var oForm = $(oEvent.target);
    var sMethod = oForm.attr('method');
    var sRestUrl = oForm.attr('action') + oForm.serialize();
    var sEncoding = oForm[0].encoding;

    function onSuccess(sResult)
    {
      alert(sResult);
    }

    function onError()
    {
      alert("failed to send [" + sMethod + "] request: " + arguments[2]);
    }

    jQuery.ajax({
      type: sMethod,
      async: true,
      url: sRestUrl,
      dataType: "text",
      contentType: sEncoding,
      processData: false
    })
    .success(onSuccess)
    .error(onError);

    oEvent.preventDefault();
  }

  $(document).ready(function(){
    // bind our handler instead default submit method
    $('form').bind("submit", onFormSubmit);
  });


})()
